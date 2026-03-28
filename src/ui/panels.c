#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "panels.h"
#include "renderer.h"
#include "habits.h"
#include "hero.h"
#include "db.h"
#include <math.h>

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

    int rows, cols;
    getmaxyx(w, rows, cols);

    wattron(w, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(w, 1, 2, "CRONICA SEMANAL");
    wattroff(w, COLOR_PAIR(1) | A_BOLD);

    // Carga los últimos 14 días (semana actual + anterior)
    DailyLog logs[14];
    int count = 0;
    db_load_weekly_log(logs, &count, 14);

    if (count == 0) {
        wattron(w, COLOR_PAIR(5));
        mvwprintw(w, rows/2, 4, "Sin historial aun. Completa habitos para registrar.");
        wattroff(w, COLOR_PAIR(5));
        wrefresh(w);
        return;
    }

    // Nombres de días de la semana
    const char *day_names[] = {"Do","Lu","Ma","Mi","Ju","Vi","Sa"};

    // --- SECCIÓN 1: Gráfica de barras por día (últimos 7) ---
    wattron(w, COLOR_PAIR(1));
    mvwprintw(w, 2, 2, "ULTIMOS 7 DIAS");
    wattroff(w, COLOR_PAIR(1));

    int chart_h  = 8;
    int base_y   = 12;
    int bar_w    = 3;
    int start_x  = 6;
    int week_count = count < 7 ? count : 7;

    // Eje Y
    wattron(w, COLOR_PAIR(6) | A_DIM);
    for (int i = 0; i <= chart_h; i++)
        mvwprintw(w, base_y - i, start_x - 1, "│");
    mvwprintw(w, base_y + 1, start_x - 1, "└");
    wattroff(w, COLOR_PAIR(6) | A_DIM);

    // Referencia 100%
    wattron(w, COLOR_PAIR(6) | A_DIM);
    mvwprintw(w, base_y - chart_h, start_x - 4, "100");
    for (int x = start_x; x < start_x + week_count * (bar_w + 3) + 2; x++)
        mvwprintw(w, base_y - chart_h, x, "·");
    wattroff(w, COLOR_PAIR(6) | A_DIM);

    // Los logs vienen DESC (hoy primero), invertimos para mostrar cronológico
    for (int i = 0; i < week_count; i++) {
        // Índice invertido: el más antiguo primero en la gráfica
        DailyLog *l = &logs[week_count - 1 - i];
        int bar_x   = start_x + 1 + i * (bar_w + 3);

        // % de completado
        int pct = (l->habits_total > 0)
                  ? (l->habits_completed * 100 / l->habits_total)
                  : 0;
        int bar_h = pct * chart_h / 100;

        // Color según % completado
        int color = (pct == 100) ? 3 :   // verde = perfecto
                    (pct >= 60)  ? 1 :   // gold  = bien
                    (pct >= 30)  ? 5 :   // violeta = regular
                                   4;   // rojo  = mal

        // Dibuja barra
        for (int bx = 0; bx < bar_w; bx++)
            for (int by = 0; by < bar_h; by++) {
                wattron(w, COLOR_PAIR(color));
                mvwprintw(w, base_y - by, bar_x + bx,
                          by == bar_h - 1 ? "▄" : "█");
                wattroff(w, COLOR_PAIR(color));
            }

        // Día perfecto: corona encima
        if (l->perfect_day) {
            wattron(w, COLOR_PAIR(1) | A_BOLD);
            mvwprintw(w, base_y - bar_h - 1, bar_x, " ★");
            wattroff(w, COLOR_PAIR(1) | A_BOLD);
        }

        // % encima de la barra
        wattron(w, COLOR_PAIR(color));
        mvwprintw(w, base_y - bar_h - (l->perfect_day ? 2 : 1),
                  bar_x, "%2d%%", pct);
        wattroff(w, COLOR_PAIR(color));

        // Nombre del día abajo
        // Extrae el día de la semana del date "YYYY-MM-DD"
        struct tm tm_day = {0};
        sscanf(l->date, "%d-%d-%d",
               &tm_day.tm_year, &tm_day.tm_mon, &tm_day.tm_mday);
        tm_day.tm_year -= 1900;
        tm_day.tm_mon  -= 1;
        mktime(&tm_day);

        wattron(w, COLOR_PAIR(6));
        mvwprintw(w, base_y + 1, bar_x, "%s", day_names[tm_day.tm_wday]);
        wattroff(w, COLOR_PAIR(6));

        // XP del día
        wattron(w, COLOR_PAIR(1));
        mvwprintw(w, base_y + 2, bar_x - 1, "%3d", l->xp_earned);
        wattroff(w, COLOR_PAIR(1));
    }

    // Leyenda XP
    wattron(w, COLOR_PAIR(1) | A_DIM);
    mvwprintw(w, base_y + 2, start_x - 4, "XP");
    wattroff(w, COLOR_PAIR(1) | A_DIM);

    // --- SECCIÓN 2: Resumen lateral ---
    int rx = start_x + week_count * (bar_w + 3) + 6;

    wattron(w, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(w, 3, rx, "RESUMEN");
    wattroff(w, COLOR_PAIR(1) | A_BOLD);

    // Calcula stats de la semana actual (primeros 7 logs)
    int perfect_days = 0, total_xp = 0, total_done = 0, total_habits = 0;
    for (int i = 0; i < week_count; i++) {
        perfect_days += logs[i].perfect_day;
        total_xp     += logs[i].xp_earned;
        total_done   += logs[i].habits_completed;
        total_habits += logs[i].habits_total;
    }
    int week_pct = (total_habits > 0)
                   ? (total_done * 100 / total_habits) : 0;

    wattron(w, COLOR_PAIR(3));
    mvwprintw(w, 5, rx, "Dias perfectos");
    mvwprintw(w, 6, rx, "★ %d / %d", perfect_days, week_count);
    wattroff(w, COLOR_PAIR(3));

    wattron(w, COLOR_PAIR(1));
    mvwprintw(w, 8,  rx, "XP esta semana");
    mvwprintw(w, 9,  rx, "⚡ %d xp", total_xp);
    wattroff(w, COLOR_PAIR(1));

    wattron(w, COLOR_PAIR(2));
    mvwprintw(w, 11, rx, "Completado");
    mvwprintw(w, 12, rx, "◈ %d%%", week_pct);
    wattroff(w, COLOR_PAIR(2));

    // Comparación con semana anterior
    if (count >= 14) {
        int prev_xp = 0, prev_done = 0, prev_total = 0;
        for (int i = 7; i < 14; i++) {
            prev_xp   += logs[i].xp_earned;
            prev_done += logs[i].habits_completed;
            prev_total+= logs[i].habits_total;
        }
        int prev_pct = prev_total > 0 ? (prev_done * 100 / prev_total) : 0;
        int diff_pct = week_pct - prev_pct;
        int diff_xp  = total_xp - prev_xp;

        wattron(w, COLOR_PAIR(1) | A_BOLD);
        mvwprintw(w, 14, rx, "VS SEMANA ANT.");
        wattroff(w, COLOR_PAIR(1) | A_BOLD);

        int color_pct = diff_pct >= 0 ? 3 : 4;
        int color_xp  = diff_xp  >= 0 ? 3 : 4;

        wattron(w, COLOR_PAIR(color_pct));
        mvwprintw(w, 15, rx, "%s%d%% completado",
                  diff_pct >= 0 ? "▲ +" : "▼ ", diff_pct);
        wattroff(w, COLOR_PAIR(color_pct));

        wattron(w, COLOR_PAIR(color_xp));
        mvwprintw(w, 16, rx, "%s%d xp",
                  diff_xp >= 0 ? "▲ +" : "▼ ", diff_xp);
        wattroff(w, COLOR_PAIR(color_xp));
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

// Dibuja un pie chart de los top hábitos por completions
static void draw_pie(WINDOW *w, int cy, int cx, int radius,
                     int *values, int count, int total) {
    if (total == 0) return;

    // Acumula ángulos por sector
    float angles[20] = {0};
    float accum = 0.0f;
    for (int i = 0; i < count; i++) {
        accum += (float)values[i] / total * 360.0f;
        angles[i] = accum;
    }

    int colors[] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5};

    // Itera cada celda dentro del bounding box
    // Las celdas son más altas que anchas, compensamos × 2 en x
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius * 2; dx <= radius * 2; dx++) {
            // Normaliza x por el aspect ratio del terminal
            float nx = (float)dx / 2.0f;
            float ny = (float)dy;
            float dist = nx*nx + ny*ny;

            if (dist > (float)(radius * radius)) continue;

            // Ángulo del punto en grados [0, 360)
            float angle = atan2f(ny, nx) * 180.0f / 3.14159f + 180.0f;

            // Determina a qué sector pertenece
            int sector = count - 1;
            for (int s = 0; s < count; s++) {
                if (angle < angles[s]) { sector = s; break; }
            }

            wattron(w, COLOR_PAIR(colors[sector % 5]));
            mvwprintw(w, cy + dy, cx + dx, "█");
            wattroff(w, COLOR_PAIR(colors[sector % 5]));
        }
    }
}

void panel_draw_powers(WINDOW *w, GameState *gs) {
    werase(w);
    draw_box_gold(w);

    int rows, cols;
    getmaxyx(w, rows, cols);

    wattron(w, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(w, 1, 2, "PODERES — DISTRIBUCION DE HABITOS");
    wattroff(w, COLOR_PAIR(1) | A_BOLD);

    if (gs->habit_count == 0) {
        wattron(w, COLOR_PAIR(5));
        mvwprintw(w, rows/2, 4, "No hay habitos registrados aun.");
        wattroff(w, COLOR_PAIR(5));
        wrefresh(w);
        return;
    }

    // Valores para el pie = total_completions por hábito
    int values[MAX_HABITS];
    int total = 0;
    for (int i = 0; i < gs->habit_count; i++) {
        values[i] = gs->habits[i].total_completions;
        total    += values[i];
    }
    // Si nadie tiene completions usa xp_reward como peso
    if (total == 0) {
        for (int i = 0; i < gs->habit_count; i++) {
            values[i] = gs->habits[i].xp_reward;
            total    += values[i];
        }
    }

    // Pie chart centrado en la mitad izquierda
    int radius = (rows - 6) / 2;
    if (radius > 8) radius = 8;
    int cy = rows / 2;
    int cx = cols / 4 + 4;
    draw_pie(w, cy, cx, radius, values, gs->habit_count, total);

    // Leyenda a la derecha del pie
    int legend_x = cx + radius * 2 + 4;
    int colors[] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5};

    wattron(w, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(w, 3, legend_x, "LEYENDA");
    wattroff(w, COLOR_PAIR(1) | A_BOLD);

    for (int i = 0; i < gs->habit_count && i < 10; i++) {
        Habit *h = &gs->habits[i];
        int y    = 5 + i * 2;
        int pct  = total > 0 ? (values[i] * 100 / total) : 0;

        wattron(w, COLOR_PAIR(colors[i % 5]) | A_BOLD);
        mvwprintw(w, y, legend_x, "█");
        wattroff(w, COLOR_PAIR(colors[i % 5]) | A_BOLD);

        wattron(w, COLOR_PAIR(6));
        mvwprintw(w, y, legend_x + 2, "%-18s", h->name);
        wattroff(w, COLOR_PAIR(6));

        wattron(w, COLOR_PAIR(1));
        mvwprintw(w, y, legend_x + 21, "%3d%%  Lv%d  🔥%d",
                  pct, h->power_level, h->streak);
        wattroff(w, COLOR_PAIR(1));
    }

    wrefresh(w);
}
