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

add_library(lattice_object_properties_parsable)
add_library(lattice::Object::Properties::Parsable ALIAS lattice_object_properties_parsable)

target_sources(lattice_object_properties_parsable
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Object/Properties/Parsable.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Object/Properties/Parsable.cppm"
)
