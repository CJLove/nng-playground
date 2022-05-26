cmake_minimum_required(VERSION 3.17)

project (pub-sub)

add_executable(nng_publisher nng_publisher.cpp )

target_link_libraries( nng_publisher nng Threads::Threads)

add_executable(nng_subscriber nng_subscriber.cpp )

target_link_libraries( nng_subscriber nng Threads::Threads)

add_executable(pubsub pubsub.c )

target_link_libraries(pubsub nng Threads::Threads)

install(TARGETS nng_publisher DESTINATION bin)
install(TARGETS nng_subscriber DESTINATION bin)
