module;

#include <yaml-cpp/yaml.h>

module Lattice.Toolchain;

import Lattice.Registry;

using namespace Lattice;

Toolchain::Toolchain(Constructable, const std::string &identifier) :
    Object(Object::Constructable(), identifier) {}

auto Toolchain::GetProperties() const -> std::bitset<Object::TOTAL_PROPERTIES> {
    std::bitset<Object::TOTAL_PROPERTIES> properties;
    properties.set(std::to_underlying(Object::Properties::Parsable));

    return properties;
}

auto Toolchain::Parse(const std::string &parsable) -> void {
    YAML::Node config = YAML::Load(parsable);

    /* Check for invalid or missing object properties */
    if (!config["compiler"])
        throw std::runtime_error("Error: required `compiler` field isn't present.");

    if (!config["languages"] || !config["languages"].IsSequence())
        throw std::runtime_error("Error: required `languages` field isn't present or is malformed.");

    if (!config["paths"] || !config["paths"]["compiler"])
        throw std::runtime_error("Error: required `paths:compiler` field isn't present.");

    /* Load object properties into object */
    m_compilerName = config["compiler"].as<std::string>();
    m_compilerPath = config["paths"]["compiler"].as<std::string>();
    
    // Languages
    for (std::size_t i = 0; i < config["languages"].size(); i++) {
        m_languages.push_back(config["languages"][i].as<std::string>());
    }

    // Include paths
    if (config["paths"]["system_include_paths"]) {
        if (!config["paths"]["system_include_paths"].IsSequence())
            throw std::runtime_error("Error: toolchain property `system_include_paths` is malformed. (should be a sequence)");

        for (std::size_t i = 0; i < config["paths"]["system_include_paths"].size(); i++) {
            m_defaultSystemIncludes.emplace_back(config["paths"]["system_include_paths"][i].as<std::string>());
        }
    }
    
    // Library paths
    if (config["paths"]["library_paths"]) {
        if (!config["paths"]["library_paths"].IsSequence())
            throw std::runtime_error("Error: toolchain property `library_paths` is malformed. (should be a sequence)");

        for (std::size_t i = 0; i < config["paths"]["library_paths"].size(); i++) {
            m_defaultLibraryPaths.emplace_back(config["paths"]["library_paths"][i].as<std::string>());
        }
    }

    // Flags
    if (config["flags"]) {
        // Compiler flags
        if (config["flags"]["compiler"]) {
            if (!config["flags"]["compiler"].IsSequence())
                throw std::runtime_error("Error: toolchain property `flags:compiler` is malformed. (should be a sequence)");

            for (std::size_t i = 0; i < config["flags"]["compiler"].size(); i++) {
                m_defaultCompilerFlags.push_back(config["flags"]["compiler"][i].as<std::string>());
            }
        }

        // Linker flags
        if (config["flags"]["linker"]) {
            if (!config["flags"]["linker"].IsSequence())
                throw std::runtime_error("Error: toolchain property `flags:linker` is malformed. (should be a sequence)");

            for (std::size_t i = 0; i < config["flags"]["linker"].size(); i++) {
                m_defaultLinkerFlags.push_back(config["flags"]["linker"][i].as<std::string>());
            }
        }
    }

    // TODO: populate other properties using `Compiler`
}

auto Toolchain::GetCompilerName() const -> std::string {
    return m_compilerName;
}

auto Toolchain::GetCompilerPath() const -> std::filesystem::path {
    return m_compilerPath;
}

auto Toolchain::GetCompilerVersion() const -> std::string {
    return m_compilerVersion;
}

auto Toolchain::GetSystemIncludePaths() const -> std::list<std::filesystem::path> {
    return m_defaultSystemIncludes;
}

auto Toolchain::GetLibraryPaths() const -> std::list<std::filesystem::path> {
    return m_defaultLibraryPaths;
}

auto Toolchain::GetCompilerFlags() const -> std::vector<std::string> {
    return m_defaultCompilerFlags;
}

auto Toolchain::GetLinkerFlags() const -> std::vector<std::string> {
    return m_defaultLinkerFlags;
}

auto Toolchain::GetLanguages() const -> std::list<std::string> {
    return m_languages;
}

auto Toolchain::GetSysrootPath() const -> std::optional<std::filesystem::path> {
    return m_optionalSysrootPath;
}

auto Toolchain::GetTargetOS() const -> std::string {
    return m_targetOS;
}

auto Toolchain::GetTargetArchitecture() const -> std::string {
    return m_targetArchitecture;
}

auto Toolchain::GetTargetABI() const -> std::string {
    return m_targetABI;
}

auto Toolchain::GetTargetTriple() const -> std::string {
    return std::format("{}-{}-{}-{}", m_targetArchitecture, m_targetVendor, m_targetOS, m_targetABI);
}

auto Toolchain::GetCompiler() const -> std::optional<std::shared_ptr<Tooling::ICompiler>> {
    auto compilerFactory = Registry<std::shared_ptr<Plugin::IFactory<Tooling::ICompiler>>>::GetInstance()->Query(m_compilerName);

    if (compilerFactory.has_value()) {
        std::shared_ptr<Tooling::ICompiler> compiler = compilerFactory.value()->Create(GetTargetTriple());
        // TODO: Configure compiler for toolchain

        return compiler;
    }

    return {};
}

auto ToolchainFactory::Create(const std::string &identifier) -> std::shared_ptr<Object> {
    return std::make_shared<Toolchain>(Toolchain::Constructable(), identifier)->As<Object>().value();
}

auto ToolchainFactory::CreateDefault() -> std::shared_ptr<Toolchain> {
    std::shared_ptr<Toolchain> defaultToolchain = std::make_shared<Toolchain>(Toolchain::Constructable(), "default");

    // TODO: Default creation logic

    return defaultToolchain;
}
