#pragma once

#include"lcl/file/FileDescriptor.hpp"

#include<cstddef>
#include<string>
#include<vector>

namespace lcl::file{
    //读取整个文件到std：：string
    std::string read_all(const std::string& path);

    //将内容写入文件（覆盖）
    void write_all(const std::string& path,const std::string& data);

    //使用read/write循环赋值文件
    void copy_file(const std::string& src,const std::string& dst);

    //使用sendfile复制文件（内核零拷贝）
    void copy_file_sendfile(const std::string& src,const std::string& dst);
}