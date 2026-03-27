#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "db.h"

static sqlite3 *DB = NULL;

// Crea el directorio ~/.ohmio/ si no existe
static void ensure_data_dir(void) {
    const char *home = getenv("HOME");
    char path[256];
    snprintf(path, sizeof(path), "%s/.ohmio", home);
    mkdir(path, 0755);  // no falla si ya existe
}

static const char *get_db_path(void) {
    static char path[256];
    snprintf(path, sizeof(path), "%s/.ohmio/data.db", getenv("HOME"));
    return path;
}

int db_open(void) {
    ensure_data_dir();
    int rc = sqlite3_open(get_db_path(), &DB);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error abriendo DB: %s\n", sqlite3_errmsg(DB));
        return -1;
    }
    // WAL mode: más rápido para escrituras frecuentes
    sqlite3_exec(DB, "PRAGMA journal_mode=WAL;", NULL, NULL, NULL);
    return db_create_tables();
}

void db_close(void) {
    if (DB) sqlite3_close(DB);
}

int db_create_tables(void) {
    const char *sql =
        // Héroe — siempre una sola fila (id=1)
        "CREATE TABLE IF NOT EXISTS hero ("
        "  id INTEGER PRIMARY KEY,"
        "  name TEXT NOT NULL,"
        "  level INTEGER DEFAULT 1,"
        "  xp INTEGER DEFAULT 0,"
        "  xp_to_next INTEGER DEFAULT 100,"
        "  strength INTEGER DEFAULT 1,"
        "  wisdom INTEGER DEFAULT 1,"
        "  vitality INTEGER DEFAULT 1,"
        "  charisma INTEGER DEFAULT 1,"
        "  discipline INTEGER DEFAULT 0,"
        "  total_days_played INTEGER DEFAULT 0,"
        "  perfect_days INTEGER DEFAULT 0,"
        "  created_at INTEGER"
        ");"

        "CREATE TABLE IF NOT EXISTS habits ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL,"
        "  icon TEXT,"
        "  xp_reward INTEGER DEFAULT 50,"
        "  streak INTEGER DEFAULT 0,"
        "  best_streak INTEGER DEFAULT 0,"
        "  total_completions INTEGER DEFAULT 0,"
        "  completed_today INTEGER DEFAULT 0,"
        "  power_level INTEGER DEFAULT 1,"
        "  category INTEGER DEFAULT 0,"
        "  last_completed INTEGER DEFAULT 0"
        ");"

        // Cada vez que completas un hábito queda registrado
        "CREATE TABLE IF NOT EXISTS completions ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  habit_id INTEGER,"
        "  completed_at INTEGER,"  // timestamp
        "  xp_earned INTEGER,"
        "  streak_at_time INTEGER,"
        "  FOREIGN KEY(habit_id) REFERENCES habits(id)"
        ");"

        "CREATE TABLE IF NOT EXISTS rewards ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL,"
        "  description TEXT,"
        "  xp_cost INTEGER DEFAULT 500,"
        "  redeemed INTEGER DEFAULT 0,"
        "  redeemed_at INTEGER DEFAULT 0"
        ");"

        // Para el análisis semanal: snapshot diario
        "CREATE TABLE IF NOT EXISTS daily_log ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  date TEXT UNIQUE,"   // formato "YYYY-MM-DD"
        "  habits_completed INTEGER,"
        "  habits_total INTEGER,"
        "  xp_earned INTEGER,"
        "  mood INTEGER,"       // 1-5
        "  sleep_hours REAL,"   // ej: 7.5
        "  perfect_day INTEGER DEFAULT 0"
        ");";

    char *err = NULL;
    int rc = sqlite3_exec(DB, sql, NULL, NULL, &err);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error creando tablas: %s\n", err);
        sqlite3_free(err);
        return -1;
    }
    return 0;
}

// --- HERO ---

int db_save_hero(const Hero *h) {
    const char *sql =
        "INSERT INTO hero (id,name,level,xp,xp_to_next,strength,wisdom,"
        "vitality,charisma,discipline,total_days_played,perfect_days,created_at)"
        "VALUES (1,?,?,?,?,?,?,?,?,?,?,?,?)"
        "ON CONFLICT(id) DO UPDATE SET"
        "  name=excluded.name, level=excluded.level, xp=excluded.xp,"
        "  xp_to_next=excluded.xp_to_next, strength=excluded.strength,"
        "  wisdom=excluded.wisdom, vitality=excluded.vitality,"
        "  charisma=excluded.charisma, discipline=excluded.discipline,"
        "  total_days_played=excluded.total_days_played,"
        "  perfect_days=excluded.perfect_days;";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(DB, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1,  h->name, -1, SQLITE_STATIC);
    sqlite3_bind_int (stmt, 2,  h->level);
    sqlite3_bind_int (stmt, 3,  h->xp);
    sqlite3_bind_int (stmt, 4,  h->xp_to_next);
    sqlite3_bind_int (stmt, 5,  h->strength);
    sqlite3_bind_int (stmt, 6,  h->wisdom);
    sqlite3_bind_int (stmt, 7,  h->vitality);
    sqlite3_bind_int (stmt, 8,  h->charisma);
    sqlite3_bind_int (stmt, 9,  h->discipline);
    sqlite3_bind_int (stmt, 10, h->total_days_played);
    sqlite3_bind_int (stmt, 11, h->perfect_days);
    sqlite3_bind_int (stmt, 12, (int)h->created_at);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int db_load_hero(Hero *h) {
    const char *sql =
        "SELECT name,level,xp,xp_to_next,strength,wisdom,vitality,"
        "charisma,discipline,total_days_played,perfect_days,created_at"
        " FROM hero WHERE id=1;";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(DB, sql, -1, &stmt, NULL);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return -1;  // no existe aún
    }

    strncpy(h->name, (const char*)sqlite3_column_text(stmt, 0), MAX_NAME);
    h->level             = sqlite3_column_int(stmt, 1);
    h->xp                = sqlite3_column_int(stmt, 2);
    h->xp_to_next        = sqlite3_column_int(stmt, 3);
    h->strength          = sqlite3_column_int(stmt, 4);
    h->wisdom            = sqlite3_column_int(stmt, 5);
    h->vitality          = sqlite3_column_int(stmt, 6);
    h->charisma          = sqlite3_column_int(stmt, 7);
    h->discipline        = sqlite3_column_int(stmt, 8);
    h->total_days_played = sqlite3_column_int(stmt, 9);
    h->perfect_days      = sqlite3_column_int(stmt, 10);
    h->created_at        = (time_t)sqlite3_column_int(stmt, 11);

    sqlite3_finalize(stmt);
    return 0;
}

// --- HABITS ---

int db_save_habit(const Habit *h) {
    const char *sql =
        "INSERT INTO habits (id,name,icon,xp_reward,streak,best_streak,"
        "total_completions,completed_today,power_level,category,last_completed)"
        "VALUES (?,?,?,?,?,?,?,?,?,?,?)"
        "ON CONFLICT(id) DO UPDATE SET"
        "  streak=excluded.streak, best_streak=excluded.best_streak,"
        "  total_completions=excluded.total_completions,"
        "  completed_today=excluded.completed_today,"
        "  power_level=excluded.power_level,"
        "  last_completed=excluded.last_completed;";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(DB, sql, -1, &stmt, NULL);
    sqlite3_bind_int (stmt, 1, h->id);
    sqlite3_bind_text(stmt, 2, h->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, h->icon, -1, SQLITE_STATIC);
    sqlite3_bind_int (stmt, 4, h->xp_reward);
    sqlite3_bind_int (stmt, 5, h->streak);
    sqlite3_bind_int (stmt, 6, h->best_streak);
    sqlite3_bind_int (stmt, 7, h->total_completions);
    sqlite3_bind_int (stmt, 8, h->completed_today);
    sqlite3_bind_int (stmt, 9, h->power_level);
    sqlite3_bind_int (stmt, 10, h->category);
    sqlite3_bind_int (stmt, 11, (int)h->last_completed);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int db_load_habits(Habit *habits, int *count) {
    const char *sql =
        "SELECT id,name,icon,xp_reward,streak,best_streak,"
        "total_completions,completed_today,power_level,category,last_completed"
        " FROM habits ORDER BY id;";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(DB, sql, -1, &stmt, NULL);
    *count = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW && *count < MAX_HABITS) {
        Habit *h = &habits[*count];
        h->id               = sqlite3_column_int(stmt, 0);
        strncpy(h->name, (const char*)sqlite3_column_text(stmt, 1), MAX_NAME);
        strncpy(h->icon, (const char*)sqlite3_column_text(stmt, 2), MAX_ICON);
        h->xp_reward        = sqlite3_column_int(stmt, 3);
        h->streak           = sqlite3_column_int(stmt, 4);
        h->best_streak      = sqlite3_column_int(stmt, 5);
        h->total_completions = sqlite3_column_int(stmt, 6);
        h->completed_today  = sqlite3_column_int(stmt, 7);
        h->power_level      = sqlite3_column_int(stmt, 8);
        h->category         = sqlite3_column_int(stmt, 9);
        h->last_completed   = (time_t)sqlite3_column_int(stmt, 10);
        (*count)++;
    }

    sqlite3_finalize(stmt);
    return 0;
}

// Registra un completion en el historial
int db_log_completion(int habit_id, int xp_earned, int streak) {
    const char *sql =
        "INSERT INTO completions (habit_id, completed_at, xp_earned, streak_at_time)"
        "VALUES (?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(DB, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, habit_id);
    sqlite3_bind_int(stmt, 2, (int)time(NULL));
    sqlite3_bind_int(stmt, 3, xp_earned);
    sqlite3_bind_int(stmt, 4, streak);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

// Guarda el snapshot del día (llamar al cerrar la app)
int db_save_daily_log(int completed, int total, int xp,
                      int mood, float sleep, int perfect) {
    const char *sql =
        "INSERT INTO daily_log "
        "(date, habits_completed, habits_total, xp_earned, mood, sleep_hours, perfect_day)"
        "VALUES (date('now'), ?, ?, ?, ?, ?, ?)"
        "ON CONFLICT(date) DO UPDATE SET"
        "  habits_completed=excluded.habits_completed,"
        "  xp_earned=excluded.xp_earned,"
        "  mood=excluded.mood,"
        "  sleep_hours=excluded.sleep_hours,"
        "  perfect_day=excluded.perfect_day;";

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(DB, sql, -1, &stmt, NULL);
    sqlite3_bind_int  (stmt, 1, completed);
    sqlite3_bind_int  (stmt, 2, total);
    sqlite3_bind_int  (stmt, 3, xp);
    sqlite3_bind_int  (stmt, 4, mood);
    sqlite3_bind_double(stmt, 5, sleep);
    sqlite3_bind_int  (stmt, 6, perfect);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 0 : -1;
}
