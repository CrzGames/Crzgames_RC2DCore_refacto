#include <RC2D/RC2D_internal.h>

/**
 * Signale à l'application que le jeu doit se terminer.
 *
 * Cette fonction modifie rc2d_game_is_running utilisé pour contrôler la boucle principale du jeu.
 */
void rc2d_event_quit(void)
{
	rc2d_game_is_running = false;
}