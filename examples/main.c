#include <RC2D/RC2D.h>

/* Global rectangle and color definitions */
static RC2D_Rect filled_rect;
static RC2D_Rect outlined_rect;
static RC2D_Color red_color;
static RC2D_Color blue_color;

void rc2d_unload(void)
{
    RC2D_log(RC2D_LOG_INFO, "My game is unloading...\n");
}

void rc2d_load(void)
{
    RC2D_log(RC2D_LOG_INFO, "My game is loading...\n");

    /* Initialize rectangles and colors */
    filled_rect = (RC2D_Rect){ 100.0f, 100.0f, 200.0f, 100.0f };
    outlined_rect = (RC2D_Rect){ 150.0f, 150.0f, 200.0f, 100.0f };
    red_color = (RC2D_Color){ 255, 0, 0, 255 };
    blue_color = (RC2D_Color){ 0, 0, 255, 255 };
}

void rc2d_update(double dt)
{

}

void rc2d_draw(void)
{
    /* Draw a filled red rectangle */
    rc2d_graphics_rectangle("fill", &filled_rect, red_color);

    /* Draw an outlined blue rectangle */
    rc2d_graphics_rectangle("line", &outlined_rect, blue_color);

    /* Draw a point at (200, 200) with yellow color */
    rc2d_graphics_point((RC2D_Vertex){ 200.0f, 200.0f }, (RC2D_Color){ 255, 255, 0, 255 });
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
    config->callbacks->rc2d_update = rc2d_update;
    config->callbacks->rc2d_load = rc2d_load;
    config->callbacks->rc2d_unload = rc2d_unload;

    RC2D_assert_release(config != NULL, RC2D_LOG_CRITICAL, "RC2D_EngineConfig config is NULL. Cannot setup the engine.");

    return config;
}