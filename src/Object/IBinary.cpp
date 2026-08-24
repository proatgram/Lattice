module;

#include <yaml-cpp/yaml.h>

module Lattice.Object.IBinary;

import Lattice.Registry;
import Lattice.Object.IToolchain;
import Lattice.Project;
import Lattice.Object.Resolver;

using namespace Lattice::Object;

IBinary::IBinary(Constructable, const std::string &identifier) : Object(Object::Constructable(), identifier) {}

auto IBinaryFactory::Create(const std::string &identifier, const std::optional<std::string> &objectData) -> std::shared_ptr<Object> {
    // We expect object data to be passed here.
    if (!objectData.has_value())
        throw std::runtime_error("Failed to create binary: No object data given to factory method.");
    YAML::Node config;
    try {
        config = YAML::Load(objectData.value());
    } catch (const YAML::ParserException &err) {
        throw std::runtime_error(std::format("Failed to create binary: Binary configuration is malformed: {}", err.what()));
    }

    // IBinary has optional `toolchain` configuration property, and requires that the toolchain
    // have a way to provide a specialization for an IBinary for the toolchain.
    // This method will fail if:
    //  1. There is no specified optional toolchain and the default isn't defined.
    //  2. The toolchain isn't capable of making a factory for IBinary (does not Provide IBinaryFactory).

    std::string defaultToolchainId;
    std::shared_ptr<Lattice::Object::Project> parentProject = Registry::GetInstance()->Query<std::shared_ptr<Lattice::Object::Project>>(config["LATTICE_PRIVATE"]["parent_project"].as<std::string>("NO_PARENT_PROJECT")).value_or(nullptr);
    if (parentProject) {}
        defaultToolchainId = parentProject->GetToolchainId();

    std::string toolchainId = config["toolchain"].as<std::string>(defaultToolchainId);
    std::shared_ptr<IToolchain> toolchain = Registry::GetInstance()->Query<std::shared_ptr<IToolchain>>(toolchainId).value_or(nullptr);
    if (!toolchain)
        throw std::runtime_error(std::format("Failed to create binary {}: Toolchain {} requested but isn't defined.", identifier, toolchainId));
    if (!toolchain->Provides<std::shared_ptr<IFactory<IBinary>>>())
        throw std::runtime_error(std::format("Failed to create binary {}: Toolchain {} doesn't provide a Binary factory.", identifier, toolchainId));

    std::optional<std::shared_ptr<IFactory<IBinary>>> binaryImplFactory = toolchain->Get<std::shared_ptr<IFactory<IBinary>>>();

    // If for some reason the toolchain says it provides the factory, but doesn't return one...?
    if (!binaryImplFactory)
        throw std::runtime_error(std::format("Failed to create binary {}: Toolchain {} doesn't provide a valid Binary factory.", identifier, toolchainId));

    // Create implemented binary object first
    std::shared_ptr<Object> binaryObject = binaryImplFactory.value().get()->Create(identifier, objectData);

    if (!binaryObject)
        throw std::runtime_error(std::format("Failed to create binary {}: Unexpected error when creating binary object.", identifier));

    // Now we can add global binary properties to the created object.
    if (auto binary = binaryObject->As<IBinary>().value_or(nullptr); binary) {
        binary->SetToolchainId(toolchainId);

        if (parentProject) {
            binary->SetOwningProject(parentProject);
        }

        if (config["dependencies"]) {
            if (config["dependencies"]["public"]) {
                for (const YAML::Node dependencyIdentifier : config["dependencies"]["public"]) {
                    binary->AddDependency({
                        IBinary::Dependency::Visibility::Public,
                        Resolver::Create({
                            dependencyIdentifier.as<std::string>("unknown@unknown"),
                            binary
                        })
                    });
                }
            }
            if (config["dependencies"]["private"]) {
                for (const YAML::Node dependencyIdentifier : config["dependencies"]["private"]) {
                    binary->AddDependency({
                        IBinary::Dependency::Visibility::Private,
                        Resolver::Create({
                            dependencyIdentifier.as<std::string>("unknown@unknown"),
                            binary
                        })
                    });
                }
            }
        }
    } else {
        throw std::runtime_error(std::format("Failed to create binary {}: Unexpected error when downcasting from Object to IBinary. This is a bug and shouldn't happen. (where: {})", identifier, __PRETTY_FUNCTION__));
    }

    return binaryObject;
}

