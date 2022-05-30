#include <iostream>
#include <unistd.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include "NetStack.h"
#include "HealthStatus.h"

void usage() 
{
    std::cerr << "Usage:\n"
        << "net-stack -n <name> -f <configFile> -p <pubEndpoint> -s <subEndpoint> -S <sub topic> -P <pub topic> -d <destIP> -D <destPort>\n";
}

int main(int argc, char **argv)
{
    int logLevel = spdlog::level::trace;
    std::string configFile = "net-stack.yaml";
    std::string name = "netStack";
    std::string pubEndpoint = "tcp://localhost:9000";
    std::string subEndpoint = "tcp://localhost:9001";
    std::vector<std::string> subTopics;
    std::vector<std::string> pubTopics;
    uint16_t healthStatusPort = 6000;
    uint16_t listenPort = 7000;
    std::string destIp = "fir.love.io";
    uint16_t destPort = 7100;
    int c;
    while ((c = getopt(argc,argv,"f:n:l:p:s:P:S:h:d:D:L:?")) != EOF) {
        switch (c) {
        case 'f':
            configFile = optarg;
            break;
        case 'n':
            name = optarg;
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
        case 'P':
            pubTopics.push_back(optarg);
            break;
        case 'S':
            subTopics.push_back(optarg);
            break;
        case 'd':
            destIp = optarg;
            break;
        case 'D':
            destPort = static_cast<uint16_t>(std::stoul(optarg));
            break;
        case 'L':
            listenPort = static_cast<uint16_t>(std::stoul(optarg));
            break;
        case 'h':
            healthStatusPort = static_cast<uint16_t>(std::stoul(optarg));
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

    std::ifstream ifs(configFile);
    if (ifs.good()) {
        std::stringstream stream;
        stream << ifs.rdbuf();
        try {
            YAML::Node m_yaml = YAML::Load(stream.str());
            if (m_yaml["log-level"]) {
                logLevel = m_yaml["log-level"].as<int>();
            }
            if (m_yaml["health-port"]) {
                healthStatusPort = m_yaml["health-port"].as<uint16_t>();
            }
            if (m_yaml["listen-port"]) {
                listenPort = m_yaml["listen-port"].as<uint16_t>();
            }
            if (m_yaml["dest-ip"]) {
                destIp = m_yaml["dest-ip"].as<std::string>();
            }
            if (m_yaml["dest-port"]) {
                destPort = m_yaml["dest-port"].as<uint16_t>();
            }
            if (m_yaml["name"]) {
                name = m_yaml["name"].as<std::string>();
            }
            if (m_yaml["pub-endpoint"]) {
                pubEndpoint = m_yaml["pub-endpoint"].as<std::string>();
            }
            if (m_yaml["sub-endpoint"]) {
                subEndpoint = m_yaml["sub-endpoint"].as<std::string>();
            }
            if (m_yaml["sub-topics"]) {
                if (m_yaml["sub-topics"].IsSequence()) {
                    subTopics = m_yaml["sub-topics"].as<std::vector<std::string>>();
                }
            }
            if (m_yaml["pub-topics"]) {
                if (m_yaml["pub-topics"].IsSequence()) {
                    pubTopics = m_yaml["pub-topics"].as<std::vector<std::string>>();
                }
            }
            
        }
        catch (...) {
            logger->error("Error parsing config file");        
        }
    }

    // Update logging pattern to reflect the service name
    auto pattern = fmt::format("%Y-%m-%d %H:%M:%S.%e|{}|%t|%L|%v",name);
    logger->set_pattern(pattern);
    // Set the log level for filtering
    spdlog::set_level(static_cast<spdlog::level::level_enum>(logLevel));

    logger->info("XPUB Endpoint {} XSUB Endpoint {}",pubEndpoint,subEndpoint);
    for (const auto &topic: pubTopics) {
        logger->info("    Pub topic {}", topic);
    }
    for (const auto &topic: subTopics) {
        logger->info("    Sub topic {}",topic);
    }

    NetStack stack(name, pubEndpoint, subEndpoint, subTopics, pubTopics, listenPort, destIp, destPort);
    HealthStatus<NetStack> healthStatus(stack, healthStatusPort);

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

}