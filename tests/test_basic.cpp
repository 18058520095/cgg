#include"lcl/file/FileIO.hpp"

#include<cassert>
#include<cstdio>
#include<iostream>
#include<string>

int main(){
    const std::string path="/tmp/lcl_test.txt";
    const std::string content="hello lcl\n";

    lcl::file::write_all(path,content);
    std::string read_back=lcl::file::read_all(path);

    assert(read_back==content);

    std::remove(path.c_str());
    std::cout<<"test_basic:all test passed\n";
    return 0;
}