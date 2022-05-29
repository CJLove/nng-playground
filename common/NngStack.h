#pragma once

#include <nngpp/protocol/pub0.h>
#include <nngpp/protocol/sub0.h>
#include "Publisher.h"
#include "Subscriber.h"
#include "HealthStatus.h"
#include <spdlog/logger.h>
#include <memory>
#include <set>

class NngStack {
public:
    NngStack(const std::string &name, const std::string &pubEndpoint, const std::string &subEndpoint, const std::vector<std::string> &topics);

    virtual ~NngStack();

    virtual void onReceivedMessage(const std::string_view &topic, const std::string_view &msg);

    virtual void onCtrlMessage(const std::string_view &msg);

    void Subscribe(const std::string &topic);

    void Unsubscribe(const std::string &topic); 

    std::set<std::string> Subscriptions();   

    void Publish(const std::string &topic, const std::string &msg);

    void Publish(const std::vector<std::string> &topics, const std::string &msg);

    void Stop();

    int Health();

    std::string Status();

protected:
    /**
     * @brief Service name
     */
    std::string m_name;

    /**
     * @brief Statistics
     */
    std::atomic<uint64_t> m_rxMessages;
    std::atomic<uint64_t> m_txMessages;
    std::set<std::string> m_subscriptions;

private:
    Publisher m_publisher;
    Subscriber<NngStack> m_subscriber;
    std::mutex m_mutex;

protected:
    std::shared_ptr<spdlog::logger> m_logger;

};