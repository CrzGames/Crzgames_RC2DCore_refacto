#ifndef RC2D_THREAD_H
#define RC2D_THREAD_H

#include <SDL3/SDL_thread.h> // Required for : SDL_Thread
#include <stdbool.h> // Required for : bool

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure représentant un thread.
 * @typedef {Object} RC2D_Thread
 * @property {SDL_Thread} thread - Le pointeur vers le thread SDL.
 * @property {string} name - Le nom du thread.
 * @property {function} threadFunction - La fonction que le thread va exécuter.
 * @property {*} data - Les données passées à la fonction du thread.
 */
typedef struct RC2D_Thread {
    SDL_Thread* thread;
    const char* name;
    int (*threadFunction)(void*);
    void* data;
} RC2D_Thread;

bool rc2d_thread_createThread(RC2D_Thread *thread);
bool rc2d_thread_createThreadDetach(RC2D_Thread *thread);
bool rc2d_thread_waitCompletion(RC2D_Thread *thread, int *status);

#ifdef __cplusplus
};
#endif

#endif // RC2D_THREAD_H