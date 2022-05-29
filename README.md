# nng-playground
Sandbox for code using Nanomsg Next Generation ([NNG](https://nng.nanomsg.org/)) and [NNGCPP](https://github.com/EvanBalster/nngpp) C++ bindings

## pub-sub
Simple publisher and subscriber apps, where the publisher `listens` for connections and subscribers `dial` to connect.

## pub-sub-proxy
Publisher and subscriber apps which connect to "known" URLs for pub and sub supported by a proxy app.


## pub-sub-stack
generic "stack" applications w/pub and sub capabilities using proxy app.
