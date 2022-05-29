#include "NngStack.h"
#include "HealthStatus.h"

NngStack::NngStack(const std::string &name, const std::string &pubEndpoint, const std::string &subEndpoint, const std::vector<std::string> &topics):
    m_name(name),
    m_rxMessages(0),
    m_txMessages(0),
    m_publisher(subEndpoint),
    m_subscriber(pubEndpoint, topics, *this),
    m_logger(spdlog::get("nng"))
{
    for (const auto &topic: topics) {
        m_subscriptions.insert(topic);
    }
}

NngStack::~NngStack()
{
    Stop();
}

void NngStack::onReceivedMessage(const std::string_view &topic, const std::string_view &msg)
{
    m_logger->info("Received message {} on topic {}", msg, topic);
    m_rxMessages++;
}

void NngStack::onCtrlMessage(const std::string_view &msg)
{
    m_logger->info("Received ctrl message {} ", msg);

    // See if CTRL message is addressed to this node then process it
}

void NngStack::Subscribe(const std::string &topic)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_subscriptions.insert(topic);
    m_subscriber.Subscribe(topic);
}

void NngStack::Unsubscribe(const std::string &topic)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_subscriptions.erase(topic);
    m_subscriber.Unsubscribe(topic);
}

std::set<std::string> NngStack::Subscriptions()
{
    return m_subscriptions;
}

void NngStack::Publish(const std::string &topic, const std::string &msg)
{
    m_logger->info("Published message {} to topic {}",msg,topic);
    m_publisher.publishMsg(topic,msg);
    m_txMessages++;
}

void NngStack::Publish(const std::vector<std::string> &topics, const std::string &msg)
{
    m_logger->info("Published message {} to topics {}",msg, fmt::join(topics, " "));
    m_publisher.publishMsg(topics,msg);
    m_txMessages+= topics.size();
}

void NngStack::Stop()
{
    m_subscriber.Stop();
    m_publisher.Stop();
}

int NngStack::Health()
{
    // Subscriber count is incremented for each received message *or* timeout in zmq::poll(),
    // which minimally proves that the subscriber thread isn't wedged
    static uint32_t lastCount = 0;
    uint32_t count = m_subscriber.Count();
    int status = (lastCount != count) ? 0 : 1;
    lastCount = count;
    return status;
}

std::string NngStack::Status()
{
    // { "subscriptions": x, "rx-msgs": x, "tx-msgs": x }
    size_t subCount = 0;
    { 
        std::lock_guard<std::mutex> guard(m_mutex);
        subCount = m_subscriptions.size();
    }
    std::string str = fmt::format("{{ \"subscriptions\": {}, \"rx-msgs\": {}, \"tx-msgs\": {} }}", subCount, m_rxMessages, m_txMessages);

    return str;
}