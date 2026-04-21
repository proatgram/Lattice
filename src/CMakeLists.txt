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
        lattice::Object
        lattice::Project
        yaml-cpp::yaml-cpp
)

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

add_library(lattice_object_buildable)
add_library(lattice::Object::Buildable ALIAS lattice_object_buildable)

target_sources(lattice_object_buildable
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Object/Buildable.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Object/Buildable.cppm"
)

add_library(lattice_object_parsable)
add_library(lattice::Object::Parsable ALIAS lattice_object_parsable)

target_sources(lattice_object_parsable
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src/Object/Parsable.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Object/Parsable.cppm"
)

add_library(lattice_project)
add_library(lattice::Project ALIAS lattice_project)

target_sources(lattice_project
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/Project.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Project.cppm"
)

target_link_libraries(lattice_project
    PUBLIC
        lattice::Object
        lattice::Object::Buildable
        lattice::Object::Parsable
    PRIVATE
        yaml-cpp::yaml-cpp
)
