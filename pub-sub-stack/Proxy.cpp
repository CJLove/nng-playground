#include "Proxy.h"
#include <fmt/core.h>
#include <exception>

Proxy::Proxy(const std::string &pubEndpoint, const std::string &subEndpoint):
    m_pubEndpoint(pubEndpoint),
    m_subEndpoint(subEndpoint),
    m_logger(spdlog::get("nng"))
{
    m_thread = std::thread(&Proxy::Run, this);
}

Proxy::~Proxy()
{
    Stop();
    m_thread.join();

}

void Proxy::Stop()
{
    m_pubSocket.release();
    m_subSocket.release();
}

int Proxy::Health()
{
    // TODO: Detect health of proxy by having internal publisher/subscriber for heartbeat topic
    return 0;
}

std::string Proxy::Status()
{
    auto str = fmt::format("{}");
    return str;
}

void Proxy::Run()
{
    m_logger->info("XPUB {} XSUB {}",m_pubEndpoint, m_subEndpoint);

    auto pubSocket = nng::pub::v0::open_raw();
    auto subSocket = nng::sub::v0::open_raw();

    pubSocket.listen(m_pubEndpoint.c_str());
    subSocket.listen(m_subEndpoint.c_str());

    try {
        nng::device(pubSocket,subSocket);
    } catch (nng::exception &e) {
        m_logger->error("Caught exception error: {} who: {} what: {}", e.get_error(), e.who(), e.what());
    } catch (std::exception &e) {
        m_logger->error("Caught exception {}", e.what());
    }
    m_logger->info("Proxy exiting");
}