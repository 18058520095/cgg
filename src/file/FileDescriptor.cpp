#include"lcl/file/FileDescriptor.hpp"

#include<fcntl.h>
#include<unistd.h>
#include<cerrno>
#include<system_error>

namespace lcl::file{
    FileDescriptor::FileDescriptor(int fd)noexcept:fd_(fd){}

    FileDescriptor::~FileDescriptor(){
        if(fd_>=0){
            ::close(fd_);
        }
    }

    FileDescriptor::FileDescriptor(FileDescriptor&& other)noexcept:fd_(other.release()){}
    FileDescriptor& FileDescriptor::operator=(FileDescriptor&& other) noexcept{
        if(this!=&other){
            reset(other.release());
        }
        return *this;
    }

        //释放旧的文件描述符，设置新的文件描述符
        void FileDescriptor::reset(int fd) noexcept{
            if(fd_>=0){
                ::close(fd_);
            }
            fd_=fd;
        }

        //放弃所有权
        int FileDescriptor::release() noexcept{
            int fd=fd_;
            fd_=-1;
            return fd;
        }

        FileDescriptor FileDescriptor::open(const std::string& path, int flags,mode_t mode){
            int fd=::open(path.c_str(),flags,mode);
            if(fd<0){
                throw std::system_error(errno,std::generic_category(),"open:"+path);
            }
            return FileDescriptor(fd);
        }
}
