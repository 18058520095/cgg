#pragma once

#include<atomic>
#include<cstdint>
#include<string>
#include<sys/types.h>
#include<vector>

namespace lcl::process{

    struct WorkerInfo
    {
        pid_t pid=-1;
        int pipe_fd=-1;     //父进程一侧
        bool busy=false;
    };
    
    //fork+socketpair+SCM_RIGHTS传递fd的C++进程池
    class ProcessPool{
        public:
        ProcessPool(const std::string& ip,std::uint16_t port,std::size_t worker_count);
        ~ProcessPool();

        ProcessPool(const ProcessPool&)=delete;
        ProcessPool& operator=(const ProcessPool&)=delete;

        //阻塞运行accept循环，收到SIGINT/SIGTERM后退出
        void run();

        //外部请求停止
        void stop()noexcept;
        
        private:
        void make_workers();
        void worker_loop(int pipe_fd);
        void cleanup();

        static void send_fd(int pipe_fd,int fd);
        static int recv_fd(int pipe_fd);

        std::string ip_;
        std::uint16_t port_;
        std::size_t worker_count_;
        std::vector<WorkerInfo> workers_;
    };
}