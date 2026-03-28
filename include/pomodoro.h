#ifndef POMODORO_H
#define POMODORO_H

#include "types.h"
#include <ncurses.h>

void pomo_init(PomodoroState *p);
int  pomo_period_seconds(const PomodoroState *p);
int  pomo_elapsed(const PomodoroState *p);
int  pomo_remaining(const PomodoroState *p);
void pomo_start(PomodoroState *p);
void pomo_pause_toggle(PomodoroState *p);
void pomo_stop(PomodoroState *p);
int  pomo_tick(PomodoroState *p, Hero *hero);
void pomo_config_modal(PomodoroState *p, WINDOW *content, WINDOW *footer);

#endif
