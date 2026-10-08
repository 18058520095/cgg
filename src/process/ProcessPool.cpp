#include"lcl/process/ProssPool.hpp"
#include"lcl/net/Socket.hpp"
#include"lcl/net/SocketAddress.hpp"

#include<sys/epoll.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<unistd.h>

#include<atomic>
#include<cerrno>
#include<cstring>
#include<csignal>
#include<iostream>
#include<system_error>

namespace{
    std::atomic<bool> g_stop{false};
    void on_signal(int){ g_stop.store(true);}
}

namespace lcl::process{
    using lcl::net::Socket;
    using lcl::net::SocketAddress;
    using lcl::net::SocketType;

    ProcessPool::ProcessPool(const std::string& ip,std::uint16_t port,std::size_t wc):ip_(ip),port_(port),worker_count_(wc){}

    ProcessPool::~ProcessPool(){
            cleanup();
        }

    //阻塞运行accept循环，收到SIGINT/SIGTERM后退出
    void ProcessPool::run(){
            g_stop.store(false);
            std::signal(SIGINT,on_signal);
            std::signal(SIGTERM,on_signal);

            auto listen_sock=Socket::create(SocketType::Stream);
            listen_sock.set_reuseaddr(true);
            listen_sock.bind(SocketAddress(ip_,port_));
            listen_sock.listen(128);

            make_workers();

            int epfd=::epoll_create1(0);
            if(epfd<0){
                throw std::system_error(errno,std::generic_category(),"epoll_create1");
            }

            struct epoll_event ev{},events[64];
            ev.events=EPOLLIN;
            ev.data.fd=listen_sock.get();
            ::epoll_ctl(epfd,EPOLL_CTL_ADD,listen_sock.get(),&ev);

            for(auto& w:workers_){
            ev.events=EPOLLIN;
            ev.data.fd=w.pipe_fd;
            }

            std::cout<<"Process running on "<<ip_<<":"<<port_<<"with"<<worker_count_<<"workers\n";

            while(!g_stop.load()){
                int n=::epoll_wait(epfd,events,64,500);
                if(n<0){
                    if(errno==EINTR)continue;
                    break;
                }
                if(n==0)continue;

                for(int i=0;i<n;i++){
                    int fd=events[i].data.fd;

                    if(fd==listen_sock.get()){
                        try{
                            auto conn=listen_sock.accept();
                            int client_fd=conn.release_fd();

                            bool dispatched=false;
                            for(auto&w:workers_){
                                if(!w.busy){
                                    try{
                                        send_fd(w.pipe_fd,client_fd);
                                        w.busy=true;
                                        dispatched=true;
                                        std::cout<<"dispatch to worker pid="<<w.pid<<"\n";
                                    }catch(const std::exception& e){
                                        std::cerr<<"send_fd"<<e.what()<<"\n";
                                    }
                                    break;
                                }
                            }
                            if(!dispatched){
                                ::close(client_fd);
                                std::cerr<<"no idle worker,dropped client\n";
                            }
                        }catch(const std::exception& e){
                                        std::cerr<<"accept"<<e.what()<<"\n";
                                    }
                    }else{
                        for(auto&w:workers_){
                            if(w.pipe_fd==fd){
                                char ack;
                                ssize_t bytes_read=::read(w.pipe_fd,&ack,1);
                                if(bytes_read==1){
                                    w.busy=false;
                                    std::cout<<"worker pid="<<w.pid<<" idle\n";
                                }else if(bytes_read<0&&errno==EINTR){
                                    continue;
                                }else if(bytes_read<0){
                                    std::cerr<<"read worker acknowledgment: "
                                             <<std::strerror(errno)<<"\n";
                                }else{
                                    std::cerr<<"worker acknowledgment pipe closed\n";
                                }
                                break;
                            }
                        }
                    }
                }
            }

            std::cout<<"ProcessPool stopping...\n";
            ::close(epfd);
            cleanup();
            std::cout<<"ProcessPool stopped\n";
        }



    //外部请求停止
    void ProcessPool::stop()noexcept{
            g_stop.store(true);
        }
        
    void ProcessPool::make_workers(){
            workers_.resize(worker_count_);

            for(std::size_t i=0;i<worker_count_;i++){
                int pipefd[2];
                if(::socketpair(AF_UNIX,SOCK_STREAM,0,pipefd)<0){
                    throw std::system_error(errno,std::generic_category(),"sockerpair");
                }

                pid_t pid=::fork();
                if(pid<0){
                    throw std::system_error(errno,std::generic_category(),"fork");
                }

                if(pid==0){
                    ::close(pipefd[0]);
                    worker_loop(pipefd[1]);
                    ::_exit(0);
                }

                ::close(pipefd[1]);
                workers_[i].pid=pid;
                workers_[i].pipe_fd=pipefd[0];
                workers_[i].busy=false;
            }
        }

    void ProcessPool::worker_loop(int pipe_fd){
        //子进程：重置信号，避免继承父进程的handler
        std::signal(SIGINT,SIG_IGN);
        std::signal(SIGTERM,SIG_DFL);

        while(true){
                int client=recv_fd(pipe_fd);
                if(client<0)break;

                try{
                    char buf[4096];
                    while(true){
                        ssize_t n=::recv(client,buf,sizeof(buf),0);
                        if(n<0)break;
                        ::send(client,buf,static_cast<std::size_t>(n),0);
                    }
                }catch(...){}

                ::close(client);

                char ack='A';
                ssize_t w;
                do{
                    w=::write(pipe_fd,&ack,1);
                }while(w<0&&errno==EINTR);
            }
            ::_exit(0);
        }

    void ProcessPool::cleanup(){
            for(auto& w:workers_){
                if(w.pipe_fd>=0){
                    ::close(w.pipe_fd);
                    w.pipe_fd=-1;
                }
            }
            for(auto& w:workers_){
                if(w.pid>0){
                    ::kill(w.pid,SIGTERM);
                    ::waitpid(w.pid,nullptr,0);
                    w.pid=-1;
                }
            }
            workers_.clear();
        }

    void ProcessPool::send_fd(int pipe_fd,int fd){
        struct msghdr msg{};
        struct iovec iov[1];
        char dummy='F';

        iov[0].iov_base=&dummy;
        iov[0].iov_len=1;
        msg.msg_iov=iov;
        msg.msg_iovlen=1;

        char cmsgbuf[CMSG_SPACE(sizeof(int))]{};
        msg.msg_control=cmsgbuf;
        msg.msg_controllen=sizeof(cmsgbuf);

        auto* cmsg=CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level=SOL_SOCKET;
        cmsg->cmsg_type=SCM_RIGHTS;
        cmsg->cmsg_len=CMSG_LEN(sizeof(int));
        std::memcpy(CMSG_DATA(cmsg),&fd,sizeof(int));

        if(::sendmsg(pipe_fd,&msg,0)<0){
            throw std::system_error(errno,std::generic_category(),"sendmsg");
        }
        
    }

    int ProcessPool::recv_fd(int pipe_fd){
        struct msghdr msg{};
        struct iovec iov[1];
        char dummy=0;

        iov[0].iov_base=&dummy;
        iov[0].iov_len=1;
        msg.msg_iov=iov;
        msg.msg_iovlen=1;

        char cmsgbuf[CMSG_SPACE(sizeof(int))]{};
        msg.msg_control=cmsgbuf;
        msg.msg_controllen=sizeof(cmsgbuf);

        ssize_t n;
        do{
            n=::recvmsg(pipe_fd,&msg,0);
        }while(n<0&&errno==EINTR);

        if(n<=0){
            return -1;
        }

        auto* cmsg=CMSG_FIRSTHDR(&msg);
        if(!cmsg||cmsg->cmsg_type!=SCM_RIGHTS)return -1;

        int fd=-1;
        std::memcpy(&fd,CMSG_DATA(cmsg),sizeof(int));
        return fd;
    }
}