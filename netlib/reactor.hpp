#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <sys/poll.h>
#include <sys/epoll.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <functional>

#include <sys/sendfile.h>

#include <iostream>

#include "../kvstore.hpp"

#define BUFFER_LENGTH 1024
#define MAX_EVENTS_SIZE 1024
#define PORT 8888
#define HTMLPATH "./root/index.html"

namespace ns_reactor {
    class reactor;
    struct conn_item {
        conn_item() {};
        ~conn_item() {};
        union ar_func_t {
            ar_func_t() {};
            ~ar_func_t() {};
            std::function<int(reactor* ts, int)> accept_func;
            std::function<int(reactor* ts, int)> receive_func;
        };
        int fd;
        char rbuffer[BUFFER_LENGTH];
        int rlen;
        char wbuffer[BUFFER_LENGTH];
        int wlen;

        ar_func_t ar_func;
        std::function<int(reactor* ts, int)> send_func;
    };  
    class reactor {
    public:
        static reactor& get_Singleton() {
            static reactor singleton;
            return singleton;
        }
        ~reactor() = default;
        reactor(const reactor&) = delete;
        reactor& opeator(const reactor&) = delete;
        int init() {
            this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
            struct sockaddr_in serveraddr;
            
            memset(&serveraddr, 0, sizeof(struct sockaddr_in));

            serveraddr.sin_family = AF_INET;
            serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
            serveraddr.sin_port = htons(PORT);

            int opt = 1;
            setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &opt, sizeof(opt)); // 防止偶发性的服务器无法进行立即重启(tcp协议的时候再说)


            if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr)) < 0) {
                perror("bind error");
                return -1;
            }

            listen(sockfd, 10);

            this->epfd = epoll_create(1);
            set_event(sockfd, EPOLLIN, EPOLL_CTL_ADD);
            connlist[sockfd].fd = sockfd;
            connlist[sockfd].ar_func.accept_func = accept_cb;

            return 0;
        }
        int run() {
            while (1) {
                int nready = epoll_wait(this->epfd, events, MAX_EVENTS_SIZE, -1);  //阻塞等待
                for (int i = 0; i < nready; i++) {
                    int connfd = events[i].data.fd;
                    if (events[i].events & EPOLLIN) {
                        int cnt = connlist[connfd].ar_func.receive_func(this, connfd);
                    } 
                    else if (events[i].events & EPOLLOUT) {
                        int cnt  = connlist[connfd].send_func(this, connfd);
                    }
                }
            }
        }
    private:
        reactor() {}


        static int accept_cb(reactor* ts, int) {
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            int clientfd = accept(ts->sockfd, (struct sockaddr*)&clientaddr, &len);
            if (clientfd < 0) {
                return -1;
            }

            ts->set_event(clientfd, EPOLLIN, EPOLL_CTL_ADD);
            ts->connlist[clientfd].fd = clientfd;
            memset(ts->connlist[clientfd].rbuffer, 0, BUFFER_LENGTH);
            ts->connlist[clientfd].rlen = 0;
            memset(ts->connlist[clientfd].wbuffer, 0, BUFFER_LENGTH);
            ts->connlist[clientfd].wlen = 0;

            ts->connlist[clientfd].ar_func.receive_func = receive_cb;
            ts->connlist[clientfd].send_func = send_cb;

            return clientfd;
        }
        static int receive_cb(reactor* ts, int fd) {
            char* buffer = ts->connlist[fd].rbuffer;

            int cnt = recv(fd, buffer, BUFFER_LENGTH, 0);
            //导致get出错
            buffer[cnt] = 0;
            // std::cout << buffer << std::endl;        //打印接收到的数据

            if (cnt == 0) {
                //两个函数都需要做
                epoll_ctl(ts->epfd, EPOLL_CTL_DEL, fd, NULL);
                close(fd);
                return -1;
            }
            ts->connlist[fd].rlen = cnt;

            kv_request<conn_item>(ts->connlist[fd]);
            ts->connlist[fd].wlen = strlen(ts->connlist[fd].wbuffer);

            ts->set_event(fd, EPOLLOUT, EPOLL_CTL_MOD);
            return cnt;
        }
        static int send_cb(reactor* ts, int fd) {
            int send_len = send(fd, ts->connlist[fd].wbuffer, ts->connlist[fd].wlen, 0);
            ts->connlist[fd].wlen = 0;
            ts->set_event(fd, EPOLLIN, EPOLL_CTL_MOD);
            return send_len;
        }

        void set_event(int fd, EPOLL_EVENTS event, int op) { 
            struct epoll_event ev;
            ev.events = event;      //EPOLL_EVENTS EPOLLIN/EPOLLOUT
            ev.data.fd = fd;
            epoll_ctl(this->epfd, op, fd, &ev);   //op: EPOLL_CTL_ADD : 1 , EPOLL_CTL_MOD:3  (epoll.h)
        }


    private:   
        int sockfd;
        int epfd;
        struct epoll_event events[MAX_EVENTS_SIZE] = {0};
        conn_item connlist[MAX_EVENTS_SIZE];
    };
}
