#include "rc2d/RC2D.h"

#include <math.h> // Required for : round()

/**
 * Renvoie le temps écoulé en secondes entre les deux dernières images. Cette fonction est utile pour
 * mettre à jour les éléments du jeu en fonction du temps réel plutôt que des cycles de l'horloge du processeur.
 * 
 * @return Temps en secondes écoulé entre les deux dernières images.
 */
double rc2d_timer_getDelta(void) 
{
	return rc2d_deltaTime;
}

/**
 * Calcule et renvoie le nombre de frames par seconde (FPS) actuel. Cette fonction est utile pour le débogage
 * et l'optimisation de la performance, en fournissant une mesure en temps réel de la fluidité du jeu.
 * 
 * @return Le nombre actuel de frames par seconde.
 */
int rc2d_timer_getFPS(void)
{
    // Calcule le FPS en prenant l'inverse du delta temps entre deux images
    double fps = 1.0 / rc2d_timer_getDelta();

    // Utilise round pour arrondir le résultat à l'entier le plus proche avant de convertir en int.
    // Cela assure que le FPS est arrondi de manière conventionnelle (0.5 et au-dessus est arrondi vers le haut)
    // pour fournir une estimation plus précise et intuitive de la fréquence d'images.
    return (int)round(fps);
}

/**
 * Initialise le timer de haute précision utilisé pour mesurer le temps écoulé. Cette fonction est appelée
 * automatiquement lors de l'initialisation du système de timer de RC2D et ne doit généralement pas être appelée directement.
 */
static Uint64 startTime = 0;
void rc2d_timer_init(void) 
{
    startTime = SDL_GetPerformanceCounter();
}

/**
 * Renvoie le temps écoulé en secondes depuis l'initialisation du timer de haute précision. Cette fonction est
 * utile pour mesurer des durées précises dans le jeu, comme le temps écoulé depuis le début d'un événement.
 * 
 * @return Le temps écoulé en secondes depuis l'initialisation du timer.
 */
double rc2d_timer_getTime(void) 
{
    Uint64 now = SDL_GetPerformanceCounter();
    double elapsedTime = (double)(now - startTime) / (double)SDL_GetPerformanceFrequency();
    return elapsedTime;
}

/**
 * Met en pause le thread actuel pendant une durée spécifiée en secondes. Cette fonction est utile pour
 * introduire des délais sans bloquer l'exécution de tout le programme.
 * 
 * @param seconds Le nombre de secondes pendant lesquelles le thread actuel doit être mis en pause.
 */
void rc2d_timer_sleep(const double seconds) 
{
    if (seconds <= 0)
    {
        rc2d_log(RC2D_LOG_WARN, "rc2d_timer_sleep warning : seconds is less than or equal to 0 \n");
        return;
    }

    // Convertit les secondes en millisecondes
    Uint32 ms = (Uint32)(seconds * 1000.0);
    SDL_Delay(ms);
}


/**
 * Ajoute un timer qui déclenchera une fonction de rappel après un intervalle spécifié. Cette fonction est utile
 * pour planifier des événements futurs sans bloquer l'exécution principale du programme.
 * 
 * @param timer Un pointeur vers une structure RC2D_Timer contenant les informations du timer à ajouter.
 * @return Retourne true si le timer a été ajouté avec succès, false en cas d'échec.
 */
bool rc2d_timer_addTimer(RC2D_Timer *timer)
{
    if (timer == NULL)
    {
        rc2d_log(RC2D_LOG_ERROR, "rc2d_timer_addTimer error : timer is NULL \n");
        return false;
    }

    timer->id = SDL_AddTimer(timer->interval, timer->callback_func, timer->callback_param);
    if (timer->id == 0)
    {
        rc2d_log(RC2D_LOG_ERROR, "rc2d_timer_addTimer error to SDL_AddTimer : %s \n", SDL_GetError());
		return false;
    }

	return true;
}

/**
 * Supprime un timer précédemment ajouté. Cette fonction est utile pour annuler des événements planifiés
 * lorsque les conditions changent ou lorsque l'événement n'est plus nécessaire.
 * 
 * @param timer Un pointeur vers une structure RC2D_Timer contenant les informations du timer à supprimer.
 * @return Retourne true si le timer a été supprimé avec succès, false en cas d'échec.
 */
bool rc2d_timer_removeTimer(RC2D_Timer *timer)
{
    if (timer == NULL)
    {
        rc2d_log(RC2D_LOG_ERROR, "rc2d_timer_removeTimer error : timer is NULL \n");
        return false;
    }

    if (SDL_RemoveTimer(timer->id) == SDL_TRUE)
    {
        timer = NULL;
        return true;
    }

    rc2d_log(RC2D_LOG_ERROR, "rc2d_timer_removeTimer error to SDL_RemoveTimer : %s \n", SDL_GetError());
    return false;
}
