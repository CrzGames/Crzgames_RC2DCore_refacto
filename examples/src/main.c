/**
 * Les macro de préprocesseur doivent être définies avant d'inclure le fichier RC2D.h
 * pour que les options de configuration soient prises en compte.
 */
#ifdef NDEBUG // Release mode
#define RC2D_ASSERT_LEVEL 1
#define RC2D_GPU_SHADER_HOT_RELOAD_ENABLED 0
#else // Debug mode
#define RC2D_ASSERT_LEVEL 3
#define RC2D_GPU_SHADER_HOT_RELOAD_ENABLED 1
#endif

#include <mygame/game.h>
#include <RC2D/RC2D.h>

const RC2D_EngineConfig* rc2d_engine_setup(int argc, char* argv[])
{
#ifdef NDEBUG // Release mode
    rc2d_logger_set_priority(RC2D_LOG_ERROR);
#else // Debug mode
    rc2d_logger_set_priority(RC2D_LOG_TRACE);
#endif

    RC2D_EngineConfig* config = rc2d_engine_getDefaultConfig();
    config->gpuOptions->debugMode = true;
    config->gpuOptions->verbose = true;
    config->gpuOptions->preferLowPower = false;
    config->gpuOptions->driver = RC2D_GPU_DRIVER_DEFAULT;
    config->callbacks->rc2d_draw = rc2d_draw;
    config->callbacks->rc2d_update = rc2d_update;
    config->callbacks->rc2d_load = rc2d_load;
    config->callbacks->rc2d_unload = rc2d_unload;

    RC2D_assert_release(config != NULL, RC2D_LOG_CRITICAL, "RC2D_EngineConfig config is NULL. Cannot setup the engine.");

    return config;
}