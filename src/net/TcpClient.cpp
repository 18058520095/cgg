#include"lcl/net/TcpClient.hpp"

namespace lcl::net{
    TcpClient::TcpClient(const std::string& ip, std::uint16_t port)
    :socket_(Socket::create(SocketType::Stream)){
        socket_.connect(SocketAddress(ip,port));
    }
}