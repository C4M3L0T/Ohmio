#ifndef DB_H
#define DB_H

#include "types.h"

int  db_open(void);
void db_close(void);
int  db_create_tables(void);

int  db_save_hero(const Hero *h);
int  db_load_hero(Hero *h);

int  db_save_habit(const Habit *h);
int  db_load_habits(Habit *habits, int *count);
int  db_log_completion(int habit_id, int xp_earned, int streak);
int  db_save_daily_log(int completed, int total, int xp,
                       int mood, float sleep, int perfect);
int db_delete_habit(int id);
int db_update_habit(const Habit *h);
int db_load_weekly_log(DailyLog *logs, int *count, int days);

#endif
