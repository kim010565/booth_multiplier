cmake_minimum_required(VERSION 3.10)
set(name_project "main")

project(main LANGUAGES C CXX CUDA)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSC_DISABLE_API_VERSION_CHECK")
file(GLOB mainfile "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(${name_project} ${mainfile})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -D_DEBUG")
    message("Using Debug Mode: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
else ()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Werror")
    message("Using Release Mode: ${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
endif ()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -march=native -mtune=native -fpermissive") 

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/external/systemc/include")
file(GLOB_RECURSE SYSTEMC_LIB "${CMAKE_CURRENT_SOURCE_DIR}/external/systemc/lib/*.so")
target_link_libraries(${name_project} ${SYSTEMC_LIB})

target_link_libraries(${name_project} stdc++fs)
