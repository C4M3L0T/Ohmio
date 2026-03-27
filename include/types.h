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

typedef struct {
	Habit habits[MAX_HABITS];
	int habit_count;
	Hero hero;
	Reward rewards[50];
	int reward_count;
	int running;
} GameState;

#endif
