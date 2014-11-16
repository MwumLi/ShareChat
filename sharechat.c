#include <ncurses.h>
#include <form.h>
#include <unistd.h>
#include <string.h>
#include "getCurtime.h"

#define KEY_ESC		27
#define KEY_SURE	10
int file_w;		//file area width
int member_w;	//chat member area width
int win_h;		//all window height
int version_h;
int status_h;

char username[128];

typedef struct winStruct{
	int height;
	int width;
	int y, x;
}winStruct;


void print_bar(WINDOW *win, char *str, int width, int color);
void print_curtime(WINDOW *win, int y, int color);

	int main(int argc, const char *argv[])
{

	WINDOW	*file_win, *member_win;
	WINDOW	*chat_win;
	WINDOW	*version_bar;
	WINDOW	*status_bar;
	WINDOW	*msg_win;

	int i, j;
	int y, x;
	int chat_w;
	int ch;

	//get username
	strcpy(username, getlogin());

	
	initscr();
	cbreak();
	noecho();
	start_color();
	keypad(stdscr, TRUE);

	//some settings for windows
	file_w = 10;
	member_w = 0;  //10
	chat_w = COLS - file_w - member_w;
	win_h = LINES - 3;

//	mvprintw(LINES-1, 0, "Press any key to exit");
	refresh();

	init_pair(1, COLOR_BLACK, COLOR_BLUE);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);		//time color
	//version  bar 
	version_bar= newwin(1, COLS, 0, 0);
	print_bar(version_bar, "version bar", COLS, 1);
	//status bar
	status_bar = newwin(1, COLS, win_h+1, 0);
	print_bar(status_bar, "status bar", COLS, 1);
	
	
	//file window and chat window
	file_win = newwin(win_h, file_w, 1, 0);
	chat_win = newwin(win_h, chat_w, 1, file_w+1);
	mvvline(1, file_w, '|',win_h);	//file_win and chat_win gave
	refresh();
	wrefresh(file_win);
	wrefresh(chat_win);
	
	
	//message window
	msg_win = newwin(1, COLS, LINES-1, 0);
	keypad(msg_win, TRUE);
	char str[1024];
	i = 0;
	while((ch=wgetch(msg_win)) != KEY_ESC) {
		switch(ch) {
			case KEY_SURE:
				str[i++]='\n';
				str[i++]='\0';
				getyx(chat_win, y, x);
				char *now = getCurtime();
				wprintw(chat_win, "%s", str);	//print message
				/*print time information*/
				print_curtime(file_win, y, 2);
				/*
				 * some msg window deal
				 */
				wmove(msg_win, 0, 0);	//msg window move head
				wclrtoeol(msg_win);		//clear msg window
				wmove(msg_win, 0, 0);	//move head
				i=0;
				//
				wrefresh(chat_win);
				wrefresh(msg_win);
				break;
			default:
				str[i++] = ch;
				waddch(msg_win, ch);
				wrefresh(msg_win);
				refresh();
				break;
		}

	}
	getch();
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

void *receive_thread(void *arg)
{
	while(1) {
		
	}
}
