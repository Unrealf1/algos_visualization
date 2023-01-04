cmake_minimum_required(VERSION 3.13)

project(algos-vis)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/CPM.cmake)

add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)

include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

file(GLOB_RECURSE SOURCES . source/*.[ch]pp)
add_executable(algvis ${SOURCES})
target_include_directories(algvis PUBLIC source external)
target_link_libraries(algvis PUBLIC project_options project_warnings)

CPMAddPackage("gh:gabime/spdlog@1.9.2")
CPMAddPackage("gh:liballeg/allegro5@5.2.8.0#5.2.8.0")
CPMAddPackage("gh:nlohmann/json@3.10.5")
CPMAddPackage("gh:Neargye/magic_enum@0.8.2")

# TODO: can I solve this better?
get_target_property(allegro_includes allegro INCLUDE_DIRECTORIES)
target_include_directories(algvis SYSTEM PUBLIC ${allegro_includes})

foreach(ADDON font image color primitives)
    target_include_directories("allegro_${ADDON}" INTERFACE
        "${allegro5_SOURCE_DIR}/addons/${ADDON}"
    )
endforeach()

target_link_libraries(algvis PUBLIC spdlog allegro allegro_primitives allegro_font nlohmann_json::nlohmann_json magic_enum) 

