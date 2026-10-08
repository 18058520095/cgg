#include"lcl/net/TcpServer.hpp"

#include<iostream>
#include<cstdlib>

int main(int argc, char* argv[]){
    if(argc!=3){
        std::cerr<<"Usage:"<<argv[0]<<"<port>\n";
        return 1;
    }

    try{
        lcl::net::TcpServer server(argv[1],static_cast<std::uint16_t>(std::atoi(argv[2])),[](lcl::net::Socket& conn){
            char buf[4096];
            while(true){
                ssize_t n=conn.recv(buf,sizeof(buf));
                if(n<=0){
                    break;
                }
                conn.send_all(buf,static_cast<std::size_t>(n));
            }
        });
        server.run();
    }catch(const std::exception& e){
        std::cerr<<"Error:"<<e.what()<<"\n";
        return 1;
    }
    return 0;
}