add_library(lattice_object_buildable)
add_library(lattice::Object::Buildable ALIAS lattice_object_buildable)

target_sources(lattice_object_buildable
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Object/Properties/Buildable.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Object/Properties/Buildable.cppm"
)

add_library(lattice_object_parsable)
add_library(lattice::Object::Parsable ALIAS lattice_object_parsable)

target_sources(lattice_object_parsable
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Object/Properties/Parsable.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Object/Properties/Parsable.cppm"
)
