#include"lcl/thread/TaskQueue.hpp"

namespace lcl::thread{
    void TaskQueue::push(Task task){
        //利用{}限制lock_guard生命周期
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if(shutdown_)return ;
            queue_.push(std::move(task));
        }

        cond_.notify_one();
    }

        //阻塞取任务；返回std：：nullopt表示关闭
        std::optional<TaskQueue::Task>TaskQueue::pop(){
            std::unique_lock<std::mutex>lock(mutex_);
            cond_.wait(lock,[this]{return shutdown_||!queue_.empty();});

            if(shutdown_&&queue_.empty()){
                return std::nullopt;
            }

            Task task=std::move(queue_.front());
            queue_.pop();
            return task;
        }

        //通知所有等待者退出
        void TaskQueue::shutdown(){
            {
                std::lock_guard<std::mutex>lock(mutex_);
                shutdown_=true;
            }
            cond_.notify_all();
        }

}