#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#epoll
#include <sys/epoll.h>
#include <sys/syscall.h>

#define UNIX_DOMAIN "/tmp/UNIX.domain"

//epoll水平触发
#define EPOLL_LT 0
//epoll边缘触发
#define EPOLL_ET 1

EPOLL_LT    //水平触发  一直触发
EPOLL_ET    //边缘触发  只触发一次



//获取系统最大文件描述符
int get_sys_max_fd(void)
{
    struct rlimit rlim;

    if (getrlimit(RLIMIT_NOFILE, &rlim) < 0) {
        return -1;
    }

    return rlim.rlim_cur;
}

//设置非阻塞
int set_nonblock(int fd)
{
    int flags;

    if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
        return -1;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        return -1;
    }

    return 0;
}

//执行系统命令
int do_system(const char *cmd)
{
    int status;

    if (cmd == NULL) {
        return -1;
    }

    status = system(cmd);
    if (status < 0) {
        return -1;
    }

    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == 0) {
            return 0;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}

//设置文件描述符可重用
int set_reuseaddr(int fd)
{
    int opt = 1;

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        return -1;
    }

    return 0;
}

//屏蔽管道破裂信号
void mask_pipe(void)
{
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    sigprocmask(SIG_BLOCK, &set, NULL);
}


int listen_domain_socket(char *path);
int epoll_init(int fd);
int epoll_listen(int fd);
void do_use_fd(int fd);

int main(int argc, char *argv[])
{
    int fd, epfd;

    if ((fd = listen_domain_socket(UNIX_DOMAIN)) < 0) {
        return -1;
    }

    if ((epfd = epoll_init(fd)) < 0) {
        return -1;
    }

    epoll_listen(epfd);

    return 0;
}

//域套接字accept
void do_use_fd(int fd)
{
    int newfd;
    struct sockaddr_un addr;
    socklen_t len = sizeof(addr);

    if ((newfd = accept(fd, (struct sockaddr *)&addr, &len)) < 0) {
        perror("accept");
        return;
    }

    printf("accept a new client: %s %d %d %d %d", addr.sun_path, addr.sun_family, addr.sun_path[0], addr.sun_path[1], addr.sun_path[2]);
    
        close(newfd);
    }
}
//监听域套接字
int listen_domain_socket(char *path)
{
    int fd;
    struct sockaddr_un addr;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(fd);
        return -1;
    }

    if (listen(fd, 5) < 0) {
        perror("listen");
        close(fd);
        return -1;
    }

    return fd;
}
//epoll_create
int epoll_create(int size)
{
    int epfd;

    epfd = syscall(__NR_epoll_create, size);
    if (epfd == -1) {
        errno = -epfd;
        epfd = -1;
    }

    return epfd;
}
//epoll_ctl
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
    int ret;

    ret = syscall(__NR_epoll_ctl, epfd, op, fd, event);
    if (ret == -1) {
        errno = -ret;
        ret = -1;
    }

    return ret;
}
//epoll_wait
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
    int ret;

    ret = syscall(__NR_epoll_wait, epfd, events, maxevents, timeout);
    if (ret == -1) {
        errno = -ret;
        ret = -1;
    }

    return ret;
}
//epoll_create
int epoll_create(int size)
{
    int epfd;

    epfd = syscall(__NR_epoll_create, size);
    if (epfd == -1) {
        errno = -epfd;
        epfd = -1;
    }

    return epfd;
}

//epollinit
int epoll_init(int fd)
{
    int epfd;
    struct epoll_event ev;

    if ((epfd = epoll_create(256)) < 0) {
        perror("epoll_create");
        return -1;
    }

    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        perror("epoll_ctl");
        return -1;
    }

    return epfd;
}

//do_use_fd
void do_use_fd(int fd)
{
    char buf[1024];
    int len;

    len = recv(fd, buf, sizeof(buf), 0);
    if (len < 0) {
        perror("recv");
        return;
    }

    printf("recv: %s len: %d fd: %d\n", buf, len, fd);
}

//epoll监听
int epoll_listen(int fd)
{
    int epfd;
    struct epoll_event ev, events[20];
    int nfds, n, i;

    if ((epfd = epoll_create(256)) < 0) {
        perror("epoll_create");
        return -1;
    }

    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        perror("epoll_ctl");
        return -1;
    }

    while (1) {
        nfds = epoll_wait(epfd, events, 20, 500);
        for (i = 0; i < nfds; ++i) {
            if (events[i].data.fd == fd) {
                n = accept(fd, NULL, NULL);
                if (n < 0) {
                    perror("accept");
                    return -1;
                }
                ev.events = EPOLLIN;
                ev.data.fd = n;
                if (epoll_ctl(epfd, EPOLL_CTL_ADD, n, &ev) < 0) {
                    perror("epoll_ctl");
                    return -1;
                }
            } else {
                do_use_fd(events[i].data.fd);
                epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, &ev);
                close(events[i].data.fd);
            }
        }
    }

    return 0;
}

//获取域套接字
int get_domain_socket(char *path)
{
    int fd;
    struct sockaddr_un addr;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(fd);
        return -1;
    }

    return fd;
}

//发送数据
int send_data(int fd, char *buf, int len)
{
    int ret;
    int send_len = 0;

    while (send_len < len) {
        ret = send(fd, buf + send_len, len - send_len, 0);
        if (ret < 0) {
            perror("send");
            return -1;
        }
        send_len += ret;
    }

    return send_len;
}

//接收数据
int recv_data(int fd, char *buf, int len)
{
    int ret;
    int recv_len = 0;

    while (recv_len < len) {
        ret = recv(fd, buf + recv_len, len - recv_len, 0);
        if (ret < 0) {
            perror("recv");
            return -1;
        }
        recv_len += ret;
    }

    return recv_len;
}

//关闭套接字
void close_socket(int fd)
{
    close(fd);
}

//主函数
int main(int argc, char *argv[])
{
    int fd;
    char buf[1024];

    if (argc != 2) {
        printf("Usage: %s path\n", argv[0]);
        return -1;
    }

    if ((fd = get_domain_socket(argv[1])) < 0) {
        return -1;
    }

    while (1) {
        memset(buf, 0, sizeof(buf));
        printf("Please input: ");
        scanf("%s", buf);
        if (send_data(fd, buf, strlen(buf)) < 0) {
            break;
        }
        if (recv_data(fd, buf, sizeof(buf)) < 0) {
            break;
        }
        printf("recv: %s\n", buf);
    }

    close_socket(fd);

    return 0;
}

