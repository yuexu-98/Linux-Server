#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

int main(){

    // (1) 创建 
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    // (2) 绑定
    bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));

    // (3) 监听
    listen(lfd, 8);


    int epfd = epoll_create(100);

    struct spoll_event epev;
    epev.events = EPOLL_IN;
    epev.data.fd = lfd;

    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &epev)



    // (4) 关闭
    close(lfd);

    return 0;
}