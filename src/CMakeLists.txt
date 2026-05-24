add_executable(lattice)
target_sources(lattice
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/main.cpp"
)
target_link_libraries(lattice
    PUBLIC
        argparse::argparse
        lattice::Lattice
)

add_library(lattice_lattice)
add_library(lattice::Lattice ALIAS lattice_lattice)

target_sources(lattice_lattice
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/Lattice.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Lattice.cppm"
)

target_link_libraries(lattice_lattice
    PRIVATE
        lattice::Registry
        lattice::Object
        lattice::Project
        lattice::Tooling::Compiler
        yaml-cpp::yaml-cpp
)

add_library(lattice_registry)
add_library(lattice::Registry ALIAS lattice_registry)

target_sources(lattice_registry
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Registry.cppm"
)

add_subdirectory(Object)
add_subdirectory(Plugin)
add_subdirectory(Tooling)
