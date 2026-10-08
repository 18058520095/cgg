#include"lcl/net/TcpServer.hpp"

#include<poll.h>
#include<unistd.h>

#include<atomic>
#include<cerrno>
#include<csignal>
#include<iostream>

namespace{
    std::atomic<bool> g_stop{false};
    void on_signal(int){ g_stop.store(true);}
}

namespace lcl::net{

    TcpServer::TcpServer(const std::string& ip,std::uint16_t port,Handler handler)
    :listen_socket_(Socket::create(SocketType::Stream)),
    handler_(std::move(handler))
    {
        listen_socket_.set_reuseaddr(true);
        listen_socket_.bind(SocketAddress(ip,port));
        listen_socket_.listen(128);
    }

            //运行accept循环
    void TcpServer::run(){
        g_stop.store(false);
        std::signal(SIGINT, on_signal);
        std::signal(SIGTERM,on_signal);

        running_.store(true);
        std::cout<<"TcpServer running...\n";
        while(running_.load()&&!g_stop.load()){
            struct pollfd pfd{listen_socket_.get(),POLLIN,0};
            int ret=::poll(&pfd,1,500);
            if(ret<0){
                if(errno==EINTR)continue;
                std::cerr<<"poll error\n";
                break;
            }
            if(ret==0){
                continue;
            }
            try{
                SocketAddress peer;
                Socket conn = listen_socket_.accept(&peer);
                std::cout<<"client"<<peer.ip()<<":"<<peer.port()<<"connected\n";
                handler_(conn);
            }catch(const std::exception& e){
                std::cerr<<"accept error:"<<e.what()<<"\n";
            }
        }
    }

            //停止服务
    void TcpServer::stop(){
        running_.store(false);
    }
}