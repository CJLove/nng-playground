FROM registry.fedoraproject.org/fedora-minimal:35

WORKDIR /net-stack

COPY . .

RUN \
    microdnf install -y libbrotli


CMD  ./net-stack -f ./config/net-stack.yaml
