#include <RC2D/RC2D_thread.h>
#include <RC2D/RC2D_logger.h>

/**
 * Crée un nouveau thread en utilisant les informations fournies dans la structure RC2D_Thread.
 * 
 * @param thread Un pointeur vers une structure RC2D_Thread contenant toutes les informations nécessaires pour créer le thread.
 * @return Retourne true si le thread a été créé avec succès, false en cas d'échec.
 */
bool rc2d_thread_createThread(RC2D_Thread *thread) 
{
    if (thread == NULL || thread->threadFunction == NULL || thread->name == NULL) 
    {
        // Échec si les données du thread sont invalides
        RC2D_log(RC2D_LOG_ERROR, "Les données du thread sont invalides.\n");
        return false;
    }

    thread->thread = SDL_CreateThread(thread->threadFunction, thread->name, thread->data);
    if (thread->thread == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "SDL_CreateThread a échoué: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

/**
 * Détache le thread spécifié, permettant au thread de s'exécuter librement sans nécessiter que d'autres threads attendent sa fin.
 * Après l'appel de cette fonction, il n'est plus possible de récupérer le code de sortie du thread.
 * 
 * @param thread Un pointeur vers une structure RC2D_Thread représentant le thread à détacher.
 * @return Retourne true si le thread a été détaché avec succès, false en cas d'échec.
 */
bool rc2d_thread_createThreadDetach(RC2D_Thread *thread) 
{
    if (thread == NULL || thread->thread == NULL) 
    {
        // Échec si les données du thread sont invalides
        RC2D_log(RC2D_LOG_ERROR, "Tentative de détacher un thread non initialisé ou déjà terminé.\n");
        return false;
    }

    SDL_DetachThread(thread->thread);

    /*
    Il est important de noter que, après avoir appelé SDL_DetachThread, vous ne devez plus accéder au pointeur SDL_Thread 
    Puisque le thread peut être terminé à tout moment et que la mémoire associée pourrait être libérée. 
    C'est pourquoi thread->thread est mis à NULL immédiatement après l'appel à SDL_DetachThread 
    */
    thread->thread = NULL;

    return true;
}

/**
 * Attend que le thread spécifié termine son exécution. Cette fonction bloque le thread appelant jusqu'à ce que le thread cible ait terminé.
 * Le code de sortie du thread terminé est retourné via le paramètre 'status'.
 * 
 * @param thread Un pointeur vers une structure RC2D_Thread représentant le thread dont l'exécution doit être attendue.
 * @param status Un pointeur vers un entier où le code de sortie du thread sera stocké.
 * @return Retourne true si l'attente s'est terminée avec succès et que le code de sortie a été récupéré, false en cas d'échec.
 */
bool rc2d_thread_waitCompletion(RC2D_Thread *thread, int *status)
{
    if (thread == NULL || thread->thread == NULL || status == NULL) 
    {
        // Échec si les données du thread sont invalides
        RC2D_log(RC2D_LOG_ERROR, "Tentative d'attendre un thread non initialisé ou déjà terminé dans rc2d_thread_waitCompletion.\n");
        return false;
    }

    // Attendre la fin du thread
    SDL_WaitThread(thread->thread, status);

    // Libération de la mémoire
    thread->thread = NULL;
    thread->name = NULL;
    thread->threadFunction = NULL;
    thread->data = NULL;
    thread = NULL;
    
    return true;
}