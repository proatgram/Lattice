add_library(lattice_plugins SHARED)
add_library(lattice::Plugins ALIAS lattice_plugins)

target_sources(lattice_plugins
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src/Plugins/Loader.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${PROJECT_SOURCE_DIR}/modules"
        FILES
            "${PROJECT_SOURCE_DIR}/modules/Plugins/Plugin.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Plugins/Loader.cppm"
)

target_link_libraries(lattice_plugins
    PRIVATE
        lattice::Registry
)
