#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <libnotify/notify.h>
#include "types.h"
#include "habits.h"
#include "db.h"

// Horas del día en las que manda recordatorios (formato 24h)
#define REMINDER_COUNT 3
static const int REMINDER_HOURS[REMINDER_COUNT] = {9, 14, 22};
static const char *REMINDER_LABELS[REMINDER_COUNT] = {
    "Mañana", "Tarde", "Noche"
};

// Evita mandar la misma notificación dos veces en la misma hora
static int last_notified_hour = -1;

static void send_notification(const char *title, const char *body,
                               NotifyUrgency urgency) {
    NotifyNotification *n = notify_notification_new(title, body, "dialog-information");
    notify_notification_set_urgency(n, urgency);
    notify_notification_set_timeout(n, 8000);   // 8 segundos
    notify_notification_show(n, NULL);
    g_object_unref(G_OBJECT(n));
}

static void check_and_notify(void) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    int hour = t->tm_hour;

    // Solo notifica en las horas definidas y una vez por hora
    int should_notify = 0;
    const char *period = "";
    for (int i = 0; i < REMINDER_COUNT; i++) {
        if (hour == REMINDER_HOURS[i] && hour != last_notified_hour) {
            should_notify = 1;
            period = REMINDER_LABELS[i];
            break;
        }
    }
    if (!should_notify) return;

    // Lee hábitos pendientes de la DB
    Habit habits[MAX_HABITS];
    int count = 0;
    if (db_open() < 0) return;
    db_load_habits(habits, &count);

    int pending = 0;
    char pending_names[512] = {0};

    for (int i = 0; i < count; i++) {
        if (!habits[i].completed_today) {
            pending++;
            if (strlen(pending_names) < 400) {
                strncat(pending_names, "- ", sizeof(pending_names) - strlen(pending_names) - 1);
                strncat(pending_names, habits[i].name,
                        sizeof(pending_names) - strlen(pending_names) - 1);
                strncat(pending_names, "\n", 2);
            }
        }
    }

    db_close();

    if (pending == 0) {
        // Todos completos — notificación de felicitación
        send_notification(
            " OHMIO — ¡Día Perfecto!",
            "Completaste todos tus hábitos de hoy.\n+XP bonus al cerrar la app.",
            NOTIFY_URGENCY_NORMAL
        );
    } else {
        // Hábitos pendientes
        char title[128];
        snprintf(title, sizeof(title),
                 " OHMIO — %s: %d hábito%s pendiente%s",
                 period, pending,
                 pending == 1 ? "" : "s",
                 pending == 1 ? "" : "s");

        char body[600];
        snprintf(body, sizeof(body), "%s", pending_names);

        NotifyUrgency urgency = (hour == 20)
            ? NOTIFY_URGENCY_CRITICAL   // noche = urgente
            : NOTIFY_URGENCY_NORMAL;

        send_notification(title, body, urgency);
    }

    last_notified_hour = hour;
}

// Notificación de streak en peligro — si son las 21h y quedan pendientes
static void check_streak_danger(void) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t->tm_hour != 21 || last_notified_hour == 21) return;

    Habit habits[MAX_HABITS];
    int count = 0;
    if (db_open() < 0) return;
    db_load_habits(habits, &count);

    for (int i = 0; i < count; i++) {
        if (!habits[i].completed_today && habits[i].streak >= 3) {
            char body[256];
            snprintf(body, sizeof(body),
                     "¡Tu racha de %d días en '%s' está en peligro!\n"
                     "Complétalo antes de medianoche.",
                     habits[i].streak, habits[i].name);

            send_notification(
                " ¡STREAK EN PELIGRO!",
                body,
                NOTIFY_URGENCY_CRITICAL
            );
        }
    }

    db_close();
    last_notified_hour = 21;
}

int main(void) {
    if (!notify_init("OHMIO Daemon")) {
        fprintf(stderr, "Error inicializando libnotify\n");
        return 1;
    }

    printf("OHMIO daemon iniciado. PID: %d\n", getpid());
    printf("Recordatorios a las: ");
    for (int i = 0; i < REMINDER_COUNT; i++)
        printf("%dh ", REMINDER_HOURS[i]);
    printf("\n");

    // Loop principal — revisa cada minuto
    while (1) {
        check_and_notify();
        check_streak_danger();
        sleep(60);
    }

    notify_uninit();
    return 0;
}
