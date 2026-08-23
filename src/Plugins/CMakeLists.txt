add_library(lattice_plugins_plugin)
add_library(lattice::Plugins::Plugin ALIAS lattice_plugins_plugin)

target_sources(lattice_plugins_plugin
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Plugins/Plugin.cppm"
)

add_library(lattice_plugins_loader)
add_library(lattice::Plugins::Loader ALIAS lattice_plugins_loader)

target_sources(lattice_plugins_loader
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/Plugins/Loader.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Plugins/Loader.cppm"
)

target_link_libraries(lattice_plugins_loader
    PUBLIC
        lattice::Plugins::Plugin
)
