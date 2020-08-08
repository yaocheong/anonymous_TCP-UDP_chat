#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <fcntl.h>

#include <libgen.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <net/if.h>

enum {online_flag, offline_flag, message_flag, file_flag};

typedef struct friend_class{		//好友消息节点
	char name[128];					//昵称
	char sex[64];					//性别
	struct sockaddr_in addr;		//IP及端口
	struct friend_class *next;		//结构体指针
}friend_node, *friend_list;

typedef struct msg_info{			//消息结构体
	char name[128];
	char sex[64];
	int msg_flag;
	char msg_buffer[2048];
}message;

typedef struct global_info{			//用户自身信息存储结构体
	char name[128];
	int udp_fd;
	friend_list list_head;
}global;

//广播信息
int
broadcast_msg_send(
	int sock, 
	const void *msg, 
	ssize_t msg_len);

//接收广播
void *recv_broadcast_msg(void *arg);

//验证节点p在链表中是否存在
bool list_for_each(friend_list head, friend_node p);

//建立新节点
friend_list new_node(friend_list info);

//在链表尾部接入新节点
void insert_node(friend_list head, friend_list new_node);

//从链表中删除节点node
void delete_node(friend_list head, friend_list node);

//清除链表
void destroy_list(friend_list head);

//在屏幕上绘制在线人员
void display(global gifo, friend_list pos);

//发送文件
void file_send(void *arg);

//接受文件
void file_recv(void *arg_1, void *arg_2);

#endif