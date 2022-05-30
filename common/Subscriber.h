#pragma once

#include <atomic>
#include <functional>
#include <nngpp/nngpp.h>
#include <nngpp/protocol/sub0.h>
#include <spdlog/spdlog.h>
#include <thread>

template <class T>
class Subscriber {
public:
    Subscriber(const std::string &endpoint, const std::vector<std::string> &topics, T &target)
        : m_socket(nng::sub::open()),
          m_shutdown(false),
          m_endpoint(endpoint),
          m_count(0),
          m_topics(topics),
          m_target(target),
          m_logger(spdlog::get("nng")) {
        nng::set_opt_reconnect_time_min(m_socket, 10);
        nng::set_opt_reconnect_time_max(m_socket, 10);

        m_thread = std::thread(&Subscriber::Run, this);
    }

    ~Subscriber() { Stop(); }

    void Stop() {
        if (m_thread.joinable()) {
            m_shutdown = true;
            m_thread.join();
        }
    }

    void Run() {
        const std::string CTRL_TOPIC = std::string("ctrl");
        const std::string sep = "|";
        try {
            m_logger->info("Subscriber Connecting to {}", m_endpoint);

            m_socket.dial(m_endpoint.c_str(), nng::flag::nonblock);
            nng::set_opt_recv_timeout(m_socket, 500);
            m_socket.set_opt(NNG_OPT_SUB_SUBSCRIBE, nng::view(CTRL_TOPIC.data(), CTRL_TOPIC.size()));
            for (const auto &topic : m_topics) {
                m_socket.set_opt(NNG_OPT_SUB_SUBSCRIBE, nng::view(topic.data(), topic.size()));
                m_logger->info("Subscribing to topic {}", topic);
            }
        } catch (nng::exception &e) {
            m_logger->error("Error connecting to {}. Error is {}", m_endpoint, e.what());
        } catch (std::exception &e) {
            m_logger->error("Error connecting to {}. Error is {}", m_endpoint, e.what());
        }

        while (!m_shutdown) {
            try {
                auto msg = m_socket.recv_msg();
                std::string_view view((char *)msg.body().data(), msg.body().size());
                auto pos = view.find(sep);
                if (pos != std::string::npos) {
                    auto topic = view.substr(0, pos);
                    auto data = view.substr(pos + 1);
                    if (topic == CTRL_TOPIC) {
                        m_target.onCtrlMessage(data);
                    } else {
                        m_target.onReceivedMessage(topic,data);
                    }
                    m_count++;
                }
            } catch (nng::exception &e) {
                if (e.get_error() == nng::error::timedout) {
                    m_count++;
                    continue;
                }
            } catch (std::exception &e) {
            }

        }
        m_socket.release();

    }

    void Subscribe(const std::string &topic) { m_socket.set_opt(NNG_OPT_SUB_SUBSCRIBE, nng::view(topic.data(), topic.size())); }

    void Unsubscribe(const std::string &topic) {
        m_socket.set_opt(NNG_OPT_SUB_UNSUBSCRIBE, nng::view(topic.data(), topic.size()));
    }

    uint32_t Count() const { return m_count; }

private:
    nng::socket m_socket;
    std::atomic_bool m_shutdown;
    std::string m_endpoint;
    std::atomic<uint32_t> m_count;
    std::vector<std::string> m_topics;

    T &m_target;
    std::shared_ptr<spdlog::logger> m_logger;

    std::thread m_thread;
};
