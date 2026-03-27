#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include "tui.h"
#include "renderer.h"
#include "panels.h"

// Dimensiones globales
int ROWS, COLS;

#define SIDEBAR_W   14
#define HEADER_H     3
#define FOOTER_H     3

// Windows principales
WINDOW *win_header;
WINDOW *win_sidebar;
WINDOW *win_content;
WINDOW *win_footer;

typedef enum {
    VIEW_HABITS = 0,
    VIEW_STATS,
    VIEW_WEEK,
    VIEW_POWERS,
    VIEW_REWARDS,
    VIEW_COUNT
} ActiveView;

static ActiveView current_view = VIEW_HABITS;

static const char *menu_items[] = {
    "  Habitos",
    "  Stats  ",
    "  Semana ",
    "  Poderes",
    "  Recomp.",
};

static const char *menu_icons[] = {
    "⚔", "★", "◈", "✦", "◉"
};

void tui_init(void) {
    setlocale(LC_ALL, "");      // habilita UTF-8 (emojis y caracteres)
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);      // getch() no bloquea → game loop fluido
    curs_set(0);                // oculta el cursor
    start_color();
    use_default_colors();

    // Paleta de colores
    // init_pair(id, foreground, background)
    init_pair(1, COLOR_YELLOW,  -1);   // gold
    init_pair(2, COLOR_CYAN,    -1);   // ice / activo
    init_pair(3, COLOR_GREEN,   -1);   // completado
    init_pair(4, COLOR_RED,     -1);   // peligro / streak roto
    init_pair(5, COLOR_MAGENTA, -1);   // violet / especial
    init_pair(6, COLOR_WHITE,   -1);   // texto normal
    init_pair(7, COLOR_BLACK,   COLOR_YELLOW);  // selección sidebar
    init_pair(8, COLOR_YELLOW,  COLOR_BLACK);   // header resaltado

    getmaxyx(stdscr, ROWS, COLS);
    tui_create_windows();
}

void tui_create_windows(void) {
    int content_w = COLS - SIDEBAR_W;
    int content_h = ROWS - HEADER_H - FOOTER_H;

    win_header  = newwin(HEADER_H,  COLS,       0,          0);
    win_sidebar = newwin(content_h, SIDEBAR_W,  HEADER_H,   0);
    win_content = newwin(content_h, content_w,  HEADER_H,   SIDEBAR_W);
    win_footer  = newwin(FOOTER_H,  COLS,       ROWS - FOOTER_H, 0);
}

void tui_destroy_windows(void) {
    delwin(win_header);
    delwin(win_sidebar);
    delwin(win_content);
    delwin(win_footer);
}

void tui_teardown(void) {
    tui_destroy_windows();
    endwin();
}

// Manejo de resize del terminal
static void handle_resize(void) {
    tui_destroy_windows();
    getmaxyx(stdscr, ROWS, COLS);
    resizeterm(ROWS, COLS);
    tui_create_windows();
    clear();
    refresh();
}

static void handle_input(int ch, GameState *gs) {
	switch (ch) {
        case 'q': case 'Q':
            gs->running = 0;
            break;

        case 'k': case KEY_UP:
            if (current_view == VIEW_HABITS)
                panel_habits_move(gs, -1);   // mueve cursor hábitos
            else
                current_view = (current_view - 1 + VIEW_COUNT) % VIEW_COUNT;
            break;

        case 'j': case KEY_DOWN:
            if (current_view == VIEW_HABITS)
                panel_habits_move(gs, 1);    // mueve cursor hábitos
            else
                current_view = (current_view + 1) % VIEW_COUNT;
            break;

        // Navegar el sidebar con Tab o H/L
        case 'h': case KEY_LEFT:
        case '\t':                           // Tab también navega
            current_view = (current_view - 1 + VIEW_COUNT) % VIEW_COUNT;
            break;

        case 'l': case KEY_RIGHT:
            current_view = (current_view + 1) % VIEW_COUNT;
            break;

        case ' ':
            if (current_view == VIEW_HABITS)
                panel_habits_complete(gs);
            break;

        case 'a': case 'A':
            if (current_view == VIEW_HABITS)
                panel_habits_add(gs, win_footer);
            break;

        case KEY_RESIZE:
            handle_resize();
            break;
	case 'e': case 'E':
	    if (current_view == VIEW_HABITS)
	        panel_habits_edit(gs, win_footer);
	    break;

	case 'd': case 'D':
	    if (current_view == VIEW_HABITS)
	        panel_habits_delete(gs, win_footer);
	    break;
    }
}

void tui_run(GameState *gs) {
    gs->running = 1;

    while (gs->running) {
        // 1. Dibuja todo
        render_header(win_header, gs);
        render_sidebar(win_sidebar, menu_items, menu_icons,
                       VIEW_COUNT, current_view);
        render_footer(win_footer, current_view);

        // 2. Dibuja el panel activo en win_content
        switch (current_view) {
            case VIEW_HABITS:  panel_draw_habits(win_content, gs);  break;
            case VIEW_STATS:   panel_draw_stats(win_content, gs);   break;
            case VIEW_WEEK:    panel_draw_week(win_content, gs);    break;
            case VIEW_POWERS:  panel_draw_powers(win_content, gs);  break;
            case VIEW_REWARDS: panel_draw_rewards(win_content, gs); break;
            default: break;
        }

        // 3. Lee input (no bloqueante)
        int ch = getch();
        if (ch != ERR)
            handle_input(ch, gs);

        // 4. ~30fps
        napms(33);
    }
}
