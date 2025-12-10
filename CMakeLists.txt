cmake_minimum_required(VERSION 3.17)
project(aoc_2025)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/or-tools_x86_64_Ubuntu-24.04_cpp_v9.12.4544")
find_package(ortools CONFIG REQUIRED)

file(GLOB_RECURSE DAY_SOURCES CONFIGURE_DEPENDS "src/days/*.cpp")

add_executable(aoc src/main.cpp ${DAY_SOURCES})
target_link_libraries(aoc PRIVATE tomlplusplus::tomlplusplus)

target_include_directories(aoc PRIVATE 
    src
    ${tomlplusplus_SOURCE_DIR}/include
)
target_link_libraries(aoc PRIVATE ortools::ortools)