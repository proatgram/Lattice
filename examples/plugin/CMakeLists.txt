add_library(ExamplePlugin SHARED)

target_sources(ExamplePlugin
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/examples/plugin/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/examples/plugin/modules/ExamplePlugin.cppm"
)

target_link_libraries(ExamplePlugin
    PUBLIC
        lattice::Plugins::Plugin
    PRIVATE
)
