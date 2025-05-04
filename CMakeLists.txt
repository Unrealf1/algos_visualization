cmake_minimum_required(VERSION 3.13)

project(algos-vis)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:preprocessor")
endif()

if (WEB_BUILD)
    include("${WEB_BUILD}/cmake/Modules/TestBigEndian.cmake")
    add_compile_definitions("__EMSCRIPTEN__")
    set(COMMON_WEB_BUILD_FLAGS "-msimd128 --use-port=sdl2 --use-port=sdl2_image:formats=bmp,png,xpm,jpg")
    # TODO: 10 threads is an arbitrary number, better consider hardware in the future
    #set(WEB_LINKER_FLAGS "-pthread -sASSERTIONS -sALLOW_MEMORY_GROWTH -sTOTAL_STACK=5242880 -sFULL_ES2 -sPTHREAD_POOL_SIZE=10")
    set(WEB_LINKER_FLAGS "-sASSERTIONS -sALLOW_MEMORY_GROWTH -sTOTAL_STACK=5242880 -sFULL_ES2")

    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_WEB_BUILD_FLAGS}")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_WEB_BUILD_FLAGS}")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${WEB_LINKER_FLAGS}")
    SET(ALLEGRO_SDL_OPTION "ON")

    find_package(SDL2 REQUIRED)
    include("${SDL2_DIR}/sdl2-config.cmake")
    file(GLOB HTML "${CMAKE_CURRENT_SOURCE_DIR}/html/*")
    file(COPY ${HTML} DESTINATION ${CMAKE_BINARY_DIR}/bin/)
    set(SDL2_INCLUDE_DIR ${SDL2_SOURCE_DIR}/include)
    set(SDL2_LIBRARY ${SDL2_LIBRARIES})
    # this is a hack to make sure that libs think that pthread is there
    # there's probably a way to make them legitemately verify that
    # set(CMAKE_HAVE_LIBC_PTHREAD 1)

else()
    SET(ALLEGRO_SDL_OPTION "OFF")
endif()

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
CPMAddPackage(
  NAME allegro5
  VERSION 5.2.9.1
  GITHUB_REPOSITORY liballeg/allegro5
  GIT_TAG 5.2.9.1
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
  "WANT_NATIVE_DIALOG OFF"
  "WANT_AUDIO OFF"
  "ALLEGRO_SDL ${ALLEGRO_SDL_OPTION}"
  "WANT_ALLOW_SSE ON"
  "SHARED OFF"
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

file(GLOB_RECURSE COMBOAPP_SOURCES . source/combo_app/*.[ch]pp)
add_executable(combo_app ${COMBOAPP_SOURCES})
target_include_directories(combo_app PUBLIC source/combo_app)
target_link_libraries(combo_app PUBLIC commonlib)

install(TARGETS combo_app DESTINATION package)
