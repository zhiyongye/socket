#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h> 
#include <errno.h>
#include<pthread.h>     
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>


/* 使用的端口号 */
#define PORT 1234

/* 接收字符串大小 */
#define BUF_SIZE 1024

/* 函数声明 */
static void data_handle(void * sock_fd);


int main() {
	
    int listenfd=0, connfd=0;
    struct sockaddr_in serv_addr, clnt_addr;
    int strLen, ret;

	// 创建套接字
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if ( listenfd == -1 ) {
		printf("Error : Could not create socket\n");
        printf("Errno %d\n", errno);
        exit(-1);
	}
    
	// 设置端口和IP地址
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);
	
	// 将套接字和IP、端口绑定
    if ( bind(listenfd,(struct scokaddr *)(&serv_addr),sizeof(serv_addr)) == -1) {
		printf("Error:Bindint with port # %d failed\n", PORT);
        printf("Errno %d\n", errno);
        if(errno == EADDRINUSE) {
			printf("Another socket is already listening on the same port\n");
		}
        exit(-1);
	}

	// 进入监听状态，等待用户发起请求
    if( listen(listenfd, 10) == -1) {
        printf("Error:Failed to listen\n");
        printf("Errno %d\n", errno);
        if(errno == EADDRINUSE) {
			printf("Another socket is already listening on the same port\n");
		}
        exit(-1);
    }

    while(1) {
		
        printf("waiting for new connection...\n");
        pthread_t thread_id;
        strLen = sizeof(clnt_addr);

        // 阻塞直到有新的accept请求
        connfd = accept(listenfd,(struct sockaddr_*)(&clnt_addr),(socklen_t *)(&strLen));
        if( connfd == -1 ) {
            fprintf(stderr, " Accept error!\n");
            continue;                       
        }
		
		// 创建线程
        printf("A new connection occurs!\n");
        if( pthread_create(&thread_id,NULL,(void *)(&data_handle),(void *)(&connfd)) == -1 ) {
            fprintf(stderr, "pthread_create error!\n");
            break;
        }
    }

    // 关闭监听使用的socket
    ret = shutdown(listenfd, SHUT_WR);
    if ( ret != -1 ) {
		printf("Server shuts down\n");
	}

    return 0;
}

static void data_handle(void * sock_fd) {
	
    int fd = *((int *)sock_fd);
    char recv_data[BUF_SIZE];
    const char * data_send = "Server has received your request!\n";
	int strLen;

	memset(recv_data, 0, BUF_SIZE);
	
	printf("waiting for request...\n");
	
	// 读取客户端发来的数据
	strLen = recv(fd, recv_data, BUF_SIZE, 0);
	if(strLen == -1) {
		fprintf(stderr, "read error!\n");
		return;
	}
	printf("read from client : %s\n", recv_data);
	
	// 向客户端发送数据
	if( send(fd, data_send, strlen(data_send), 0) == -1 ) {
		fprintf(stderr, "Server Send Data Failed!\n");
		return;
	}

	// 关闭线程及与客户端的连接
    printf("terminating current client_connection...\n");
    close(fd);
    pthread_exit(NULL);
}