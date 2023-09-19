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

foreach(SRC_DIR algos maze util visual)
    file(GLOB_RECURSE COMMON_SOURCES_${SRC_DIR} . source/${SRC_DIR}/*.[ch]pp)
    set(COMMON_SOURCES ${COMMON_SOURCES} ${COMMON_SOURCES_${SRC_DIR}})
endforeach()

add_library(commonlib STATIC ${COMMON_SOURCES})
target_include_directories(commonlib PUBLIC source external)
target_link_libraries(commonlib PUBLIC project_options project_warnings)

CPMAddPackage("gh:gabime/spdlog@1.9.2")
CPMAddPackage("gh:liballeg/allegro5@5.2.8.0#5.2.8.0")
CPMAddPackage("gh:nlohmann/json@3.10.5")
CPMAddPackage("gh:Neargye/magic_enum@0.8.2")

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# TODO: can I solve this better?
get_target_property(allegro_includes allegro INCLUDE_DIRECTORIES)
target_include_directories(commonlib SYSTEM PUBLIC ${allegro_includes})

install(TARGETS RUNTIME DESTINATION package)
foreach(ADDON font image color primitives)
    target_include_directories("allegro_${ADDON}" INTERFACE
        "${allegro5_SOURCE_DIR}/addons/${ADDON}"
    )
endforeach()
install(TARGETS allegro allegro_font allegro_primitives
    RUNTIME DESTINATION package)

target_link_libraries(commonlib PUBLIC spdlog allegro allegro_primitives allegro_font nlohmann_json::nlohmann_json magic_enum) 

set(CMAKE_INSTALL_PREFIX .)
list(APPEND CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX})

file(GLOB_RECURSE ALGVIS_SOURCES . source/algvis/*.[ch]pp)
add_executable(algvis ${ALGVIS_SOURCES})
target_link_libraries(algvis PUBLIC commonlib)

install(TARGETS algvis DESTINATION package)
install(FILES config.json DESTINATION package)
