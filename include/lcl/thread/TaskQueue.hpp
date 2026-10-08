#pragma once

#include<condition_variable>
#include<functional>
#include<mutex>
#include<optional>
#include<queue>

namespace lcl::thread{

    //线程安全任务队列
    class TaskQueue{
        public:
        using Task=std::function<void()>;

        void push(Task task);

        //阻塞取任务；返回std：：nullopt表示关闭
        std::optional<Task>pop();

        //通知所有等待者退出
        void shutdown();

        private:
        std::queue<Task> queue_;
        std::mutex mutex_;
        std::condition_variable cond_;
        bool shutdown_=false;
    };
}