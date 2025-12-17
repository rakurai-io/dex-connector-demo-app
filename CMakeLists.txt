cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(rakurai-dex-connector-demo)

# Try to find the library using pkg-config first, fallback to direct search
find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(rakurai-dex-connector-library rakurai-dex-connector)
endif()

# If pkg-config didn't find it, search directly
if(NOT rakurai-dex-connector-library_FOUND)
    message(STATUS "pkg-config not found or package not available, searching for library directly...")
    
    # Find the library file
    find_library(rakurai-dex-connector-library_LIBRARIES
        NAMES rakurai-dex-connector
        PATHS /usr/lib /usr/local/lib /lib
        NO_DEFAULT_PATH
    )
    
    # Find header files (check common locations)
    find_path(rakurai-dex-connector-library_INCLUDE_DIRS
        NAMES dex_handler.h dex_listener.h
        PATHS /usr/include /usr/local/include
        PATH_SUFFIXES rakurai-dex-connector dex-connector rakurai
        NO_DEFAULT_PATH
    )
    
    # If not found with suffixes, try /usr/include directly
    if(NOT rakurai-dex-connector-library_INCLUDE_DIRS)
        if(EXISTS "/usr/include/dex_handler.h" OR EXISTS "/usr/include/dex_listener.h")
            set(rakurai-dex-connector-library_INCLUDE_DIRS "/usr/include")
        endif()
    endif()
    
    if(rakurai-dex-connector-library_LIBRARIES AND rakurai-dex-connector-library_INCLUDE_DIRS)
        set(rakurai-dex-connector-library_FOUND TRUE)
        message(STATUS "Found rakurai-dex-connector library: ${rakurai-dex-connector-library_LIBRARIES}")
        message(STATUS "Found rakurai-dex-connector headers: ${rakurai-dex-connector-library_INCLUDE_DIRS}")
    else()
        message(FATAL_ERROR "rakurai-dex-connector library not found. Library: ${rakurai-dex-connector-library_LIBRARIES}, Headers: ${rakurai-dex-connector-library_INCLUDE_DIRS}")
    endif()
endif()

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

message("Build type: ${CMAKE_BUILD_TYPE}")

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions(-DFLAGS="RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
    message("-- Building with Flags: ${CMAKE_CXX_FLAGS_RELEASE}")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DFLAGS="DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
    message("-- Building with Flags: ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h )

add_executable(${PROJECT_NAME} ${SOURCES})

# Linking the rakurai-dex-connector library
target_link_libraries(${PROJECT_NAME} ${rakurai-dex-connector-library_LIBRARIES})
include_directories(${PROJECT_NAME} PUBLIC ${rakurai-dex-connector-library_INCLUDE_DIRS})

# Linking spdlog library. It is used for logging in the demo applicaiton
target_link_libraries(${PROJECT_NAME} -lspdlog)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/cmake_modules)

# RapidJson This package is used to parse json files/objects (config.json) in this demo application.
find_package(rapidjson REQUIRED)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${rapidjson_INCLUDE_DIR})
add_dependencies(${PROJECT_NAME} rapidjson_project)

# Logging The logging level with which the demo application should be compiled. 
if(NOT DEFINED LOG_LEVEL)
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=0)
    message("-- Building of LOG_LEVEL=TRACE")
elseif(LOG_LEVEL STREQUAL "TRACE")
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=0)
    message("-- Building of LOG_LEVEL=TRACE")
elseif(LOG_LEVEL STREQUAL "DEBUG")
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=1)
    message("-- Building of LOG_LEVEL=DEBUG")
elseif(LOG_LEVEL STREQUAL "INFO")
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=2)
    message("-- Building of LOG_LEVEL=INFO")
elseif(LOG_LEVEL STREQUAL "WARN")
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=3)
    message("-- Building of LOG_LEVEL=WARN")
elseif(LOG_LEVEL STREQUAL "ERROR")
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=4)
    message("-- Building of LOG_LEVEL=ERROR")
elseif(LOG_LEVEL STREQUAL "CRITICAL")
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=5)
    message("-- Building of LOG_LEVEL=CRITICAL")
elseif(LOG_LEVEL STREQUAL "OFF")
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=6)
    message("-- Building of LOG_LEVEL=OFF")
else()
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=0)
    message("-- Building of LOG_LEVEL=TRACE")
endif()