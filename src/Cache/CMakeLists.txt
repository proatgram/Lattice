add_library(lattice_cache_serialization SHARED)
add_library(lattice::Cache::Serialization ALIAS lattice_cache_serialization)

target_sources(lattice_cache_serialization
    PRIVATE
        "${PROJECT_SOURCE_DIR}/src/Cache/Serialization.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${PROJECT_SOURCE_DIR}/modules"
        FILES
            "${PROJECT_SOURCE_DIR}/modules/Cache/Serialization.cppm"
)

add_library(lattice_cache_manager SHARED)
add_library(lattice::Cache::Manager ALIAS lattice_cache_manager)

target_sources(lattice_cache_manager
    PRIVATE
    "${PROJECT_SOURCE_DIR}/src/Cache/CacheManager.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${PROJECT_SOURCE_DIR}/modules"
        FILES
        "${PROJECT_SOURCE_DIR}/modules/Cache/CacheManager.cppm"
)

target_link_libraries(lattice_cache_manager
    PRIVATE
        lattice::Cache::Serialization
)
