cmake_minimum_required(VERSION 3.17)

project (stacks)

include_directories(
    ${CMAKE_SOURCE_DIR}/common
)

find_package(yaml-cpp 0.6.2 REQUIRED)

find_package(httplib REQUIRED)

add_executable(nng-stack NngStackMain.cpp)
target_link_libraries( nng-stack nng common Threads::Threads yaml-cpp httplib::httplib)

add_executable(conv-stack ConvStack.cpp ConvStackMain.cpp)
target_link_libraries( conv-stack nng common Threads::Threads yaml-cpp httplib::httplib)

add_executable(nng-proxy Proxy.cpp ProxyMain.cpp )
target_link_libraries( nng-proxy nng Threads::Threads yaml-cpp httplib::httplib)

add_executable(net-stack NetStack.cpp NetStackMain.cpp)
target_link_libraries( net-stack nng common Threads::Threads yaml-cpp httplib::httplib)

#add_executable(inproc-stack NngStack.cpp Publisher.cpp Proxy.cpp InprocStackMain.cpp)
#target_link_libraries( inproc-stack zmq Threads::Threads )




# Install targets
install(TARGETS nng-stack DESTINATION bin)
install(TARGETS nng-proxy DESTINATION bin)
# install(TARGETS conv-stack DESTINATION bin)
# install(TARGETS net-stack DESTINATION bin)
# install(TARGETS inproc-stack DESTINATION bin)
