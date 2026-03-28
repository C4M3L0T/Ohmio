#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include "renderer.h"
#include "types.h"
#include "pomodoro.h"

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

    int cols;
    int rows;
    getmaxyx(w, rows, cols);
    (void)rows;

    // ── ZONA IZQUIERDA: héroe ──────────────────────────
    wattron(w, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(w, 1, 2, "  OHMIO");
    wattroff(w, COLOR_PAIR(1) | A_BOLD);

    wattron(w, COLOR_PAIR(6));
    mvwprintw(w, 1, 12, "%s", gs->hero.name);
    wattroff(w, COLOR_PAIR(6));

    wattron(w, COLOR_PAIR(5) | A_BOLD);
    mvwprintw(w, 1, 12 + (int)strlen(gs->hero.name) + 2,
              "LVL %d", gs->hero.level);
    wattroff(w, COLOR_PAIR(5) | A_BOLD);

    // Barra XP
    int xp_bar_x = 12 + (int)strlen(gs->hero.name) + 10;
    wattron(w, COLOR_PAIR(1));
    mvwprintw(w, 1, xp_bar_x, "XP");
    wattroff(w, COLOR_PAIR(1));
    draw_bar(w, 1, xp_bar_x + 3, 16, gs->hero.xp, gs->hero.xp_to_next, 1);
    wattron(w, COLOR_PAIR(6));
    mvwprintw(w, 1, xp_bar_x + 20, "%d/%d",
              gs->hero.xp, gs->hero.xp_to_next);
    wattroff(w, COLOR_PAIR(6));

    // ── SEPARADOR VERTICAL ────────────────────────────
    int sep_x = cols / 2;
    wattron(w, COLOR_PAIR(1));
    mvwprintw(w, 1, sep_x, "│");
    wattroff(w, COLOR_PAIR(1));

    // ── ZONA DERECHA: pomodoro ────────────────────────
    PomodoroState *p = &gs->pomo;
    int remaining    = pomo_remaining(p);
    int min          = remaining / 60;
    int sec          = remaining % 60;

    // Icono y estado
    const char *pomo_icon;
    int pomo_color;
    switch (p->status) {
        case POMO_WORK:
            pomo_icon  = p->paused ? "⏸" : "";
            pomo_color = p->paused ? 5 : 3;
            break;
        case POMO_SHORT_BREAK:
            pomo_icon  = "";
            pomo_color = 2;
            break;
        case POMO_LONG_BREAK:
            pomo_icon  = "";
            pomo_color = 5;
            break;
        case POMO_DONE:
            pomo_icon  = "★";
            pomo_color = 1;
            break;
        default:
            pomo_icon  = "";
            pomo_color = 6;
    }

    // Timer MM:SS
    wattron(w, COLOR_PAIR(pomo_color) | A_BOLD);
    mvwprintw(w, 1, sep_x + 2, "%s %02d:%02d", pomo_icon, min, sec);
    wattroff(w, COLOR_PAIR(pomo_color) | A_BOLD);

    // Bolitas de periodos
    int dot_x = sep_x + 14;
    for (int i = 1; i <= p->config.periods_per_cycle; i++) {
        if (i < p->current_period ||
           (i == p->current_period && p->status == POMO_DONE)) {
            wattron(w, COLOR_PAIR(3) | A_BOLD);
            mvwprintw(w, 1, dot_x + (i-1)*2, "●");
        } else if (i == p->current_period && p->status == POMO_WORK) {
            wattron(w, COLOR_PAIR(pomo_color) | A_BOLD);
            mvwprintw(w, 1, dot_x + (i-1)*2, "◉");
        } else {
            wattron(w, COLOR_PAIR(6) | A_DIM);
            mvwprintw(w, 1, dot_x + (i-1)*2, "○");
        }
        wattroff(w, COLOR_PAIR(3) | A_BOLD);
        wattroff(w, COLOR_PAIR(pomo_color) | A_BOLD);
        wattroff(w, COLOR_PAIR(6) | A_DIM);
    }

    // Mini barra de progreso del periodo actual
    int total      = pomo_period_seconds(p);
    int elapsed    = total - remaining;
    int mini_bar_x = dot_x + p->config.periods_per_cycle * 2 + 2;
    int mini_bar_w = cols - mini_bar_x - 3;
    if (mini_bar_w > 4)
        draw_bar(w, 1, mini_bar_x, mini_bar_w,
                 elapsed, total > 0 ? total : 1, pomo_color);

    // XP ganado en sesión
    if (p->xp_earned_session > 0) {
        wattron(w, COLOR_PAIR(1));
        mvwprintw(w, 1, cols - 12, "+%dxp", p->xp_earned_session);
        wattroff(w, COLOR_PAIR(1));
    }

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

	if (view == 0) {
	    wattron(w, COLOR_PAIR(3));
	    mvwprintw(w, 1, 18, "[SPC] Completar");
	    wattroff(w, COLOR_PAIR(3));
	    wattron(w, COLOR_PAIR(2));
	    mvwprintw(w, 1, 36, "[a] Agregar");
	    wattroff(w, COLOR_PAIR(2));
	    wattron(w, COLOR_PAIR(1));
	    mvwprintw(w, 1, 50, "[e] Editar");
	    wattroff(w, COLOR_PAIR(1));
	    wattron(w, COLOR_PAIR(4));
	    mvwprintw(w, 1, 63, "[d] Borrar");
	    wattroff(w, COLOR_PAIR(4));
	}

    wattron(w, COLOR_PAIR(4));
    mvwprintw(w, 1, COLS - 12, "[q] Salir");
    wattroff(w, COLOR_PAIR(4));

    wrefresh(w);
}
