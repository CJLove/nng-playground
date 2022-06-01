# Containerization

## Proxy
```bash
$ kubectl apply -f nng-proxy-config-map.yaml
$ kubectl apply -f nng-proxy.yaml
$ kubectl apply -f nng-proxy-service.yaml
```

Note: this is a ClusterIP service 

## Stack A
Deploy the `nng-stack` container with a ConfigMap as "stackA":
```bash
$ kubectl apply -f nng-stackA-config-map.yaml
$ kubectl apply -f nng-stackA.yaml
```

## Stack B
Deploy the `nng-stack` container with a ConfigMap as "stackB":
```bash
$ kubectl apply -f nng-stackB-config-map.yaml
$ kubectl apply -f nng-stackB.yaml
```

## Converter
Deploy the `conv-stack` container with a ConfigMap as "converter":
```bash
$ kubectl apply -f conv-stack-config-map.yaml
$ kubectl apply -f conv-stack.yaml
```

## Netstack A
Deploy the `net-stack` container with a ConfigMap as "net-stack-a":
```bash
$ kubectl apply -f net-stack-a.yaml
```

NodePort service listening on port 30002

## Netstack B
Deply the `net-stack` container with a ConfigMap as "net-stack-b":
```bash
$ kubectl apply -f net-stack-b.yaml
```

NodePort service listening on port 30003

## Topics
- stack-a-ingress (from stackA to converter)
- stack-a-egress (from converter to stackA)
- stack-b-ingress (from stackB to converter)
- stack-b-egress (from converter to stackB)
- net-stack-a-ingress (from net-stack-a to converter)
- net-stack-a-egress (from converter to net-stack-a)
- net-stack-b-ingress (from net-stack-b to converter)
- net-stack-b-egress (from converter to net-stack-b)