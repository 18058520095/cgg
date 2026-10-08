#include"lcl/net/Socket.hpp"

#include<fcntl.h>
#include<unistd.h>

#include<cerrno>
#include<system_error>
#include <cstring>
#include <stdexcept>

namespace lcl::net{

    Socket Socket::create(SocketType type){
        int fd=::socket(AF_INET,type==SocketType::Stream?SOCK_STREAM:SOCK_DGRAM,0);
        if(fd<0){
            throw std::system_error(errno,std::system_category(),"socket");
        }
        Socket s(fd);
        s.type_=type;
        return s;
    }

    Socket Socket::from_fd(int fd){
        Socket s(fd);
        return s;
    }

    void Socket::bind(const SocketAddress& addr){
        if(::bind(fd_.get(),addr.addr(),addr.length())<0){
            throw std::system_error(errno,std::system_category(),"bind");
        }
    }
    void Socket::listen(int backlog){
        if(::listen(fd_.get(),backlog)<0){
            throw std::system_error(errno,std::system_category(),"listen");
        }
    }
    Socket Socket::accept(SocketAddress* peer){
        sockaddr_in addr{};
        socklen_t len=sizeof(addr);

        int cfd=::accept(fd_.get(),peer?reinterpret_cast<sockaddr*>(&addr):nullptr,peer?&len:nullptr);
        if(cfd<0){
            throw std::system_error(errno,std::system_category(),"accept");
        }
        if(peer){
            *peer=SocketAddress();

            //让peer->addr_填充
            std::memcpy(peer->addr(),&addr,sizeof(addr));
        }
        Socket s(cfd);
        s.type_=SocketType::Stream;
        return s;
    }

    void Socket::connect(const SocketAddress& addr){
        if(::connect(fd_.get(),addr.addr(),addr.length())<0){
            throw std::system_error(errno,std::system_category(),"connect");
        }
    }

    ssize_t Socket::send(const void* data,std::size_t len,int flags){
        ssize_t n=::send(fd_.get(),data,len,flags);
        if(n<0){
            throw std::system_error(errno,std::system_category(),"send");
        }
        return n;
    }
    ssize_t Socket::recv(void* data,std::size_t len,int flags){
        ssize_t n=::recv(fd_.get(),data,len,flags);
        if(n<0){
            throw std::system_error(errno,std::system_category(),"recv");
        }
        return n;
    }

    void Socket::send_all(const void* data,std::size_t len,int flags){
        const char* p=static_cast<const char*>(data);
        std::size_t total=0;
        while(total<len){
            ssize_t n=::send(fd_.get(),p+total,len-total,flags);
            total+=static_cast<std::size_t>(n);
        }
    }
    void Socket::recv_all(void* buf,std::size_t len,int flags){
        char* p=static_cast<char*>(buf);
        std::size_t total=0;
        while(total<len){
            ssize_t n=::recv(fd_.get(),p+total,len-total,flags);
            if(n<=0){
                throw std::system_error(errno,std::system_category(),"recv");
            }
            if(n==0){
                throw std::runtime_error("connection closed");
            }
            total+=static_cast<std::size_t>(n);
        }
    }

    ssize_t Socket::send_to(const void* data,std::size_t len,const SocketAddress& to){
        ssize_t n=::sendto(fd_.get(),data,len,0,to.addr(),to.length());
        if(n<0){
            throw std::system_error(errno,std::system_category(),"sendto");
        }
        return n;
    }
    ssize_t Socket::recv_from(void* data,std::size_t len,SocketAddress* from){
        sockaddr_in addr{};
        socklen_t alen=sizeof(addr);
        ssize_t n=::recvfrom(fd_.get(),data,len,0,from?reinterpret_cast<sockaddr*>(&addr):nullptr,from?&alen:nullptr);
        if(n<0){
            throw std::system_error(errno,std::system_category(),"recvfrom");
        }
        if(from){
            std::memcpy(from->addr(),&addr,sizeof(addr));
        }
        return n;
    }

    void Socket::set_reuseaddr(bool on){
        int v=on?1:0;
        if(::setsockopt(fd_.get(),SOL_SOCKET,SO_REUSEADDR,&v,sizeof(v))<0){
            throw std::system_error(errno,std::system_category(),"setsockopt");
        }
    }
    void Socket::set_nonblocking(bool on){
        int flags=::fcntl(fd_.get(),F_GETFL,0);
        if(flags<0){
            throw std::system_error(errno,std::system_category(),"fcntl F_GETFL");
        }
        flags=on?(flags|O_NONBLOCK):(flags&~O_NONBLOCK);
        if(::fcntl(fd_.get(),F_SETFL,flags)<0){
            throw std::system_error(errno,std::system_category(),"fcntl F_SETFL");
        }
    }
}