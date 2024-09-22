cmake_minimum_required(VERSION 3.16)
project(ParaAlgFramework)

# default to Release building
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Defaulting CMAKE_BUILD_TYPE to Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type")
endif()

# set default for MARCH
if (NOT MARCH)
    set(MARCH native)
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSTIONS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSTIONS ON)

# setup modern c++ flags
string(REPLACE "-O2" "-O3" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=${MARCH} -pedantic -Wall -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=${MARCH} -pedantic -Wall -Wextra")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/scripts/")

find_package(TBB)
find_package(OpenMP)

add_executable(ParaAlgFramework main.cpp)

if (OpenMP_CXX_FOUND)
    add_definitions(-DHAS_OMP)
    target_link_libraries(ParaAlgFramework PUBLIC OpenMP::OpenMP_CXX)
endif ()

if (TBB_FOUND)
    add_definitions(-DHAS_TBB)
    target_link_libraries(ParaAlgFramework PUBLIC tbb)

    if (TBB_INTERFACE_VERSION LESS 11004)
        message("Using TBB preview features")
        add_definitions(-DTBB_PREVIEW_GLOBAL_CONTROL)
    endif ()
endif ()
