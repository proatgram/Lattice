export module Lattice.Plugins.Plugin;

export import std;

export namespace Lattice::Plugins {
        struct PluginVersionSupport {
            const char *targetedLatticeVersion{nullptr};
        };

        struct PluginInstance {
            using InitializeFunc_t = void(*)(void);
            using GetIDFunc_t = const char* (*)(void);
            using GetPluginVersionSupportFunc_t = PluginVersionSupport (*)(void);

            InitializeFunc_t Initialize{nullptr};
            GetIDFunc_t GetID{nullptr};
            GetPluginVersionSupportFunc_t GetPluginVersionSupport{nullptr};
        };

        using GetPluginInstanceFunc_t = PluginInstance (*)(void);
}  // export namespace Lattice::Plugins
