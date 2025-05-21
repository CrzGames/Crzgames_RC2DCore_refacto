#ifndef RC2D_TIMER_H
#define RC2D_TIMER_H

#include <SDL3/SDL_timer.h> // Required for : SDL_TimerID, SDL_TimerCallback, Uint32

#include <stdbool.h> // Required for : bool

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Structure représentant un minuteur haute précision basé sur des nanosecondes.
 * 
 * Ce minuteur permet d'exécuter une fonction de rappel à des intervalles réguliers spécifiés en nanosecondes.
 * Le rappel est exécuté sur un thread séparé géré par SDL.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_Timer {
	/**
	 * Identifiant unique du minuteur SDL.
	 */
	SDL_TimerID id;
	
    /**
     * \brief Intervalle entre deux exécutions de la callback, en nanosecondes.
     * 
     * Cette valeur détermine la fréquence à laquelle la fonction `callback_func` est appelée.
     * Exemple : `1000000000` = 1 seconde.
     */
	Uint64 interval;
	
    /**
     * \brief Pointeur vers la fonction de rappel exécutée à chaque intervalle.
     * 
     * La fonction doit suivre le prototype `SDL_NSTimerCallback`, prendre trois paramètres :
     * `(void *userdata, SDL_TimerID timerID, Uint64 interval)` et retourner un `Uint64`.
     * 
     * Si la fonction retourne `0`, le minuteur est annulé et supprimé.
     * Si elle retourne une valeur différente de `0`, le minuteur continue à s'exécuter avec le nouvel intervalle spécifié.
     */
	SDL_NSTimerCallback callback_func;

    /**
     * \brief Paramètre utilisateur transmis au callback.
     * 
     * Ce pointeur permet de passer des données personnalisées au callback.
     */
	void* userdata;           
} RC2D_Timer;

/**
 * \brief Ajoute un timer qui déclenchera une fonction de rappel après un intervalle spécifié. 
 * 
 * Cette fonction est utile pour planifier des événements futurs sans bloquer l'exécution principale du programme.
 * 
 * \param {RC2D_Timer *} timer - Pointeur vers la structure RC2D_Timer contenant les informations du timer.
 * \return {bool} - true si le timer a été ajouté avec succès, false sinon.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_timer_removeTimer
 */
bool rc2d_timer_addTimer(RC2D_Timer *timer);

/**
 * \brief Supprime un timer précédemment ajouté. 
 * 
 * Cette fonction est utile pour annuler des événements planifiés lorsque les conditions 
 * changent ou lorsque l'événement n'est plus nécessaire.
 * 
 * \param {RC2D_Timer *} timer - Pointeur vers la structure RC2D_Timer à supprimer.
 * \return {bool} - true si le timer a été supprimé avec succès, false sinon.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_timer_addTimer
 */
bool rc2d_timer_removeTimer(RC2D_Timer *timer);

/**
 * \brief Renvoie le temps écoulé en secondes entre les deux dernières images. 
 * 
 * Cette fonction est utile pour mettre à jour les éléments du jeu en fonction du temps réel 
 * plutôt que des cycles de l'horloge du processeur.
 * 
 * \return Le temps écoulé en secondes entre les deux dernières images.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
double rc2d_timer_getDelta(void);

/**
 * \brief Calcule et renvoie le nombre de frames par seconde (FPS) actuel. 
 * 
 * Cette fonction est utile pour le débogage et l'optimisation de la performance, 
 * en fournissant une mesure en temps réel de la fluidité du jeu.
 * 
 * \return Le nombre actuel de frames par seconde.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
int rc2d_timer_getFPS(void);

/**
 * \brief Renvoie le temps écoulé en secondes depuis l'initialisation du timer de haute précision. 
 * 
 * Cette fonction est utile pour mesurer des durées précises dans le jeu, 
 * comme le temps écoulé depuis le début d'un événement.
 * 
 * \return Le temps écoulé en secondes depuis l'initialisation du timer.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
double rc2d_timer_getTime(void);

/**
 * \brief Met en pause le thread actuel pendant une durée spécifiée en secondes. 
 * 
 * Cette fonction est utile pour introduire des délais sans bloquer l'exécution de tout le programme.
 * 
 * \param {double} seconds - Le nombre de secondes pendant lesquelles le thread doit être mis en pause.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_timer_sleep(const double seconds);

#ifdef __cplusplus
};
#endif

#endif // RC2D_TIMER_H