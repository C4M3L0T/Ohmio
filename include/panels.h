#ifndef PANELS_H
#define PANELS_H

#include <ncurses.h>
#include "types.h"

void panel_draw_habits(WINDOW *w, GameState *gs);
void panel_draw_stats(WINDOW *w, GameState *gs);
void panel_draw_week(WINDOW *w, GameState *gs);
void panel_draw_powers(WINDOW *w, GameState *gs);
void panel_draw_rewards(WINDOW *w, GameState *gs);

void panel_habits_move(GameState *gs, int dir);
void panel_habits_complete(GameState *gs);
void panel_habits_add(GameState *gs, WINDOW *footer);
void panel_habits_edit(GameState *gs, WINDOW *footer);
void panel_habits_delete(GameState *gs, WINDOW *footer);

#endif
