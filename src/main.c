#include <stdio.h>
#include "types.h"
#include "hero.h"
#include "habits.h"
#include "db.h"

int main(void) {
    if (db_open() < 0) return 1;

    // Primera vez: crear héroe
    Hero h;
    if (db_load_hero(&h) < 0) {
        hero_init(&h, "Angel");
        db_save_hero(&h);
    }

    // Crear un hábito de prueba
    Habit habits[MAX_HABITS];
    int count;
    db_load_habits(habits, &count);

    if (count == 0) {
        habit_init(&habits[0], 0, "Leer 30 min", "📚", 50, CAT_MENTAL);
        habit_init(&habits[1], 1, "Ejercicio",   "⚔",  80, CAT_PHYSICAL);
        count = 2;
        db_save_habit(&habits[0]);
        db_save_habit(&habits[1]);
    }

    // Completar hábito 0
    int xp = habit_complete(&habits[0]);
    if (xp > 0) {
        printf("Hábito completado! +%d XP\n", xp);
        db_log_completion(habits[0].id, xp, habits[0].streak);
        db_save_habit(&habits[0]);
        int leveled = hero_add_xp(&h, xp);
        if (leveled) printf("⚡ LEVEL UP! Ahora eres nivel %d\n", h.level);
        db_save_hero(&h);
    }

    printf("Héroe: %s | Nivel %d | XP %d/%d\n",
           h.name, h.level, h.xp, h.xp_to_next);

    db_close();
    return 0;
}
