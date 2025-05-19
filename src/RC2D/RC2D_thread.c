#include <RC2D/RC2D_thread.h>
#include <RC2D/RC2D_logger.h>

#include <SDL3/SDL_thread.h>
#include <SDL3/SDL_properties.h>

// Convertit RC2D_ThreadPriority en SDL_ThreadPriority
static SDL_ThreadPriority rc2d_thread_convert_priority(RC2D_ThreadPriority priority) 
{
    switch (priority) 
    {
        case RC2D_THREAD_PRIORITY_LOW:
            return SDL_THREAD_PRIORITY_LOW;
        case RC2D_THREAD_PRIORITY_NORMAL:
            return SDL_THREAD_PRIORITY_NORMAL;
        case RC2D_THREAD_PRIORITY_HIGH:
            return SDL_THREAD_PRIORITY_HIGH;
        case RC2D_THREAD_PRIORITY_TIME_CRITICAL:
            return SDL_THREAD_PRIORITY_TIME_CRITICAL;
        default:
            return SDL_THREAD_PRIORITY_NORMAL;
    }
}

// Convertit SDL_ThreadState en RC2D_ThreadState
static RC2D_ThreadState rc2d_thread_convert_state(SDL_ThreadState state) 
{
    switch (state) 
    {
        case SDL_THREAD_UNKNOWN:
            return RC2D_THREAD_STATE_UNKNOWN;
        case SDL_THREAD_ALIVE:
            return RC2D_THREAD_STATE_ALIVE;
        case SDL_THREAD_DETACHED:
            return RC2D_THREAD_STATE_DETACHED;
        case SDL_THREAD_COMPLETE:
            return RC2D_THREAD_STATE_COMPLETE;
        default:
            return RC2D_THREAD_STATE_UNKNOWN;
    }
}

RC2D_Thread *rc2d_thread_new(RC2D_ThreadFunction fn, const char *name, void *data) 
{
    /**
     * Crée un thread avec des options par défaut
     * Taille de pile par défaut, priorité normale, non détaché
     */
    RC2D_ThreadOptions options = {0};
    return rc2d_thread_new_with_options(fn, name, data, &options);
}

RC2D_Thread *rc2d_thread_newWithOptions(RC2D_ThreadFunction fn, const char *name, void *data, const RC2D_ThreadOptions *options) 
{
    if (!fn) 
    {
        RC2D_log(RC2D_LOG_ERROR, "la fonction fn est NULL");
        return NULL;
    }

    // Crée un ensemble de propriétés pour le thread
    SDL_PropertiesID props = SDL_CreateProperties();
    if (!props) 
    {
        RC2D_log(RC2D_LOG_ERROR, "echec de SDL_CreateProperties : %s", SDL_GetError());
        return NULL;
    }

    // Configure les propriétés du thread
    SDL_SetPointerProperty(props, SDL_PROP_THREAD_CREATE_ENTRY_FUNCTION_POINTER, fn);
    SDL_SetStringProperty(props, SDL_PROP_THREAD_CREATE_NAME_STRING, name ? name : "");
    SDL_SetPointerProperty(props, SDL_PROP_THREAD_CREATE_USERDATA_POINTER, data);
    if (options && options->stack_size > 0) 
    {
        SDL_SetNumberProperty(props, SDL_PROP_THREAD_CREATE_STACKSIZE_NUMBER, options->stack_size);
    }

    // Crée le thread
    RC2D_Thread *thread = SDL_CreateThreadWithProperties(props);
    SDL_DestroyProperties(props);
    if (!thread) 
    {
        RC2D_log(RC2D_LOG_ERROR, "echec de SDL_CreateThreadWithProperties : %s", SDL_GetError());
        return NULL;
    }

    // Applique la priorité si spécifiée
    if (options && options->priority != RC2D_THREAD_PRIORITY_NORMAL) 
    {
        SDL_ThreadID thread_id = SDL_GetThreadID(thread);
        if (thread_id && !SDL_SetCurrentThreadPriority(rc2d_thread_convert_priority(options->priority))) 
        {
            RC2D_LOG_WARNING("echec de la définition de la priorité pour le thread %s : %s", name ? name : "sans nom", SDL_GetError());
        }
    }

    // Détache le thread automatiquement si requis
    if (options && options->auto_detach) 
    {
        SDL_DetachThread(thread);
    }

    return thread;
}

void rc2d_thread_wait(RC2D_Thread *thread, int *status) 
{
    if (!thread) 
    {
        RC2D_LOG_WARNING("le thread est NULL");
        if (status) *status = -1;
        return;
    }

    // Attend la fin du thread et récupère le statut
    SDL_WaitThread(thread, status);
}

void rc2d_thread_detach(RC2D_Thread *thread) 
{
    if (!thread) 
    {
        RC2D_LOG_WARNING("le thread est NULL");
        return;
    }

    // Détache le thread pour qu'il se termine automatiquement
    SDL_DetachThread(thread);
}

RC2D_ThreadState rc2d_thread_getState(RC2D_Thread *thread) 
{
    if (!thread) 
    {
        RC2D_LOG_WARNING("le thread est NULL");
        return RC2D_THREAD_STATE_UNKNOWN;
    }

    // Récupère et convertit l'état du thread
    return rc2d_thread_convert_state(SDL_GetThreadState(thread));
}

bool rc2d_thread_setPriority(RC2D_ThreadPriority priority) 
{
    // Définit la priorité du thread actuel
    if (!SDL_SetCurrentThreadPriority(rc2d_thread_convert_priority(priority))) 
    {
        RC2D_log(RC2D_LOG_ERROR, "echec de la définition de la priorité : %s", SDL_GetError());
        return false;
    }

    return true;
}