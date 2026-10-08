#pragma once

#include"lcl/file/FileDescriptor.hpp"
#include"lcl/net/SocketAddress.hpp"

#include <string>

namespace lcl::net{
    enum class SocketType{
        Stream,     //TCP
        Datagram,   //UDP
    };

    //RALL封装socket
    class Socket{
        public:
        Socket()noexcept=default;
        explicit Socket(int fd)noexcept:fd_(fd){}

        SocketType type_;
        file::FileDescriptor fd_;

        static Socket create(SocketType type);
        static Socket from_fd(int fd);

        void bind(const SocketAddress& addr);
        void listen(int backlog=128);
        Socket accept(SocketAddress* peer=nullptr);

        void connect(const SocketAddress& addr);

        ssize_t send(const void* data,std::size_t len,int flags=0);
        ssize_t recv(void* data,std::size_t len,int flags=0);   

        void send_all(const void* data,std::size_t len,int flags=0);
        void recv_all(void* data,std::size_t len,int flags=0);

        ssize_t send_to(const void* data,std::size_t len,const SocketAddress& to);
        ssize_t recv_from(void* buf,std::size_t len,SocketAddress* from);

        void set_reuseaddr(bool on=true);
        void set_nonblocking(bool on=true);

        [[nodiscard]] int get() const noexcept{
            return fd_.get();
        }
        [[nodiscard]] bool valid() const noexcept{
            return fd_.valid();
        }
        [[nodiscard]] int release_fd() noexcept{
            return fd_.release();}
    };
}