cmake_minimum_required(VERSION 3.19)
project(XMLValidation C)

set(CMAKE_C_STANDARD 99)

add_executable(XMLValidation main.c)

if (WIN32)
    target_include_directories(XMLValidation PRIVATE ${PROJECT_SOURCE_DIR}/header-files)
    target_link_libraries(XMLValidation ${PROJECT_SOURCE_DIR}/Binaries/Windows.Intel/libxml2.lib)
endif (WIN32)

if (APPLE)
    set(CMAKE_EXE_LINKER_FLAGS -lxml2)
endif (APPLE)

if(UNIX AND NOT APPLE)
    target_include_directories(XMLValidation PUBLIC /usr/include/libxml2)
    set(CMAKE_EXE_LINKER_FLAGS -lxml2)
endif()