#include <string.h>
#include <math.h>
#include <time.h>
#include "hero.h"

// XP necesario para subir al siguiente nivel
// Curva: nivel 1→2 = 100xp, 2→3 = 210xp, etc.
int xp_for_next_level(int level) {
    return (int)(XP_BASE * pow(1.4, level - 1));
}

void hero_init(Hero *h, const char *name) {
    strncpy(h->name, name, MAX_NAME);
    h->level          = 1;
    h->xp             = 0;
    h->xp_to_next     = xp_for_next_level(1);
    h->strength       = 1;
    h->wisdom         = 1;
    h->vitality       = 1;
    h->charisma       = 1;
    h->discipline     = 0;
    h->total_days_played = 0;
    h->perfect_days   = 0;
    h->created_at     = time(NULL);
}

// Retorna 1 si subió de nivel, 0 si no
int hero_add_xp(Hero *h, int amount) {
    h->xp += amount;
    if (h->xp >= h->xp_to_next) {
        h->xp       -= h->xp_to_next;
        h->level    += 1;
        h->xp_to_next = xp_for_next_level(h->level);
        return 1;  // level up!
    }
    return 0;
}

// Recalcula stats según los hábitos completados
void hero_recalculate_stats(Hero *h, Habit *habits, int count) {
    // Cada categoría de hábito alimenta un stat diferente
    for (int i = 0; i < count; i++) {
        if (!habits[i].completed_today) continue;
        switch (habits[i].category) {
            case CAT_PHYSICAL:  h->strength  += 1; break;
            case CAT_MENTAL:    h->wisdom    += 1; break;
            case CAT_HEALTH:    h->vitality  += 1; break;
            case CAT_SOCIAL:    h->charisma  += 1; break;
            case CAT_CREATIVE:  h->wisdom    += 1; break;
        }
    }
}

// Discipline = % de días perfectos sobre días totales
void hero_update_discipline(Hero *h) {
    if (h->total_days_played == 0) { h->discipline = 0; return; }
    h->discipline = (h->perfect_days * 100) / h->total_days_played;
}
