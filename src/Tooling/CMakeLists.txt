add_library(lattice_tooling SHARED)
add_library(lattice::Tooling ALIAS lattice_tooling)

target_sources(lattice_tooling
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src/Tooling/ICompiler.cpp"
        "${PROJECT_SOURCE_DIR}/src/Tooling/ILinker.cpp"
        "${PROJECT_SOURCE_DIR}/src/Tooling/Configuration.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${PROJECT_SOURCE_DIR}/modules"
        FILES
            "${PROJECT_SOURCE_DIR}/modules/Tooling/ICompiler.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Tooling/ILinker.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Tooling/Configuration.cppm"
)

target_link_libraries(lattice_tooling
    PUBLIC
        lattice::Object
        lattice::IFactory
        lattice::System
)
