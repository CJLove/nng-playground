#include <iostream>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include "Proxy.h"
#include "HealthStatus.h"
#include "yaml-cpp/yaml.h"
#include <sstream>
#include <fstream>
#include <string.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <zmq.hpp>

using namespace std;

void usage() {
    std::cerr << "Usage\n"
              << "nng-proxy [-f <configFile>][-l <logLevel>][-p <PUB endpoint>][-s <SUB endpoitn>][-t <threads>][-i <statInterval>]\n";
}

int main(int argc, char *argv[]) {
    int logLevel = spdlog::level::trace;
    std::string configFile = "nng-proxy.yaml";
    std::string pubEndpoint = "tcp://localhost:9000";
    std::string subEndpoint = "tcp://localhost:9001";
    uint16_t healthStatusPort = 6000;
    int statisticsInterval = 60;
    int c;
    while ((c = getopt(argc, argv, "f:l:p:s:h:i:?")) != EOF) {
        switch (c) {
            case 'f':
                configFile = optarg;
                break;
            case 'l':
                logLevel = std::stoi(optarg);
                break;
            case 'p':
                pubEndpoint = optarg;
                break;
            case 's':
                subEndpoint = optarg;
                break;
            case 'i':
                statisticsInterval = std::stoi(optarg);
                break;
            case 'h':
                healthStatusPort = static_cast<uint16_t>(std::stoi(optarg));
                break;
            case '?':
            default:
                usage();
                exit(1);
        }
    }
    auto logger = spdlog::stdout_logger_mt("nng");
    // Log format:
    // 2022-05-07 20:27:55.585|zmq-proxy|3425239|I|XPUB Port 9200 XSUB Port 9210
    logger->set_pattern("%Y-%m-%d %H:%M:%S.%e|zmq-proxy|%t|%L|%v");

    std::ifstream ifs(configFile);
    if (ifs.good()) {
        std::stringstream stream;
        stream << ifs.rdbuf();
        try {
            YAML::Node m_yaml = YAML::Load(stream.str());

            if (m_yaml["log-level"]) {
                logLevel = m_yaml["log-level"].as<int>();
            }
            if (m_yaml["pub-endpoint"]) {
                pubEndpoint = m_yaml["pub-endpoint"].as<std::string>();
            }
            if (m_yaml["sub-endpoint"]) {
                subEndpoint = m_yaml["sub-endpoint"].as<std::string>();
            }
            if (m_yaml["health-port"]) {
                healthStatusPort = m_yaml["health-port"].as<uint16_t>();
            }
        } catch (...) {
            logger->error("Error parsing config file");        
        }
    }    

    // Set the log level for filtering
    spdlog::set_level(static_cast<spdlog::level::level_enum>(logLevel));
    logger->info("PUB Endpoint {} SUB Endpoint {}", pubEndpoint, subEndpoint);

    // Start proxy
    Proxy proxy(pubEndpoint, subEndpoint);
    HealthStatus<Proxy> healthStatus(proxy,healthStatusPort);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(statisticsInterval));

        // TODO: status
    }
}