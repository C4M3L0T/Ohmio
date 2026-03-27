#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include "renderer.h"
#include "types.h"

// Dibuja una caja con borde estilo RPG
void draw_box_gold(WINDOW *w) {
    wattron(w, COLOR_PAIR(1));
    box(w, 0, 0);
    wattroff(w, COLOR_PAIR(1));
}

// Barra de progreso  ████░░░░
void draw_bar(WINDOW *w, int y, int x, int width,
              int current, int max, int color_pair) {
    int filled = (max > 0) ? (current * width / max) : 0;
    if (filled > width) filled = width;

    wattron(w, COLOR_PAIR(color_pair));
    for (int i = 0; i < width; i++)
        mvwaddstr(w, y, x + i, i < filled ? "█" : "░");
    wattroff(w, COLOR_PAIR(color_pair));
}

void render_header(WINDOW *w, GameState *gs) {
    werase(w);
    draw_box_gold(w);

    // Título
    wattron(w, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(w, 1, 2, " OHMIO");
    wattroff(w, COLOR_PAIR(1) | A_BOLD);

    // Nombre y nivel del héroe
    wattron(w, COLOR_PAIR(6));
    mvwprintw(w, 1, 14, "%s", gs->hero.name);
    wattroff(w, COLOR_PAIR(6));

    // Nivel
    wattron(w, COLOR_PAIR(5) | A_BOLD);
    mvwprintw(w, 1, 14 + strlen(gs->hero.name) + 2,
              "LVL %d", gs->hero.level);
    wattroff(w, COLOR_PAIR(5) | A_BOLD);

    // Barra de XP en el header
    int bar_x = COLS - 28;
    wattron(w, COLOR_PAIR(1));
    mvwprintw(w, 1, bar_x - 5, "XP");
    wattroff(w, COLOR_PAIR(1));
    draw_bar(w, 1, bar_x, 20, gs->hero.xp, gs->hero.xp_to_next, 1);

    // XP numérico
    wattron(w, COLOR_PAIR(6));
    mvwprintw(w, 1, bar_x + 21, "%d/%d",
              gs->hero.xp, gs->hero.xp_to_next);
    wattroff(w, COLOR_PAIR(6));

    wrefresh(w);
}

void render_sidebar(WINDOW *w, const char **items, const char **icons,
                    int count, int active) {
    werase(w);
    draw_box_gold(w);

    wattron(w, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(w, 1, 2, "MENU");
    wattroff(w, COLOR_PAIR(1) | A_BOLD);

    for (int i = 0; i < count; i++) {
        int y = 3 + i * 2;
        if (i == active) {
            wattron(w, COLOR_PAIR(7) | A_BOLD);
            mvwprintw(w, y, 1, " %s %s ", icons[i], items[i]);
            wattroff(w, COLOR_PAIR(7) | A_BOLD);
        } else {
            wattron(w, COLOR_PAIR(6));
            mvwprintw(w, y, 1, " %s %s", icons[i], items[i]);
            wattroff(w, COLOR_PAIR(6));
        }
    }

    wrefresh(w);
}

void render_footer(WINDOW *w, int view) {
    werase(w);
    draw_box_gold(w);

    wattron(w, COLOR_PAIR(1));
    mvwprintw(w, 1, 2, "[j/k] Navegar");
    wattroff(w, COLOR_PAIR(1));

    if (view == 0) {  // VIEW_HABITS
        wattron(w, COLOR_PAIR(3));
        mvwprintw(w, 1, 18, "[SPACE] Completar");
        wattroff(w, COLOR_PAIR(3));
        wattron(w, COLOR_PAIR(2));
        mvwprintw(w, 1, 38, "[a] Agregar");
        wattroff(w, COLOR_PAIR(2));
    }

    wattron(w, COLOR_PAIR(4));
    mvwprintw(w, 1, COLS - 12, "[q] Salir");
    wattroff(w, COLOR_PAIR(4));

    wrefresh(w);
}
