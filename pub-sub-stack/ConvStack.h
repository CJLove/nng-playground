#pragma once

#include "NngStack.h"
#include <map>
#include <mutex>

class ConvStack: public NngStack {
public:
    ConvStack(const std::string &name, const std::string &pubEndpoint, const std::string &subEndpoint, 
              std::vector<std::string> &subTopics, std::map<std::string, std::vector<std::string>> &conversionMap);

    ~ConvStack() = default;

    virtual void onReceivedMessage(const std::string &topic, const PubSubMessage &msg) override;

    virtual void onCtrlMessage(const PubSubMessage &msg) override;

    void AddConversion(const std::string &subTopic, std::vector<std::string> &pubTopics);

    void RemoveConversion(const std::string &subTopic);

    int Health();

    std::string Status();

private:
    std::map<std::string, std::vector<std::string> > m_conversionMap;
    std::mutex m_mutex;
};