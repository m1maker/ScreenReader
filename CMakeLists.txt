# CMakeLists.txt : CMake project for Screen Reader, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.28)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(INCLUDES "Include")

# Enable Hot Reload for MSVC compilers if supported.
if (MSVC)
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
endif()
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_COMPILER clang++)
project(screenreader VERSION 1.0.0 LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
		espeak-ng
		GIT_REPOSITORY https://github.com/espeak-ng/espeak-ng.git
  GIT_TAG master
  GIT_SHALLOW ON
		SYSTEM
)
FetchContent_MakeAvailable(espeak-ng)

file(GLOB_RECURSE ALL_CXX_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/SRC/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SRC/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/SRC/*.cppm"
)

find_program(CLANG_FORMAT_EXE NAMES clang-format)

if(CLANG_FORMAT_EXE)
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_CXX_FILES}
        COMMENT "Formatting all files in SRC/ recursively"
    )
endif()

find_package(Git)
find_library(BRLAPI "libbrlapi.so")


function(get_git_version VERSION_HASH VERSION_TAG)
    if(GIT_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --always --tags --dirty=-dirty
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )

        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    endif()

    if(NOT GIT_VERSION)
        set(GIT_VERSION "v${PROJECT_VERSION}-unknown")
    endif()

    if(NOT GIT_HASH)
        set(GIT_HASH "unknown")
    endif()

    set(${VERSION_TAG} ${GIT_VERSION} PARENT_SCOPE)
    set(${VERSION_HASH} ${GIT_HASH} PARENT_SCOPE)
endfunction()

get_git_version(GIT_COMMIT_HASH GIT_VERSION_TAG)

configure_file(
    SRC/Version.h.in
    ${CMAKE_BINARY_DIR}/generated/Version.h
)

include_directories(${CMAKE_BINARY_DIR}/generated)

if(LINUX)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(ATSPI REQUIRED atspi-2)
    pkg_check_modules(SPEECHD REQUIRED speech-dispatcher)

    pkg_check_modules(GOBJECT REQUIRED gobject-2.0)
    pkg_check_modules(GLIB REQUIRED glib-2.0)
    pkg_check_modules(GDK REQUIRED gdk-3.0)
endif()

add_executable(${PROJECT_NAME})

add_subdirectory(SRC/Core)
add_subdirectory(SRC/Core/Outputs)
add_subdirectory(SRC/Proxies)
add_subdirectory(SRC/Traits)

if(WIN32)
    add_subdirectory(SRC/Platforms/Windows)
elseif(LINUX)
    add_subdirectory(SRC/Platforms/Linux)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/SRC
)

if(LINUX)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${ATSPI_INCLUDE_DIRS}
        ${SPEECHD_INCLUDE_DIRS}
        ${GDK_INCLUDE_DIRS}
    )
endif()

set(libraries espeak-ng)

if(LINUX)
    list(APPEND libraries
        ${ATSPI_LIBRARIES}
        ${GOBJECT_LIBRARIES}
        ${GLIB_LIBRARIES}
        ${GDK_LIBRARIES}
        ${SPEECHD_LIBRARIES}
        ${BRLAPI}
    )
endif()

target_link_libraries(${PROJECT_NAME} ${libraries})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
