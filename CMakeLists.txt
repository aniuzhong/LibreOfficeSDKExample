cmake_minimum_required(VERSION 3.27.4)

project(LibreOfficeSDKDemo CXX)

set(CMAKE_CXX_FLAGS            "${CMAKE_CXX_FLAGS} -std=c++23 -Wall -Wextra -pedantic -fvisibility=hidden -fpic -fvisibility=hidden")
set(CMAKE_CXX_FLAGS_DEBUG      "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE    "{CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")
set(CMAKE_SHARED_LINKER_FLAGS  "-Wl,--allow-shlib-undefined -Wl,-export-dynamic -Wl,-z,defs -Wl,--no-whole-archive")
set(CMAKE_EXE_LINKER_FLAGS     "-Wl,--allow-shlib-undefined -Wl,-export-dynamic -Wl,-z,defs -Wl,--no-whole-archive")

set(SDK_INCLUDE_DIR  "/usr/lib/libreoffice/sdk/include")
set(SDK_LIBRARY_DIR  "/usr/lib/libreoffice/sdk/lib")
set(SDK_BINARY_DIR   "/usr/lib/libreoffice/sdk/bin")
set(SDK_PROGRAM_DIR  "/usr/lib/libreoffice/program")
set(SDK_LIBRARIES    "uno_sal" "uno_salhelpergcc3" "uno_purpenvhelpergcc3" "uno_cppu" "uno_cppuhelpergcc3")

execute_process(COMMAND "${SDK_BINARY_DIR}/cppumaker" -Gc -O${CMAKE_BINARY_DIR}/include "${SDK_PROGRAM_DIR}/types.rdb" "${SDK_PROGRAM_DIR}/types/offapi.rdb")

include_directories(${CMAKE_BINARY_DIR}/include)
include_directories(${SDK_INCLUDE_DIR})

add_compile_definitions(UNX)
add_compile_definitions(DGCC)
add_compile_definitions(LINUX)
add_compile_definitions(DCPPU_ENV=gcc3)

add_subdirectory(presentation)