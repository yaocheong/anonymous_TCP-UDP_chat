<h4>
	基于Linux C语言的局域网内UDP/TCP匿名聊天系统
</h4>

* common.h 	头文件,包含结构体定义与函数声明
* client 	关于客户端的主函数存储
* list	 	关于好友链表的函数存储
* file_IO	关于文件收发的函数存储
* broadcast 关于广播收发的函数存储

<h5>
	需求
</h5>

	1、程序运行的时候，别人可以收到你的上线通知，并且知道你的昵称、性别
	2、程序形成一条好友链表
	3、可以指定某个好友进行聊天
	4、当好友下线的时候我们将好友列表更新一下
	5、单独聊天还可以选择文件发送给对方