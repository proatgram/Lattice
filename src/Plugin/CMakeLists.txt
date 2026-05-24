add_library(lattice_ifactory)
add_library(lattice::IFactory ALIAS lattice_ifactory)

target_sources(lattice_ifactory
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Plugin/IFactory.cppm"
)
