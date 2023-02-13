``` c++
#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>

const int MAX_EVENTS = 1024;
const int PORT = 8080;

int main() {
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        std::cerr << "socket error" << std::endl;
        return -1;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "bind error" << std::endl;
        return -1;
    }

    if (listen(listenfd, 10) < 0) {
        std::cerr << "listen error" << std::endl;
        return -1;
    }

    int epollfd = epoll_create(MAX_EVENTS);
    if (epollfd < 0) {
        std::cerr << "epoll_create error" << std::endl;
        return -1;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;    //这是水平触发, EPOLLIN | EPOLLET 边缘触发
    ev.data.fd = listenfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev) < 0) {
        std::cerr << "epoll_ctl error" << std::endl;
        return -1;
    }

    std::vector<struct epoll_event> events(MAX_EVENTS);
    while (true) {
        int nfds = epoll_wait(epollfd, &events[0], MAX_EVENTS, -1);
        if (nfds < 0) {
            std::cerr << "epoll_wait error" << std::endl;
            return -1;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == listenfd) {
                struct sockaddr_in clientaddr;
                socklen_t clientlen = sizeof(clientaddr);
                int connfd = accept(listenfd, (struct sockaddr*)&clientaddr, &clientlen);
                if (connfd < 0) {
                    std::cerr << "accept error" << std::endl;
                    return -1;
                }

                ev.events = EPOLLIN;
                ev.data.fd = connfd;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev) < 0) {
                    std::cerr << "epoll_ctl error" << std::endl;
                    return -1;
                }
            } else {
                char buf[1024];
                int n = read(events[i].data.fd, buf, sizeof(buf));
                if (n < 0) {
                    std::cerr << "read error" << std::endl;
                    return -1;
                } else if (n == 0) {
                    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL) < 0) {
                        std::cerr << "epoll_ctl error" << std::endl;
                        return -1;
                    }
                    close(events[i].data.fd);
                } else {
                    write(events[i].data.fd, buf, n);
                }
            }
        }
    }

    return 0;
}

```

``` c
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define MAX_EVENTS 64
#define PORT 8080
#define BUF_LEN 1024

int main(int argc, char *argv[])
{
    int listen_sock, conn_sock, nfds, epoll_fd;
    struct epoll_event ev, events[MAX_EVENTS];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    char buf[BUF_LEN];

    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == -1) {
        perror("socket");
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(listen_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        return 1;
    }

    if (listen(listen_sock, SOMAXCONN) == -1) {
        perror("listen");
        return 1;
    }

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        return 1;
    }

    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
        perror("epoll_ctl");
        return 1;
    }

    for (;;) {
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            return 1;
        }

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == listen_sock) {
                client_len = sizeof(client_addr);
                conn_sock = accept(listen_sock, (struct sockaddr*)&client_addr, &client_len);
                if (conn_sock == -1) {
                    perror("accept");
                    return 1;
                }

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_sock;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                    perror("epoll_ctl");
                    return 1;
                }
            } else {
                int done = 0;
                while (1) {
                    ssize_t count = read(events[n].data.fd, buf, BUF_LEN);
                    if (count == -1) {
                        if (errno != EAGAIN) {
                            perror("read");
                            done = 1;
                        }
                        break;
                    } else if (count == 0) {
                        done = 1;
                        break;
                    }

                    printf("recv: %s len: %d fd: %d n: %d nfds: %d  ", buf, count, events[n].data.fd, n, nfds);
                    write(1, buf, count);
                }
                
                if (done) {
                    printf("Closed connection on descriptor %d  ", events[n].data.fd);
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[n].data.fd, NULL) == -1) {
                        perror("epoll_ctl");
                        return 1;
                    }
                    close(events[n].data.fd);
                }
            }
        }
    }

    return 0;
}
```


``` c
// 边缘触发
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_EVENTS 10

int main() {
    int listener, conn_sock, nfds, epollfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;
    char buf[1024];
    struct epoll_event ev, events[MAX_EVENTS];

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("socket error");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(8888);

    if (bind(listener, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind error");
        exit(1);
    }

    if (listen(listener, 5) < 0) {
        perror("listen error");
        exit(1);
    }

    epollfd = epoll_create(10);
    if (epollfd < 0) {
        perror("epoll_create error");
        exit(1);
    }

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listener;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &ev) < 0) {
        perror("epoll_ctl error");
        exit(1);
    }

    while (1) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds < 0) {
            perror("epoll_wait error");
            exit(1);
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == listener) {
                conn_sock = accept(listener, (struct sockaddr*)&cli_addr, &cli_len);
                if (conn_sock < 0) {
                    perror("accept error");
                    continue;
                }

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_sock;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) < 0) {
                    perror("epoll_ctl error");
                    exit(1);
                }
            } else {
                int fd = events[i].data.fd;
                int len = read(fd, buf, sizeof(buf));
                if (len < 0) {
                    perror("read error");
                    exit(1);
                } else if (len == 0) {
                    close(fd);
                    continue;
                }

                printf("recv: %s len: %d fd: %d \r \n", buf, len, fd);
            }
        }
    }

    return 0;
}


```