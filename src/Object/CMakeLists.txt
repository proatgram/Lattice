add_library(lattice_object)
add_library(lattice::Object ALIAS lattice_object)

target_sources(lattice_object
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Object/Object.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Object/Object.cppm"
)

target_link_libraries(lattice_object
    PUBLIC
        lattice::IFactory
)

add_library(lattice_project)
add_library(lattice::Project ALIAS lattice_project)

target_sources(lattice_project
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/Object/Project.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Object/Project.cppm"
)

target_link_libraries(lattice_project
    PUBLIC
        lattice::Object
        lattice::Object::Buildable
        lattice::Object::Parsable
        lattice::IFactory
    PRIVATE
        yaml-cpp::yaml-cpp
)

add_library(lattice_provider)
add_library(lattice::Provider ALIAS lattice_provider)

target_sources(lattice_provider
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/Object/Provider.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Object/Provider.cppm"
)

target_link_libraries(lattice_provider
    PUBLIC
        lattice::Object
        lattice::IFactory
)

add_library(lattice_toolchain)
add_library(lattice::Toolchain ALIAS lattice_toolchain)

target_sources(lattice_toolchain
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/Object/Toolchain.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Object/Toolchain.cppm"
)

target_link_libraries(lattice_toolchain
    PUBLIC
        lattice::Object
        lattice::Object::Parsable
        lattice::IFactory
        lattice::Tooling::Compiler
        lattice::Registry
        yaml-cpp::yaml-cpp
)

add_subdirectory(Properties)
