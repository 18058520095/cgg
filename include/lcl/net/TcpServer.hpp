#pragma once

#include"lcl/net/Socket.hpp"

#include <functional>
#include<atomic>
#include<string>
#include<cstdlib>

namespace lcl::net{
    class TcpServer{
        public:
            using Handler = std::function<void(Socket&)>;

            TcpServer(const std::string& ip,std::uint16_t port,Handler handler);

            //运行accept循环
            void run();

            //停止服务
            void stop();

        private:
            Socket listen_socket_;
            Handler handler_;
            std::atomic<bool> running_{false};
    };
}