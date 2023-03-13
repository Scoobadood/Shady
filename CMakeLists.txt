cmake_minimum_required(VERSION 3.22)
project(image_toys)

#set(CMAKE_OSX_ARCHITECTURES "arm64")
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall")

list(APPEND CMAKE_PREFIX_PATH $CMAKE "${CMAKE_SOURCE_DIR}/cmake")

# Write targets to bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)


include(FetchContent)

# Gtest
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        main
)
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
#set(BUILD_GTEST ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Spdlog
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.x
)
FetchContent_MakeAvailable(spdlog)
include_directories(${spdlog_SOURCE_DIR}/include)


add_subdirectory(common)
add_subdirectory(xform-graph)
add_subdirectory(iv)
#add_subdirectory(image_viewer)
