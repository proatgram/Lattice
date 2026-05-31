add_library(lattice_artifact)
add_library(lattice::Artifact ALIAS lattice_artifact)

target_sources(lattice_artifact
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
            "${CMAKE_SOURCE_DIR}/modules/Artifacts/Artifact.cppm"
)

add_library(lattice_artifact_compiledobject)
add_library(lattice::Artifact::CompiledObject ALIAS lattice_artifact_compiledobject)

target_sources(lattice_artifact_compiledobject
    PRIVATE
    "${CMAKE_SOURCE_DIR}/src/Artifacts/CompiledObject.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Artifacts/CompiledObject.cppm"
)


target_link_libraries(lattice_artifact_compiledobject
    PUBLIC
        lattice::Artifact
        lattice::Object
)
