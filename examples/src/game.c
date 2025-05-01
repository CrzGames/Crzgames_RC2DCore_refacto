#include <mygame/game.h>
#include <RC2D/RC2D.h>

static SDL_GPUShader* fragmentShader;

void rc2d_unload(void) 
{
    RC2D_log(RC2D_LOG_INFO, "My game is unloading...\n");
}

void rc2d_load(void) 
{
    RC2D_log(RC2D_LOG_INFO, "My game is loading...\n");
    fragmentShader = rc2d_gpu_loadShader("test.fragment");
    RC2D_assert_release(fragmentShader != NULL, RC2D_LOG_CRITICAL, "Failed to load fragment shader");
}

void rc2d_update(double dt) 
{
    RC2D_log(RC2D_LOG_INFO, "My game is updating... Delta time: %f\n", dt);
}

void rc2d_draw(void) {
    // Utiliser fragmentShader pour le rendu
    // Le shader est automatiquement mis à jour par le framework si modifié
}