#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>

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


    // 初始化poll所需
    struct pollfd fds[1024];
    for(int i=0;i<1024;i++){
        fds[i].fd = -1;
        fds[i].events = POLLIN;
    }
    fds[0].fd = lfd;
    int nfds = 0;

    while(1){

        int ret = poll(fds, nfds + 1, -1);

        if (ret == -1){
            perror("select");
            exit(-1);
        }else if (ret == 0){
            continue;
        }else if (ret > 0){ // 检测到数据
            
            if(fds[0].revents & POLLIN){

                // 新的客户端连接进来
                struct sockaddr_in cliaddr;
                socklen_t nAddrLen;
                nAddrLen = sizeof(struct sockaddr_in);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &nAddrLen);

                // 新的文件描述符加入
                for (int i = 1; i < 1024; i++){
                    if(fds[i].fd == -1){
                        fds[i].fd = cfd;
                        fds[i].events = POLLIN;
                    }
                }

                // 更新最大文件描述符
                nfds = nfds > cfd ? nfds:cfd;
            }

            // 数据处理
            for(int i = 1; i <= nfds; i++){
                if(fds[i].revents & POLLIN){

                    char buf[1024] = {0};
                    int len = read(i, buf, sizeof(buf));
                    if (len == -1){
                        perror("read");
                        exit(-1);
                    }else if (len==0){
                        printf("client closed ... \n");
                        close(i); //先关闭
                        fds[i].fd = -1; //置为不可用
                    }else if (len > 0){
                        printf("read buf = %s\n", buf);
                        write(i, buf, strlen(buf)+1);
                    }
                }
            }
        }
    }

    // (4) 关闭
    close(lfd);

    return 0;
}