add_library(lattice_object_properties_iproperty)
add_library(lattice::Object::Properties::IProperty ALIAS lattice_object_properties_iproperty)

target_sources(lattice_object_properties_iproperty
    PRIVATE
        "${CMAKE_SOURCE_DIR}/src/Object/Properties/IProperty.cpp"
    PUBLIC FILE_SET CXX_MODULES
        BASE_DIRS "${CMAKE_SOURCE_DIR}/modules"
        FILES
        "${CMAKE_SOURCE_DIR}/modules/Object/Properties/IProperty.cppm"
)
