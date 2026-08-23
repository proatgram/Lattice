export module ExamplePlugin;

import Lattice.Plugins.Plugin;

extern "C" {
    auto GetPluginId() -> const char* {
        return "Example Plugin";
    }

    auto GetPluginInstance() -> Lattice::Plugins::PluginInstance {
        return Lattice::Plugins::PluginInstance{
            .GetID = &GetPluginId
        };
    }

}
