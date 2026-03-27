#ifndef RENDERER_H
#define RENDERER_H

#include <ncurses.h>
#include "types.h"

void draw_box_gold(WINDOW *w);
void draw_bar(WINDOW *w, int y, int x, int width,
              int current, int max, int color_pair);

void render_header(WINDOW *w, GameState *gs);
void render_sidebar(WINDOW *w, const char **items, const char **icons,
                    int count, int active);
void render_footer(WINDOW *w, int view);

#endif
