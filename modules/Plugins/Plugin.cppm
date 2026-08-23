export module Lattice.Plugins.Plugin;

export import std;

export namespace Lattice::Plugins {
    extern "C" {
        struct PluginInstance {
            using InitializeFunc_t = void(*)();
            using GetIDFunc_t = const char* (*)();
            using GetTargettedVersionFunc_t = const char* (*)();

            InitializeFunc_t Initialize{nullptr};
            GetIDFunc_t GetID{nullptr};
            GetTargettedVersionFunc_t GetTargettedVersion{nullptr};
        };

        using GetPluginInstanceFunc_t = PluginInstance (*)();
    }
}  // export namespace Lattice::Plugins
