#pragma once

#include <thread>

// Override number of threads in httplib thread pool
#undef CPPHTTPLIB_THREAD_POOL_COUNT
#define CPPHTTPLIB_THREAD_POOL_COUNT 2
#include <httplib.h>

/**
 * @brief HealthStatus supports HTTP liveness checks and status endpoints for a containerized service
 *          /healthz - endpoint returning HTTP status code 200 (OK) for healthy and 400+ for non-healthy
 *          /status - endpoint returning JSON string with service-specific statistics
 * 
 * @tparam T - Class supporting the following methods
 *              int Health() - 0 indicates ok, non-zero indicates errors
 *              std::string Status() - JSON string populated by the class with current statistics
 */
template <class T>
class HealthStatus {
public:
    HealthStatus(T& stack, uint16_t port): m_stack(stack), m_port(port)
    {
        m_svr.Get("/healthz", [&](const httplib::Request & /*req*/, httplib::Response &res) {
            int health = m_stack.Health();
            res.status = (health == 0) ? HEALTHY : UNHEALTHY_BASE + health;
        });
        m_svr.Get("/status", [&](const httplib::Request & /*req*/, httplib::Response &res)
        {
            std::string status = m_stack.Status();
            res.set_content(status,"application/json");
        });

        m_thread = std::thread(&HealthStatus::Run, this);
    }

    virtual ~HealthStatus()
    {
        m_svr.stop();
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    void Run()
    {
        m_svr.listen("0.0.0.0",m_port);
    }

    void stop() 
    {
        m_svr.stop();
    }

protected:
    /**
     * @brief HTTP status code constants used for /healthz endpoint
     */
    enum HealthStatus_e {
        HEALTHY = 200,
        UNHEALTHY_BASE = 400
    };

    /**
     * @brief The service-specific class supporting Health() and Status() methods
     */
    T& m_stack;

    /**
     * @brief Port to expose health and status endpoints
     */
    uint16_t m_port;

    /**
     * @brief Thread to listen for HTTP requests
     */
    std::thread m_thread;

    /**
     * @brief HTTP Server realized via httplib
     */
    httplib::Server m_svr;

};