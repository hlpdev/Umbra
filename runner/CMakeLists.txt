cmake_minimum_required(VERSION 3.30)
project(runner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE CLI_SRC CONFIGURE_DEPENDS LIST_DIRECTORIES false
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.[ch]pp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_executable(runner ${CLI_SRC})

#[[ DEPENDENCIES ]]

find_package(tomlplusplus CONFIG REQUIRED)
target_link_libraries(runner PRIVATE tomlplusplus::tomlplusplus)

#[[ END DEPENDENCIES ]]

set_target_properties(runner PROPERTIES OUTPUT_NAME "umbra-runner")

target_include_directories(runner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(runner PRIVATE engine)

add_dependencies(runner engine)

bundle_runtime_dlls(runner)