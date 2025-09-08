cmake_minimum_required(VERSION 3.30)
project(engine LANGUAGES CXX VERSION ${CMAKE_PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(engine SHARED src/umbra.cpp)

set_target_properties(engine PROPERTIES OUTPUT_NAME "Umbra")

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(engine PRIVATE UMBRA_BUILD)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/magics.t.hpp ${CMAKE_CURRENT_SOURCE_DIR}/include/Umbra/magics.hpp @ONLY)