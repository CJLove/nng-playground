#pragma once

#include "NngStack.h"
#include "UdpSocket.h"

class NetStack: public NngStack {
public:
    NetStack(const std::string &name, const std::string &pubEndpoint, const std::string &subEndpoint, 
             const std::vector<std::string> &subTopics, const std::vector<std::string> &pubTopics,
             uint16_t listenPort, const std::string &dest, uint16_t destPort);

    virtual ~NetStack();

    virtual void onReceivedMessage(const std::string &topic, const PubSubMessage &msg) override;

    virtual void onCtrlMessage(const PubSubMessage &msg) override;

    int Health();

    std::string Status();

    /**
     * @brief Receive data from UDP socket
     * 
     * @param data 
     * @param size 
     */
    void onReceiveData(const char *data, size_t size); 

private:
    UdpSocket<NetStack> m_socket;
    std::vector<std::string> m_pubTopics;
    uint16_t m_listenPort;
    std::string m_dest;
    uint16_t m_destPort;
    std::atomic<uint64_t> m_udpRxMsgs;
    std::atomic<uint64_t> m_udpTxMsgs;
};