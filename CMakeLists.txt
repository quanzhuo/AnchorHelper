cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0091 NEW)
project(AnchorHelper
        VERSION 0.1
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD	11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD	99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_subdirectory(dnssd)
if(MSVC)
    add_subdirectory(pthread)
endif()
add_subdirectory(helper)
