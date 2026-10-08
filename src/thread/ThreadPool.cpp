#include"lcl/thread/ThreadPool.hpp"
#include <cstdio>

namespace lcl::thread{
    ThreadPool::ThreadPool(std::size_t worker_count){
        if(worker_count==0)worker_count=1;
        workers_.reserve(worker_count);

        for(std::size_t i=0;i<worker_count;++i){
            workers_.emplace_back([this]{worker_loop();});
        }
    }

        ThreadPool::~ThreadPool(){
            shutdown();
        }


        void ThreadPool::submit(TaskQueue::Task task){
            queue_.push(std::move(task));
        }

        void ThreadPool::shutdown(){
            queue_.shutdown();
            for(auto& t:workers_){
                if(t.joinable()){
                    t.join();
                }
            }
            workers_.clear();
        }

        void ThreadPool::worker_loop(){
            while(true){
                auto task=queue_.pop();
                if(!task)break;
                try{
                    (*task)();
                }catch(const std::exception& e){
                    //记录，但不要让线程退出
                    std::fprintf(stderr,"task exception:%s\n",e.what());
                }
            }
        }
}