#include "rc2d/rc2d_keyboard.h"
#include "rc2d/rc2d_logger.h"

/**
 * Vérifie si une touche spécifique est actuellement pressée.
 *
 * @param key La touche à vérifier, spécifiée en tant que RC2D_KeyCode.
 * @return true si la touche est pressée, false sinon.
 */
bool rc2d_keyboard_isDown(const RC2D_KeyCode key) 
{
    RC2D_Scancode scancode = rc2d_keyboard_getScancodeFromKey(key);
    
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state == NULL)
    {
        rc2d_log(RC2D_LOG_WARN, "Erreur : Impossible de récupérer l'état du clavier dans rc2d_keyboard_isDown.\n");
        return false;
    }

    return state[scancode];
}

/**
 * Vérifie si un scancode spécifique est actuellement pressé.
 *
 * @param scancode Le scancode à vérifier, spécifié en tant que RC2D_Scancode.
 * @return true si le scancode est pressé, false sinon.
 */
bool rc2d_keyboard_isScancodeDown(const RC2D_Scancode scancode) 
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state == NULL)
    {
        rc2d_log(RC2D_LOG_WARN, "Erreur : Impossible de récupérer l'état du clavier dans rc2d_keyboard_isScancodeDown.\n");
        return false;
    }

    return state[scancode];
}

/**
 * Active ou désactive la saisie de texte.
 *
 * @param enabled Si true, la saisie de texte est activée. Si false, elle est désactivée.
 */
void rc2d_keyboard_setTextInput(const bool enabled)
{
    if (enabled) 
	{
        SDL_StartTextInput();
    } 
	else 
	{
        SDL_StopTextInput();
    }
}

/**
 * Vérifie si un clavier à l'écran est disponible.
 *
 * @return true si un clavier à l'écran est supporté, false sinon.
 */
bool rc2d_keyboard_hasScreenKeyboard(void) 
{
    return SDL_HasScreenKeyboardSupport();
}

/**
 * Convertit une touche spécifiée par un RC2D_KeyCode en son scancode correspondant.
 *
 * @param key La touche à convertir, spécifiée en tant que RC2D_KeyCode.
 * @return Le scancode correspondant à la touche spécifiée.
 */
RC2D_Scancode rc2d_keyboard_getScancodeFromKey(const RC2D_KeyCode key) 
{
    // Convertit la touche RC2D_KeyCode en SDL_KeyCode
    SDL_KeyCode sdl_key = (SDL_KeyCode)key;

    // Convertit la touche SDL_KeyCode en SDL_Scancode
    SDL_Scancode sdl_scancode = SDL_GetScancodeFromKey(sdl_key);

    // Convertit le SDL_Scancode en RC2D_Scancode
    RC2D_Scancode scancode = (RC2D_Scancode)sdl_scancode;

    if (scancode == RC2D_SCANCODE_UNKNOWN) 
	{
        rc2d_log(RC2D_LOG_WARN, "Erreur : Impossible de trouver le scancode pour la touche spécifiée.\n");
    }

    return scancode;
}

/**
 * Convertit un scancode spécifié par un RC2D_Scancode en sa touche correspondante.
 *
 * @param scancode Le scancode à convertir, spécifié en tant que RC2D_Scancode.
 * @return La touche correspondant au scancode spécifié.
 */
RC2D_KeyCode rc2d_keyboard_getKeyFromScancode(const RC2D_Scancode scancode) 
{
    // Convertit le RC2D_Scancode en SDL_Scancode
    SDL_Scancode sdl_scancode = (SDL_Scancode)scancode;

    // Convertit le SDL_Scancode en SDL_KeyCode
    SDL_KeyCode sdl_key = SDL_GetKeyFromScancode(sdl_scancode);

    // Convertit le SDL_KeyCode en RC2D_KeyCode
    RC2D_KeyCode key = (RC2D_KeyCode)sdl_key;

    return key;
}