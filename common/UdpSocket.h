#pragma once

#include <cstdint>
#include <fmt/core.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>

constexpr size_t MAX_PACKET_SIZE = 65535;

/**
 * @brief Status structure returned by socket class methods.
 *
 */
struct SocketRet {
    /**
     * @brief Error message text
     */
    std::string m_msg;

    /**
     * @brief Indication of whether the operation succeeded or failed
     */
    bool m_success = false;
};

/**
 * @brief The UdpSocket class represents a UDP unicast or multicast socket connection
 *
 */
template <class T>
class UdpSocket {
public:
    /**
     * @brief Construct a new UDP Socket object
     *
     * @param callback - the callback recipient
     */
    explicit UdpSocket(T &callback) : m_sockaddr({}), m_fd(-1), m_callback(callback), m_thread(&UdpSocket::ReceiveTask, this) {}

    UdpSocket(const UdpSocket &) = delete;
    UdpSocket(UdpSocket &&) = delete;

    /**
     * @brief Destroy the UDP Socket object
     *
     */
    ~UdpSocket() { finish(); }

    UdpSocket &operator=(const UdpSocket &) = delete;
    UdpSocket &operator=(UdpSocket &&) = delete;

    /**
     * @brief Start a UDP multicast socket by binding to the server address and joining the
     *          multicast group.
     *
     * @param mcastAddr - multicast group address to join
     * @param port - port number to listen/connect to
     * @return SocketRet - indication that multicast setup was successful
     */
    SocketRet startMcast(const char *mcastAddr, uint16_t port) {
        SocketRet ret;
        if ((m_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            ret.m_success = false;
            ret.m_msg = fmt::format("Error: socket() failed: errno {}", errno);
            return ret;
        }
        // Allow multiple sockets to use the same port
        unsigned yes = 1;
        if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&yes), sizeof(yes)) < 0) {
            ret.m_success = false;
            ret.m_msg = fmt::format("Error: setsockopt(SO_REUSEADDR) failed: errno {}", errno);
            return ret;
        }
        // Set TX and RX buffer sizes
        int option_value = RX_BUFFER_SIZE;
        if (setsockopt(m_fd, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&option_value), sizeof(option_value)) < 0) {
            ret.m_success = false;
            ret.m_msg = fmt::format("Error: setsockopt(SO_RCVBUF) failed: errno {}", errno);
            return ret;
        }
    }

    /**
     * @brief Start a UDP unicast socket by binding to the server address and storing the
     *          IP address and port number for the peer.
     *
     * @param remoteAddr - remote IP address
     * @param localPort - local port to listen on
     * @param port - remote port to connect to when sending messages
     * @return SocketRet - Indication that unicast setup was successful
     */
    SocketRet startUnicast(const char *remoteAddr, uint16_t localPort, uint16_t port) {
        SocketRet ret;

        // store the remoteaddress for use by sendto()
        memset(&m_sockaddr, 0, sizeof(sockaddr));
        m_sockaddr.sin_family = AF_INET;
        int inetSuccess = inet_aton(remoteAddr, &m_sockaddr.sin_addr);
        if (inetSuccess == 0) {  // inet_addr failed to parse address
            // if hostname is not in IP strings and dots format, try resolve it
            struct hostent *host = nullptr;
            struct in_addr **addrList = nullptr;
            if ((host = gethostbyname(remoteAddr)) == nullptr) {
                ret.m_success = false;
                ret.m_msg = fmt::format("Failed to resolve hostname {}", remoteAddr);

                return ret;
            }
            addrList = reinterpret_cast<struct in_addr **>(host->h_addr_list);
            m_sockaddr.sin_addr = *addrList[0];
        }

        m_sockaddr.sin_port = htons(port);

        // return the result of setting up the local server
        return startUnicast(localPort);
    }

    /**
     * @brief Start a UDP unicast socket by binding to the server address
     *
     * @param localPort - local port to listen on
     * @return SocketRet - Indication that unicast setup was successful
     */
    SocketRet startUnicast(uint16_t localPort) {
        SocketRet ret;
        if ((m_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            ret.m_success = false;
            ret.m_msg = fmt::format("Error: socket() failed: errno {}", errno);

            return ret;
        }
        // Allow multiple sockets to use the same port
        unsigned yes = 1;
        if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&yes), sizeof(yes)) < 0) {
            ret.m_success = false;
            ret.m_msg = fmt::format("Error: setsockopt(SO_REUSEADDR) failed: errno {}", errno);

            return ret;
        }

        // Set TX and RX buffer sizes
        int option_value = RX_BUFFER_SIZE;
        if (setsockopt(m_fd, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char *>(&option_value), sizeof(option_value)) < 0) {
            ret.m_success = false;
            ret.m_msg = fmt::format("Error: setsockopt(SO_RCVBUF) failed: errno {}", errno);
            return ret;
        }

        option_value = TX_BUFFER_SIZE;
        if (setsockopt(m_fd, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char *>(&option_value), sizeof(option_value)) < 0) {
            ret.m_success = false;
            ret.m_msg = fmt::format("Error: setsockopt(SO_SNDBUF) failed: errno {}", errno);
            return ret;
        }

        sockaddr_in localAddr{};
        localAddr.sin_family = AF_INET;
        localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        localAddr.sin_port = htons(localPort);

        if (::bind(m_fd, reinterpret_cast<struct sockaddr *>(&localAddr), sizeof(localAddr)) < 0) {
            ret.m_success = false;
            ret.m_msg = fmt::format("Error: bind() failed: errno {}", errno);
            return ret;
        }

        ret.m_success = true;
        return ret;
    }

    /**
     * @brief Send a message over UDP
     *
     * @param msg - pointer to the message data
     * @param size - length of the message data
     * @return SocketRet - indication that the message was sent successfully
     */
    SocketRet sendMsg(const unsigned char *msg, size_t size) {
        SocketRet ret;
        // If destination addr/port specified
        if (m_sockaddr.sin_port != 0) {
            ssize_t numBytesSent =
                sendto(m_fd, &msg[0], size, 0, reinterpret_cast<struct sockaddr *>(&m_sockaddr), sizeof(m_sockaddr));
            if (numBytesSent < 0) {  // send failed
                ret.m_success = false;
                ret.m_msg = fmt::format("Error: errno {}", errno);

                return ret;
            }
            if (static_cast<size_t>(numBytesSent) < size) {  // not all bytes were sent
                ret.m_success = false;
                ret.m_msg = fmt::format("Only {} bytes of {} was sent to client", numBytesSent, size);
                return ret;
            }
        }
        ret.m_success = true;
        return ret;
    }

    /**
     * @brief Shutdown the UDP socket
     *
     * @return SocketRet - indication that the UDP socket was shut down successfully
     */
    SocketRet finish() {
        if (m_thread.joinable()) {
            m_stop = true;
            m_thread.join();
        }
        SocketRet ret;
        if (close(m_fd) == -1) {  // close failed
            ret.m_success = false;
            ret.m_msg = fmt::format("Error: errno {}", errno);

            return ret;
        }
        ret.m_success = true;
        return ret;
    }

private:
    /**
     * @brief Publish a UDP message received from a peer
     *
     * @param msg - pointer to the message data
     * @param msgSize - length of the message data
     */
    void publishUdpMsg(const char *msg, size_t msgSize) { m_callback.onReceiveData(msg, msgSize); }

    /**
     * @brief The receive thread for receiving data from UDP peer(s).
     */
    void ReceiveTask() {
        constexpr int64_t USEC_DELAY = 500000;
        while (!m_stop) {
            if (m_fd != -1) {
                fd_set fds;
                struct timeval tv {
                    0, USEC_DELAY
                };
                FD_ZERO(&fds);
                FD_SET(m_fd, &fds);
                int selectRet = select(m_fd + 1, &fds, nullptr, nullptr, &tv);
                if (selectRet <= 0) {  // select failed or timeout
                    if (m_stop) {
                        break;
                    }
                } else if (FD_ISSET(m_fd, &fds)) {

                    std::array<char, MAX_PACKET_SIZE> msg;
                    ssize_t numOfBytesReceived = recv(m_fd, &msg[0], MAX_PACKET_SIZE, 0);
                    if (numOfBytesReceived < 0) {
                        SocketRet ret;
                        ret.m_success = false;
                        m_stop = true;
                        if (numOfBytesReceived == 0) {
                            ret.m_msg = fmt::format("Closed connection");
                        } else {
                            ret.m_msg = fmt::format("Error: errno {}", errno);
                        }
                        break;
                    }
                    publishUdpMsg(&msg[0], static_cast<size_t>(numOfBytesReceived));
                }
            }
        }
    }

    /**
     * @brief The remote or multicast socket address
     */
    struct sockaddr_in m_sockaddr;

    /**
     * @brief The socket file descriptor
     */
    int m_fd;

    /**
     * @brief Indicator that the receive thread should exit
     */
    bool m_stop = false;

    /**
     * @brief Pointer to the callback recipient
     */
    T &m_callback;

    /**
     * @brief Handle of the receive thread
     */
    std::thread m_thread;

    const int TX_BUFFER_SIZE = 10240;
    const int RX_BUFFER_SIZE = 10240;
};