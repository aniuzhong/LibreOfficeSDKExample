cmake_minimum_required(VERSION 3.27.4)

project(LibreOfficeSDKDemo CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -fvisibility=hidden -fpic -fvisibility=hidden -std=c++20")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
set(CMAKE_CXX_FLAGS_RELEASE "{CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")

set(SDK_INC_DIR "$ENV{OO_SDK_HOME}/include")
set(SDK_LIB_DIR "$ENV{OO_SDK_HOME}/lib")
set(SDK_LIBS "uno_sal" "uno_salhelpergcc3" "uno_purpenvhelpergcc3" "uno_cppu" "uno_cppuhelpergcc3")

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories("$ENV{OO_SDK_HOME}/include")

add_compile_definitions(UNX)
add_compile_definitions(DGCC)
add_compile_definitions(LINUX)
add_compile_definitions(DCPPU_ENV=gcc3)

file(GLOB SOURCES "${CURRENT_SOURCE_DIR}*.cpp")
foreach(SOURCE ${SOURCES})
    get_filename_component(TARGET ${SOURCE} NAME_WE)
    add_executable(${TARGET} ${SOURCE})    
    target_link_directories(${TARGET} PRIVATE ${SDK_LIB_DIR})
    target_link_libraries(${TARGET} PRIVATE ${SDK_LIBS})
endforeach(SOURCE ${SOURCES})
