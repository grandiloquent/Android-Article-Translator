cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(mbedtls/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mbedtls)

add_library(
        native-lib

        SHARED
        utils_string.c
        https.c
        main.c
        native-lib.c)

find_library(
        log-lib
        log)

add_library(cJSON ${CMAKE_CURRENT_SOURCE_DIR}/cJSON/cJSON.c)
add_library(tmd5 ${CMAKE_CURRENT_SOURCE_DIR}/tmd5/tmd5.c)

target_link_libraries( # Specifies the target library.
        native-lib
        cJSON
        tmd5
        mbedtls
        ${log-lib})