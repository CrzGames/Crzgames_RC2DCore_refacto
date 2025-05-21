#ifndef RC2D_EVENT_H
#define RC2D_EVENT_H

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Signal à l'application que le jeu doit se terminer.
 *
 * Cette fonction modifie rc2d_game_is_running utilisé pour contrôler la boucle principale du jeu.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_event_quit(void);

#ifdef __cplusplus
}
#endif

#endif // RC2D_EVENT_H