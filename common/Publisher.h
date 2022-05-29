#pragma once

#include <nngpp/nngpp.h>
#include <nngpp/protocol/pub0.h>
#include <functional>
#include <atomic>
#include <spdlog/spdlog.h>

//template <class T>
class Publisher {
public:
    Publisher(const std::string &endpoint);

    ~Publisher();

    void publishMsg(const std::string &topic, const std::string &appMsg);

    void publishMsg(const std::vector<std::string> &topics, const std::string &appMsg);

    void Stop();

private:
    nng::socket m_socket;

    std::shared_ptr<spdlog::logger> m_logger;
};