add_library(ExamplePlugin SHARED)

target_sources(ExamplePlugin
    PRIVATE
        "${PROJECT_SOURCE_DIR}/examples/plugin/src/ExamplePlugin.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${PROJECT_SOURCE_DIR}/examples/plugin/modules"
        FILES
            "${PROJECT_SOURCE_DIR}/examples/plugin/modules/ExamplePlugin.cppm"
)

target_link_libraries(ExamplePlugin
    PUBLIC
        lattice::Plugins
    PRIVATE
        lattice::Registry
)
