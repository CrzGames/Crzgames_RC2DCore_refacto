#include <mygame/game.h>
#include <RC2D/RC2D.h>

void rc2d_unload(void)
{
    RC2D_log(RC2D_LOG_INFO, "My game is unloading...\n");
}

void rc2d_load(void)
{
    RC2D_log(RC2D_LOG_INFO, "My game is loading...\n");
}

void rc2d_update(double dt)
{
    RC2D_log(RC2D_LOG_INFO, "My game is updating... Delta time: %f\n", dt);
    // Update game logic here
}

void rc2d_draw(void)
{
    // Draw game graphics here
}