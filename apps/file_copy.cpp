#include"lcl/file/FileIO.hpp"

#include<iostream>

int main(int argc, char* argv[]){
    if(argc!=3){
        std::cerr<<"Usage:"<<argv[0]<<"<src><dst>\n";
        return 1;
    }

    try{
        lcl::file::copy_file(argv[1],argv[2]);
        std::cout<<"copied:"<<argv[1]<<"->"<<argv[2]<<"\n";
    }catch(const std::exception& e){
        std::cerr<<"Error:"<<e.what()<<"\n";
        return 1;

    }
    return 0;

}