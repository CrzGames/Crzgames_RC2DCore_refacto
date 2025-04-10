#include "rc2d/rc2d_event.h"
#include "rc2d/RC2D.h"

/**
 * Vide la file d'événements de tous les événements en attente.
 *
 * Cette fonction est destinée à être utilisée lorsque vous avez besoin de réinitialiser
 * l'état des événements, par exemple, lors du chargement d'une nouvelle scène ou lorsqu'une pause
 * est terminée et que vous ne voulez pas que les événements en attente soient traités.
 */
void rc2d_event_clear(void)
{
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
}

/**
 * Interroge la file d'événements pour les événements en attente et les stocke dans la structure rc2d_event.
 *
 * @return 1 si un événement est disponible, 0 si aucun événement n'est disponible.
 */
int rc2d_event_poll(void)
{
	return SDL_PollEvent(&rc2d_event);
}

/**
 * Met à jour la file d'événements avec les événements en attente.
 *
 * Cette fonction doit être appelée régulièrement pour s'assurer que les événements
 * du système d'exploitation sont traités et ajoutés à la file d'événements.
 */
void rc2d_event_pump(void)
{
	SDL_PumpEvents();
}

/**
 * Ajoute un événement personnalisé à la file d'événements.
 *
 * @return 1 si l'événement est correctement ajouté, 0 en cas d'erreur.
 */
int rc2d_event_push(void)
{
	return SDL_PushEvent(&rc2d_event);
}

/**
 * Signale à l'application que le jeu doit se terminer.
 *
 * Cette fonction modifie le flag gameIsRunning utilisé pour contrôler la boucle principale du jeu.
 */
void rc2d_event_quit(void)
{
	gameIsRunning = false;
}

/**
 * Attend qu'un événement soit disponible et le stocke dans la structure rc2d_event.
 *
 * @return 1 si un événement est traité, 0 en cas d'erreur ou si le délai d'attente est dépassé.
 */
int rc2d_event_wait(void)
{
	int result = SDL_WaitEvent(&rc2d_event);
	if (result == 0)
	{
		rc2d_log(RC2D_LOG_ERROR, "SDL_WaitEvent in rc2d_event_wait: %s", SDL_GetError());
	}

	return result;
}