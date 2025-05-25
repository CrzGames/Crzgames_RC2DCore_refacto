/**
 * IMPORTANT:
 * - Doit être inclus avant tout autre fichier d'en-tête SDL / RC2D.
 * 
 * - SDL_MAIN_USE_CALLBACKS et <SDL3/SDL_main.h> doivent être inclus UNE SEULE FOIS
 * dans un fichier source (.c/.cpp).
 * 
 * - A partir du moment ou SDL_MAIN_USE_CALLBACKS est défini et <SDL3/SDL_main.h> est inclus,
 * la fonction main() ne doit pas être définie dans le code de l'application.
 */
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>

#include <RC2D/RC2D_engine.h>
#include <RC2D/RC2D_internal.h>
#include <RC2D/RC2D_logger.h>
#include <RC2D/RC2D_graphics.h>
#include <RC2D/RC2D_memory.h>

/**
 * SDL3 Callback: Initialisation
 * 
 * Cette fonction est appelée une seule fois au démarrage de l'application.
 */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) 
{
    /**
     * La définition de la fonction rc2d_setup doit être définie par l'utilisateur.
     * On passe les arguments de la ligne de commande à la fonction rc2d_setup.
     */
    const RC2D_EngineConfig* config = rc2d_engine_setup(argc, argv);

    /**
     * Si la configuration est NULL (const RC2D_EngineConfig* config), 
     * alors on utilisera la configuration par défaut de RC2D.
     */
    rc2d_engine_configure(config);

    /**
     * Initialise le moteur RC2D.
     * 
     * Si l'initialisation échoue, on retourne SDL_APP_FAILURE.
     */
	if(!rc2d_engine_init())
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
    if (rc2d_engine_state.config != NULL && 
        rc2d_engine_state.config->callbacks != NULL && 
        rc2d_engine_state.config->callbacks->rc2d_load != NULL) 
    {
        rc2d_engine_state.config->callbacks->rc2d_load();
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "No rc2d_load callback provided. Skipping load step.\n");
    }

    /**
     * 1. Affiche enfin la fenêtre après tout l'init (GPU, logique, textures…)
     * 
     * 2. Demandez que la fenêtre soit surélevée au-dessus des autres fenêtres 
     *    et obtenez le focus d'entrée.
     */
    SDL_ShowWindow(rc2d_engine_state.window);
    SDL_RaiseWindow(rc2d_engine_state.window);

    /**
     * Pour rc2d_last_frame_time, nous ne voulons pas que le deltatime de la 
     * premiere image inclue le temps pris par rc2d_load (donc ont le fait ici)
     */
    rc2d_engine_state.last_frame_time = SDL_GetPerformanceCounter();

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
     * Si le jeu n'est plus en cours d'exécution, on sort de la boucle principale.
     * 
     * Si elle renvoie SDL_APP_SUCCESS, l'application appelle SDL_AppQuit et 
     * termine avec un code de sortie signalant la réussite à la plateforme.
     */
    if (!rc2d_engine_state.game_is_running) 
    {
        return SDL_APP_SUCCESS;
    }

    /**
     * Ordre de la boucle principale de l'application :
     * 
     * 1. Calculer le delta time pour la frame actuelle.
     * 2. Appele les fonction interne de hot reload des shaders / pipeline graphics (si RC2D_GPU_SHADER_HOT_RELOAD_ENABLED est défini à 1).
     * 3. Appeler la fonction de mise à jour du jeu.
     * 4. Effacer l'écran.
     * 5. Appeler la fonction de dessin du jeu.
     * 6. Présenter le rendu à l'écran.
     * 7. Terminer le calcul du delta time pour la frame actuelle.
     */
    rc2d_engine_deltatime_start();
#if RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
    rc2d_gpu_hotReloadGraphicsShadersAndGraphicsPipeline();
    rc2d_gpu_hotReloadComputeShader();
#endif
    if (rc2d_engine_state.config != NULL && 
        rc2d_engine_state.config->callbacks != NULL && 
        rc2d_engine_state.config->callbacks->rc2d_update != NULL) 
    {
        rc2d_engine_state.config->callbacks->rc2d_update(rc2d_engine_state.delta_time);
    }
    rc2d_graphics_clear();
    if (!rc2d_engine_state.skip_rendering &&
        rc2d_engine_state.config != NULL && 
        rc2d_engine_state.config->callbacks != NULL && 
        rc2d_engine_state.config->callbacks->rc2d_draw != NULL) 
    {
        rc2d_engine_state.config->callbacks->rc2d_draw();
    }
    rc2d_graphics_present();
    rc2d_engine_deltatime_end();

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
    return rc2d_engine_processevent(event);
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
    if (rc2d_engine_state.config != NULL && 
        rc2d_engine_state.config->callbacks != NULL && 
        rc2d_engine_state.config->callbacks->rc2d_unload != NULL)
    {
        rc2d_engine_state.config->callbacks->rc2d_unload();
    }

    /**
     * Affiche un rapport des fuites mémoire détectées.
     * Cela est utile pour identifier les fuites de mémoire dans l'application.
     * Note : Ce rapport est affiché uniquement si RC2D_MEMORY_DEBUG_ENABLED est défini à 1. 
     */
    RC2D_memory_report();

    /**
     * Si le résultat est SDL_APP_FAILURE, cela signifie que l'application a échoué 
     * et doit être terminée avec un code d'erreur.
     */
    if (result == SDL_APP_FAILURE) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Application failed: %s", SDL_GetError());
    }

    /**
     * Libére les ressources notamment celles des librairies externes de RC2D 
     * et des modules interne à RC2D.
     */
    rc2d_engine_quit();
}