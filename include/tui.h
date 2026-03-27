#ifndef TUI_H
#define TUI_H

#include "types.h"

extern int ROWS, COLS;

void tui_init(void);
void tui_create_windows(void);
void tui_destroy_windows(void);
void tui_teardown(void);
void tui_run(GameState *gs);

#endif
