#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "panels.h"
#include "renderer.h"
#include "habits.h"
#include "hero.h"
#include "db.h"

// Índice del hábito seleccionado en la lista
static int habit_cursor = 0;

void panel_draw_habits(WINDOW *w, GameState *gs) {
    werase(w);
    draw_box_gold(w);

    int rows, cols;
    getmaxyx(w, rows, cols);

    // Título del panel
    wattron(w, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(w, 1, 2, "HABITOS DE HOY");
    wattroff(w, COLOR_PAIR(1) | A_BOLD);

    // Resumen rápido: X/Y completados
    int done  = habits_completed_today(gs->habits, gs->habit_count);
    int total = gs->habit_count;

    wattron(w, COLOR_PAIR(done == total ? 3 : 6));
    mvwprintw(w, 1, cols - 14, "%d/%d completados", done, total);
    wattroff(w, COLOR_PAIR(done == total ? 3 : 6));

    // Barra de progreso del día
    draw_bar(w, 2, 2, cols - 4, done, total, done == total ? 3 : 2);

    // Lista de hábitos
    for (int i = 0; i < gs->habit_count; i++) {
        Habit *h = &gs->habits[i];
        int y = 4 + i * 2;
        if (y >= rows - 2) break;

        // Highlight del cursor
        if (i == habit_cursor) {
            wattron(w, A_REVERSE);
        }

        // Icono de estado
        if (h->completed_today) {
            wattron(w, COLOR_PAIR(3));
            mvwprintw(w, y, 2, "[✓]");
            wattroff(w, COLOR_PAIR(3));
        } else {
            wattron(w, COLOR_PAIR(4));
            mvwprintw(w, y, 2, "[ ]");
            wattroff(w, COLOR_PAIR(4));
        }

        // Nombre del hábito
        wattron(w, h->completed_today ? COLOR_PAIR(3) : COLOR_PAIR(6));
        mvwprintw(w, y, 6, "%s %s", h->icon, h->name);
        wattroff(w, h->completed_today ? COLOR_PAIR(3) : COLOR_PAIR(6));

        // Streak y XP al lado derecho
        wattron(w, COLOR_PAIR(1));
        mvwprintw(w, y, cols - 20, "🔥 %d  +%dxp  Lv%d",
                  h->streak, h->xp_reward, h->power_level);
        wattroff(w, COLOR_PAIR(1));

        if (i == habit_cursor) wattroff(w, A_REVERSE);
    }

    wrefresh(w);
}

// Mueve el cursor dentro de la lista de hábitos
void panel_habits_move(GameState *gs, int dir) {
    habit_cursor = (habit_cursor + dir + gs->habit_count) % gs->habit_count;
}

// Completa el hábito bajo el cursor
void panel_habits_complete(GameState *gs) {
    if (gs->habit_count == 0) return;

    Habit *h = &gs->habits[habit_cursor];
    int xp = habit_complete(h);
    if (xp <= 0) return;  // ya completado

    db_log_completion(h->id, xp, h->streak);
    db_save_habit(h);

    int leveled = hero_add_xp(&gs->hero, xp);
    db_save_hero(&gs->hero);

    if (leveled) {
        // Aquí llamarás a la animación de level up (Fase 3.5)
        // anim_level_up(win_content, gs->hero.level);
    }
}

// Agregar hábito — pide nombre al usuario en el footer
void panel_habits_add(GameState *gs, WINDOW *footer) {
    if (gs->habit_count >= MAX_HABITS) return;

    // Activa el cursor temporalmente para input
    nodelay(stdscr, FALSE);
    curs_set(1);
    echo();
	// Pide el nombre en el footer
    werase(footer);
    draw_box_gold(footer);
    wattron(footer, COLOR_PAIR(2));
    mvwprintw(footer, 1, 2, "Nombre del habito: ");
    wattroff(footer, COLOR_PAIR(2));
    wrefresh(footer);

    char name[MAX_NAME] = {0};
    mvwgetnstr(footer, 1, 21, name, MAX_NAME - 1);

    // Reanuda el game loop
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);

    if (strlen(name) == 0) return;

    int id = gs->habit_count;
    habit_init(&gs->habits[id], id, name, "◆", 50, CAT_MENTAL);
    db_save_habit(&gs->habits[id]);
    gs->habit_count++;

}

void panel_draw_stats(WINDOW *w, GameState *gs) {
    werase(w);
    draw_box_gold(w);

    wattron(w, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(w, 1, 2, "STATS DEL HEROE");
    wattroff(w, COLOR_PAIR(1) | A_BOLD);

    int y = 3;
    // Cada stat con su barra
    const char *labels[] = {"Fuerza  ", "Sabiduria", "Vitalidad", "Carisma ", "Disciplina"};
    int values[] = {
        gs->hero.strength, gs->hero.wisdom,
        gs->hero.vitality, gs->hero.charisma,
        gs->hero.discipline
    };
    int colors[] = {4, 5, 3, 2, 1};

    for (int i = 0; i < 5; i++) {
        wattron(w, COLOR_PAIR(colors[i]));
        mvwprintw(w, y + i*2, 2, "%s", labels[i]);
        wattroff(w, COLOR_PAIR(colors[i]));
        draw_bar(w, y + i*2, 12, 20, values[i], 100, colors[i]);
        wattron(w, COLOR_PAIR(6));
        mvwprintw(w, y + i*2, 34, "%d", values[i]);
        wattroff(w, COLOR_PAIR(6));
    }

    // Días jugados y días perfectos
    wattron(w, COLOR_PAIR(1));
    mvwprintw(w, y + 12, 2, "Dias jugados : %d", gs->hero.total_days_played);
    mvwprintw(w, y + 13, 2, "Dias perfectos: %d", gs->hero.perfect_days);
    wattroff(w, COLOR_PAIR(1));

    wrefresh(w);
}

void panel_draw_week(WINDOW *w, GameState *gs) {
    (void)gs;
    werase(w);
    draw_box_gold(w);

    wattron(w, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(w, 1, 2, "SEMANA ACTUAL");
    wattroff(w, COLOR_PAIR(1) | A_BOLD);

    const char *days[] = {"Lun", "Mar", "Mie", "Jue", "Vie", "Sab", "Dom"};
    int cols;
    int rows;
    getmaxyx(w, rows, cols);
    (void)rows;
    int col_w = (cols - 4) / 7;

    for (int i = 0; i < 7; i++) {
        int x = 2 + i * col_w;
        wattron(w, COLOR_PAIR(1));
        mvwprintw(w, 2, x, "%s", days[i]);
        wattroff(w, COLOR_PAIR(1));
        // Placeholder — en siguiente fase lees daily_log de la DB
        wattron(w, COLOR_PAIR(6));
        mvwprintw(w, 4, x, "---");
        wattroff(w, COLOR_PAIR(6));
    }

    wattron(w, COLOR_PAIR(5));
    mvwprintw(w, 8, 2, "Historico disponible proxima fase");
    wattroff(w, COLOR_PAIR(5));

    wrefresh(w);
}

void panel_draw_powers(WINDOW *w, GameState *gs) {
    werase(w);
    draw_box_gold(w);

    wattron(w, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(w, 1, 2, "PODERES — TOP HABITOS");
    wattroff(w, COLOR_PAIR(1) | A_BOLD);

    // Muestra cada hábito como un poder con su power_level
    for (int i = 0; i < gs->habit_count && i < 10; i++) {
        Habit *h = &gs->habits[i];
        int y = 3 + i * 2;

        wattron(w, COLOR_PAIR(5) | A_BOLD);
        mvwprintw(w, y, 2, "%s", h->icon);
        wattroff(w, COLOR_PAIR(5) | A_BOLD);

        wattron(w, COLOR_PAIR(6));
        mvwprintw(w, y, 6, "%-20s", h->name);
        wattroff(w, COLOR_PAIR(6));

        // Barra de poder
        draw_bar(w, y, 28, 15, h->power_level, 10, 5);

        wattron(w, COLOR_PAIR(1));
        mvwprintw(w, y, 45, "Lv%d  🔥%d", h->power_level, h->streak);
        wattroff(w, COLOR_PAIR(1));
    }

    wrefresh(w);
}

void panel_draw_rewards(WINDOW *w, GameState *gs) {
    werase(w);
    draw_box_gold(w);

    wattron(w, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(w, 1, 2, "RECOMPENSAS");
    wattroff(w, COLOR_PAIR(1) | A_BOLD);

    wattron(w, COLOR_PAIR(1));
    mvwprintw(w, 2, 2, "XP disponible: %d", gs->hero.xp);
    wattroff(w, COLOR_PAIR(1));

    if (gs->reward_count == 0) {
        wattron(w, COLOR_PAIR(5));
        mvwprintw(w, 5, 2, "No hay recompensas definidas aun.");
        mvwprintw(w, 6, 2, "Agrega recompensas en ~/.ohmio/rewards.toml");
        wattroff(w, COLOR_PAIR(5));
    }

    for (int i = 0; i < gs->reward_count; i++) {
        Reward *r = &gs->rewards[i];
        int y = 5 + i * 2;

        int can_afford = gs->hero.xp >= r->xp_cost;
        int color = r->redeemed ? 3 : (can_afford ? 2 : 4);

        wattron(w, COLOR_PAIR(color));
        mvwprintw(w, y, 2, "%s%-25s  %d XP  %s",
                  r->redeemed ? "[✓] " : "[ ] ",
                  r->name, r->xp_cost,
                  r->redeemed ? "CANJEADA" : (can_afford ? "DISPONIBLE" : "BLOQUEADA"));
        wattroff(w, COLOR_PAIR(color));
    }

    wrefresh(w);
}

// Pide un string al usuario en el footer, retorna 0 si cancela
static int prompt_string(WINDOW *footer, const char *label,
                          char *out, int maxlen) {
    nodelay(stdscr, FALSE);
    curs_set(1);
    echo();

    werase(footer);
    draw_box_gold(footer);
    wattron(footer, COLOR_PAIR(2));
    mvwprintw(footer, 1, 2, "%s", label);
    wattroff(footer, COLOR_PAIR(2));
    wattron(footer, COLOR_PAIR(6));
    mvwprintw(footer, 1, 2 + strlen(label), "[ESC cancela]: ");
    wattroff(footer, COLOR_PAIR(6));
    wrefresh(footer);

    char buf[256] = {0};
    // Muestra el valor actual como placeholder
    mvwprintw(footer, 1, 2 + strlen(label) + 15, "%s", out);
    wrefresh(footer);

    int result = mvwgetnstr(footer, 1, 2 + strlen(label) + 15,
                            buf, maxlen - 1);

    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);

    if (result == ERR || strlen(buf) == 0) return 0;
    strncpy(out, buf, maxlen - 1);
    out[maxlen - 1] = '\0';
    return 1;
}

// Pide confirmación Y/N
static int prompt_confirm(WINDOW *footer, const char *msg) {
    nodelay(stdscr, FALSE);
    noecho();

    werase(footer);
    draw_box_gold(footer);
    wattron(footer, COLOR_PAIR(4) | A_BOLD);
    mvwprintw(footer, 1, 2, "%s [s/n]: ", msg);
    wattroff(footer, COLOR_PAIR(4) | A_BOLD);
    wrefresh(footer);

    int ch = wgetch(footer);
    nodelay(stdscr, TRUE);
    return (ch == 's' || ch == 'S');
}

void panel_habits_edit(GameState *gs, WINDOW *footer) {
    if (gs->habit_count == 0) return;
    Habit *h = &gs->habits[habit_cursor];

    // Categorías disponibles
    const char *cat_names[] = {
        "0-Fisico", "1-Mental", "2-Salud", "3-Social", "4-Creativo"
    };

    // Edita nombre
    char new_name[MAX_NAME];
    strncpy(new_name, h->name, MAX_NAME);
    if (!prompt_string(footer, "Nombre: ", new_name, MAX_NAME)) return;

    // Edita icono
    char new_icon[MAX_ICON];
    strncpy(new_icon, h->icon, MAX_ICON);
    prompt_string(footer, "Icono:  ", new_icon, MAX_ICON);

    // Edita XP — pide número como string y convierte
    char xp_str[8];
    snprintf(xp_str, sizeof(xp_str), "%d", h->xp_reward);
    if (prompt_string(footer, "XP base:", xp_str, 8))
        h->xp_reward = atoi(xp_str);

    // Edita categoría
    char cat_str[2];
    snprintf(cat_str, sizeof(cat_str), "%d", h->category);

    // Muestra opciones en footer
    nodelay(stdscr, FALSE);
    werase(footer);
    draw_box_gold(footer);
    wattron(footer, COLOR_PAIR(1));
    mvwprintw(footer, 1, 2,
              "Cat [0]Fisico [1]Mental [2]Salud [3]Social [4]Creativo (actual:%d): ",
              h->category);
    wattroff(footer, COLOR_PAIR(1));
    wrefresh(footer);
    int ch = wgetch(footer) - '0';
    if (ch >= 0 && ch <= 4) h->category = ch;
    nodelay(stdscr, TRUE);

    // Aplica cambios
    strncpy(h->name, new_name, MAX_NAME - 1);
    h->name[MAX_NAME - 1] = '\0';
    strncpy(h->icon, new_icon, MAX_ICON - 1);
    h->icon[MAX_ICON - 1] = '\0';

    db_update_habit(h);
}

void panel_habits_delete(GameState *gs, WINDOW *footer) {
    if (gs->habit_count == 0) return;
    Habit *h = &gs->habits[habit_cursor];

    char msg[128];
    snprintf(msg, sizeof(msg), "Borrar '%s'?", h->name);
    if (!prompt_confirm(footer, msg)) return;

    db_delete_habit(h->id);

    // Elimina del array en memoria desplazando
    for (int i = habit_cursor; i < gs->habit_count - 1; i++)
        gs->habits[i] = gs->habits[i + 1];

    gs->habit_count--;

    // Ajusta cursor si quedó fuera del rango
    if (habit_cursor >= gs->habit_count && habit_cursor > 0)
        habit_cursor--;
}
