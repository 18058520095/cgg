#include<lcl/net/Socket.hpp>
#include<lcl/net/SocketAddress.hpp>

#include<string>
#include<cstdlib>
#include<iostream>

int main(int argc,char* argv[]){
    if(argc!=3){
        std::cerr<<"Usage:"<<argv[1]<<"<ip><port>\n";
        return 1;
    }

    try{
        using namespace lcl::net;

        auto sock=Socket::create(SocketType::Datagram);
        SocketAddress server(argv[1],static_cast<std::uint16_t>(std::atoi(argv[2])));

        std::string line;
        char buf[4096];
        while(std::getline(std::cin,line)){
            line+='\n';
            sock.send_to(line.data(),line.size(),server);

            SocketAddress from;
            ssize_t n=sock.recv_from(buf,sizeof(buf)-1,&from);
            buf[n]='\0';
            std::cout<<"echo:"<<buf;
        }
    }catch(const std::exception& e){
        std::cerr<<"error:"<<e.what()<<"\n";
        return 1;
    }
    return 0;
}