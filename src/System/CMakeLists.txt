add_library(lattice_system SHARED)
add_library(lattice::System ALIAS lattice_system)

target_sources(lattice_system
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src/System/Command.cpp"
        "${PROJECT_SOURCE_DIR}/src/System/Process.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${PROJECT_SOURCE_DIR}/modules"
        FILES
            "${PROJECT_SOURCE_DIR}/modules/System/Command.cppm"
            "${PROJECT_SOURCE_DIR}/modules/System/Process.cppm"
)

target_link_libraries(lattice_system
    PRIVATE
        atomic
)
