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
	time_t last_completed
} Habit;


