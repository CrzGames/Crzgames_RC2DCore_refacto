#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include <RC2D/RC2D_engine.h>
#include <RC2D/RC2D_internal.h>
#include <RC2D/RC2D_logger.h>

/**
 * SDL3 Callback: Initialisation
 * 
 * Cette fonction est appelée une seule fois au démarrage de l'application.
 */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) 
{
    // Met à NULL le pointeur d'état de l'application
    *appstate = NULL;

    /**
     * Récupérer la configuration de l'application RC2D, dont le 
     * prototype est défini dans le fichier RC2D.h
     * 
     * La définition de la fonction rc2d_setup doit être définie par l'utilisateur.
     */
    const RC2D_Config* config = rc2d_setup();
    if (config == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to get configuration from rc2d_setup");

        /**
         * SDL_APP_FAILURE : Cela vas appeler SDL_AppQuit et terminer 
         * le processus avec un code de sortie signalant une erreur 
         * à la plateforme.
         */
        return SDL_APP_FAILURE;
    }

    // Set the config to the engine
    (!rc2d_configure(config))
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to configure rc2d engine");

        /**
         * SDL_APP_FAILURE : Cela vas appeler SDL_AppQuit et terminer 
         * le processus avec un code de sortie signalant une erreur 
         * à la plateforme.
         */
        return SDL_APP_FAILURE;
    }

    // Initliaze le framework RC2D
	if(!rc2d_init())
    {
        /**
         * SDL_APP_FAILURE : Cela vas appeler SDL_AppQuit et terminer 
         * le processus avec un code de sortie signalant une erreur 
         * à la plateforme.
         */
        return SDL_APP_FAILURE;
    }

    /**
     * La premiere callback de la boucle de jeu est rc2d_load, qui est appelée avant le début de la boucle de jeu.
     * Cela peut être utilisé pour initialiser des ressources, charger des données, etc.
     */
    if (rc2d_callbacks_engine.rc2d_load != NULL) 
    {
        rc2d_callbacks_engine.rc2d_load();
    }

    /**
     * 1. Affiche enfin la fenêtre après tout l'init (GPU, logique, textures…)
     * 
     * 2. Demandez que la fenêtre soit surélevée au-dessus des autres fenêtres 
     *    et obtenez le focus d'entrée.
     */
    SDL_ShowWindow(rc2d_window);
    SDL_RaiseWindow(rc2d_window);

    /**
     * Pour rc2d_last_frame_time, nous ne voulons pas que le deltatime de la 
     * premiere image inclue le temps pris par rc2d_load (donc ont le fait ici)
     */
    rc2d_last_frame_time = SDL_GetPerformanceCounter();

    /**
     * SDL_APP_CONTINUE : Cela vas appeler la fonction SDL_AppIterate 
     * à intervalle régulier, et la boucle principale de l'application commence.
     * 
     * SDL_AppInit est appelée une seule fois au démarrage de l'application.
     */
    return SDL_APP_CONTINUE;
}

/**
 * SDL3 Callback: Boucle principale de l'application
 * 
 * Cette fonction s'exécute une fois par image dans la boucle principale de l'application.
 */
SDL_AppResult SDL_AppIterate(void *appstate) 
{
    /**
     * Si le jeu n'est pas en cours d'exécution, on sort de la boucle principale.
     * 
     * Si elle renvoie SDL_APP_SUCCESS, l'application appelle SDL_AppQuit et 
     * termine avec un code de sortie signalant la réussite à la plateforme.
     */
    if (!rc2d_game_is_running) 
    {
        return SDL_APP_SUCCESS;
    }

    rc2d_deltatimeframerates_start();
    if (rc2d_callbacks_engine.rc2d_update != NULL) rc2d_callbacks_engine.rc2d_update(rc2d_delta_time);
    rc2d_graphics_clear();
    if (rc2d_callbacks_engine.rc2d_draw != NULL) rc2d_callbacks_engine.rc2d_draw();
    rc2d_graphics_present();
    rc2d_deltatimeframerates_end();

    /**
     * SDL_APP_CONTINUE : La boucle principale de l'application continue.
     */
    return SDL_APP_CONTINUE;
}

/**
 * SDL3 Callback: Gestion des événements
 * 
 * Cette fonction est appelée lorsqu'un nouvel événement (entrée de souris, clavier, etc.) se produit.
 */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) 
{
    rc2d_event = *event;
    return rc2d_processevent();
}

/**
 * SDL3 Callback: Nettoyage
 * 
 * Cette fonction est appelée lorsque l'application se termine.
 */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    /**
     * La boucle de jeu est terminée, appelez la fonction de déchargement si elle est définie
     * Cela peut être utilisé pour libérer des ressources ou effectuer d'autres tâches de nettoyage 
     * avant la fermeture du programme.
     */
    if (rc2d_callbacks_engine.rc2d_unload != NULL)
    {
        rc2d_callbacks_engine.rc2d_unload();
    }

    /**
     * Libére les ressources notamment celles des librairies externes de RC2D 
     * et des modules interne à RC2D.
     */
    rc2d_quit();

    /**
     * Si le résultat est SDL_APP_FAILURE, cela signifie que l'application a échoué 
     * et doit être terminée avec un code d'erreur.
     */
    if (result == SDL_APP_FAILURE) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Application failed: %s", SDL_GetError());
    }
}