module ExamplePlugin;

import std;

auto Initialize() -> void {
    std::cout << "Initializing Example Plugin!" << std::endl;
}

auto GetID() -> const char* {
    return "example-plugin";
}

auto GetPluginVersionSupport() -> Lattice::Plugins::PluginVersionSupport {
    return {
        .targetedLatticeVersion = "0.0.1"
    };
}

extern "C" {
    export auto GetPluginInstance() -> Lattice::Plugins::PluginInstance {
        return {
            .Initialize = &Initialize,
            .GetID = &GetID,
            .GetPluginVersionSupport = &GetPluginVersionSupport
        };
    }
}
