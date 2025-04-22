#include <RC2D/RC2D.h>

const RC2D_EngineConfig* rc2d_engine_setup(void)
{
    // Configuration par défaut de l'application
    static RC2D_EngineConfig config = rc2d_engine_getDefaultConfig();


    return &config;
}