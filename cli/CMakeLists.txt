cmake_minimum_required(VERSION 3.30)
project(cli LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cli src/entry.cpp)

set_target_properties(cli PROPERTIES OUTPUT_NAME "umbra")

target_include_directories(cli PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cli PRIVATE engine)

add_dependencies(cli engine)

bundle_runtime_dlls(cli)