module;

#include <yaml-cpp/yaml.h>

module Lattice.Object.ILibrary;

import Lattice.System.Process;
import Lattice.Tooling.Compiler;
import Lattice.Registry;
import Lattice.Object.Toolchain;

using namespace Lattice;

ILibrary::ILibrary(Constructable, const std::string &identifier) : Object(Object::Constructable(), identifier) {}

auto ILibrary::GetProperties() const -> std::bitset<Object::TOTAL_PROPERTIES> {
    return std::bitset<TOTAL_PROPERTIES>()
        .set(std::to_underlying(Properties::Parsable))
        .set(std::to_underlying(Properties::Buildable));
}

auto ILibrary::Build() -> void {

    // Obtains a toolchain if it can.
    std::optional<std::shared_ptr<Toolchain>> toolchain = Registry<std::shared_ptr<Toolchain>>::GetInstance()->Query(GetToolchainId().value_or("default"));
    if (!toolchain)
        throw std::runtime_error(std::format("Failed to query toolchain {} for library object \"{}\".", GetToolchainId().value_or("default"), m_identifier));

    // Obtains a compiler from said toolchain if it can.
    std::optional<std::shared_ptr<Tooling::ICompiler>> compiler = toolchain.value()->GetCompiler();
    if (!compiler)
        throw std::runtime_error(std::format("Failed to obtain a compiler for library object \"{}\".", m_identifier));

    // Creates a configuration using this object.
    Tooling::CompilerConfiguration compilerConfig = compiler.value()->CreateConfiguration(shared_from_this());

    System::Command compilerCommand = compiler.value()->CreateCommand(compilerConfig);

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

    std::string libraryLanguage;

    if (config["language"]) {
        libraryLanguage = config["language"].as<std::string>();
    } else if (auto defaultToolchain = Registry<std::shared_ptr<Toolchain>>::GetInstance()->Query("default"); defaultToolchain.has_value()) {
        libraryLanguage = defaultToolchain.value().get()->GetLanguages().front();
    } else {
        throw std::runtime_error("Failed to create library: Language isn't defined and default toolchain unavailable.");
    }

    auto libraryImplFactory = Registry<std::shared_ptr<Plugin::IFactory<ILibrary>>>::GetInstance()->Query(libraryLanguage);

    if (!libraryImplFactory)
        throw std::runtime_error(std::format("Failed to create library: Factory isn't registered for language {}.", libraryLanguage));

    auto library = libraryImplFactory.value().get()->Create(identifier, objectData)->As<Object>();

    if (!library)
        throw std::runtime_error("Failed to create library: Unexpected error when upcasting from ILibrary to Object. This is a bug and shouldn't happen.");

    return library.value();
}
