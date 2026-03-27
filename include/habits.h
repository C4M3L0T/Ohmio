#ifndef HABITS_H
#define HABITS_H

#include "types.h"

void habit_init(Habit *h, int id, const char *name,
                const char *icon, int xp, HabitCategory cat);
int  habit_calculate_xp(const Habit *h);
int  habit_complete(Habit *h);
void habits_reset_day(Habit *habits, int count);
int  habits_completed_today(const Habit *habits, int count);

#endif
