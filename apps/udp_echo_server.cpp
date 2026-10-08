#include<lcl/net/Socket.hpp>
#include<lcl/net/SocketAddress.hpp>

#include<atomic>
#include<csignal>
#include<cstdlib>
#include<iostream>
#include<poll.h>

namespace{
    std::atomic<bool> g_running(true);
    void on_signal(int){
        g_running.store(false);
    }
}

int main(int argc,char* argv[]){
    if(argc!=3){
        std::cerr<<"Usage:"<<argv[0]<<"<ip><port>\n";
        return 1;
    }

    std::signal(SIGINT,on_signal);
    std::signal(SIGTERM,on_signal);

    try{
        using namespace lcl::net;

        auto sock=Socket::create(SocketType::Datagram);
        sock.set_reuseaddr(true);
        sock.bind(SocketAddress(argv[1],static_cast<std::uint16_t>(std::atoi(argv[2]))));

        std::cout<<"udp echo server listening on"<<argv[1]<<":"<<argv[2]<<"\n";

        char buf[4096];
        while(g_running.load()){
            struct pollfd pfd{ sock.get(),POLLIN,0};
            int ret=::poll(&pfd,1,500);
            if(ret<0){
                if(errno==EINTR)continue;
                break;
            }
            if(ret==0){
                continue;
            }

            try{
                SocketAddress peer;
                ssize_t n=sock.recv_from(buf,sizeof(buf),&peer);
                std::cout<<"recv:"<<n<<"bytes from"<<peer.ip()<<":"<<peer.port()<<"\n";
                sock.send_to(buf,static_cast<std::size_t>(n),peer);
            }catch(const std::exception& e){
                std::cerr<<"upd error:"<<e.what()<<"\n";
            }
        }
        std::cout<<"upd server exiting\n";
    }catch(const std::exception& e){
        std::cerr<<"fatal:"<<e.what()<<"\n";
        return 1;
    }
}