#[[
    ДЗ-1. Си продвинутый уровень. гр.Д01-134 Попов. В.Г
]]

cmake_minimum_required(VERSION 3.28)

project(HW1 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(C_SRC_PATH ${CMAKE_SOURCE_DIR}/src)
set(SOURCES_C   ${C_SRC_PATH}/A0.c
                ${C_SRC_PATH}/A1.c
                ${C_SRC_PATH}/A2.c
                ${C_SRC_PATH}/A3.c
                ${C_SRC_PATH}/A4.c
                ${C_SRC_PATH}/A5.c
                ${C_SRC_PATH}/A6.c
                ${C_SRC_PATH}/A7.c
                ${C_SRC_PATH}/A8.c
                ${C_SRC_PATH}/A9.c
                ${C_SRC_PATH}/A10.c
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

foreach(C_FILE ${SOURCES_C})
    get_filename_component(TARGET_NAME ${C_FILE} NAME_WE)
    add_executable(${TARGET_NAME} ${C_FILE})
    target_compile_options(${TARGET_NAME} PRIVATE -g -Wall -Wextra -Wpedantic)
    target_link_libraries(${TARGET_NAME} PRIVATE m)  
endforeach()
