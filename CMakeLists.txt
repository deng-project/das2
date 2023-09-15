# das2: Improved DENG asset manager library
# licence: Apache, see LICENCE file
# file: CMakeLists.txt - Main CMake configuration file
# author: Karl-Mihkel Ott

cmake_minimum_required(VERSION 3.10)
project(das2 LANGUAGES CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

option(DAS2_BUILD_DEMOS "Build all demo applications along with main das2 library" OFF)
option(DAS2_BUILD_STATIC "Build static das2 library instead of dynamic one" OFF)
option(DAS2_BUILD_DASTOOL "Build dastool" OFF)

# Converter options
option(DAS2_WAVEFRONT_OBJ "Build conversion support from and to Wavefront Obj format" ON)
option(DAS2_COLLADA "Build conversion support from and to Collada (.dae) format" ON)
option(DAS2_GLTF "Build conversion support from and to GLTF format" ON)
option(DAS2_STL "Build conversion support from and to STL format" ON)
option(DAS2_STANFORD_PLY "Build conversion support from and to PLY format" ON)
option(DAS2_STANFORD_X3D "Build conversion support from and to X3D format" ON)
option(DAS2_FILMBOX_PROPRIETARY "Build conversion support from and to fbx (built on top of proprietary SDK)" ON)
option(DAS2_FILMBOX_RSE "Build conversion support from and to fbx (reverse engineered implementation)" ON)

include(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/cvar)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/libdas2.cmake)

if (DAS2_BUILD_DASTOOL)
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/dastool.cmake)
endif()

if (DAS2_BUILD_DEMOS)
    message(STATUS "Adding demo build configurations")
    if (DAS2_WAVEFRONT_OBJ)
        include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/Demos/ParseWavefrontObj.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/Demos/ConvertWavefrontObj.cmake)
    endif()
endif()
