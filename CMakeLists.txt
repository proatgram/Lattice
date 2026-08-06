cmake_minimum_required(VERSION 4.4.0)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "f35a9ac6-8463-4d38-8eec-5d6008153e7d")
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
