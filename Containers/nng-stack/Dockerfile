FROM registry.fedoraproject.org/fedora-minimal:35

WORKDIR /nng-stack

COPY . .

RUN \
    microdnf install -y libbrotli

CMD ./nng-stack -f ./config/nng-stack.yaml
