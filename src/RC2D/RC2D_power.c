#include <RC2D/RC2D_power.h>
#include <RC2D/RC2D_logger.h>

#include <SDL3/SDL_power.h>

RC2D_PowerInfo rc2d_system_getPowerInfo(void)
{
    // Initialiser la structure RC2D_PowerInfo avec des valeurs par défaut
    RC2D_PowerInfo powerInfo = {0};
    powerInfo.state = RC2D_POWERSTATE_ERROR;
    powerInfo.batteryLevel = -1;
    powerInfo.batteryTimeSeconds = -1;

    // Obtenir l'état de l'alimentation du système
	SDL_PowerState powerState = SDL_GetPowerInfo(&powerInfo.batteryLevel, &powerInfo.batteryTimeSeconds);
    if (powerState == SDL_POWERSTATE_ERROR)
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de déterminer l'état de l'alimentation : %s dans rc2d_system_getPowerInfo().", SDL_GetError());
        return powerInfo;
    }

    // Convertir l'état de l'alimentation SDL en état de l'alimentation RC2D
    switch (powerState)
    {
        case SDL_POWERSTATE_UNKNOWN:
            powerInfo.state = RC2D_POWERSTATE_UNKNOWN;
            break;
        case SDL_POWERSTATE_ON_BATTERY:
            powerInfo.state = RC2D_POWERSTATE_ON_BATTERY;
            break;
        case SDL_POWERSTATE_NO_BATTERY:
            powerInfo.state = RC2D_POWERSTATE_NO_BATTERY;
            break;
        case SDL_POWERSTATE_CHARGING:
            powerInfo.state = RC2D_POWERSTATE_CHARGING;
            break;
        case SDL_POWERSTATE_CHARGED:
            powerInfo.state = RC2D_POWERSTATE_CHARGED;
            break;
        default:
            powerInfo.state = RC2D_POWERSTATE_UNKNOWN;
            break;
    }

    // Retourner les informations sur l'alimentation
    return powerInfo;
}