module;

#include <yaml-cpp/yaml.h>

module Lattice.Object.ILibrary;

import Lattice.System.Process;
import Lattice.Tooling.Compiler;
import Lattice.Registry;
import Lattice.Object.IToolchain;
import Lattice.Project;

using namespace Lattice;

ILibrary::ILibrary(Constructable, const std::string &identifier) : Object(Object::Constructable(), identifier) {}

auto ILibrary::GetProperties() const -> std::bitset<Object::TOTAL_PROPERTIES> {
    return std::bitset<TOTAL_PROPERTIES>()
        .set(std::to_underlying(Properties::Parsable))
        .set(std::to_underlying(Properties::Buildable));
}

auto ILibrary::Build() -> void {

    std::shared_ptr<IToolchain> toolchain = Registry<std::shared_ptr<IToolchain>>::GetInstance()->Query(GetToolchainId().value_or("default")).value_or(nullptr);

    if (!toolchain)
        throw std::runtime_error(std::format("Failed to query toolchain {} for library object \"{}\".", GetToolchainId().value_or("default"), m_identifier));

    // Obtains a compiler from said toolchain if it can.
    if (!toolchain->Provides<std::shared_ptr<Tooling::ICompiler>>())
        throw std::runtime_error(std::format(""));

    std::shared_ptr<Tooling::ICompiler> compiler = toolchain->Get<std::shared_ptr<Tooling::ICompiler>>().value_or(nullptr);
    if (!compiler)
        throw std::runtime_error(std::format("Failed to obtain a compiler for library object \"{}\".", m_identifier));

    // Creates a configuration using this object.
    Tooling::CompilerConfiguration compilerConfig = compiler->CreateConfiguration(shared_from_this());

    System::Command compilerCommand = compiler->CreateCommand(compilerConfig);

    std::expected<std::shared_ptr<System::Process>, std::string> compilerProcess = System::Process::Spawn(compilerCommand);

    if (!compilerProcess)
        throw std::runtime_error(std::format("Failed to build library \"{}\": Failed to spawn compiler process: {}", m_identifier, compilerProcess.error()));

    compilerProcess.value()->Wait();
}

auto ILibraryFactory::Create(const std::string &identifier, const std::optional<std::string> &objectData) -> std::shared_ptr<Object> {
    // We expect object data to be passed here.
    if (!objectData.has_value())
        throw std::runtime_error("Failed to create library: No object data given to factory method.");
    YAML::Node config;
    try {
        config = YAML::Load(objectData.value());
    } catch (const YAML::ParserException &err) {
        throw std::runtime_error(std::format("Failed to create library: Library configuration is malformed: {}", err.what()));
    }

    // ILibrary has optional `toolchain` configuration property, and requires that the toolchain
    // have a way to provide a specialization for an ILibrary for the toolchain.
    // This method will fail if:
    //  1. There is no specified optional toolchain and the default isn't defined.
    //  2. The toolchain isn't capable of making a factory for ILibrary (does not Provide ILibraryFactory).

    std::string defaultToolchainId;
    std::shared_ptr<Project> parentProject = Registry<std::shared_ptr<Project>>::GetInstance()->Query(config["LATTICE_PRIVATE"]["parent_project"].as<std::string>("NO_PARENT_PROJECT")).value_or(nullptr);
    if (parentProject)
        defaultToolchainId = parentProject->GetToolchainId().value_or("default");

    std::string toolchainId = config["toolchain"].as<std::string>(defaultToolchainId);
    std::shared_ptr<IToolchain> toolchain = Registry<std::shared_ptr<IToolchain>>::GetInstance()->Query(toolchainId).value_or(nullptr);
    if (!toolchain)
        throw std::runtime_error(std::format("Failed to create library {}: Toolchain {} requested but isn't defined.", identifier, toolchainId));
    if (!toolchain->Provides<std::shared_ptr<ILibraryFactory>>())
        throw std::runtime_error(std::format("Failed to create library {}: Toolchain {} doesn't provide a Library factory.", identifier, toolchainId));

    std::optional<std::shared_ptr<ILibraryFactory>> libraryImplFactory = toolchain->Get<std::shared_ptr<ILibraryFactory>>();

    // If for some reason the toolchain says it provides the factory, but doesn't return one...?
    if (!libraryImplFactory)
        throw std::runtime_error(std::format("Failed to create library {}: Toolchain {} doesn't provide a valid Library factory.", identifier, toolchainId));

    // Create implemented library object first
    std::shared_ptr<Object> libraryObject = libraryImplFactory.value().get()->Create(identifier, objectData);

    if (!libraryObject)
        throw std::runtime_error(std::format("Failed to create library {}: Unexpected error when creating library object.", identifier));

    // Now we can add global library properties to the created object.
    if (auto library = libraryObject->As<ILibrary>().value_or(nullptr); library) {
        library->SetToolchainId(toolchainId);
    } else {
        throw std::runtime_error(std::format("Failed to create library {}: Unexpected error when downcasting from Object to ILibrary. This is a bug and shouldn't happen. (where: {})", identifier, __PRETTY_FUNCTION__));
    }

    return libraryObject;
}
