add_library(lattice_system_command)
add_library(lattice::System::Command ALIAS lattice_system_command)

target_sources(lattice_system_command
    PRIVATE
    "${CMAKE_SOURCE_DIR}/src/System/Command.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/System/Command.cppm"
)

target_link_libraries(lattice_system_command
    PUBLIC
        lattice::IFactory
)

add_library(lattice_system_process)
add_library(lattice::System::Process ALIAS lattice_system_process)

target_sources(lattice_system_process
    PRIVATE
    "${CMAKE_SOURCE_DIR}/src/System/Process.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/System/Process.cppm"
)

target_link_libraries(lattice_system_process
    PUBLIC
        lattice::IFactory
        lattice::System::Command
        atomic
)

