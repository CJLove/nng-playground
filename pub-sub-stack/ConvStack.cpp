#include "ConvStack.h"
#include <fmt/format.h>

ConvStack::ConvStack(const std::string &name, const std::string &pubEndpoint, const std::string &subEndpoint, 
                     std::vector<std::string> &subTopics, std::map<std::string, std::vector<std::string>> &conversionMap):
    NngStack(name, pubEndpoint, subEndpoint, subTopics),
    m_conversionMap(conversionMap)
{

}

void ConvStack::onReceivedMessage(const std::string_view &topic, const std::string_view &msg) 
{
    //m_logger->info("{} receive message on topic {}",topic);
    std::lock_guard guard(m_mutex);
    std::string topicStr { topic };
    std::string msgStr { msg };
    auto f = m_conversionMap.find(topicStr);
    if (f != m_conversionMap.end()) {
        auto topics = m_conversionMap[topicStr];
        Publish(topics,msgStr);
        m_logger->info("{} receive message on topic {} publishing to {}", m_name, topic, fmt::join(topics," "));
    }
    m_rxMessages++;
}

void ConvStack::onCtrlMessage(const std::string_view &msg)
{
    m_logger->info("Received ctrl message {}", msg);
}

void ConvStack::AddConversion(const std::string &subTopic, std::vector<std::string> &pubTopics)
{
    std::lock_guard guard(m_mutex);
    Subscribe(subTopic);
    m_conversionMap[subTopic] = pubTopics;

}

void ConvStack::RemoveConversion(const std::string &subTopic)
{
    std::lock_guard guard(m_mutex);
    Unsubscribe(subTopic);
    m_conversionMap.erase(subTopic);
}

int ConvStack::Health() 
{
    int status = NngStack::Health();
    // TBD: Additional health checks for this stack

    return status;
}

std::string ConvStack::Status()
{
    // { "conversions": x, "subscriptions": x, "rx-msgs": x, "tx-msgs": x }
    std::string str = fmt::format("{{ \"conversions\": {}, \"subscriptions\": {}, \"rx-msgs\": {}, \"tx-msgs\": {} }}", 
        m_conversionMap.size(), m_subscriptions.size(), m_rxMessages, m_txMessages);

    return str;
}