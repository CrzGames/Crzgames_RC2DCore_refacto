#include <RC2D/RC2D_messagebox.h>
#include <RC2D/RC2D_logger.h>
#include <RC2D/RC2D_internal.h>

#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_stdinc.h> // Required for : SDL_malloc, SDL_free

bool rc2d_window_showMessageBox(const char *title, const char *message, const char *buttonlist[], uint32_t numButtons, uint32_t *pressedButton, RC2D_MessageBoxType type, bool attachToWindow) 
{
    // Vérifie si la fenêtre est valide (non NULL)
    if (title == NULL || message == NULL || buttonlist == NULL || pressedButton == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Les paramètre de rc2d_window_showMessageBox() ne peuvent pas être NULL.\n");
        return false;
    }

    // Convertit le type de boîte de message en type SDL correspondant
    SDL_MessageBoxFlags messageBoxType = SDL_MESSAGEBOX_INFORMATION;
    switch (type) 
    {
        case RC2D_MESSAGEBOX_ERROR:
            messageBoxType = SDL_MESSAGEBOX_ERROR;
            break;
        case RC2D_MESSAGEBOX_WARNING:
            messageBoxType = SDL_MESSAGEBOX_WARNING;
            break;
        case RC2D_MESSAGEBOX_INFORMATION:
            messageBoxType = SDL_MESSAGEBOX_INFORMATION;
            break;
    }

    // Alloue dynamiquement de l'espace pour le tableau de boutons
    SDL_MessageBoxButtonData *buttons = (SDL_MessageBoxButtonData*)malloc(numButtons * sizeof(SDL_MessageBoxButtonData));
    if (buttons == NULL) {
        // Gestion de l'échec de l'allocation de mémoire
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'allouer de la mémoire pour le tableau de boutons.\n");
        return false;
    }

    // Initialise les données des boutons
    for (uint32_t i = 0; i < numButtons; ++i) 
    {
        buttons[i].flags = 0;
        buttons[i].buttonid = i;
        buttons[i].text = buttonlist[i];
    }

    // Configure les données de la boîte de message SDL
    SDL_MessageBoxData messageBoxData;
    messageBoxData.flags = messageBoxType;
    messageBoxData.title = title;
    messageBoxData.message = message;
    messageBoxData.numbuttons = numButtons;
    messageBoxData.buttons = buttons;
    messageBoxData.window = attachToWindow ? rc2d_engine_state.window : NULL;

    // Affiche la boîte de message SDL et récupère l'index du bouton pressé
    int buttonId;
    if (SDL_ShowMessageBox(&messageBoxData, &buttonId) == 0) 
    {
        *pressedButton = buttonId;
    } 
    else 
    {
        *pressedButton = 0; // Aucun bouton pressé
    }

    // Libère la mémoire allouée pour le tableau de boutons
    SDL_free(buttons);

    return true;
}

bool rc2d_window_showSimpleMessageBox(const char *title, const char *message, RC2D_MessageBoxType type, bool attachToWindow) {
    if (title == NULL || message == NULL)
    {
        // Vérifie si les pointeurs sont valides (non NULL)
        RC2D_log(RC2D_LOG_ERROR, "Les paramètre de rc2d_window_showSimpleMessageBox() ne peuvent pas être NULL.\n");
        return false;
    }

    // Convertit le type de boîte de message en type SDL correspondant
    uint32_t sdlType = SDL_MESSAGEBOX_INFORMATION;
    switch (type) 
	{
        case RC2D_MESSAGEBOX_ERROR:
            sdlType = SDL_MESSAGEBOX_ERROR;
            break;
        case RC2D_MESSAGEBOX_WARNING:
            sdlType = SDL_MESSAGEBOX_WARNING;
            break;
        case RC2D_MESSAGEBOX_INFORMATION:
            sdlType = SDL_MESSAGEBOX_INFORMATION;
            break;
    }

    // Affiche la boîte de message simple SDL
    if (SDL_ShowSimpleMessageBox(sdlType, title, message, attachToWindow ? rc2d_engine_state.window : NULL) == 0) 
	{
        return true;
    } 
	else 
	{
        return false;
    }
}