#!/bin/bash

arg=$1

case $arg in

nng-proxy)
    cp ../bin/nng-proxy nng-proxy/
    cd nng-proxy
    docker build -t nng-proxy:latest -t fir.love.io:3005/nng-proxy:latest .
    docker push fir.love.io:3005/nng-proxy:latest
    cd ..
    ;;
nng-stack)
    cp ../bin/nng-stack nng-stack/
    cd nng-stack
    docker build -t nng-stack:latest -t fir.love.io:3005/nng-stack:latest .
    docker push fir.love.io:3005/nng-stack:latest
    cd ..
    ;;
nng-conv-stack)
    cp ../bin/conv-stack nng-conv-stack/
    cd nng-conv-stack
    docker build -t nng-conv-stack:latest -t fir.love.io:3005/nng-conv-stack:latest .
    docker push fir.love.io:3005/nng-conv-stack:latest
    cd ..
    ;;
nng-net-stack)
    cp ../bin/net-stack nng-net-stack/
    cd nng-net-stack
    docker build -t nng-net-stack:latest -t fir.love.io:3005/nng-net-stack:latest .
    docker push fir.love.io:3005/nng-net-stack:latest
    cd ..
    ;;
esac

