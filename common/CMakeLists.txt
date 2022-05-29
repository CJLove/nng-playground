cmake_minimum_required(VERSION 3.17)

project(common)

set (common_SRC
    Publisher.cpp
    NngStack.cpp
)

include_directories(
    .
)

add_library( common STATIC ${common_SRC})

target_link_libraries( common nng Threads::Threads )
