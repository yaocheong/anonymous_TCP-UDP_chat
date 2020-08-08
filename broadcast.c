#include "common.h"

int broadcast_msg_send(int sock, const void *msg, ssize_t msg_len)
{
	struct sockaddr_in dest_addr;
	ssize_t send_size;

	memset(&dest_addr, 0, sizeof(struct sockaddr_in));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(25535);
	dest_addr.sin_addr.s_addr = inet_addr("192.168.33.255");

	send_size = sendto(sock, msg, msg_len, 
			0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	if(send_size == -1)
	{
		perror("发送UDP数据失败\n");
		return -1;
	}
    	
	return 0;
}

void *recv_broadcast_msg(void *arg)
{
	message recv_msg, msg;
	global *ginfo = arg;
	socklen_t skt_len = sizeof(struct sockaddr_in);
	// int retval;

	ssize_t recv_size, send_size;

	friend_list new;
	friend_node cache_node;

	while(1)
	{
		bzero(&recv_msg, sizeof(recv_msg));

		recv_size = recvfrom(ginfo->udp_fd, &recv_msg, 
			sizeof(recv_msg), 0, 
			(struct sockaddr *)&(cache_node.addr), &skt_len);
		if (recv_size == -1)
		{
			perror("接收UDP数据失败\n");
			break;
		}

		switch(recv_msg.msg_flag)
		{
			case online_flag:
				strcpy(cache_node.name, recv_msg.name);
				//若已存在该节点则退出
				if (list_for_each(ginfo->list_head,cache_node))
					break;
				
				new = new_node(&cache_node);
				insert_node(ginfo->list_head, new);
				printf("[%s:%d]:", 
					inet_ntoa(new->addr.sin_addr), 
					ntohs(new->addr.sin_port));
				printf("%s 上线了\n", new->name);

				//返回上线消息
				strcpy(msg.name, ginfo->name);
				msg.msg_flag = online_flag;

				send_size = sendto(ginfo->udp_fd, &msg, 
					msg.msg_buffer - msg.name, 
					0, 
					(struct sockaddr *)&cache_node.addr, 
					sizeof(struct sockaddr_in));

				break;

			case offline_flag:
				strcpy(cache_node.name, recv_msg.name);
				//若不存在该节点则退出
				if (!list_for_each(ginfo->list_head,cache_node))
					break;
				
				delete_node(ginfo->list_head, &cache_node);
				printf("%s 离线了\n", cache_node.name);

				break;

			case message_flag:
				printf("%s 说 %s\n", 
					recv_msg.name, 
					recv_msg.msg_buffer);

				break;

			case file_flag:
				printf("%s 即将给你发送文件 [%s]\n", 
					recv_msg.name, 
					recv_msg.msg_buffer);

				sleep(1);
				file_recv(&cache_node, &recv_msg);

				break;
		}
	}

	return  NULL;
}