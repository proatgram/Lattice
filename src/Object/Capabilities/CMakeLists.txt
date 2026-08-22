add_library(lattice_object_capabilities_capability)
add_library(lattice::Object::Capabilities::ICapability ALIAS lattice_object_capabilities_capability)

target_sources(lattice_object_capabilities_capability
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/Capabilities/ICapability.cppm"
)

add_library(lattice_object_capabilities_buildable)
add_library(lattice::Object::Capabilities::Buildable ALIAS lattice_object_capabilities_buildable)

target_sources(lattice_object_capabilities_buildable
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Object/Capabilities/Buildable.cppm"
)

target_link_libraries(lattice_object_capabilities_buildable
    PUBLIC
        lattice::Object::Capabilities::ICapability
)


add_library(lattice_object_capabilities_languageidentifiable)
add_library(lattice::Object::Capabilities::LanguageIdentifiable ALIAS lattice_object_capabilities_languageidentifiable)

target_sources(lattice_object_capabilities_languageidentifiable
    PUBLIC
    "${CMAKE_SOURCE_DIR}/src/Object/Capabilities/LanguageIdentifiable.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/Capabilities/LanguageIdentifiable.cppm"
)

target_link_libraries(lattice_object_capabilities_languageidentifiable
    PUBLIC
        lattice::Object::Capabilities::ICapability
)

add_library(lattice_object_capabilities_linkable)
add_library(lattice::Object::Capabilities::Linkable ALIAS lattice_object_capabilities_linkable)

target_sources(lattice_object_capabilities_linkable
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/Capabilities/Linkable.cppm"
)

target_link_libraries(lattice_object_capabilities_linkable
    PUBLIC
        lattice::Object
        lattice::Object::Capabilities::ICapability
)

add_library(lattice_object_capabilities_artifactprovider)
add_library(lattice::Object::Capabilities::ArtifactProvider ALIAS lattice_object_capabilities_artifactprovider)

target_sources(lattice_object_capabilities_artifactprovider
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Object/Capabilities/ArtifactProvider.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/Capabilities/ArtifactProvider.cppm"
)

target_link_libraries(lattice_object_capabilities_artifactprovider
    PUBLIC
        lattice::Artifact
        lattice::Object::Capabilities::ICapability
)

add_library(lattice_object_capabilities_hastoolchain)
add_library(lattice::Object::Capabilities::HasToolchain ALIAS lattice_object_capabilities_hastoolchain)

target_sources(lattice_object_capabilities_hastoolchain
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Object/Capabilities/HasToolchain.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/Capabilities/HasToolchain.cppm"
)

target_link_libraries(lattice_object_capabilities_hastoolchain
    PUBLIC
        lattice::Registry
        lattice::Object::IToolchain
        lattice::Object::Capabilities::ICapability
)

add_library(lattice_object_capabilities_hasdependencies)
add_library(lattice::Object::Capabilities::HasDependencies ALIAS lattice_object_capabilities_hasdependencies)

target_sources(lattice_object_capabilities_hasdependencies
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Object/Capabilities/HasDependencies.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/Capabilities/HasDependencies.cppm"
)

target_link_libraries(lattice_object_capabilities_hasdependencies
    PUBLIC
        lattice::Object::Capabilities::ProjectIdentifiable
        lattice::Object::Capabilities::ICapability

        lattice::Object::Resolver
)

add_library(lattice_object_capabilities_projectidentifiable)
add_library(lattice::Object::Capabilities::ProjectIdentifiable ALIAS lattice_object_capabilities_projectidentifiable)

target_sources(lattice_object_capabilities_projectidentifiable
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Object/Capabilities/ProjectIdentifiable.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/Capabilities/ProjectIdentifiable.cppm"
)

target_link_libraries(lattice_object_capabilities_projectidentifiable
    PUBLIC
        lattice::Object::Project
        lattice::Object::Capabilities::ICapability
)

add_library(lattice_object_capabilities_hasproperties)
add_library(lattice::Object::Capabilities::HasProperties ALIAS lattice_object_capabilities_hasproperties)

target_sources(lattice_object_capabilities_hasproperties
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/Capabilities/HasProperties.cppm"
)

target_link_libraries(lattice_object_capabilities_hasproperties
    PUBLIC
        lattice::Object::Capabilities::ICapability
        lattice::Object::Properties::IProperty
)

