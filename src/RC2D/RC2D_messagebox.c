#include <RC2D/RC2D_messagebox.h>
#include <RC2D/RC2D_logger.h>
#include <RC2D/RC2D_memory.h>

#include <SDL3/SDL_messagebox.h>

// Convertit RC2D_MessageBoxType en SDL_MessageBoxFlags
static SDL_MessageBoxFlags rc2d_messagebox_convert_type(RC2D_MessageBoxType type) 
{
    switch (type) 
    {
        case RC2D_MESSAGEBOX_ERROR: return SDL_MESSAGEBOX_ERROR;
        case RC2D_MESSAGEBOX_WARNING: return SDL_MESSAGEBOX_WARNING;
        case RC2D_MESSAGEBOX_INFORMATION: return SDL_MESSAGEBOX_INFORMATION;
        default: return SDL_MESSAGEBOX_INFORMATION;
    }
}

bool rc2d_messagebox_showSimple(RC2D_MessageBoxType type, const char *title, const char *message, SDL_Window *window) 
{
    if (!message) 
    {
        RC2D_log(RC2D_LOG_ERROR, "message NULL");
        return false;
    }

    // Appelle SDL_ShowSimpleMessageBox avec les paramètres convertis
    if (!SDL_ShowSimpleMessageBox(rc2d_messagebox_convert_type(type), title, message, window)) 
    {
        RC2D_log(RC2D_LOG_ERROR, "echec de SDL_ShowSimpleMessageBox : %s", SDL_GetError());
        return false;
    }

    return true;
}

bool rc2d_messagebox_show(const RC2D_MessageBoxOptions *options, int *button_id) {
    if (!options || !options->message) 
    {
        RC2D_log(RC2D_LOG_ERROR, "options ou message NULL");
        return false;
    }

    // Prépare les données SDL
    SDL_MessageBoxData sdl_data = {0};
    sdl_data.flags = rc2d_messagebox_convert_type(options->type);
    sdl_data.window = options->window;
    sdl_data.title = options->title ? options->title : "";
    sdl_data.message = options->message;

    // Configure les boutons
    SDL_MessageBoxButtonData *sdl_buttons = NULL;
    if (options->num_buttons > 0 && options->buttons) 
    {
        // Alloue de la mémoire pour les boutons
        sdl_buttons = RC2D_malloc(options->num_buttons * sizeof(SDL_MessageBoxButtonData));
        if (!sdl_buttons)
         {
            RC2D_log(RC2D_LOG_ERROR, "echec d'allocation des boutons");
            return false;
        }

        // Remplit les données des boutons
        for (int i = 0; i < options->num_buttons; i++) 
        {
            sdl_buttons[i].flags = 0;
            if (options->buttons[i].return_key_default) 
            {
                sdl_buttons[i].flags |= SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
            }
            if (options->buttons[i].escape_key_default) 
            {
                sdl_buttons[i].flags |= SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
            }
            sdl_buttons[i].buttonID = options->buttons[i].button_id;
            sdl_buttons[i].text = options->buttons[i].text ? options->buttons[i].text : "";
        }
        sdl_data.numbuttons = options->num_buttons;
        sdl_data.buttons = sdl_buttons;
    } 
    else 
    {
        // Bouton OK par défaut
        static const SDL_MessageBoxButtonData default_button = {
            .flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT,
            .buttonID = 0,
            .text = "OK"
        };
        sdl_data.numbuttons = 1;
        sdl_data.buttons = &default_button;
    }

    // Configure l'ordre des boutons
    if (options->buttons_left_to_right) 
    {
        sdl_data.flags |= SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT;
    } 
    else 
    {
        sdl_data.flags |= SDL_MESSAGEBOX_BUTTONS_RIGHT_TO_LEFT;
    }

    // Configure le schéma de couleurs
    SDL_MessageBoxColorScheme sdl_color_scheme = {0};
    if (options->color_scheme) 
    {
        for (int i = 0; i < SDL_MESSAGEBOX_COLOR_COUNT; i++) 
        {
            sdl_color_scheme.colors[i].r = options->color_scheme[i][0];
            sdl_color_scheme.colors[i].g = options->color_scheme[i][1];
            sdl_color_scheme.colors[i].b = options->color_scheme[i][2];
        }

        sdl_data.colorScheme = &sdl_color_scheme;
    }

    // Affiche la boîte de dialogue
    int result_button_id = 0;
    if (!SDL_ShowMessageBox(&sdl_data, &result_button_id)) 
    {
        RC2D_log(RC2D_LOG_ERROR, "echec de SDL_ShowMessageBox : %s", SDL_GetError());
        RC2D_safe_free(sdl_buttons);
        return false;
    }

    // Stocke l'ID du bouton sélectionné
    if (button_id) 
    {
        *button_id = result_button_id;
    }

    // Nettoie les boutons alloués
    RC2D_safe_free(sdl_buttons);
    return true;
}