
#include <nngpp/nngpp.h>
#include <nngpp/protocol/pub0.h>
#include <string>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <unistd.h>
#include <iostream>

using namespace std;

void usage() {
    std::cerr << "Usage\n"
              << "nng-pub [-l <logLevel>][-t <Topic>][-p <endpoint>][-m <msg>]\n";
}


int main(int argc, char**argv)
{
    int logLevel = spdlog::level::trace;
    std::string topic = "topic1";
    std::string endpoint = "tcp://localhost:6001";
    std::string message = "Hello world!";
    int c;
    while ((c = getopt(argc, argv, "l:t:p:m:?")) != EOF) {
        switch (c) {
            case 'l':
                logLevel = std::stoi(optarg);
                break;
            case 't':
                topic = optarg;
                break;
            case 'p':
                endpoint = optarg;
                break;
            case 'm':
                message = std::string(optarg);
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

    logger->info("PUB Endpoint {}",endpoint);
    logger->info("Using Topic {}", topic);

    nng::socket sock = nng::pub::open();
    nng::set_opt_reconnect_time_min(sock, 10);
    nng::set_opt_reconnect_time_max(sock, 10);
    try {
        sock.dial(endpoint.c_str(),nng::flag::nonblock);
    }
    catch (std::exception &e) {
        logger->error("Caught {}", e.what());
        exit(1);
    }

    while (true) {
        const std::string sep = "|";
        nng::msg msg = nng::make_msg(0);

        msg.body().append(nng::view(topic.data(),topic.size()));
        msg.body().append(nng::view(sep.data(),1));
        msg.body().append(nng::view(message.data(),message.size()));

        logger->info("Msg hdr {} body {}",msg.header().size(), msg.body().size());

        logger->info("Sent topic '{}' message {}",topic, (char*)msg.body().data());

        sock.send(std::move(msg),0);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}