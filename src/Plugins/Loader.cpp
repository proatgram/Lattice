module;

#include <filesystem>
#ifdef __WIN32

#else
#ifdef __linux__

#include <dlfcn.h>

#else

#endif
#endif

module Lattice.Plugins.Loader;

using namespace Lattice::Plugins;

Loader::Loader(Loader::Constructable) {};

auto Loader::GetInstance() -> std::shared_ptr<Loader> {
    static std::shared_ptr<Loader> instance = std::make_shared<Loader>(Constructable());

    return instance;
}

auto Loader::LoadFile(const std::filesystem::path &file) -> PluginInstance {
    if (!std::filesystem::exists(file) || !std::filesystem::is_regular_file(file))
        throw std::runtime_error(std::format("Failed to load plugin at {}: Not a regular file.", file.string()));
    
    GetPluginInstanceFunc_t loaderFunction{nullptr};

#ifdef __linux__
    // Shouldn't need dlclose I don't think since the plugins will be loaded
    // for the duration of program runtime.
    void *handle = dlopen(file.c_str(), RTLD_GLOBAL | RTLD_LAZY);

    if (!handle)
        throw std::runtime_error(std::format("Failed to load plugin at {}: Failed to dlopen file.", file.string()));
#endif

   loaderFunction = (GetPluginInstanceFunc_t)dlsym(handle, "GetPluginInstance");  
   if (!loaderFunction)
       throw std::runtime_error(std::format("Failed to load plugin at {}: Failed to find symbol void(GetPluginInstance(void)).", file.string()));

   PluginInstance pluginInstance = loaderFunction();

   m_loadedPlugins.push_back(pluginInstance);

   return pluginInstance;
}

auto Loader::LoadDirectory(const std::list<std::filesystem::path> &directories) -> std::list<PluginInstance> {
    std::list<PluginInstance> loaded;

    for (const std::filesystem::path &directory : directories) {
        if (directory == std::filesystem::path{} || !std::filesystem::exists(directory))
            continue;

        if (!std::filesystem::is_directory(directory))
            throw std::runtime_error(std::format("Failed to load plugins in {}: Not a directory.", directory.string()));


        for (const std::filesystem::path &file : std::filesystem::recursive_directory_iterator(directory)) {
            if (std::filesystem::is_directory(file))
                continue;

            loaded.push_back(LoadFile(file));
        }
    }

    return loaded;
}

auto Loader::GetLoadedPlugins() const -> std::list<PluginInstance> {
    return m_loadedPlugins;
}
