#pragma once
#include <thread>
#include <spdlog/spdlog.h>
#include <nngpp/protocol/pub0.h>
#include <nngpp/protocol/sub0.h>
#include <nngpp/platform/platform.h>


class Proxy {
public:

    Proxy(const std::string &pubEndpoint, const std::string &subEndpoint);

    ~Proxy();

    void Stop();

    void Run();

    int Health();

    std::string Status();

private:
    std::string m_pubEndpoint;
    std::string m_subEndpoint;
    nng::socket m_pubSocket;
    nng::socket m_subSocket;
    std::thread m_thread;
    std::mutex m_mutex;
    std::shared_ptr<spdlog::logger> m_logger;
};
