#include <RC2D/RC2D.h>

void rc2d_unload(void)
{
    RC2D_log(RC2D_LOG_INFO, "RC2D Engine is unloading...\n");
}

void rc2d_load(void)
{
    RC2D_log(RC2D_LOG_INFO, "RC2D Engine is loading...\n");
}

void rc2_update(double dt)
{
    RC2D_log(RC2D_LOG_INFO, "RC2D Engine is updating...\n");
}

void rc2d_draw(void)
{
    RC2D_log(RC2D_LOG_INFO, "RC2D Engine is drawing...\n");
}
    
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
    config->callbacks->rc2d_update = rc2_update;
    config->callbacks->rc2d_load = rc2d_load;
    config->callbacks->rc2d_unload = rc2d_unload;

    RC2D_assert_release(config != NULL, RC2D_LOG_CRITICAL, "RC2D_EngineConfig config is NULL. Cannot setup the engine.");

    return config;
}