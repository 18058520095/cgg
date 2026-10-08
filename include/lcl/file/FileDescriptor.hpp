#pragma once

#include<string>

namespace lcl::file{
    class FileDescriptor{
        public:
        FileDescriptor() noexcept = default;
        explicit FileDescriptor(int fd) noexcept;

        ~FileDescriptor();

        FileDescriptor(const FileDescriptor&) = delete;
        FileDescriptor& operator=(const FileDescriptor&) = delete;

        FileDescriptor(FileDescriptor&& other) noexcept;
        FileDescriptor& operator=(FileDescriptor&& other) noexcept;

        //释放旧的文件描述符，设置新的文件描述符
        void reset(int fd = -1) noexcept;

        //放弃所有权
        [[nodiscard]] int release() noexcept;

        [[nodiscard]] int get() const noexcept { return  fd_; }

        [[nodiscard]] bool valid() const noexcept{return fd_  >= 0;}

        explicit operator bool() const noexcept {return valid();}

        static FileDescriptor open(const std::string& path, int flags,mode_t mode = 0644);
        
        private:
        int fd_= -1;
    };
}