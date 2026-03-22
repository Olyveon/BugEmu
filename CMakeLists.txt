cmake_minimum_required(VERSION 4.1)
project(BugEmu)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

function(include_dependency libName gitURL gitTag)
    FetchContent_Declare(${libName}
            GIT_REPOSITORY      ${gitURL}
            GIT_TAG             ${gitTag}
            GIT_SHALLOW         TRUE
            GIT_PROGRESS        TRUE
    )
    FetchContent_MakeAvailable(${libName})
endfunction()

# --- SDL3 ---
find_package(SDL3 QUIET)
if (NOT SDL3_FOUND)
    include_dependency(SDL3 https://github.com/libsdl-org/SDL.git release-3.4.2)
endif ()

# --- nfd ---
find_package(nfd)
if(NOT nfd_FOUND)
    include_dependency(nfd https://github.com/btzy/nativefiledialog-extended.git v1.3.0)
endif()

# --- Dear ImGui ---
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v1.91.9b
        GIT_SHALLOW    TRUE
        GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(imgui)

# ImGui has no CMakeLists.txt, so we build it as an INTERFACE/STATIC lib manually
add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp

        # SDL3 + SDL_Renderer backend
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(imgui PUBLIC SDL3::SDL3)

# --- BugEmu executable ---
add_executable(BugEmu
        main.cpp
        bugCpu.cpp
        bugCpu.h
)

target_compile_definitions(BugEmu PRIVATE SDL_MAIN_USE_CALLBACKS)

target_link_libraries(BugEmu PRIVATE
        SDL3::SDL3
        imgui
        nfd
)

if (WIN32)
    add_custom_command(
            TARGET BugEmu POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:BugEmu>"
            VERBATIM
    )
endif ()