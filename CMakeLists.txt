cmake_minimum_required(VERSION 3.30)
project(UmbraContainer LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)
foreach(TYPE RUNTIME LIBRARY ARCHIVE)
    set(CMAKE_${TYPE}_OUTPUT_DIRECTORY ${BIN_DIR})
    foreach(cfg Debug Release RelWithDebInfo MinSizeRel)
        set(CMAKE_${TYPE}_OUTPUT_DIRECTORY_${cfg} ${BIN_DIR}/${cfg})
    endforeach()
endforeach()

if (APPLE)
    set(CMAKE_BUILD_RPATH "@loader_path")
    set(CMAKE_INSTALL_RPATH "@loader_path")
elseif(UNIX)
    set(CMAKE_BUILD_RPATH "$ORIGIN")
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
endif()

add_subdirectory(engine)
add_subdirectory(cli)