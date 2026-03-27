#include <stdio.h>
#include "types.h"
#include "hero.h"
#include "habits.h"
#include "db.h"
#include "tui.h"

int main(void) {
    if (db_open() < 0) return 1;

    GameState gs = {0};

    // Carga o crea héroe
    if (db_load_hero(&gs.hero) < 0) {
        hero_init(&gs.hero, "Angel");
        db_save_hero(&gs.hero);
    }

    // Carga hábitos
    db_load_habits(gs.habits, &gs.habit_count);

    // Arranca la TUI
    tui_init();
    tui_run(&gs);
    tui_teardown();

    // Guarda snapshot del día al salir
    int done = habits_completed_today(gs.habits, gs.habit_count);
    int perfect = (done == gs.habit_count && gs.habit_count > 0);
    db_save_daily_log(done, gs.habit_count, gs.hero.xp,
                      3, 7.5, perfect);

    db_close();
    return 0;
}
