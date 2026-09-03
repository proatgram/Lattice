module;

#include <filesystem>
#include <thread>
#include <yaml-cpp/yaml.h>

module Lattice;

import Lattice.Registry;

import Lattice.Object.IToolchain;
import Lattice.Object.ILibrary;
import Lattice.Object.IBinary;
import Lattice.Object.Resolver;
import Lattice.Plugins.Loader;
import Lattice.Object.BuildGraph;

Lattice::Lattice::Lattice(Lattice::Constructable) {
    auto ok = Registry::GetInstance()->Register<std::shared_ptr<Object::ProjectFactory::FactoryType>>("project", Object::ProjectFactory::GetInstance());
    if (!ok)
        throw std::runtime_error("Irrecoverable error: Built in object type \"factory\" failed to register. This is a bug.");

    ok = Registry::GetInstance()->Register<std::shared_ptr<Object::IToolchainFactory::FactoryType>>("toolchain", Object::IToolchainFactory::GetInstance());
    if (!ok)
        throw std::runtime_error("Irrecoverable error: Built in object type \"toolchain\" failed to register. This is a bug.");

    ok = Registry::GetInstance()->Register<std::shared_ptr<Object::ILibraryFactory::FactoryType>>("library", Object::ILibraryFactory::GetInstance());
    if (!ok)
        throw std::runtime_error("Irrecoverable error: Built in object type \"library\" failed to register. This is a bug.");

    ok = Registry::GetInstance()->Register<std::shared_ptr<Object::IBinaryFactory::FactoryType>>("binary", Object::IBinaryFactory::GetInstance());
    if (!ok)
        throw std::runtime_error("Irrecoverable error: Built in object type \"binary\" failed to register. This is a bug.");

    Plugins::Loader::GetInstance()->LoadDirectory();
    Plugins::Loader::GetInstance()->InitializeAllLoaded();
}

auto Lattice::Lattice::GetInstance() -> std::shared_ptr<Lattice> {
    static std::shared_ptr<Lattice> instance = std::make_shared<Lattice>(Constructable());
    return instance;
}

auto LoadIncludes(const std::string &include, const std::filesystem::path &workingDirectory) -> std::vector<YAML::Node> {
    // Include can either be:
    //  - Directory
    //  - File (including or excluding .yaml)

    std::vector<YAML::Node> includeNodes;
    YAML::Node currentNode;
    std::filesystem::path includeWorkingDirectory;
    if (include.contains(".yaml") && std::filesystem::exists(workingDirectory / include) && std::filesystem::is_regular_file(workingDirectory / include)) {
        // First is easy, if it contains '.yaml', then it'll be a file.
        currentNode = YAML::LoadFile((workingDirectory / include).string());
        includeWorkingDirectory = (workingDirectory / include).parent_path();
    } else if (std::filesystem::exists(workingDirectory / include) && std::filesystem::is_directory(workingDirectory / include) && std::filesystem::exists(workingDirectory / include / "lattice.yaml")) {
        // Next we check if it's a directory, and if it is if it has a lattice.yaml in it
        currentNode = YAML::LoadFile((workingDirectory / include / "lattice.yaml").string());
        includeWorkingDirectory = workingDirectory / include;
    } else if (std::filesystem::exists(workingDirectory / std::string(include + ".yaml")) && std::filesystem::is_regular_file(workingDirectory / include)) {
        // Next, we check if there exists a file with that include with the ".yaml" extention
        currentNode = YAML::LoadFile((workingDirectory / std::string(include + ".yaml")));
        includeWorkingDirectory = (workingDirectory / std::string(include + ".yaml")).parent_path();
    } else {
        // If none of those exist, we can't resolve the include, so we fail.
        throw std::runtime_error(std::format("Couldn't load configuration file {}.", include));
    }

    includeNodes.push_back(currentNode);

    if (currentNode["include"] && currentNode.IsSequence()) {
        for (std::size_t i = 0;i < currentNode["include"].size(); i++) {
            std::vector<YAML::Node> include = LoadIncludes(currentNode["include"][i].as<std::string>(), includeWorkingDirectory);
            includeNodes.insert(std::end(includeNodes), std::begin(include), std::end(include));
        }
    }

    return includeNodes;
}

auto Lattice::Lattice::LoadConfig(const std::filesystem::path configPath) -> void {
    // Load all of the includes
    std::vector<YAML::Node> configs;
    configs.push_back(YAML::LoadFile(configPath.string()));
    if (configs[0]["include"] && configs[0]["include"].IsSequence()) {
        for (std::size_t i = 0; i < configs[0]["include"].size(); i++) {
            std::vector<YAML::Node> includes = LoadIncludes(configs[0]["include"][i].as<std::string>(), configPath.parent_path());
            configs.insert(std::end(configs), std::begin(includes), std::end(includes));
        }
    }

    // Make sure we have the projects defined
    for (const YAML::Node config : configs) {
        if (config["project"]) {
            YAML::Node projects = config["project"];
            if (!projects.IsMap())
                // TODO: Get file-level tracking for this.
                throw std::runtime_error(std::format("Error parsing configurations: \"project:\" is ill-formed."));

            for (YAML::const_iterator it = projects.begin(); it != projects.end(); ++it) {
                std::expected<std::reference_wrapper<std::shared_ptr<Object::Project>>, std::string> project =
                    Registry::GetInstance()->Register<std::shared_ptr<Object::Project>>(
                        it->first.as<std::string>(),
                        Registry::GetInstance()->Query<std::shared_ptr<Object::ProjectFactory::FactoryType>>("project").value()->Create(
                            it->first.as<std::string>(), YAML::Dump(it->second))->As<Object::Project>().value());
                Object::Resolver::Create({.identifier = it->first.as<std::string>(), .dependee = {}});

                if (!project)
                    // TODO: Get file-level tracking for this.
                    throw std::runtime_error(std::format("Error parsing configurations: Project \"{}\" was defined more than once.", it->first.as<std::string>()));
            }
        }

        if (config["toolchain"]) {
            YAML::Node toolchains = config["toolchain"];

            if (!toolchains.IsMap())
                // TODO: Get file-level tracking for this.
                throw std::runtime_error(std::format("Error parsing configurations: \"toolchain:\" is ill-formed."));

            for (YAML::const_iterator it = toolchains.begin(); it != toolchains.end(); ++it) {
                std::string toolchainId = it->first.as<std::string>("undefined");
                std::string toolchainConfig = YAML::Dump(it->second);
                
                std::expected<std::reference_wrapper<std::shared_ptr<Object::IToolchain>>, std::string> toolchain =
                    Registry::GetInstance()->
                        Register<std::shared_ptr<Object::IToolchain>>(
                                it->first.as<std::string>(),
                                Registry::GetInstance()->Query<std::shared_ptr<Object::IToolchainFactory::FactoryType>>("toolchain").value()->
                                    Create(
                                        it->first.as<std::string>(), YAML::Dump(it->second))->As<Object::IToolchain>().value());
                if (!toolchain)
                    // TODO: Get file-level tracking for this.
                    throw std::runtime_error(std::format("Error parsing configurations: Toolchain \"{}\" was defined more than once.", it->first.as<std::string>()));
            }
        }
    }

    for (const YAML::Node config : configs) {
        for (YAML::const_iterator it = config.begin(); it != config.end(); ++it) {
            std::string objectType = it->first.as<std::string>();
            YAML::Node objectYAML = it->second;
            if (objectType == "project" || objectType == "toolchain") {
                continue; // Already handled project objects
            } else if (Registry::GetInstance()->Contains<std::shared_ptr<Object::Project>>(objectType)) {
                // We're defining project-level objects here
                // Loop through the object types defined in the project
                for (YAML::const_iterator projectObjects = objectYAML.begin(); projectObjects != objectYAML.end(); projectObjects++) {
                    std::string projectObjectType = projectObjects->first.as<std::string>();
                    YAML::Node objectNodes = projectObjects->second;


                    // If we have an object factory for a given object defined,
                    // we create the object using the given object factory.
                    if (auto objectFactory = Registry::GetInstance()->Query<std::shared_ptr<IFactory<Object::Object>>>(projectObjectType); objectFactory.has_value()) {
                        for (YAML::const_iterator object = objectNodes.begin(); object != objectNodes.end(); object++) {
                            std::string objectIdentifier = object->first.as<std::string>();
                            std::string objectConfig; 
                            YAML::Node node = object->second;

                            // Inject some internal bits for the factories to use.
                            node["LATTICE_PRIVATE"]["parent_project"] = objectType;

                            objectConfig = YAML::Dump(node);

                            std::shared_ptr<Object::Object> newObject = objectFactory.value()->Create(objectIdentifier, objectConfig);

                            // Add it to the project-local object store  
                            Registry::GetInstance()->Query<std::shared_ptr<Object::Project>>(objectType).value()->AddObject(objectIdentifier, newObject);
                        }
                    }
                }
            } else if (auto objectFactory = Registry::GetInstance()->Query<std::shared_ptr<IFactory<Object::Object>>>(objectType); objectFactory.has_value()) {
                // Meanwhile here, we can define global objects
                for (YAML::const_iterator objectEntry = objectYAML.begin(); objectEntry != objectYAML.end(); ++objectEntry) {
                    // If we have an object factory for a given object defined,
                    // we create the object using the given object factory.
                    std::shared_ptr<Object::Object> newObject = objectFactory.value()->Create(objectEntry->first.as<std::string>(), YAML::Dump(objectEntry->second));

                }
            } 
        }
    }

    if (auto err = Object::Resolver::TryResolveAll(); !err.has_value()) {
        std::stringstream errss;
        for (const auto &[unresolvableId, message] : err.error()) {
            errss << "Unable to resolve " << unresolvableId << ". Msg: " << message << std::endl;
        }

        throw std::runtime_error(std::format("Irrecoverable error(s) when attempting to resolve dependencies.\nThe following IDs failed to resolve correctly:\n\n{}", errss.str()));
    }
}

auto Lattice::Lattice::StartBuild(const std::optional<std::list<std::string>> &objects, const std::optional<std::size_t> &numberJobs) -> void {
    std::shared_ptr<Object::BuildGraph> buildGraph;
    if (objects) {
        std::list<std::shared_ptr<Object::Resolver>> resolvers;
        for (const std::string &objectId : objects.value()) {
            resolvers.push_back(Object::Resolver::Create({.identifier = objectId, .dependee = {}}));
        }

        buildGraph = Object::BuildGraph::Generate(resolvers);
    } else {
        buildGraph = Object::BuildGraph::Generate();
    }

    std::map<std::string, std::future<void>> currentJobs;
    std::map<std::string, std::future<void>> finishedJobs;
    std::mutex jobsMutex;
    while (true) {
        {
            std::lock_guard<std::mutex> lock(jobsMutex);
            if (buildGraph->IsCompleted())
                break;
        }
        std::size_t jobs;
        {
            std::lock_guard<std::mutex> lock(jobsMutex);
            jobs = std::min(std::min(buildGraph->GetReady().size(), numberJobs.value_or(1)), numberJobs.value_or(1) - currentJobs.size());
        }
        for (int i = 0; i < jobs; i++) {
            std::list<std::shared_ptr<Object::BuildGraph::DependencyNode>> readyNodes;
            {
                std::lock_guard<std::mutex> lock(jobsMutex);
                readyNodes = buildGraph->GetReady();
            }
            for (const std::shared_ptr<Object::BuildGraph::DependencyNode> &node : readyNodes) {
                {
                    std::lock_guard<std::mutex> lock(jobsMutex);
                    if (currentJobs.contains(node->object->GetResolvedObject()->GetIdentifier()) || finishedJobs.contains(node->object->GetResolvedObject()->GetIdentifier()))
                        continue;
                }
                std::lock_guard<std::mutex> lock(jobsMutex);
                currentJobs[node->object->GetResolvedObject()->GetIdentifier()] = std::async(std::launch::async, [node, &buildGraph, &currentJobs, &finishedJobs, &jobsMutex]() -> void {
                    std::cout << "Building " << node->object->GetResolvedObject()->GetIdentifier() << "..." <<std::endl;
                    if (const std::shared_ptr<Object::Capabilities::Buildable> &buildable = node->object->GetResolvedObject()->GetCapability<Object::Capabilities::Buildable>().value_or(nullptr); buildable)
                        buildable->Build();


                    std::lock_guard<std::mutex> lock(jobsMutex);
                    buildGraph->UpdateBuilt(node);
                    finishedJobs.insert({node->object->GetResolvedObject()->GetIdentifier(), std::move(currentJobs.extract(node->object->GetResolvedObject()->GetIdentifier()).mapped())});
                });

                break;
            }
        }
    }

    while (true) {
        {
            std::lock_guard<std::mutex> lock(jobsMutex);
            if (currentJobs.size() == 0)
                break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
