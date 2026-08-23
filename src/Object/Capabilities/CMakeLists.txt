target_sources(lattice_object
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src/Object/Capabilities/HasToolchain.cpp"
        "${PROJECT_SOURCE_DIR}/src/Object/Capabilities/HasDependencies.cpp"
        "${PROJECT_SOURCE_DIR}/src/Object/Capabilities/LanguageIdentifiable.cpp"
        "${PROJECT_SOURCE_DIR}/src/Object/Capabilities/ProjectIdentifiable.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${PROJECT_SOURCE_DIR}/modules"
        FILES
            "${PROJECT_SOURCE_DIR}/modules/Object/Capabilities/LanguageIdentifiable.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Object/Capabilities/ICapability.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Object/Capabilities/HasProperties.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Object/Capabilities/Buildable.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Object/Capabilities/HasDependencies.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Object/Capabilities/Linkable.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Object/Capabilities/ProjectIdentifiable.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Object/Capabilities/ArtifactProvider.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Object/Capabilities/HasToolchain.cppm"
)
