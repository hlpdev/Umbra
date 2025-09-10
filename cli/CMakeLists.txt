cmake_minimum_required(VERSION 3.30)
project(cli LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE CLI_SRC CONFIGURE_DEPENDS LIST_DIRECTORIES false
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.[ch]pp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_executable(cli ${CLI_SRC})

#[[ DEPENDENCIES ]]

find_package(unofficial-sodium CONFIG REQUIRED)
target_link_libraries(cli PRIVATE unofficial-sodium::sodium)

#[[ END DEPENDENCIES ]]

set_target_properties(cli PROPERTIES OUTPUT_NAME "umbra")

target_include_directories(cli PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cli PRIVATE engine)

add_dependencies(cli engine)

bundle_runtime_dlls(cli)