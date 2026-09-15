add_library(lattice_logger SHARED)
add_library(lattice::Logger ALIAS lattice_logger)

target_sources(lattice_logger
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src/Logger/Logger.cpp"
        "${PROJECT_SOURCE_DIR}/src/Logger/EscapeSequences.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${PROJECT_SOURCE_DIR}/modules"
        FILES
            "${PROJECT_SOURCE_DIR}/modules/Logger/Logger.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Logger/EscapeSequences.cppm"
)
