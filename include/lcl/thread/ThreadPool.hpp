#pragma once

#include"lcl/thread/TaskQueue.hpp"

#include<thread>
#include<vector>

namespace lcl::thread{

    //固定线程数线程池
    class ThreadPool{
        public:
        explicit ThreadPool(std::size_t worker_count=std::thread::hardware_concurrency());
        ~ThreadPool();

        ThreadPool(const ThreadPool&)=delete;
        ThreadPool& operator=(const ThreadPool)=delete;

        void submit(TaskQueue::Task task);

        void shutdown();

        private:
        void worker_loop();

        TaskQueue queue_;
        std::vector<std::thread> workers_;
    };
}