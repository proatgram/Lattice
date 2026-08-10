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

add_library(lattice_object_project)
add_library(lattice::Object::Project ALIAS lattice_object_project)

target_sources(lattice_object_project
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/Object/Project.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Object/Project.cppm"
)

target_link_libraries(lattice_object_project
    PUBLIC
        lattice::Object
        lattice::Object::Properties::Buildable
        lattice::IFactory
    PRIVATE
        yaml-cpp::yaml-cpp
)

add_library(lattice_object_provider)
add_library(lattice::Object::Provider ALIAS lattice_object_provider)

target_sources(lattice_object_provider
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/Object/Provider.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Object/Provider.cppm"
)

target_link_libraries(lattice_object_provider
    PUBLIC
        lattice::Object
        lattice::IFactory
)

add_library(lattice_object_itoolchain)
add_library(lattice::Object::IToolchain ALIAS lattice_object_itoolchain)

target_sources(lattice_object_itoolchain
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/Object/IToolchain.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Object/IToolchain.cppm"
)

target_link_libraries(lattice_object_itoolchain
    PUBLIC
        lattice::Object
        lattice::IFactory
        lattice::Object::Properties::LanguageIdentifiable
        lattice::Tooling::ICompiler
        lattice::Registry
        yaml-cpp::yaml-cpp
)

add_library(lattice_object_ilibrary)
add_library(lattice::Object::ILibrary ALIAS lattice_object_ilibrary)

target_sources(lattice_object_ilibrary
    PRIVATE
    "${CMAKE_SOURCE_DIR}/src/Object/ILibrary.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/ILibrary.cppm"
)

target_link_libraries(lattice_object_ilibrary
    PUBLIC
        lattice::Object
        lattice::Object::Project
        lattice::Object::Properties::LanguageIdentifiable
        lattice::Object::Properties::HasToolchain

        lattice::IFactory
        lattice::Registry

        yaml-cpp::yaml-cpp
)

add_subdirectory(Properties)
