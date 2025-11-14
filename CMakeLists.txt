# CMakeLists.txt : CMake project for Screen Reader, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.25)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(INCLUDES "Include")

# Enable Hot Reload for MSVC compilers if supported.
if (MSVC)
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
endif()

project(ScreenReader VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)

pkg_check_modules(ATSPI REQUIRED atspi-2)
pkg_check_modules(SPEECHD REQUIRED speech-dispatcher)

pkg_check_modules(GOBJECT REQUIRED gobject-2.0)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GDK REQUIRED gdk-3.0)

set(SCREENREADER_SOURCES
    SRC/main.cpp
    SRC/Engine.cpp
    SRC/EngineSpeechDispatcher.cpp
    SRC/SpeechEngine.cpp
    SRC/ObjectAtspi.cpp
    SRC/EventListenerAtspi.cpp
    SRC/EventHandler.cpp
    SRC/EventToSpeech.cpp
    SRC/Object.cpp
)

add_executable(ScreenReader
    ${SCREENREADER_SOURCES}
)

set_target_properties(ScreenReader PROPERTIES
    CXX_EXTENSIONS OFF
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(ScreenReader PRIVATE
    ${CMAKE_SOURCE_DIR}/SRC
    ${ATSPI_INCLUDE_DIRS}
    ${SPEECHD_INCLUDE_DIRS}
    ${GDK_INCLUDE_DIRS}
)

target_link_libraries(ScreenReader
    ${ATSPI_LIBRARIES}
    ${SPEECHD_LIBRARIES}
    ${GOBJECT_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${GDK_LIBRARIES}
)

install(TARGETS ScreenReader DESTINATION bin)

