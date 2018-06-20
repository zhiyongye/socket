#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/* 使用的端口号 */
#define PORT 1234

#define BUF_SIZE 1024

int main(){
	
	int sock_fd;
	struct sockaddr_in serv_addr;
	char buffer[BUF_SIZE];
	int strLen;
	
    //创建套接字
	sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if ( sock_fd == -1 ) {
		fprintf(stderr, "Error: Client Socket can not be created !! \n");
        fprintf(stderr, "errno : %d\n", errno);
		exit(-1);
	}

    //向服务器（特定的IP和端口）发起请求
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("172.16.101.136");
    serv_addr.sin_port = htons(PORT);
    if ( connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		fprintf(stderr, "Connect Server Failed!\n");
		exit(-1);
	}
	
	// 向服务器发送数
	strcpy(buffer, "Hello World!");
	if (send(sock_fd, buffer, BUF_SIZE, 0) == -1 ) {
		fprintf(stderr, "Send Data Failed!\n");
		exit(-1);
	}
	printf("Message Sending to server: %s\n", buffer);
   
    //读取服务器传回的数据
	strLen = recv(sock_fd, buffer, BUF_SIZE, 0);
    if ( strLen == -1) {
		fprintf(stderr, "Recieve Data Failed!\n");
		exit(-1);
	}
    printf("Message form server: %s\n", buffer);
   
    //关闭套接字
    close(sock_fd);

    return 0;
}