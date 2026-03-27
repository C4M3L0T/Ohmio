#ifndef HERO_H
#define HERO_H

#include "types.h"

int  xp_for_next_level(int level);
void hero_init(Hero *h, const char *name);
int  hero_add_xp(Hero *h, int amount);
void hero_recalculate_stats(Hero *h, Habit *habits, int count);
void hero_update_discipline(Hero *h);

#endif
