cmake_minimum_required(VERSION 4.0.0)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES TRUE)

project(Lattice
    VERSION 0.1
    DESCRIPTION "Another C++ build system."
    HOMEPAGE_URL "https://github.com/proatgram/lattice"
    LANGUAGES CXX
)

find_package(argparse REQUIRED)
find_package(yaml-cpp REQUIRED)

add_subdirectory(src)
