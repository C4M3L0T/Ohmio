#include <string.h>
#include <time.h>
#include "habits.h"

void habit_init(Habit *h, int id, const char *name,
                const char *icon, int xp, HabitCategory cat) {
    h->id               = id;
    strncpy(h->name, name, MAX_NAME);
    strncpy(h->icon, icon, MAX_ICON);
    h->xp_reward        = xp;
    h->streak           = 0;
    h->best_streak      = 0;
    h->total_completions = 0;
    h->completed_today  = 0;
    h->power_level      = 1;
    h->category         = cat;
    h->last_completed   = 0;
}

// Calcula el XP real con multiplicadores
int habit_calculate_xp(const Habit *h) {
    float multiplier = 1.0f;

    // Streak bonus: +10% por cada 7 días de racha
    multiplier += (h->streak / 7) * 0.10f;

    // Power level bonus
    multiplier += (h->power_level - 1) * 0.15f;

    // Cap en 3x para no romper el balance
    if (multiplier > 3.0f) multiplier = 3.0f;

    return (int)(h->xp_reward * multiplier);
}

// Devuelve el XP ganado, o -1 si ya estaba completado
int habit_complete(Habit *h) {
    if (h->completed_today) return -1;

    h->completed_today    = 1;
    h->total_completions += 1;
    h->last_completed     = time(NULL);

    // Verifica si fue completado ayer para mantener streak
    // (simplificado — en Fase 2 lo validas contra la DB)
    h->streak += 1;
    if (h->streak > h->best_streak)
        h->best_streak = h->streak;

    // Cada 7 días de streak el hábito evoluciona
    if (h->streak % 7 == 0)
        h->power_level += 1;

    return habit_calculate_xp(h);
}

// Se llama al inicio de cada nuevo día
void habits_reset_day(Habit *habits, int count) {
    time_t now = time(NULL);
    struct tm *today = localtime(&now);

    for (int i = 0; i < count; i++) {
        if (!habits[i].completed_today) {
            // Si no completó ayer, rompe el streak
            habits[i].streak = 0;
        }
        habits[i].completed_today = 0;
    }
}

// Cuántos hábitos completados hoy
int habits_completed_today(const Habit *habits, int count) {
    int n = 0;
    for (int i = 0; i < count; i++)
        if (habits[i].completed_today) n++;
    return n;
}
