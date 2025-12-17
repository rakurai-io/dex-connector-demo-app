include(ExternalProject)

if(NOT TARGET rapidjson_project)
ExternalProject_Add(rapidjson_project 
    URL https://github.com/Tencent/rapidjson/archive/v1.1.0.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/rapidjson
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
endif()

ExternalProject_Get_Property(rapidjson_project SOURCE_DIR)
set(rapidjson_INCLUDE_DIR ${SOURCE_DIR}/include)