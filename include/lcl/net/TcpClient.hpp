#pragma once

#include"lcl/net/Socket.hpp"

#include<string>

namespace lcl::net{
    class TcpClient{
        public:
        TcpClient(const std::string& ip,std::uint16_t port);

        Socket& socket()noexcept{
            return socket_;
        }

        private:
        Socket socket_;
    };
}