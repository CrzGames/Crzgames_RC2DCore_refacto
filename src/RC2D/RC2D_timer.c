#include <RC2D/RC2D_internal.h> // Required for : rc2d_delta_time
#include <RC2D/RC2D_logger.h> // Required for : RC2D_log
#include <RC2D/RC2D_timer.h> // Required for : RC2D_Timer

#include <math.h> // Required for : round()

double rc2d_timer_getDelta(void) 
{
	return rc2d_delta_time;
}

int rc2d_timer_getFPS(void)
{
    // Calcule le FPS en prenant l'inverse du delta temps entre deux images
    double fps = 1.0 / rc2d_timer_getDelta();

    /**
     * Utilise round pour arrondir le résultat à l'entier le plus proche avant de convertir en int.
     * Cela assure que le FPS est arrondi de manière conventionnelle (0.5 et au-dessus est arrondi vers le haut)
     * pour fournir une estimation plus précise et intuitive de la fréquence d'images.
     */
    return (int)round(fps);
}

/**
 * \brief Initialise le timer de haute précision utilisé pour mesurer le temps écoulé. 
 * 
 * Cette fonction est appelée automatiquement lors de l'initialisation du système de timer de RC2D 
 * et ne doit généralement pas être appelée directement.
 * 
 * \threadsafety Il est possible d'appeler cette fonction en toute sécurité à partir de n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static Uint64 startTime = 0;
void rc2d_timer_init(void) 
{
    startTime = SDL_GetPerformanceCounter();
}

double rc2d_timer_getTime(void) 
{
    Uint64 now = SDL_GetPerformanceCounter();
    double elapsedTime = (double)(now - startTime) / (double)SDL_GetPerformanceFrequency();
    return elapsedTime;
}

void rc2d_timer_sleep(const double seconds) 
{
    if (seconds <= 0)
    {
        RC2D_log(RC2D_LOG_WARN, "rc2d_timer_sleep warning : seconds is less than or equal to 0 \n");
        return;
    }

    // Convertit les secondes en nanosecondes
    Uint64 ns = (Uint64)(seconds * 1e9);
    SDL_DelayPrecise(ns);
}

bool rc2d_timer_addTimer(RC2D_Timer *timer)
{
    if (timer == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "rc2d_timer_addTimer error : timer is NULL \n");
        return false;
    }

    timer->id = SDL_AddTimerNS(timer->interval, timer->callback_func, timer->userdata);
    if (timer->id == 0)
    {
        RC2D_log(RC2D_LOG_ERROR, "rc2d_timer_addTimer error to SDL_AddTimer : %s \n", SDL_GetError());
		return false;
    }

	return true;
}

bool rc2d_timer_removeTimer(RC2D_Timer *timer)
{
    if (timer == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "rc2d_timer_removeTimer error : timer is NULL \n");
        return false;
    }

    if (SDL_RemoveTimer(timer->id))
    {
        timer = NULL;
        return true;
    }

    RC2D_log(RC2D_LOG_ERROR, "rc2d_timer_removeTimer error to SDL_RemoveTimer : %s \n", SDL_GetError());
    return false;
}
