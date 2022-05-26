cmake_minimum_required(VERSION 3.17)

project (pub-sub)

add_executable(nng_pub nng_pub.cpp )

target_link_libraries( nng_pub nng Threads::Threads)

add_executable(nng_sub nng_sub.cpp )

target_link_libraries( nng_sub nng Threads::Threads)

add_executable(pubsub pubsub.c )

target_link_libraries(pubsub nng Threads::Threads)

install(TARGETS nng_pub DESTINATION bin)
install(TARGETS nng_sub DESTINATION bin)
