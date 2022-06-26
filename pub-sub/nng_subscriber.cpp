
#include <nngpp/nngpp.h>
#include <nngpp/protocol/sub0.h>
#include <string>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <unistd.h>
#include <iostream>

using namespace std;

struct PubSubMessage {
    const char *m_topic;
    const char *m_msg;
    size_t m_topicSize;
    size_t m_msgSize;

    PubSubMessage(): m_topic(nullptr), m_msg(nullptr),m_topicSize(0), m_msgSize(0)
    {

    }

    bool populate(const char sep, nng::msg &msg)
    {
        for (size_t i = 0; i < msg.body().size(); i++) {
            if (msg.body().data<char>()[i] == sep) {
                m_topic = msg.body().data<char>();
                m_topicSize = i;
                m_msg = &msg.body().data<char>()[i+1];
                m_msgSize = msg.body().size() - (m_topicSize + 1);
                return true;
            }
        }
        return false;
    }

};

void usage() {
    std::cerr << "Usage\n"
              << "nng_subscriber [-l <logLevel>][-t <Topic>][-s <endpoint>]\n";
}


int main(int argc, char**argv)
{
    int logLevel = spdlog::level::trace;
    std::string topic = "topic1";
    std::vector<std::string> endpoints = { };
    int c;
    while ((c = getopt(argc, argv, "l:t:s:?")) != EOF) {
        switch (c) {
            case 'l':
                logLevel = std::stoi(optarg);
                break;
            case 't':
                topic = optarg;
                break;
            case 's':
                endpoints.push_back(std::string(optarg));
                break;

            case '?':
            default:
                usage();
                exit(1);
        }
    }
    auto logger = spdlog::stdout_logger_mt("nng");
    // Log format:
    // 2018-10-08 21:08:31.633|020288|I|Thread Worker thread 3 doing something
    logger->set_pattern("%Y-%m-%d %H:%M:%S.%e|%t|%L|%v");
    // Set the log level for filtering
    spdlog::set_level(static_cast<spdlog::level::level_enum>(logLevel));

    for (const auto &endpoint: endpoints) {
        logger->info("PUB Endpoint {}",endpoint);
    }
    logger->info("Using Topic {}", topic);

    nng::socket sock = nng::sub::open();
    nng::set_opt_reconnect_time_min(sock, 10);
    nng::set_opt_reconnect_time_max(sock, 10);

    try {
        for (const auto &endpoint: endpoints) {
            sock.dial(endpoint.c_str(), nng::flag::nonblock);
        }
    }
    catch (std::exception &e) {
        logger->error("Caught {}", e.what());
        exit(1);
    }
    sock.set_opt(NNG_OPT_SUB_SUBSCRIBE, nng::view(topic.data(),topic.size()));

    const std::string sep ="|";

    while (true) {
        auto nngMsg = sock.recv_msg();
        PubSubMessage msg;
        if (msg.populate('|', nngMsg)) {
            std::string topic{ msg.m_topic, msg.m_topicSize };
            std::string data{ msg.m_msg, msg.m_msgSize };
            logger->info("Received topic '{}' message {}",topic,data);
        }

    }
}