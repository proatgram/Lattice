module;

#include <cstdlib>

export module Lattice.Plugins.Loader;

export import std;

export import Lattice.Plugins.Plugin;

#ifdef _WIN32
    inline constexpr std::string_view system_plugin_directory = "C:\\Program Files\\Common Files\\lattice\\plugins\\";
    inline constexpr std::string_view local_plugin_directory = "{}\\lattice\\plugins\\";
#else
#ifdef __linux__
    inline constexpr std::string_view system_plugin_directory = "/usr/lib/lattice/plugins";
    inline constexpr std::string_view local_plugin_directory = "{}/.local/lib/lattice/plugins";
#else
    inline constexpr std::string_view system_plugin_directory = "/Library/Application Support/lattice/plugins/";
    inline constexpr std::string_view local_plugin_directory = "{}/Library/Application Support/lattice/plugins/";
#endif
#endif

export namespace Lattice::Plugins {
    class Loader {
        struct Constructable{};
        public:
            Loader(Constructable);
            static auto GetInstance() -> std::shared_ptr<Loader>;

            auto LoadFile(const std::filesystem::path &file) -> PluginInstance;
            auto LoadDirectory(const std::list<std::filesystem::path> &directories = 
                    {
#ifdef __linux__
                    (getenv("HOME") != nullptr ? std::filesystem::path{std::format(local_plugin_directory, getenv("HOME"))} : std::filesystem::path{}),
#endif
                    system_plugin_directory
                    }
                ) -> std::list<PluginInstance>;

            auto InitializeAllLoaded() const -> void;

            auto GetLoadedPlugins() const -> std::list<PluginInstance>;

        private:
            std::list<PluginInstance> m_loadedPlugins;
    };
}  // export namespace Lattice::Plugins
