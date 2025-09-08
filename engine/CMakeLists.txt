cmake_minimum_required(VERSION 3.30)
project(engine LANGUAGES CXX VERSION ${CMAKE_PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS LIST_DIRECTORIES false
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.[ch]pp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_library(engine SHARED ${ENGINE_SRC})

#[[ DEPENDENCIES ]]

find_package(unofficial-sodium CONFIG REQUIRED)
target_link_libraries(engine PRIVATE unofficial-sodium::sodium)

find_path(SOL2_INCLUDE_DIRS "sol/abort.hpp")
target_include_directories(engine PRIVATE ${SOL2_INCLUDE_DIRS})

#[[ END DEPENDENCIES ]]

target_sources(engine PRIVATE ${ENGINE_SRC} ${ENGINE_HDR})
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(engine PRIVATE UMBRA_BUILD)

set_target_properties(engine PROPERTIES OUTPUT_NAME "umbra")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/magics.t.hpp ${CMAKE_CURRENT_SOURCE_DIR}/include/Umbra/magics.hpp @ONLY)

bundle_runtime_dlls(engine)