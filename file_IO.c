#include "common.h"

#define BUF_SIZE  8192

unsigned char fileBuf[BUF_SIZE];

//发送文件, 要求输入文件名
void file_send(void *arg)
{
	message *msg = arg;

	int skfd, cnfd;
	FILE *fp = NULL;
	struct sockaddr_in sockAddr, cltAddr;
	socklen_t addrLen;
	unsigned int fileSize;
	
	int size, netSize;
	char buf[10];
 
	//创建tcp socket
	skfd = socket(AF_INET, SOCK_STREAM, 0);
	if(skfd == -1) 
	{
		perror("文件传输申请套接字失败\n");
		return;
	}

	int sw = 1;
	int retval = setsockopt(skfd, SOL_SOCKET, 
		SO_REUSEADDR, &sw, sizeof(sw));
	if (retval == -1)
	{
		perror("设置程序允许复用套接字出错\n");
		return;
	}

	//创建结构  绑定地址端口号
	memset(&sockAddr, 0, sizeof(struct sockaddr_in));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockAddr.sin_port = htons(25535);
 
	//bind
	if(bind(skfd, (struct sockaddr *)(&sockAddr), sizeof(struct sockaddr)) < 0) {
		perror("绑定异常\n");
		return;
	}
 
	//listen   监听 用户
	if(listen(skfd, 1) < 0) 
	{
		perror("监听异常\n");
		return;
	}
 
	/* 调用accept,服务器端一直阻塞，直到客户程序与其建立连接成功为止*/
	addrLen = sizeof(struct sockaddr_in);
	if((cnfd = accept(skfd, (struct sockaddr *)(&cltAddr), &addrLen)) < 0) 
	{
		perror("建立连接异常\n");
		return;
	}

	fp = fopen(msg->msg_buffer, "r");
	if( fp == NULL ) {
		perror("文件打开异常\n");
		close(cnfd);
		close(skfd);
		return;
	}
 
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);			//获取文件大小
	fseek(fp, 0, SEEK_SET);
	printf("文件大小:%u Byte\r\n",fileSize);
	
	if(write(cnfd, (unsigned char *)&fileSize, 4) != 4) 
	{
		perror("文件大小发送异常\n");
		close(cnfd);
		close(skfd);
		return;
	}
 
	if( read(cnfd, buf, 2) != 2) {
		perror("应答读取异常\n");
		close(cnfd);
		close(skfd);
		return;
	}
	
	unsigned int total = 0;
	while( ( size = fread(fileBuf, 1, BUF_SIZE,fp) ) > 0 ) 
	{
		unsigned int size2 = 0;

		while( size2 < size ) 
		{
			if( (netSize = write(cnfd, fileBuf + size2, size - size2) ) < 0 ) 
			{
				perror("数据发送异常\n");
				close(cnfd);
				close(skfd);
				return;
			}
			size2 += netSize;
		}
		total += size;

		printf("已发送:%u Byte\r",total);	//显示传送进度

	}
	printf("\n");
	fclose(fp);
	close(cnfd);
	close(skfd);
}

//接受文件, 要求输入文件名
void file_recv(void *arg_1, void *arg_2)
{
	friend_list pos = arg_1;
	message *msg = arg_2;

	int skfd;
	FILE *fp = NULL;
	struct sockaddr_in sockAddr;
	unsigned int fileSize, fileSize2;
	int size, nodeSize;
 
	//创建tcp socket
	skfd=socket(AF_INET,SOCK_STREAM,0);
	if(skfd == -1) 
	{
		perror("文件传输申请套接字失败\n");
		return;
	}
 
	//创建结构设定待连接的服务器地址端口号
	memset(&sockAddr, 0, sizeof(struct sockaddr_in));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = pos->addr.sin_addr.s_addr;
	sockAddr.sin_port = htons(25535);
 
	/* 客户端调用connect主动发起连接请求 */
	if(connect(skfd, (struct sockaddr *)(&sockAddr), sizeof(struct sockaddr)) < 0) {
		perror("连接错误\n");
		return;
	}
 
	size = read(skfd, (unsigned char *)&fileSize, 4);
	if( size != 4 ) 
	{
		printf("文件大小获取错误!\n");
		close(skfd);
		return;
	}
	printf("文件大小:%u Byte\n", fileSize);
	
	if( (size = write(skfd, "OK", 2) ) < 0 ) 
	{
		perror("数据写入异常\n");
		close(skfd);
		return;
	}
 
	fp = fopen(msg->msg_buffer, "w");
	if( fp == NULL ) 
	{
		perror("文件打开异常\n");
		close(skfd);
		return;
	}
	
	unsigned int total = 0;
	fileSize2 = 0;
	while(memset(fileBuf, 0, sizeof(fileBuf)), (size = read(skfd, fileBuf, sizeof(fileBuf))) > 0) 
	{
		unsigned int size2 = 0;
		while( size2 < size ) 
		{
			if( (nodeSize = fwrite(fileBuf + size2, 1, size - size2, fp) ) < 0 ) 
			{
				perror("文件写入异常\n");
				close(skfd);
				exit(1);
			}
			size2 += nodeSize;
		}
		fileSize2 += size;
		
		
		total += size;
		printf("已接收:%u Byte\r",total);	//显示传送进度
		fflush(stdout);
		
		if(fileSize2 >= fileSize) break;
	}
	printf("\n");
	fclose(fp);
	close(skfd);
}