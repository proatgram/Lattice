set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(lattice)
target_sources(lattice
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src/main.cpp"
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
        "${PROJECT_SOURCE_DIR}/src/Lattice.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${PROJECT_SOURCE_DIR}/modules"
        FILES
            "${PROJECT_SOURCE_DIR}/modules/Lattice.cppm"
)

target_link_libraries(lattice_lattice
    PRIVATE
        lattice::Plugins
        lattice::Registry
        lattice::Object
        yaml-cpp::yaml-cpp
)

add_library(lattice_registry SHARED)
add_library(lattice::Registry ALIAS lattice_registry)

target_sources(lattice_registry
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src/Registry.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${PROJECT_SOURCE_DIR}/modules"
        FILES
            "${PROJECT_SOURCE_DIR}/modules/Registry.cppm"
)

add_library(lattice_ifactory SHARED)
add_library(lattice::IFactory ALIAS lattice_ifactory)

target_sources(lattice_ifactory
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${PROJECT_SOURCE_DIR}/modules"
        FILES
            "${PROJECT_SOURCE_DIR}/modules/IFactory.cppm"
)

add_subdirectory(Object)
add_subdirectory(Tooling)
add_subdirectory(System)
add_subdirectory(Plugins)
