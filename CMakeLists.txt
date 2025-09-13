cmake_minimum_required(VERSION 3.30)
project(Umbra LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bundle/$<CONFIG>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}" CACHE PATH "" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}" CACHE PATH "" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}" CACHE PATH "" FORCE)

if (WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "Detected MinGW on Windows, but triplet is x64-windows (MSVC). Use MSVC or switch triplet.")
endif()

function(bundle_runtime_dlls tgt)
    set(_dll_script "${CMAKE_CURRENT_BINARY_DIR}/copy_${tgt}_dlls.cmake")
    file(GENERATE OUTPUT "${_dll_script}" CONTENT
            "set(dlls \"$<TARGET_RUNTIME_DLLS:${tgt}>\")
set(dest \"$<TARGET_FILE_DIR:${tgt}>\")
if(dlls)
  foreach(f IN LISTS dlls)
    if(EXISTS \"${f}\")
      execute_process(COMMAND \"${CMAKE_COMMAND}\" -E copy_if_different \"${f}\" \"${dest}\")
    endif()
  endforeach()
endif()
")
    add_custom_command(TARGET ${tgt} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Bundling runtime DLLs for ${tgt}"
            COMMAND ${CMAKE_COMMAND} -P "${_dll_script}"
            VERBATIM)

    # --- PDB copy (MSVC only) ---
    if(MSVC)
        set(_pdb_script "${CMAKE_CURRENT_BINARY_DIR}/copy_${tgt}_pdb.cmake")
        file(GENERATE OUTPUT "${_pdb_script}" CONTENT
                "set(src \"$<TARGET_PDB_FILE:${tgt}>\")
set(dst \"$<TARGET_FILE_DIR:${tgt}>\")
if(NOT \"${src}\" STREQUAL \"\" AND EXISTS \"${src}\")
  # Write to a concrete file path rather than a directory to dodge quoting bugs
  file(COPY_FILE \"${src}\" \"${dst}/$<TARGET_FILE_BASE_NAME:${tgt}>.pdb\" ONLY_IF_DIFFERENT)
endif()
")
        add_custom_command(TARGET ${tgt} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E echo "Copying PDB for ${tgt} (if any)"
                COMMAND ${CMAKE_COMMAND} -P "${_pdb_script}"
                VERBATIM)
    endif()
endfunction()

add_subdirectory(engine)
add_subdirectory(cli)
add_subdirectory(runner)