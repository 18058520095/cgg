#include"lcl/net/SocketAddress.hpp"

#include <cstring>
#include <stdexcept>

namespace lcl::net{
    SocketAddress::SocketAddress() noexcept{
        std::memset(&addr_,0,sizeof(addr_));
        addr_.sin_family=AF_INET;
    }
    SocketAddress::SocketAddress(const std::string& ip,std::uint16_t port){
        std::memset(&addr_,0,sizeof(addr_));
        addr_.sin_family=AF_INET;
        addr_.sin_port=htons(port);
        if(::inet_pton(AF_INET,ip.c_str(),&addr_.sin_addr)<=0){
            throw std::runtime_error("invalid IP address"+ip);
        }
    }

    std::string SocketAddress::ip() const{
        char buf[INET_ADDRSTRLEN];
        ::inet_ntop(AF_INET,&addr_.sin_addr,buf,sizeof(buf));
        return buf;
    }
    std::uint16_t SocketAddress::port() const{
        return ntohs(addr_.sin_port);
    }
}