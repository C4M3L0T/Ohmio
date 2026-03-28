#ifndef TYPES_H
#define TYPES_H

#include <time.h>

#define MAX_HABITS 20
#define MAX_NAME 64
#define MAX_ICON 8
#define XP_BASE 100 

typedef enum{
	CAT_PHYSICAL = 0,	//Habitos físicos
	CAT_MENTAL,		//Habitos mentales
	CAT_HEALTH,		//Vitalidad 
	CAT_SOCIAL,		//Carisma	
	CAT_CREATIVE,		//Mental + bonus
} HabitCategory;

typedef struct{
	int id;
	char name[MAX_NAME];
	char icon[MAX_ICON];
	int xp_reward;
	int streak;
	int best_streak;
	int total_completions;
	int completed_today;
	int power_level;
	HabitCategory category;
	time_t last_completed;
} Habit;

typedef struct{
	char name[MAX_NAME];
	int level;
	int xp;
	int xp_to_next;
	// Stats derivados de categorías de hábitos
	int strength;
	int wisdom;
	int vitality;
	int charisma;
	int discipline;
	int total_days_played;
	int perfect_days;
	time_t created_at;
} Hero;

typedef struct {
	int id;
	char name[MAX_NAME];
	char description[256];
	int xp_cost;
	int redeemed;
	time_t redeemed_at;
} Reward;

typedef enum {
    POMO_IDLE = 0,
    POMO_WORK,
    POMO_SHORT_BREAK,
    POMO_LONG_BREAK,
    POMO_DONE
} PomoStatus;

typedef struct {
    int work_minutes;        // duración del periodo de trabajo
    int short_break_minutes; // duración descanso corto
    int long_break_minutes;  // duración descanso largo
    int periods_per_cycle;   // periodos antes del descanso largo
    int xp_per_pomodoro;     // XP por cada pomodoro completado
    int xp_bonus_cycle;      // XP bonus al completar el ciclo completo
} PomoConfig;

typedef struct {
    PomoConfig config;
    PomoStatus status;
    int        current_period;   // 1-based, qué pomodoro vas
    int        completed_today;  // cuántos pomodoros completó hoy
    time_t     period_start;     // cuando arrancó el periodo actual
    int        paused;           // 0 o 1
    time_t     pause_start;      // cuando pausó
    int        pause_elapsed;    // segundos acumulados en pausa
    int        xp_earned_session;// XP ganado en esta sesión
} PomodoroState;

typedef struct {
	Habit habits[MAX_HABITS];
	int habit_count;
	Hero hero;
	Reward rewards[50];
	int reward_count;
	int running;
	PomodoroState pomo;
} GameState;

typedef struct {
    char date[12];          // "YYYY-MM-DD"
    int  habits_completed;
    int  habits_total;
    int  xp_earned;
    int  mood;
    float sleep_hours;
    int  perfect_day;
} DailyLog;





#endif
