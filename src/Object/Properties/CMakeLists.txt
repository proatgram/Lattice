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
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/Properties/LanguageIdentifiable.cppm"
)

add_library(lattice_object_properties_linkable)
add_library(lattice::Object::Properties::Linkable ALIAS lattice_object_properties_linkable)

target_sources(lattice_object_properties_linkable
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Object/Properties/Linkable.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/Properties/Linkable.cppm"
)

target_link_libraries(lattice_object_properties_linkable
    PUBLIC
        lattice::Artifact
)
