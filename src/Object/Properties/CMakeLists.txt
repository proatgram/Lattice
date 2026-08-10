add_library(lattice_object_properties_buildable)
add_library(lattice::Object::Properties::Buildable ALIAS lattice_object_properties_buildable)

target_sources(lattice_object_properties_buildable
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Object/Properties/Buildable.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Object/Properties/Buildable.cppm"
)

add_library(lattice_object_properties_languageidentifiable)
add_library(lattice::Object::Properties::LanguageIdentifiable ALIAS lattice_object_properties_languageidentifiable)

target_sources(lattice_object_properties_languageidentifiable
    PUBLIC
    "${CMAKE_SOURCE_DIR}/src/Object/Properties/LanguageIdentifiable.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/Properties/LanguageIdentifiable.cppm"
)

add_library(lattice_object_properties_linkable)
add_library(lattice::Object::Properties::Linkable ALIAS lattice_object_properties_linkable)

target_sources(lattice_object_properties_linkable
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/Properties/Linkable.cppm"
)

target_link_libraries(lattice_object_properties_linkable
    PUBLIC
        lattice::Object
)

add_library(lattice_object_properties_artifactprovider)
add_library(lattice::Object::Properties::ArtifactProvider ALIAS lattice_object_properties_artifactprovider)

target_sources(lattice_object_properties_artifactprovider
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Object/Properties/ArtifactProvider.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/Properties/ArtifactProvider.cppm"
)

target_link_libraries(lattice_object_properties_artifactprovider
    PUBLIC
        lattice::Artifact
)

add_library(lattice_object_properties_hastoolchain)
add_library(lattice::Object::Properties::HasToolchain ALIAS lattice_object_properties_hastoolchain)

target_sources(lattice_object_properties_hastoolchain
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Object/Properties/HasToolchain.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/Properties/HasToolchain.cppm"
)

target_link_libraries(lattice_object_properties_hastoolchain
    PUBLIC
        lattice::Object::IToolchain
        lattice::Registry
)
