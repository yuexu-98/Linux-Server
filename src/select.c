#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>


int main(){

    // (1) 创建
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    // (2) 绑定
    bind(lfd, (struct  sockaddr *)&saddr, sizeof(saddr);

    // (3) 监听
    listen(lfd, 8);

    // fd_set 集合：存放需要检测的文件描述符
    fd_set rdset, tmp; 
    FD_ZERO(&rdset);
    FD_SET(lfd, &rdset);

    // 初始化最大的监听文件描述符
    int maxfd = lfd; 

    while(1){

        tmp = rdset;

        // 调用select，使内核检测哪些文件描述符有数据
        int ret = select(maxfd+1, &tmp, NULL, NULL, NULL);

        if (ret == -1){
            perror("select");
            exit(-1);
        }else if (ret == 0){
            continue;
        }else if (ret > 0){ // 检测到数据
            
            if(FD_ISSET(lfd, &tmp)){
                // 新的客户端连接进来
                struct  sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);

                // 新的文件描述符加入
                FD_SET(cfd, &rdset);
                // 更新最大文件描述符
                maxfd = maxfd > cfd ? maxfd:cfd;
            }

            for(int i = lfd+1; i <= maxfd; i++){
                if(FD_ISSET(i, &tmp)){
                    char buf[1024] = {0};
                    int len = read(i, buf, sizeof(buf));
                    if (len == -1){
                        perror("read");
                        exit(-1);
                    }else if (len==0){
                        printf("client closed ... \n");
                        close(i);
                        FD_CLR(i, &rdset);
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