#include <ncurses.h>

void destory_win(WINDOW *win);

int main(int argc, const char *argv[])
{
	WINDOW *box_win;
	int startx, starty, width, height;
	int ch;
	
	initscr();
	cbreak();
	keypad(stdscr, TRUE);

	height = 3;
	width = 10;
	starty = (LINES-height)/2;
	startx = (COLS-width)/2;
	printw("Press F1 to exit");
	refresh();

	box_win = newwin(height, width, starty, startx);
	box(box_win, 0, 0);
	wrefresh(box_win);
	getch();
	destory_win(box_win);
	endwin();
	return 0;
}

void destory_win(WINDOW *win)
{
	wborder(win, ' ',' ', ' ',' ', ' ',' ', ' ',' ');
//	box(win,' ',' ');
	wrefresh(win);
	delwin(win);
}

