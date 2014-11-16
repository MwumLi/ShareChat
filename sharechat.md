### 数据结构  

	typedef struct chatMember {
		char *name;
		char *ip;
		int  port;
	}chatMember;	//聊天成员信息

	typedef struct roomMember {
		char				*name;
		vector<chatMember>	chat_list; /聊天成员列表
	}roomMember;	//聊天房间信息


	vector<chatMember> rooms; //房间列表


### 发送者

	#define BUF_SIZE	1024
	#define ROOM_NAME_SIZE	128
	#define SERVER_PORT 8888
	
	#define CONNECT		0
	#define JOIN		1
	#define EXIT		2
	#define DOWNLOAD	3
	#define UPLOAD		4
	#define MSG_MSG		5
	#define MSG_FILE	6

	#define CONN_SUCCESS 10
	#define JOIN_SUCCESS 11

	typedef struct sendMsg {
		int		msg_type;
		char	room[ROOM_NAME_SIZE];
		char	msg[BUF_SIZE];
	}sendMsg;

#### Window
10 

insertln()
scroll()
