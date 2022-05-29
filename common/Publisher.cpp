#include "Publisher.h"
#include <fmt/core.h>

Publisher::Publisher(const std::string &endpoint) : m_socket(nng::pub::open()), m_logger(spdlog::get("nng")) {
    try {
        m_logger->info("Publisher Connecting to {}", endpoint);
        nng::set_opt_reconnect_time_min(m_socket, 10);
        nng::set_opt_reconnect_time_max(m_socket, 10);
        m_socket.dial(endpoint.c_str(), nng::flag::nonblock);

    } catch (nng::exception &e) {
        m_logger->error("Error connecting to {}. Error is {} {}", endpoint, e.what(), e.who());
    } catch (std::exception &e) {
        m_logger->error("Error connecting to {}. Error is {}", endpoint, e.what());
    }
}

Publisher::~Publisher() { Stop(); }

void Publisher::publishMsg(const std::string &topic, const std::string &appMsg) {
    const std::string sep{"|"};
    try {
        auto msg = nng::make_msg(0);
        msg.body().append(nng::view(topic.data(), topic.size()));
        msg.body().append(nng::view(sep.data(), 1));
        msg.body().append(nng::view(appMsg.data(), appMsg.size()));

        m_socket.send(std::move(msg), 0);
    } catch (nng::exception &e) {
        m_logger->error("Error publishing message to topic {}: {}", topic, e.what());
    } catch (std::exception &e) {
        m_logger->error("Error publishing message to topic {}: {}", topic, e.what());
    }
}

void Publisher::publishMsg(const std::vector<std::string> &topics, const std::string &appMsg) {
    const std::string sep{"|"};
    try {
        for (const auto &topic : topics) {
            auto msg = nng::make_msg(0);
            msg.body().append(nng::view(topic.data(), topic.size()));
            msg.body().append(nng::view(sep.data(), 1));
            msg.body().append(nng::view(appMsg.data(), appMsg.size()));

            m_socket.send(std::move(msg), 0);
        }

    } catch (nng::exception &e) {
        m_logger->error("Error publishing message to topics {}: {}", fmt::join(topics, " "), e.what());
    } catch (std::exception &e) {
        m_logger->error("Error publishing message to topics {}: {}", fmt::join(topics, " "), e.what());
    }
}

void Publisher::Stop() { m_socket.release(); }