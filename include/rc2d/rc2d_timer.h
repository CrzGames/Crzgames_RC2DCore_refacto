#ifndef RC2D_TIMER_H
#define RC2D_TIMER_H

#include <SDL.h> 
#include <stdbool.h> // Required for : bool

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure représentant un minuteur.
 * @typedef {struct} RC2D_Timer
 * @property {SDL_TimerID} id - L'identifiant du minuteur SDL.
 * @property {Uint32} interval - L'intervalle de temps en millisecondes.
 * @property {SDL_TimerCallback} callback_func - La fonction de rappel du minuteur.
 * @property {void*} callback_param - Le paramètre passé à la fonction de rappel.
 */
typedef struct RC2D_Timer {
	SDL_TimerID id;                
	Uint32 interval;                
	SDL_TimerCallback callback_func;
	void* callback_param;           
} RC2D_Timer;

bool rc2d_timer_addTimer(RC2D_Timer *timer);
bool rc2d_timer_removeTimer(RC2D_Timer *timer);

double rc2d_timer_getDelta(void);

int rc2d_timer_getFPS(void);

void rc2d_timer_init(void); // Internal use in engine
double rc2d_timer_getTime(void);

void rc2d_timer_sleep(const double seconds);

#ifdef __cplusplus
};
#endif

#endif // RC2D_TIMER_H