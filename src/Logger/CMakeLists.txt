add_library(lattice_logger SHARED)
add_library(lattice::Logger ALIAS lattice_logger)

target_sources(lattice_logger
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src/Logger/ILogger.cpp"
        "${PROJECT_SOURCE_DIR}/src/Logger/EscapeSequences.cpp"
        "${PROJECT_SOURCE_DIR}/src/Logger/TextLogger.cpp"
        "${PROJECT_SOURCE_DIR}/src/Logger/ProgressLogger.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${PROJECT_SOURCE_DIR}/modules"
        FILES
            "${PROJECT_SOURCE_DIR}/modules/Logger/ILogger.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Logger/EscapeSequences.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Logger/TextLogger.cppm"
            "${PROJECT_SOURCE_DIR}/modules/Logger/ProgressLogger.cppm"
)

target_link_libraries(lattice_logger
    PRIVATE
        lattice::Registry
)
