#include <time.h>
#include <string.h>
#include "pomodoro.h"
#include "hero.h"
#include "db.h"

void pomo_init(PomodoroState *p) {
    p->config.work_minutes        = 25;
    p->config.short_break_minutes = 5;
    p->config.long_break_minutes  = 15;
    p->config.periods_per_cycle   = 4;
    p->config.xp_per_pomodoro     = 30;
    p->config.xp_bonus_cycle      = 100;

    p->status           = POMO_IDLE;
    p->current_period   = 1;
    p->completed_today  = 0;
    p->period_start     = 0;
    p->paused           = 0;
    p->pause_start      = 0;
    p->pause_elapsed    = 0;
    p->xp_earned_session = 0;
}

// Segundos totales del periodo actual
int pomo_period_seconds(const PomodoroState *p) {
    switch (p->status) {
        case POMO_WORK:        return p->config.work_minutes        * 60;
        case POMO_SHORT_BREAK: return p->config.short_break_minutes * 60;
        case POMO_LONG_BREAK:  return p->config.long_break_minutes  * 60;
        default:               return 0;
    }
}

// Segundos transcurridos (descontando pausas)
int pomo_elapsed(const PomodoroState *p) {
    if (p->period_start == 0) return 0;
    int elapsed = (int)(time(NULL) - p->period_start) - p->pause_elapsed;
    if (p->paused)
        elapsed -= (int)(time(NULL) - p->pause_start);
    return elapsed < 0 ? 0 : elapsed;
}

// Segundos restantes
int pomo_remaining(const PomodoroState *p) {
    int total   = pomo_period_seconds(p);
    int elapsed = pomo_elapsed(p);
    int rem     = total - elapsed;
    return rem < 0 ? 0 : rem;
}

// Arranca un nuevo periodo de trabajo
void pomo_start(PomodoroState *p) {
    if (p->status == POMO_IDLE || p->status == POMO_DONE) {
        p->current_period = 1;
        p->xp_earned_session = 0;
    }
    p->status        = POMO_WORK;
    p->period_start  = time(NULL);
    p->paused        = 0;
    p->pause_elapsed = 0;
}

void pomo_pause_toggle(PomodoroState *p) {
    if (p->status == POMO_IDLE || p->status == POMO_DONE) return;

    if (p->paused) {
        // Reanuda: acumula el tiempo pausado
        p->pause_elapsed += (int)(time(NULL) - p->pause_start);
        p->paused = 0;
    } else {
        p->pause_start = time(NULL);
        p->paused = 1;
    }
}

void pomo_stop(PomodoroState *p) {
    p->status        = POMO_IDLE;
    p->period_start  = 0;
    p->paused        = 0;
    p->pause_elapsed = 0;
}

// Llama cada frame — retorna 1 si el periodo terminó (para notificar/animar)
int pomo_tick(PomodoroState *p, Hero *hero) {
    if (p->status == POMO_IDLE || p->status == POMO_DONE) return 0;
    if (p->paused) return 0;
    if (pomo_remaining(p) > 0) return 0;

    // El periodo terminó
    if (p->status == POMO_WORK) {
        p->completed_today++;
        int xp = p->config.xp_per_pomodoro;

        // ¿Completó un ciclo completo?
        int cycle_done = (p->current_period % p->config.periods_per_cycle == 0);
        if (cycle_done) xp += p->config.xp_bonus_cycle;

        p->xp_earned_session += xp;
        hero_add_xp(hero, xp);
        db_save_hero(hero);

        // Siguiente: descanso corto o largo
        if (cycle_done) {
            p->status = POMO_LONG_BREAK;
        } else {
            p->status = POMO_SHORT_BREAK;
        }
    } else {
        // Terminó un descanso — siguiente pomodoro o fin del ciclo
        if (p->current_period >= p->config.periods_per_cycle &&
            p->status == POMO_LONG_BREAK) {
            p->status = POMO_DONE;
            return 1;
        }
        p->current_period++;
        p->status = POMO_WORK;
    }

    // Reinicia el timer para el nuevo periodo
    p->period_start  = time(NULL);
    p->pause_elapsed = 0;
    p->paused        = 0;
    return 1;
}
