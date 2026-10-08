#include"lcl/net/TcpClient.hpp"

#include<iostream>
#include<cstdlib>
#include<string>

int main(int argc, char* argv[]){
    if(argc!=3){
        std::cerr<<"Usage:"<<argv[0]<<"<host><port>\n";
        return 1;
    }

    try{
        lcl::net::TcpClient client(argv[1],static_cast<std::uint16_t>(std::atoi(argv[2])));
        auto& sock=client.socket();

        std::string line;
        while(std::getline(std::cin,line)){
            line+='\n';
            sock.send_all(line.data(),line.size());

            char buf[4096];
            ssize_t n=sock.recv(buf,sizeof(buf));
            if(n==0){
                break;
            }
            std::cout.write(buf,n);
        }
    }catch(const std::exception& e){
        std::cerr<<"Error:"<<e.what()<<"\n";
        return 1;
    }
    return 0;
}