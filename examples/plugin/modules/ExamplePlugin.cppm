export module ExamplePlugin;

import Lattice.Plugins.Plugin;
import Lattice.Registry;

export extern "C" {
    auto GetPluginInstance() -> Lattice::Plugins::PluginInstance;
}
