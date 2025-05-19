#ifndef RC2D_THREAD_H
#define RC2D_THREAD_H

#include <SDL3/SDL_thread.h> // Requis pour : SDL_Thread, SDL_ThreadFunction, SDL_TLSID, SDL_ThreadPriority
#include <stdbool.h>         // Requis pour : bool

/* Configuration pour les définitions de fonctions C, même en C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Structure opaque représentant un thread dans RC2D.
 *
 * Encapsule SDL_Thread pour fournir une interface de haut niveau pour la gestion des threads.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct SDL_Thread RC2D_Thread;

/**
 * \brief Signature des fonctions d'entrée des threads.
 *
 * \param data Données définies par l'utilisateur passées au thread.
 * \return Un entier représentant le code de sortie du thread.
 *
 * \since Ce type est disponible depuis RC2D 1.0.0.
 */
typedef int (*RC2D_ThreadFunction)(void *data);

/**
 * \brief Niveaux de priorité des threads dans RC2D.
 *
 * Correspond à SDL_ThreadPriority pour définir la priorité de planification des threads.
 *
 * \since Cette énumération est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_ThreadPriority {
    /**
     * Priorité basse, adaptée aux tâches en arrière-plan comme les logs.
     */
    RC2D_THREAD_PRIORITY_LOW = 0,

    /**
     * Priorité normale, adaptée à la plupart des tâches de jeu.
     */
    RC2D_THREAD_PRIORITY_NORMAL = 1,

    /**
     * Priorité haute, adaptée aux tâches critiques comme la physique.
     */
    RC2D_THREAD_PRIORITY_HIGH = 2,

    /**
     * Priorité critique, pour les tâches extrêmement sensibles au temps.
     * \note Peut ne pas être supportée sur toutes les plateformes.
     */
    RC2D_THREAD_PRIORITY_TIME_CRITICAL = 3
} RC2D_ThreadPriority;

/**
 * \brief État d'un thread dans RC2D.
 *
 * Indique l'état actuel d'un thread, basé sur SDL_ThreadState.
 *
 * \since Cette énumération est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_ThreadState {
    /**
     * Le thread est invalide ou inconnu.
     */
    RC2D_THREAD_STATE_UNKNOWN = 0,

    /**
     * Le thread est en cours d'exécution.
     */
    RC2D_THREAD_STATE_ALIVE = 1,

    /**
     * Le thread est détaché et ne peut pas être attendu.
     */
    RC2D_THREAD_STATE_DETACHED = 2,

    /**
     * Le thread a terminé et doit être nettoyé avec rc2d_thread_wait.
     */
    RC2D_THREAD_STATE_COMPLETE = 3
} RC2D_ThreadState;

/**
 * \brief Structure pour les options avancées de création de threads.
 *
 * Permet de personnaliser les propriétés des threads comme la taille de la pile et la priorité.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_ThreadOptions {
    /**
     * Taille de la pile du thread en octets (0 pour la valeur par défaut du système).
     */
    Uint32 stack_size;

    /**
     * Priorité initiale du thread.
     */
    RC2D_ThreadPriority priority;

    /**
     * Indique si le thread doit être détaché automatiquement à la fin.
     */
    bool auto_detach;
} RC2D_ThreadOptions;

/**
 * \brief Crée un nouveau thread avec des options par défaut.
 *
 * \param fn Fonction à exécuter dans le thread.
 * \param name Nom du thread (UTF-8, utilisé pour le débogage).
 * \param data Données utilisateur passées à la fonction.
 * \return Pointeur vers RC2D_Thread en cas de succès, NULL sinon.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
RC2D_Thread *rc2d_thread_new(RC2D_ThreadFunction fn, const char *name, void *data);

/**
 * \brief Crée un nouveau thread avec des options personnalisées.
 *
 * \param fn Fonction à exécuter dans le thread.
 * \param name Nom du thread (UTF-8, utilisé pour le débogage).
 * \param data Données utilisateur passées à la fonction.
 * \param options Options de création du thread (peut être NULL pour les valeurs par défaut).
 * \return Pointeur vers RC2D_Thread en cas de succès, NULL sinon.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
RC2D_Thread *rc2d_thread_newWithOptions(RC2D_ThreadFunction fn, const char *name, void *data, const RC2D_ThreadOptions *options);

/**
 * \brief Attend la fin d'un thread et récupère son code de retour.
 *
 * \param thread Pointeur vers le thread à attendre.
 * \param status Pointeur pour stocker le code de retour du thread (peut être NULL).
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread, mais un seul thread peut attendre un thread donné.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_thread_wait(RC2D_Thread *thread, int *status);

/**
 * \brief Détache un thread pour qu'il se termine automatiquement.
 *
 * \param thread Pointeur vers le thread à détacher.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_thread_detach(RC2D_Thread *thread);

/**
 * \brief Récupère l'état actuel d'un thread.
 *
 * \param thread Pointeur vers le thread à interroger.
 * \return L'état du thread (RC2D_ThreadState).
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
RC2D_ThreadState rc2d_thread_getState(RC2D_Thread *thread);

/**
 * \brief Définit la priorité du thread actuel.
 *
 * \param priority Priorité à définir.
 * \return true en cas de succès, false sinon.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
bool rc2d_thread_setPriority(RC2D_ThreadPriority priority);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_THREAD_H