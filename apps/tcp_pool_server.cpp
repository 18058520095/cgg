#include"lcl/net/Socket.hpp"
#include"lcl/net/SocketAddress.hpp"
#include"lcl/thread/ThreadPool.hpp"

#include<iostream>
#include<cstdlib>

int main(int argc,char* argv[]){
    if(argc!=4){
        std::cerr<<"Usage: "<<argv[0]<<" <port> <thread_num>"<<std::endl;
        return 1;
    }

    try{
        using namespace lcl;

        auto listen_sock=net::Socket::create(net::SocketType::Stream);
        listen_sock.set_reuseaddr(true);
        listen_sock.bind(net::SocketAddress(argv[1],static_cast<std::uint16_t>(std::atoi(argv[2]))));
        listen_sock.listen(128);

        thread::ThreadPool pool(static_cast<std::size_t>(std::atoi(argv[3])));
        
        std::cout<<"tcp_pool_server listening on"<<argv[1]<<":"<<argv[2]<<"with"<<argv[3]<<"workers\n";

        while(true){
            auto conn=listen_sock.accept();
            auto conn_ptr=std::make_shared<net::Socket>(std::move(conn));

            pool.submit([conn_ptr]{
                try{
                    char buf[4096];
                    while(true){
                        ssize_t n=conn_ptr->recv(buf,sizeof(buf));
                        if(n==0)break;
                        conn_ptr->send_all(buf,static_cast<std::size_t>(n));
                    }
                }catch(const std::exception&){
                    //客户端断开，忽略
                }
            });
        }
    }catch(const std::exception& e){
        std::cerr<<"error"<<e.what()<<"\n";
        return 1;
    }
    return 0;
}