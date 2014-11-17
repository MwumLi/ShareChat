#include <ncurses.h>
#include <form.h>
#include <unistd.h>
#include <string.h>
#include <panel.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "getCurtime.h"
/*use panel*/

/*
 * Some micro
 * */
#define KEY_ESC		27
#define KEY_SURE	10

#define BUF_SIZE    512
#define ROOM_NAME_SIZE  128
#define SERVER_PORT 8888
#define CONNECT     0
#define JOIN        1
#define EXIT        2
#define DOWNLOAD    3
#define UPLOAD      4
#define MSG_MSG     5
#define MSG_FILE    6

#define CONN_SUCCESS 10
#define JOIN_SUCCESS 11

#define COMMAND_NUM 3
#define WINDOW_NUM 6

/*
 * Some struct defination
 * */
typedef struct winStruct{
	int height;
	int width;
	int y, x;
}winStruct;

typedef struct sendMsg {
	int		msg_type;
	char	room[ROOM_NAME_SIZE];
	char	member[128];
	char	msg[BUF_SIZE];
}sendMsg;

/*
 * Some function declation
 * */
void	*init_curses();
void	print_bar(WINDOW *win, char *str, int width, int color);
void	print_curtime(WINDOW *win, int y, int color);
int		judge_command(char *str);

int		connect_server(char *str); 
int		join_room(char *str);
int		exit_room();

void conn_interface();
void chat_interface();


void *pthread_recv(void *arg);


/*
 * Some global variable
 * */
char *command[]={"/connect", "/join", "/exit"};

//window's width, height
int time_w;		//time area width
int member_w;	//chat member area width
int win_h;		//time, chat and member 's window height
int version_h;
int status_h;
int chat_w;
char username[128];

int p_run = 1;
PANEL	*share_panels[WINDOW_NUM];
WINDOW	*share_wins[WINDOW_NUM];

//about network
struct sockaddr_in server_info;
sendMsg send_msg;
sendMsg recv_msg;
int		connected = 0;
int		joined = 0;
char	room[ROOM_NAME_SIZE];
int		client_socket;
int main(int argc, const char *argv[])
{

	//some windows structure
	WINDOW	*time_win, *member_win;
	WINDOW	*chat_win;
	WINDOW	*version_bar;
	WINDOW	*status_bar;
	WINDOW	*msg_win;
	pthread_t ptid;
	//some simle varible
	int i, j;
	int y, x;
	int ch;

	//some network stauture
	char ip_str[16];
	
	/*
	 * Init ncurses enviroment
	 * */
	init_curses();

	//get username
	strcpy(username, getlogin());
	
	//some settings for windows
	time_w = 10;
	member_w = 0;  //10
	chat_w = COLS - time_w - member_w;
	win_h = LINES - 3;

	/*
	 * Init some color pairs 
	 * */
	init_pair(1, COLOR_BLACK, COLOR_BLUE);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);		//time color

	//Create connect interface
	conn_interface();

	//print version bar
	print_bar(share_wins[0], "version bar", COLS, 1);
	print_bar(share_wins[1], "status bar", COLS, 1);
	refresh();
	
	//Enable key
	keypad(share_wins[5], TRUE);
	//Init msg string
	memset(&send_msg, 0, sizeof(send_msg));
	wmove(share_wins[5], 0, 0);
	wmove(share_wins[3], 0, 0);

	client_socket = socket(AF_INET, SOCK_DGRAM, 0);
	server_info.sin_port = htons(SERVER_PORT);
	server_info.sin_family = AF_INET;

	pthread_create(&ptid, NULL, pthread_recv, NULL);
	i = 0;
	int len, up;
	char print[200];
	int r_pos;
	while(1) {
		ch = wgetch(share_wins[5]);
		switch (ch) {
			case '\t':
				
				break;
			case KEY_SURE:
				//print time
				getyx(share_wins[3], y, x);
				memset(print, 0, 200);
				print_curtime(share_wins[2], y, 2);
				
				if(send_msg.msg[0] == '/') {
					int ret = judge_command(send_msg.msg);
					if(ret < COMMAND_NUM) {
						send_msg.msg_type = ret;
						switch(ret) {
							case 0:
								connect_server(send_msg.msg);
								sendto(client_socket, &send_msg, sizeof(send_msg), 0, (struct sockaddr *)&server_info, sizeof(struct sockaddr_in));
								strcpy(print, "Try to connect server...\n");
								break;
							case 1:
								join_room(send_msg.msg);
								sendto(client_socket, &send_msg, sizeof(send_msg), 0, (struct sockaddr *)&server_info, sizeof(struct sockaddr_in));
								strcpy(print, "Try to join room...\n");

								break;
							case 2:
								exit_room();
								sendto(client_socket, &send_msg, sizeof(send_msg), 0, (struct sockaddr *)&server_info, sizeof(struct sockaddr_in));
								p_run = 0;
//								pthread_join(ptid, NULL);	//wait pthread end
								goto proagram_end;
								break;
						}
					} else {
						strcpy(print, "Enter error:cann't find command...\n");
					} //end ret <= COMMAND_NUM 
				} // end send_msg.msg[0]  == '/'
				else {	  
					//general message
					send_msg.msg[i++]='\n';
					send_msg.msg[i++]='\0';
					send_msg.msg_type = MSG_MSG;	//type
					strcpy(send_msg.member, username); //user
					strcpy(send_msg.room, room); //room
					sendto(client_socket, &send_msg, sizeof(send_msg), 0, (struct sockaddr *)&server_info, sizeof(struct sockaddr_in));
					
					
					strcpy(print, username );
					strcat(print, " say:\n");
					strcat(print, send_msg.msg);
					len = strlen(send_msg.msg);
					up = len/chat_w;
					if(len%chat_w) 
					  up +=1;
					up+=1;
					len = 0;
				}

				if(len != 0)
				  up = 1;
				if(y+up >= win_h) {
					up = y+up-win_h;
					y -= up;
					x = 0;
					for(j=0; j<up; j++) {
						scroll(share_wins[2]);
						scroll(share_wins[3]);

						touchwin(share_wins[2]);
						touchwin(share_wins[3]);
					}
					wmove(share_wins[3], y, x);
				}

				//print
				wprintw(share_wins[3], "%s", print);	
				//refresh windows
				wrefresh(share_wins[3]);
				
				
				//init after print and send
				i = 0;
				memset(&send_msg, 0, sizeof(send_msg));
				wmove(share_wins[5], 0, 0);
				wclrtoeol(share_wins[5]);
				wmove(share_wins[5], 0, 0);
				break;
			case KEY_LEFT:
				getyx(share_wins[5],y, x);
				if(x <= 0)
				  break;
				wmove(share_wins[5],0, --x);
				wrefresh(share_wins[5]);
				break;
			case KEY_RIGHT:
				  
				getyx(share_wins[5],y, x);
				if(x >= i)
				  break;
				wmove(share_wins[5],0, ++x);
				wrefresh(share_wins[5]);
				break;
			default:
				send_msg.msg[i++] = ch;
				waddch(share_wins[5], ch);
				wrefresh(share_wins[5]);
		}
	
	}
proagram_end:
	for(i=0; i<WINDOW_NUM; i++)
	  del_panel(share_panels[i]);
	for(i=0; i<WINDOW_NUM; i++)
	  delwin(share_wins[i]);
	endwin();
	return 0;
}

/*
 * print_bar -- print somethint in bar, which is a window
 * @win: a poiner to point a window ,in which will be print 
 * @str: message, which wiil be display in @win 
 * @width: the width of window 
 * @color: the bar's foreground color and background color
 *
 * */
void print_bar(WINDOW *win, char *str, int width, int color)
{
	int length = strlen(str);
	int i;
	//start color
	wattron(win, COLOR_PAIR(color));
	//print information
	mvwprintw(win, 0, 0, "%s", str);
	for(i=length; i<width; i++)
	  waddch(win,' ');
	wrefresh(win);
	//off color
	wattroff(win, COLOR_PAIR(color));
}

void print_curtime(WINDOW *win, int y, int color)
{
	char *cur_time;

	cur_time = getCurtime();
	//start color
	wattron(win, COLOR_PAIR(color));
	//print current time
	mvwprintw(win, y, 0, "%s", cur_time);
	wrefresh(win);
	//off color
	wattroff(win, COLOR_PAIR(color));
}

void *init_curses()
{
	initscr();
	cbreak();
	noecho();
	start_color();
	keypad(stdscr, TRUE);

}
void conn_interface()
{
	int		i;
	/*
	 * Create 6 windows
	 * */
	//Create share_wins[0] as version  bar 
	share_wins[0]= newwin(1, COLS, 0, 0);
	//Create share_wins[1] as status bar
	share_wins[1] = newwin(1, COLS, win_h+1, 0);
	//Create share_wins[2] as time window 
	share_wins[2] = newwin(win_h, time_w, 1, 0);
	//Create share_wins[3] as chat window
	share_wins[3] = newwin(win_h, chat_w, 1, time_w+1);
	//Create share_wins[4] as member window
	share_wins[4] = newwin(win_h, 10, 1, COLS-10);
	//Create share_wins[5] as message window
	share_wins[5] = newwin(1, COLS, LINES-1, 0);
	/*
	 * Attach a panel to each window
	 * */
	for (i = 0; i < WINDOW_NUM; i++) {
		share_panels[i] = new_panel(share_wins[i]);
	}

	//Hide panels[4] as member window
	panel_hidden(share_panels[4]);
	
	//make boundary line bwtween time window and chat window
	mvvline(1, time_w, '|',win_h);	

	update_panels();
	doupdate();

	//Enable scrlll
	scrollok(share_wins[2], 1);
	scrollok(share_wins[3], 1);
	scrollok(share_wins[4], 1);


}
void chat_interface()
{
	//Change chat window
	chat_w = COLS-10-1-time_w-1;
	wresize(share_wins[3], win_h, chat_w);
	replace_panel(share_panels[3], share_wins[3]);

	//Show member window
	show_panel(share_panels[4]);
	
	//make boundary line bwtween chat window and member window
	mvvline(1, COLS-10-1, '|', win_h);

	update_panels();
	doupdate();

	refresh();
	//Enable scrlll
	scrollok(share_wins[2], 1);
	scrollok(share_wins[3], 1);
	scrollok(share_wins[4], 1);

}
int	judge_command(char *str)
{
	int i;
	for (i = 0; i < COMMAND_NUM; i++) {
		if(strstr(str, command[i]) != NULL)
		  return i;
	}
	return COMMAND_NUM;
}
int	connect_server(char *str) 
{
	char *ip;
	ip = strchr(str, ' ');
	if(ip == NULL)	
	  return -1;
	ip++;
	if(inet_pton(AF_INET, ip, &server_info.sin_addr) == 1)
	  return 0;
	return -1;
}

int join_room(char *str)
{
	char *aroom;	
	aroom = strchr(str, ' ');
	if(aroom == NULL)	
	  return -1;
	aroom++;
	strcpy(room, aroom);
	strcpy(send_msg.room, room);
	strcpy(send_msg.member, username);
	return 0;

}
int exit_room(void)
{
	strcpy(send_msg.room, room);
	strcpy(send_msg.member, username);
}
void *pthread_recv(void *arg)
{
	int y, x;
	char print[200];
	sendMsg  msg;
	int len, up;
	int i;
	while(p_run) {
		memset(&recv_msg, 0, sizeof(sendMsg));
		memset(print, 0, 200);
		len = up =0;

		recvfrom(client_socket, &recv_msg, sizeof(sendMsg), 0, NULL, NULL);

		switch (recv_msg.msg_type) {
			case CONN_SUCCESS:
				connected = 1;			

				sprintf(print, "Welcome you to share chat!\n");

				break;
			case JOIN_SUCCESS:
				joined = 1;
				chat_interface();
				//update member window
				sprintf(print, "You join room %s\n",recv_msg.room);
				break;
			case EXIT:
				//update member window
				sprintf(print, "%s exit room %s\n", recv_msg.msg, recv_msg.room);
				break;
			case JOIN:
				//update member window
				sprintf(print, "%s join room %s\n", recv_msg.msg, recv_msg.room);
				break;			
			case MSG_MSG:
				sprintf(print, "%s say:\n", recv_msg.member);
				len = strlen(print);
				up = len/chat_w;
				if(len%chat_w) 
				  up +=1;

				strcat(print, recv_msg.msg);
				len = strlen(print);
				//update time area 
				break;
			case MSG_FILE:				
				break;
			default:
				;
		}

		up += len/chat_w;
		if(len%chat_w) 
		  up +=1;

		getyx(share_wins[3], y, x);

		if((y+up) >= win_h) {
			up = (y+up-win_h);
			y = y - up;
			x = 0;
			wmove(share_wins[3], y, x);
			wmove(share_wins[2], y, x);
			for(i=0; i<up; i++) {
				scroll(share_wins[2]);
				scroll(share_wins[3]);

				touchwin(share_wins[2]);
				touchwin(share_wins[3]);

			}
		}
		print_curtime(share_wins[2], y, 2);
		wprintw(share_wins[3], print);

		wrefresh(share_wins[2]);
		wrefresh(share_wins[3]);
	}

}


