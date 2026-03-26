#include <ncurses.h>

int main() {
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	start_color();

	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	attron(COLOR_PAIR(1));
	mvprintw(5,10,"OHMIO — Loading your destiny...");
	attroff(COLOR_PAIR(1));

	refresh();
	getch();
	endwin();
	return 0;
}
