add_library(lattice_object SHARED)
add_library(lattice::Object ALIAS lattice_object)

target_sources(lattice_object
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src/Object/Object.cpp"
        "${PROJECT_SOURCE_DIR}/src/Object/ILibrary.cpp"
        "${PROJECT_SOURCE_DIR}/src/Object/Provider.cpp"
        "${PROJECT_SOURCE_DIR}/src/Object/IToolchain.cpp"
        "${PROJECT_SOURCE_DIR}/src/Object/Project.cpp"
        "${PROJECT_SOURCE_DIR}/src/Object/IBinary.cpp"
        "${PROJECT_SOURCE_DIR}/src/Object/Resolver.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${PROJECT_SOURCE_DIR}/modules"
        FILES
            "${PROJECT_SOURCE_DIR}/modules/Object/Object.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Object/ILibrary.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Object/Provider.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Object/IBinary.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Object/Resolver.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Object/Project.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Object/IToolchain.cppm"
)

target_link_libraries(lattice_object
    PUBLIC
        lattice::IFactory
    PRIVATE
        lattice::Registry

        yaml-cpp::yaml-cpp
)

add_subdirectory(Capabilities)
add_subdirectory(Properties)
