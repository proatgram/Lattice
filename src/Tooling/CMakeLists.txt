add_library(lattice_tooling_icompiler)
add_library(lattice::Tooling::ICompiler ALIAS lattice_tooling_icompiler)

target_sources(lattice_tooling_icompiler
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/Tooling/ICompiler.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Tooling/ICompiler.cppm"
)

target_link_libraries(lattice_tooling_icompiler
    PUBLIC
        lattice::Tooling::Configuration
        lattice::Object
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

add_library(lattice_tooling_ilinker)
add_library(lattice::Tooling::ILinker ALIAS lattice_tooling_ilinker)

target_sources(lattice_tooling_ilinker
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/Tooling/ILinker.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Tooling/ILinker.cppm"
)

target_link_libraries(lattice_tooling_ilinker
    PUBLIC
        lattice::Tooling::Configuration
        lattice::Object
        lattice::IFactory
        lattice::System::Command
)
