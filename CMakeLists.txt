cmake_minimum_required(VERSION 3.30)
project(Spatio_Temporal_FingerPrinting C)

set(CMAKE_C_STANDARD 23)

include_directories(${CMAKE_SOURCE_DIR}/Mastik-main/mastik)

# Add Mastik source files
file(GLOB MASTIK_SOURCES
        ${CMAKE_SOURCE_DIR}/Mastik-main/src/*.c
)

add_library(mastik STATIC ${MASTIK_SOURCES})

# Define your own sources
set(PROJECT_SOURCES
        main.c
        utils.c
        utils.h
)


# Define the executable
add_executable(Spatio_Temporal_FingerPrinting main.c
        ${PROJECT_SOURCES}
)


# Link the static Mastik library
target_link_libraries(Spatio_Temporal_FingerPrinting PRIVATE ${CMAKE_SOURCE_DIR}/Mastik-main/src/libmastik.a)