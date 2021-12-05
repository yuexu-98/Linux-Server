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


    // -- MARK 

    // 调用epoll_create()在内核创建一个epoll实例
    int epfd = epoll_create(100);
    

    // 将监听的文件描述符相关的检测信息添加到epoll实例中
    struct epoll_event epev;
    epev.events = EPOLL_IN;
    epev.data.fd = lfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &epev)


    struct epoll_event epevs[1024];
    while(1){
        int ret = epoll_wait(epfd, epevs, 1024, -1);
        if(ret == -1){
            perror("epoll_wait");
            exit(-1);
        }
        printf("ret = %d\n", ret);

        for(int i = 0; i < ret; i++){
            int curfd = epevs[i].data.fd;

            if (curfd == lfd){
                // 有新的客户端连接；
                struct sockaddr_in cliaddr;
                // int len = sizepf(cliaddr);
                // int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);
                socklen_t nAddrLen;
                nAddrLen = sizeof(struct sockaddr_in);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &nAddrLen);
                
                // 加入新的文件描述符
                epev.events = EPOLL_IN;
                epev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &epev)
            }else{

                // 有数据到达
                char buf[1024] = {0};
                int len = read(curfd, buf, sizeof(buf));

                if (len == -1){
                    perror("read");
                    exit(-1);
                }else if (len==0){
                    printf("client closed ... \n");
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfds,  NULL);
                    close(curfd);
                }else if (len > 0){
                    printf("read buf = %s\n", buf);
                    write(curfd, buf, strlen(buf)+1);
                }
            }
        }
    }


    // (4) 关闭
    close(lfd);
    close(epfd);

    return 0;
}