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
CPMAddPackage(
  NAME allegro5
  VERSION 5.2.8.0
  GITHUB_REPOSITORY liballeg/allegro5
  GIT_TAG 5.2.8.0
  OPTIONS
  "WANT_PHYSFS OFF"
  "WANT_VIDEO OFF"
  "WANT_DOCS OFF"
  "WANT_DOCS_HTML OFF"
  "WANT_DOCS_MAN OFF"
  "WANT_DEMO OFF"
  "WANT_EXAMPLES OFF"
  "WANT_POPUP_EXAMPLES OFF"
  "WANT_TESTS OFF"
  "WANT_NATIVE_DIALOG"
)
CPMAddPackage("gh:nlohmann/json@3.10.5")
CPMAddPackage("gh:Neargye/magic_enum@0.8.2")
CPMAddPackage("gh:aiekick/ImGuiFileDialog@0.6.5")
target_compile_definitions(ImGuiFileDialog PRIVATE USE_STD_FILESYSTEM=1)

target_link_libraries(ImGuiFileDialog PUBLIC imgui)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# TODO: can I solve this better?
get_target_property(allegro_includes allegro INCLUDE_DIRECTORIES)
target_include_directories(commonlib SYSTEM PUBLIC ${allegro_includes})

install(TARGETS
    RUNTIME DESTINATION package
    LIBRARY DESTINATION package)
foreach(ADDON font image color primitives)
    target_include_directories("allegro_${ADDON}" INTERFACE
        "${allegro5_SOURCE_DIR}/addons/${ADDON}"
    )
    install(TARGETS "allegro_${ADDON}"
        RUNTIME DESTINATION package
        LIBRARY DESTINATION package)
endforeach()
install(TARGETS allegro
    RUNTIME DESTINATION package
    LIBRARY DESTINATION package)

file(GLOB SOURCES_IMGUI external/imgui/*.[ch]pp)
add_library(imgui STATIC ${SOURCES_IMGUI} external/imgui/backends/imgui_impl_allegro5.cpp)
target_include_directories(imgui PUBLIC external/imgui)
target_include_directories(imgui SYSTEM PUBLIC ${allegro_includes})
target_link_libraries(imgui PUBLIC allegro allegro_font allegro_image allegro_color allegro_primitives)

target_link_libraries(commonlib PUBLIC spdlog allegro allegro_primitives allegro_font nlohmann_json::nlohmann_json magic_enum imgui ImGuiFileDialog)

set(CMAKE_INSTALL_PREFIX .)
list(APPEND CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX})

file(GLOB_RECURSE ALGVIS_SOURCES . source/algvis/*.[ch]pp)
add_executable(algvis ${ALGVIS_SOURCES})
target_include_directories(algvis PUBLIC source/algvis)
target_link_libraries(algvis PUBLIC commonlib)

install(TARGETS algvis DESTINATION package)
install(FILES config.json DESTINATION package)

file(GLOB_RECURSE DESIGNER_SOURCES . source/designer/*.[ch]pp)
add_executable(designer ${DESIGNER_SOURCES})
target_include_directories(designer PUBLIC source/designer)
target_link_libraries(designer PUBLIC commonlib)

install(TARGETS designer DESTINATION package)
