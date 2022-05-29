cmake_minimum_required(VERSION 3.17)

project (pub-sub-proxy)

add_executable(nng_pub nng_pub.cpp )

target_link_libraries( nng_pub nng Threads::Threads)

add_executable(nng_sub nng_sub.cpp )

target_link_libraries( nng_sub nng Threads::Threads)

add_executable(nng_proxy nng_proxy.cpp )

target_link_libraries( nng_proxy nng Threads::Threads)

install(TARGETS nng_pub DESTINATION bin)
install(TARGETS nng_sub DESTINATION bin)
install(TARGETS nng_proxy DESTINATION bin)
