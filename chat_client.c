#include "common.h"

int main(int argc, const char *argv[])
{
	if (argc != 2)
	{
		printf("用法:%s <昵称>\n", argv[0]);
		goto create_err;
	}

	global ginfo;
	int retval;

	strcpy(ginfo.name, argv[1]);
	ginfo.list_head = new_node(NULL);

	ginfo.udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (ginfo.udp_fd == -1)
	{
		perror("申请套接字失败\n");
		goto request_socket_err;
	}

	int sw = 1;
	retval = setsockopt(ginfo.udp_fd, SOL_SOCKET, 
		SO_BROADCAST, &sw, sizeof(sw));
	if (retval == -1)
	{
		perror("设置程序允许广播出错\n");
		goto setsock_broadcase_err;
	}

	struct sockaddr_in native_addr;
	memset(&native_addr, 0, sizeof(struct sockaddr_in));
	native_addr.sin_family = AF_INET;
	native_addr.sin_port = htons(25535);
	native_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	retval = bind(ginfo.udp_fd, 
		(struct sockaddr *)&native_addr, 
		sizeof(native_addr));
	if(retval == -1)
	{
		perror("绑定udp套接字异常\n");
		goto bind_socket_err;
	}

	pthread_t tid;
	pthread_create(&tid, NULL, recv_broadcast_msg, &ginfo);

	message msg;
	strcpy(msg.name, argv[1]);
	msg.msg_flag = online_flag;
	broadcast_msg_send(ginfo.udp_fd, 
		&msg, msg.msg_buffer-msg.name);

	char input[256];
	friend_list pos;
	ssize_t send_size;
	while(1)
	{
		display(ginfo, pos);
		printf("****************************************\n");
		printf("输入“group”发送群聊\n");
		printf("输入“chat”与好友单独聊天\n");
		printf("输入“exit”退出\n");
		printf("输入任意字符刷新\n");
		printf("****************************************\n");
		scanf("%s", input);
		if (strcmp(input, "exit") == 0)
		{
			msg.msg_flag = offline_flag;
			broadcast_msg_send(ginfo.udp_fd, 
				&msg, msg.msg_buffer - msg.name);
			break;
		}
		else if (strcmp(input, "group") == 0)
		{
			system("clear");
			msg.msg_flag = message_flag;

			while(1)
			{
				bzero(msg.msg_buffer, sizeof(msg.msg_buffer));
				printf("****************************************\n");
				printf("请输入消息内容：to all\n");
				printf("输入“exit”退出\n");
				printf("****************************************\n");
				scanf("%s", msg.msg_buffer);

				if (strcmp(msg.msg_buffer, "exit") == 0)
					break;

				broadcast_msg_send(ginfo.udp_fd, 
					&msg, sizeof(msg));
			}

			system("clear");
		}
		else if (strcmp(input, "chat") == 0)
		{
			printf("请输入好友昵称\n");
			scanf("%s", input);

			for (pos = ginfo.list_head->next; pos != NULL; pos = pos->next)
				if (strcmp(pos->name, input) == 0)
					break;
			if (pos == NULL)
			{
				printf("该好友不存在!\n");
				printf("输入任意字符刷新\n");
				scanf("%s", input);
				system("clear");
				continue;
			}

			system("clear");
			msg.msg_flag = message_flag;
			while(1)
			{
				bzero(msg.msg_buffer, sizeof(msg.msg_buffer));
				printf("****************************************\n");
				printf("请输入消息内容：to %s\n", pos->name);
				printf("输入“send”向好友发送文件\n");
				printf("输入“exit”退出\n");
				printf("****************************************\n");
				scanf("%s", msg.msg_buffer);

				if (strcmp(msg.msg_buffer, "exit") == 0)
					break;
				else if (strcmp(msg.msg_buffer, "send") == 0)
				{
					printf("请输入要发送的文件名:\n");
					scanf("%s", msg.msg_buffer);
					msg.msg_flag = file_flag;
					send_size = sendto(ginfo.udp_fd, 
						&msg, sizeof(msg),
						0, 
						(struct sockaddr *)&(pos->addr), 
						sizeof(pos->addr));

					file_send(&msg);

					continue;
				}

				send_size = sendto(ginfo.udp_fd, 
					&msg, sizeof(msg),
					0, 
					(struct sockaddr *)&(pos->addr), 
					sizeof(pos->addr));
				if (send_size == -1)
				{
					perror("发送UDP数据失败\n");
					goto send_udp_msg_err;
				}
				system("clear");
			}
		}
		system("clear");
	}

	destroy_list(ginfo.list_head);
	close(ginfo.udp_fd);
create_err:
	return 0;

setsock_broadcase_err:
bind_socket_err:
send_udp_msg_err:
	close(ginfo.udp_fd);
request_socket_err:
	destroy_list(ginfo.list_head);
	return -1;
}