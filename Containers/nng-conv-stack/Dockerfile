FROM registry.fedoraproject.org/fedora-minimal:35

WORKDIR /conv-stack

COPY . .

RUN \
    microdnf install -y libbrotli


CMD  ./conv-stack -f ./config/conv-stack.yaml
