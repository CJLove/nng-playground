
#include <nngpp/nngpp.h>
#include <nngpp/protocol/sub0.h>
#include <string>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <unistd.h>
#include <iostream>

using namespace std;

void usage() {
    std::cerr << "Usage\n"
              << "nng_sub [-l <logLevel>][-t <Topic>][-s <endpoint>]\n";
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
    auto logger = spdlog::stdout_logger_mt("zmq");
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
    try {
        for (const auto &endpoint: endpoints) {
            sock.dial(endpoint.c_str());
        }
    }
    catch (std::exception &e) {
        logger->error("Caught {}", e.what());
        exit(1);
    }
    sock.set_opt(NNG_OPT_SUB_SUBSCRIBE, nng::view(topic.data(),topic.size()));

    while (true) {
        auto msg = sock.recv_msg();

        logger->info("Received message {}",(char*)msg.body().data());

    }
}