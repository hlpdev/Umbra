cmake_minimum_required(VERSION 3.30)
project(Umbra LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bundle/$<CONFIG>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}" CACHE PATH "" FORCE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR}" CACHE PATH "" FORCE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR}" CACHE PATH "" FORCE)

function(bundle_runtime_dlls tgt)
    # ---- Runtime DLLs (safe everywhere; empty list when not applicable) ----
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
            VERBATIM
    )

    # ---- PDB copy (only where it makes sense) ----
    if(MSVC)
        get_target_property(_type ${tgt} TYPE)
        if(_type STREQUAL "EXECUTABLE" OR _type STREQUAL "SHARED_LIBRARY" OR _type STREQUAL "MODULE_LIBRARY")
            add_custom_command(TARGET ${tgt} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E echo "Copying PDB for ${tgt} (if any)"
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "$<TARGET_PDB_FILE:${tgt}>"
                    "$<TARGET_FILE_DIR:${tgt}>"
                    VERBATIM
            )
        endif()
    endif()
endfunction()

add_subdirectory(engine)
add_subdirectory(cli)
add_subdirectory(runner)