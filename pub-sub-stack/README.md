# Stack executables

Stacks are comprised of NNG `pub0` and `sub0` sockets and can be configured to subscribe to one or more topics and publishe to one or more topics.

## nng-proxy
- Proxy app for use with standalone stack apps

## nng-stack
- Executable with a single NngStack instance
- Supports interactive loop for accepting messages to be published to specific topics
- Uses `TCP` transport to a proxy

## conv-stack
- Executable with a single ConvStack instance configured to convert from a message received on one topic to publish the message to one or more topics.
- Uses `TCP` transport to a proxy

## net-stack
- Executable with a single NngStack instance
- Listens on a specific UDP port for external messages and publishes them to one or more topics
- Sends messages received on subscribed topic(s) to an external UDP IP/port
