add_library(lattice_tooling_compiler)
add_library(lattice::Tooling::Compiler ALIAS lattice_tooling_compiler)

target_sources(lattice_tooling_compiler
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/Tooling/Compiler.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Tooling/Compiler.cppm"
)

target_link_libraries(lattice_tooling_compiler
    PUBLIC
        lattice::Tooling::Configuration
        lattice::IFactory
        lattice::System::Command
)

add_library(lattice_tooling_configuration)
add_library(lattice::Tooling::Configuration ALIAS lattice_tooling_configuration)

target_sources(lattice_tooling_configuration
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/Tooling/Configuration.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Tooling/Configuration.cppm"
)

