cmake_minimum_required(VERSION 3.15)
project(GameBoyEmulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

file(GLOB_RECURSE ALL_CORE_SOURCES "src/*.cpp")

set(LIB_SOURCES ${ALL_CORE_SOURCES})
foreach(source_file ${LIB_SOURCES})
    if(${source_file} MATCHES "main.cpp")
        list(REMOVE_ITEM LIB_SOURCES ${source_file})
    endif()
endforeach()

add_library(EmulatorCore STATIC ${LIB_SOURCES})
target_include_directories(EmulatorCore PUBLIC include)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    add_executable(GeminiBoy src/main.cpp)
    target_link_libraries(GeminiBoy PRIVATE EmulatorCore)
else()
    message(FATAL_ERROR "Could not find src/main.cpp! Please check your file structure.")
endif()

file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

if(TEST_SOURCES)
    add_executable(unit_tests ${TEST_SOURCES})
    target_link_libraries(unit_tests PRIVATE EmulatorCore Catch2::Catch2WithMain)
    target_include_directories(unit_tests PRIVATE include)
else()
    message(WARNING "No .cpp files found in tests/ folder. unit_tests target will not be built.")
endif()

if(MSVC)
    target_compile_options(EmulatorCore PRIVATE /W4)
else()
    target_compile_options(EmulatorCore PRIVATE -Wall -Wextra -Wpedantic)
endif()
