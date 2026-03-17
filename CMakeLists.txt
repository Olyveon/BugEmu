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

# add SDL3 support
find_package(SDL3 QUIET)
if (NOT SDL_FOUND)
    include_dependency(SDL3 https://github.com/libsdl-org/SDL.git release-3.4.2)
endif ()
# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.


add_executable(BugEmu main.cpp
        bugCpu.cpp
        bugCpu.h)



target_compile_definitions(BugEmu PRIVATE SDL_MAIN_USE_CALLBACKS)

target_link_libraries(BugEmu PRIVATE SDL3::SDL3)

if (WIN32)
    add_custom_command(
            TARGET BugEmu POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:BugEmu>"
            VERBATIM
    )
endif ()