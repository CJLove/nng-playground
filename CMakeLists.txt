cmake_minimum_required(VERSION 3.17)

project(nng-playground)

#---------------------------------------------------------------------------------------
# compiler config
#---------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    add_compile_options("-Wall")
    add_compile_options("-Wextra")
    add_compile_options("-Wconversion")
    add_compile_options("-pedantic")
    add_compile_options("-Wfatal-errors")
    
endif()

# fmt library dependency
find_package(fmt CONFIG REQUIRED)

# nng library dependency
find_package(nng CONFIG REQUIRED)

# nngpp library dependency
find_package(nngpp CONFIG REQUIRED)


# spdlog library dependency
find_package(spdlog CONFIG REQUIRED)

# Threads
find_package(Threads)

add_subdirectory(pub-sub-proxy)
add_subdirectory(pub-sub)

add_subdirectory(common)

add_subdirectory(pub-sub-stack)
