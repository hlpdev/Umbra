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

find_package(zstd CONFIG REQUIRED)
target_link_libraries(engine PRIVATE zstd::libzstd)

find_package(tomlplusplus CONFIG REQUIRED)
target_link_libraries(engine PRIVATE tomlplusplus::tomlplusplus)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(engine PRIVATE fmt::fmt)

find_package(Lua REQUIRED)
target_include_directories(engine PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(engine PRIVATE ${LUA_LIBRARIES})

find_package(imgui CONFIG REQUIRED)
target_link_libraries(engine PRIVATE imgui::imgui)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(engine PRIVATE glfw)

find_package(lodepng CONFIG REQUIRED)
target_link_libraries(engine PRIVATE lodepng)

find_package(Stb REQUIRED)
target_include_directories(engine PRIVATE ${Stb_INCLUDE_DIR})

find_package(OGRE CONFIG REQUIRED)
target_link_libraries(engine PRIVATE OgreMain OgreBites Codec_STBI OgrePaging)

#[[ END DEPENDENCIES ]]

target_sources(engine PRIVATE ${ENGINE_SRC} ${ENGINE_HDR})
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(engine PRIVATE UMBRA_BUILD_DLL)

if (MSVC OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    target_compile_options(engine PRIVATE /bigobj)
endif()

set_target_properties(engine PROPERTIES OUTPUT_NAME "umbra")


# vcpkg manifest builds usually set these. Provide sane fallbacks for CLion.
if(NOT VCPKG_INSTALLED_DIR)
    # CLion puts it next to the build dir:
    set(VCPKG_INSTALLED_DIR "${CMAKE_BINARY_DIR}/vcpkg_installed")
endif()
if(NOT VCPKG_TARGET_TRIPLET)
    # Common defaults; override from cache if you use another triplet.
    if(WIN32)
        set(VCPKG_TARGET_TRIPLET "x64-windows")
    elseif(APPLE)
        set(VCPKG_TARGET_TRIPLET "x64-osx")
    else()
        set(VCPKG_TARGET_TRIPLET "x64-linux")
    endif()
endif()

set(OGRE_PLUGINS_DIR
        "$<IF:$<CONFIG:Debug>,
      ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/plugins/ogre,
      ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/plugins/ogre>")

add_custom_command(TARGET engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying OGRE plugins from: ${OGRE_PLUGINS_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${OGRE_PLUGINS_DIR}"
        "$<TARGET_FILE_DIR:engine>"
        VERBATIM)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/magics.t.hpp ${CMAKE_CURRENT_SOURCE_DIR}/include/Umbra/magics.hpp @ONLY)

bundle_runtime_dlls(engine)