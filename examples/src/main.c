#include <mygame/game.h>
#include <RC2D/RC2D.h>

/**
 * \brief Fonction de configuration du moteur RC2D.
 * 
 * Cette fonction est appelée au démarrage de l'application pour configurer le moteur RC2D.
 * Elle initialise les paramètres par défaut et peut être modifiée par l'utilisateur pour personnaliser le
 * comportement du moteur.
 * 
 * \param {int} argc - Nombre d'arguments de la ligne de commande.
 * \param {char**} argv - Tableau des arguments de la ligne de commande.
 * \return {const RC2D_EngineConfig*} Pointeur vers la configuration du moteur RC2D.
 * 
 * \warning L'implementation de cette fonction doit être définie par l'utilisateur, puisqu'elle est appelée par le moteur RC2D.
 */
const RC2D_EngineConfig* rc2d_engine_setup(int argc, char* argv[])
{
    RC2D_EngineConfig* config = rc2d_engine_getDefaultConfig();
#ifdef NDEBUG // Release mode
    rc2d_logger_set_priority(RC2D_LOG_CRITICAL);
    config->gpuOptions->debugMode = false;
    config->gpuOptions->verbose = false;
#else // Debug mode
    rc2d_logger_set_priority(RC2D_LOG_TRACE);
    config->gpuOptions->debugMode = true;
    config->gpuOptions->verbose = true;
#endif
    config->gpuOptions->preferLowPower = false;
    config->gpuOptions->driver = RC2D_GPU_DRIVER_DEFAULT;
    config->windowWidth = 800;
    config->windowHeight = 600;
    config->logicalWidth = 1920;
    config->logicalHeight = 1080;
    config->callbacks->rc2d_draw = rc2d_draw;
    config->callbacks->rc2d_update = rc2d_update;
    config->callbacks->rc2d_load = rc2d_load;
    config->callbacks->rc2d_unload = rc2d_unload;
    config->logicalPresentationMode = RC2D_LOGICAL_PRESENTATION_LETTERBOX;
    config->pixelartMode = false;
    
    RC2D_assert_release(config != NULL, RC2D_LOG_CRITICAL, "RC2D_EngineConfig config is NULL. Cannot setup the engine.");

    return config;
}