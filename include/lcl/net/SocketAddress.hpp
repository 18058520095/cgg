#pragma once

#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>

#include<cstdint>
#include<string>

namespace lcl::net{

    //封装IPV4地址和端口
    class SocketAddress{
    public:
        SocketAddress() noexcept;
        SocketAddress(const std::string& ip,std::uint16_t port);

        [[nodiscard]] const sockaddr* addr() const noexcept{
            return reinterpret_cast<const sockaddr*>(&addr_);
        }
        [[nodiscard]] sockaddr* addr()noexcept{
            return reinterpret_cast<sockaddr*>(&addr_);
        }
        [[nodiscard]]socklen_t length() const noexcept{
            return sizeof(addr_);
        }
        [[nodiscard]]std::string ip() const;
        [[nodiscard]]std::uint16_t port() const;
    private:
        sockaddr_in addr_{};
    };
}