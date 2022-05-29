
#include <iostream>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <sstream>
#include <string.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <exception>
#include <nngpp/protocol/pub0.h>
#include <nngpp/protocol/sub0.h>
#include <nngpp/platform/platform.h>

using namespace std;

void usage() {
    std::cerr << "Usage\n"
              << "zmq_proxy [-l <logLevel>][-p <pubEndpoint>][-s <subEndpoint>]\n";
}

int main(int argc, char *argv[]) {
    int logLevel = spdlog::level::trace;
    std::string pubEndpoint = "tcp://localhost:6000";
    std::string subEndpoint = "tcp://localhost:6001";
    int c;
    while ((c = getopt(argc, argv, "l:p:s:?")) != EOF) {
        switch (c) {
            case 'l':
                logLevel = std::stoi(optarg);
                break;
            case 'p':
                pubEndpoint = optarg;
                break;
            case 's':
                subEndpoint = optarg;
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

    logger->info("XPUB Port {} XSUB Port {}", pubEndpoint, subEndpoint);

    auto pubSocket = nng::pub::v0::open_raw();
    auto subSocket = nng::sub::v0::open_raw();

    pubSocket.listen(pubEndpoint.c_str());
    subSocket.listen(subEndpoint.c_str());

    try {
        nng::device(pubSocket,subSocket);
    } catch (nng::exception &e) {
        logger->error("Caught exception error: {} who: {} what: {}", e.get_error(), e.who(), e.what());
    } catch (std::exception &e) {
        logger->error("Caught exception {}", e.what());
    }

    return 0;
}
