#include"lcl/file/FileIO.hpp"

#include<fcntl.h>
#include<sys/sendfile.h>
#include<sys/stat.h>
#include<unistd.h>

#include<cerrno>
#include<system_error>

namespace lcl::file{

    std::string read_all(const std::string& path){
        auto fd=FileDescriptor::open(path,O_RDONLY);
        
        struct stat st{};
        if(::fstat(fd.get(),&st)<0){
            throw std::system_error(errno,std::generic_category(),"fstat:"+path);
        }

        std::string data;
        data.resize(static_cast<std::size_t>(st.st_size));

        std::size_t total=0;
        while(total<data.size()){
            ssize_t n=::read(fd.get(),data.data()+total,data.size()-total);
            if(n<0){
                throw std::system_error(errno,std::generic_category(),"read:"+path);
            }
            if(n==0){
                break;
            }
            total+=static_cast<std::size_t>(n);
        }
        data.resize(total);
        return data;
    }

    //将内容写入文件（覆盖）
    void write_all(const std::string& path,const std::string& data){
        auto fd=FileDescriptor::open(path,O_WRONLY|O_CREAT|O_TRUNC,0644);

        std::size_t total=0;
        while(total<data.size()){
            ssize_t n=::write(fd.get(),data.data()+total,data.size()-total);
            if(n<0){
                throw std::system_error(errno,std::generic_category(),"write:"+path);
            }
            total+=static_cast<std::size_t>(n);
        }
    }

    //使用read/write循环赋值文件
    void copy_file(const std::string& src,const std::string& dst){
        auto in=FileDescriptor::open(src,O_RDONLY);
        auto out=FileDescriptor::open(dst,O_WRONLY|O_CREAT|O_TRUNC,0644);

        constexpr std::size_t bufsize=64*1024;
        std::vector<char>buf(bufsize);

        while(true){
            ssize_t n=::read(in.get(),buf.data(),buf.size());
            if(n<0){
                throw std::system_error(errno,std::generic_category(),"read:"+src); 
        }
            if(n==0){
                break;
        }

            ssize_t written=0;
            while(written<n){
                ssize_t w=::write(out.get(),buf.data()+written,n-written);
                if(w<0){
                throw std::system_error(errno,std::generic_category(),"write:"+dst);
                }
                written+=w;
            }
        }
    }

    //使用sendfile复制文件（内核零拷贝）
    void copy_file_sendfile(const std::string& src,const std::string& dst){
        auto in=FileDescriptor::open(src,O_RDONLY);
        auto out=FileDescriptor::open(dst,O_WRONLY|O_CREAT|O_TRUNC,0644);

        struct stat st{};
        if(::fstat(in.get(),&st)<0){
            throw std::system_error(errno,std::generic_category(),"fstat:"+src);
        }

        off_t offset=0;
        off_t remain=st.st_size;
        while(remain>0){
            ssize_t n=::sendfile(out.get(),in.get(),&offset,static_cast<size_t>(remain));
            if(n<0){
                throw std::system_error(errno,std::generic_category(),"sendfile:"+src);
            }
            remain-=n;
        }
    }
}