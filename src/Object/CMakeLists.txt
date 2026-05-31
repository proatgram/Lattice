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

add_library(lattice_object_toolchain)
add_library(lattice::Object::Toolchain ALIAS lattice_object_toolchain)

target_sources(lattice_object_toolchain
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/Object/Toolchain.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Object/Toolchain.cppm"
)

target_link_libraries(lattice_object_toolchain
    PUBLIC
        lattice::Object
        lattice::IFactory
        lattice::Tooling::Compiler
        lattice::Registry
        yaml-cpp::yaml-cpp
)

add_library(lattice_object_library)
add_library(lattice::Object::Library ALIAS lattice_object_library)

target_sources(lattice_object_library
    PRIVATE
    "${CMAKE_SOURCE_DIR}/src/Object/ILibrary.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/ILibrary.cppm"
)

target_link_libraries(lattice_object_library
    PUBLIC
        lattice::Object
        lattice::Object::Properties::Buildable
        lattice::Object::Properties::LanguageIdentifiable

        lattice::IFactory
        lattice::Registry
        lattice::Object::Toolchain
        lattice::Tooling::Compiler
        lattice::System::Process

        yaml-cpp::yaml-cpp
)

add_subdirectory(Properties)
