#include"lcl/process/ProssPool.hpp"

#include<cstdlib>
#include<iostream>

int main(int argc,char * argv[]){
    if(argc!=4){
        std::cerr<<"Usage:"<<argv[0]<<"<IP><PORT>\n";
        return 1;
    }

    try{
        lcl::process::ProcessPool pool(
            argv[1],
            static_cast<std::uint16_t>(std::atoi(argv[2])),
            static_cast<std::size_t>(std::atoi(argv[3]))
        );
        pool.run();
    }catch(const std::exception& e){
        std::cerr<<"fatal:"<<e.what()<<"\n";
        return 1;
    }
}