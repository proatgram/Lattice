module;

#include <yaml-cpp/yaml.h>

module Lattice.Object.IToolchain;

import Lattice.Registry;

using namespace Lattice::Object;

IToolchain::IToolchain(Constructable, const std::string &identifier) :
    Object(Object::Constructable(), identifier) {}

auto IToolchainFactory::Create(const std::string &identifier, const std::optional<std::string> &objectData) -> std::shared_ptr<Object> {
    YAML::Node config;

    try {
        config = YAML::Load(objectData.value_or(""));
    } catch (const YAML::ParserException &err) {
        throw std::runtime_error(std::format("Failed to create toolchain {}: Toolchain configuration is malformed: {}", identifier, err.what()));
    }

    if (!config["languages"])
        throw std::runtime_error(std::format("Failed to create toolchain {}: Required `languages` configuration entry doesn't exist.", identifier));

    // TODO: Get toolchain implementation identifier from Plugins
    // First query plugins and see if there are any providing IToolchainFactory's.
    // Then query what languages each supports, and if we have a match we use it.
    // If there are multiple matches, we run through a try catch for each and see
    // which works. If none work, propogate error.

    auto toolchainImplFactories = Registry::GetInstance()->All<std::shared_ptr<IFactory<IToolchain>>>();
    std::shared_ptr<IFactory<IToolchain>> toolchainImplFactory{nullptr};

    for (std::shared_ptr<IFactory<IToolchain>> toolchain : toolchainImplFactories) {
        std::shared_ptr<Capabilities::LanguageIdentifiable> languageIdentifiable = std::dynamic_pointer_cast<Capabilities::LanguageIdentifiable>(toolchain);
        
        if (!languageIdentifiable)
            // toolchainImplFactory remains nullptr, will error
            continue;

        std::list<std::string> requestedLanguages = config["languages"].as<std::list<std::string>>(std::list<std::string>{});
        std::list<std::string> toolchainProviderSupportedLanguages = languageIdentifiable->GetSupportedLanguages();

        if (std::all_of(requestedLanguages.begin(), requestedLanguages.end(), [&toolchainProviderSupportedLanguages](std::string language) -> bool {
            return std::find(toolchainProviderSupportedLanguages.begin(), toolchainProviderSupportedLanguages.end(), language) != toolchainProviderSupportedLanguages.end();
        })) {
            toolchainImplFactory = toolchain;
            break;
        }
    }

    if (!toolchainImplFactory)
        throw std::runtime_error(std::format("Failed to create toolchain {}: Toolchain provider failed to give a valid factory.", identifier));

    std::shared_ptr<Object> toolchainObject = toolchainImplFactory->Create(identifier, objectData);

    if (!toolchainObject)
        throw std::runtime_error(std::format("Failed to create toolchain {}: Toolchain creation by factory failed.", identifier));

    if (std::shared_ptr<IToolchain> toolchain = toolchainObject->As<IToolchain>().value_or(nullptr); toolchain) {
        toolchain->SetSupportedLanguages(config["languages"].as<std::list<std::string>>());
    } else {
        throw std::runtime_error(std::format("Failed to create toolchain {}: Unexpected error when downcasting from Object to IToolchain. This is a bug and shouldn't happen. (where: {})", identifier, __PRETTY_FUNCTION__));
    }

    return toolchainObject;
}

// Will attempt to create a default toolchain based on the project and
// environment variables.
auto IToolchainFactory::CreateDefault() -> std::shared_ptr<IToolchain> {
    // TODO: Implement CreateDefault()
    
    return {};
}
