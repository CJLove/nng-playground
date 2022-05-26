cmake_minimum_required(VERSION 3.17)

project (pub-sub-proxy)

add_executable(nng-pub nng-pub.cpp )

target_link_libraries( nng-pub nng Threads::Threads)

add_executable(nng-sub nng-sub.cpp )

target_link_libraries( nng-sub nng Threads::Threads)

add_executable(nng-proxy nng-proxy.cpp )

target_link_libraries( nng-proxy nng Threads::Threads)

install(TARGETS nng-pub DESTINATION bin)
install(TARGETS nng-sub DESTINATION bin)
install(TARGETS nng-proxy DESTINATION bin)
