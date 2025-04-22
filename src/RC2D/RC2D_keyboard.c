#include <RC2D/RC2D_keyboard.h>
#include <RC2D/RC2D_logger.h>
#include <RC2D/RC2D_internal.h>

bool rc2d_keyboard_isDown(const RC2D_Keycode key) 
{
    SDL_Keymod mods = SDL_GetModState();
    SDL_Scancode sdl_scancode = SDL_GetScancodeFromKey((SDL_Keycode)key, &mods);

    if (sdl_scancode == SDL_SCANCODE_UNKNOWN) 
    {
        RC2D_log(RC2D_LOG_WARN, "Erreur : SDL_GetScancodeFromKey a échoué pour la touche spécifiée.\n");
        return false;
    }

    const bool *state = SDL_GetKeyboardState(NULL);
    if (state == NULL)
    {
        RC2D_log(RC2D_LOG_WARN, "Erreur : Impossible de récupérer l'état du clavier dans rc2d_keyboard_isDown.\n");
        return false;
    }

    return state[sdl_scancode];
}

bool rc2d_keyboard_isScancodeDown(const RC2D_Scancode scancode) 
{
    
    const bool *state = SDL_GetKeyboardState(NULL);
    if (state == NULL)
    {
        RC2D_log(RC2D_LOG_WARN, "Erreur : Impossible de récupérer l'état du clavier dans rc2d_keyboard_isScancodeDown.\n");
        return false;
    }

    return state[scancode];
}

void rc2d_keyboard_setTextInput(const bool enabled)
{
    if (enabled) 
	{
        if (!SDL_StartTextInput(rc2d_engine_state.window))
        {
            RC2D_log(RC2D_LOG_WARN, "Impossible de démarrer la saisie de texte : %s\n", SDL_GetError());
        }
    } 
	else 
	{
        if (!SDL_StopTextInput(rc2d_engine_state.window))
        {
            RC2D_log(RC2D_LOG_WARN, "Impossible d'arrêter la saisie de texte : %s\n", SDL_GetError());
        }
    }
}

bool rc2d_keyboard_hasScreenKeyboardSupport(void) 
{
    return SDL_HasScreenKeyboardSupport();
}

RC2D_Scancode rc2d_keyboard_getScancodeFromKey(const RC2D_Keycode key) 
{
    // Récupérer l'état des modificateurs de touches (Shift, Ctrl, Alt, etc.)
    SDL_Keymod mods = SDL_GetModState();

    // Convertir le RC2D_Keycode en scancode SDL
    SDL_Scancode sdl_scancode = SDL_GetScancodeFromKey((SDL_Keycode)key, &mods);

    // Vérifier si la conversion a échoué
    if (sdl_scancode == SDL_SCANCODE_UNKNOWN) 
    {
        RC2D_log(RC2D_LOG_WARN, "Erreur : Impossible de trouver le scancode pour la touche spécifiée.\n");
    }

    // Retourner le scancode correspondant
    return (RC2D_Scancode)sdl_scancode;
}

RC2D_Keycode rc2d_keyboard_getKeyFromScancode(const RC2D_Scancode scancode) 
{
    // Récupérer l'état des modificateurs de touches (Shift, Ctrl, Alt, etc.)
    SDL_Keymod mods = SDL_GetModState();

    // Convertir le RC2D_Scancode en keycode SDL
    SDL_Keycode sdl_key = SDL_GetKeyFromScancode((SDL_Scancode)scancode, mods, true);

    // Vérifier si la conversion a échoué
    if (sdl_key == SDLK_UNKNOWN)
    {
        RC2D_log(RC2D_LOG_WARN, "Erreur : Impossible de trouver le keycode pour le scancode spécifié.\n");
    }

    // Retourner le keycode correspondant
    return (RC2D_Keycode)sdl_key;
}