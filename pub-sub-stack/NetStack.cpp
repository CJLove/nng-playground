#include "NetStack.h"

NetStack::NetStack(const std::string &name, const std::string &pubEndpoint, const std::string &subEndpoint, 
             const std::vector<std::string> &subTopics, const std::vector<std::string> &pubTopics,
             uint16_t listenPort, const std::string &dest, uint16_t destPort):
    NngStack(name, pubEndpoint, subEndpoint, subTopics),
    m_socket(*this),
    m_pubTopics(pubTopics),
    m_listenPort(listenPort),
    m_dest(dest),
    m_destPort(destPort),
    m_udpRxMsgs(0),
    m_udpTxMsgs(0)
{
    SocketRet ret = m_socket.startUnicast(m_dest.c_str(), m_listenPort, m_destPort);
    if (ret.m_success) {
        m_logger->info("Listening on 0.0.0.0:{}",m_listenPort);
    } else {
        m_logger->error("Error setting up UDP socket: {}",ret.m_msg);
    }
}

NetStack::~NetStack()
{
    m_socket.finish();

}

void NetStack::onReceivedMessage(const std::string_view &topic, const std::string_view &msg)
{
    m_logger->info("Received message {} on topic {}", msg, topic);
    m_rxMessages++;
    m_socket.sendMsg(reinterpret_cast<const unsigned char*>(&msg[0]),msg.size());
}

void NetStack::onCtrlMessage(const std::string_view &msg)
{
    m_logger->info("Received ctrl message {}", msg);    
}

void NetStack::onReceiveData(const  char *data, size_t size)
{
    std::string msg(data,size);
    m_logger->info("Received message {} on UDP, publishing to topic {}", msg, m_pubTopics[0]);
    Publish(m_pubTopics, std::string(data,size));
}

int NetStack::Health()
{
    int health = NngStack::Health();

    return health;
}

std::string NetStack::Status()
{
    // { "sub-topics": x, "pub-topics": x, "rx-msgs": x, "tx-msgs": x, "udp-rx-msgs": x, "udp-tx-msgs": x }
    std::string str = fmt::format("{{ \"sub-topics\" {}, \"pub-topics\": {}, \"rx-msgs\": {}, \"tx-msgs\": {}, \"udp-rx-msgs\": {}, \"udp-tx-msgs\": {} }}",
        m_subscriptions.size(), m_pubTopics.size(), m_rxMessages, m_txMessages, m_udpRxMsgs, m_udpTxMsgs);

    return str;
}