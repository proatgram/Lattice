target_sources(lattice_object
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src/Object/Properties/IProperty.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${PROJECT_SOURCE_DIR}/modules"
        FILES
        "${PROJECT_SOURCE_DIR}/modules/Object/Properties/IProperty.cppm"
)
