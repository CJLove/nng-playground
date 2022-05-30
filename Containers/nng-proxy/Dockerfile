FROM registry.fedoraproject.org/fedora-minimal:35

WORKDIR /nng-proxy

COPY . .

RUN \
    microdnf install -y libbrotli

CMD  ./nng-proxy -f ./config/nng-proxy.yaml
