#include "rc2d/rc2d_window.h" 
#include "rc2d/RC2D.h"

/**
 * Définit le titre de la fenêtre de l'application.
 *
 * @param title Le titre à attribuer à la fenêtre.
 */
void rc2d_window_setTitle(const char *title)
{
    if (title == NULL)
    {
        // Vérifie si le titre est valide (non NULL)
        rc2d_log(RC2D_LOG_ERROR, "Le titre de la fenêtre ne peut pas être NULL dans la fonction rc2d_window_setTitle().\n");
        return;
    }

	SDL_SetWindowTitle(rc2d_sdl_window, title);
}

/**
 * Configure le mode plein écran de la fenêtre.
 *
 * @param fullscreen Détermine si la fenêtre doit passer en mode plein écran ou non.
 * @param type Le type de mode plein écran à utiliser.
 */
void rc2d_window_setFullscreen(const bool fullscreen, const RC2D_FullscreenType type) 
{
    int result;
    if (fullscreen) 
    {
        switch (type) 
        {
            case RC2D_FULLSCREEN_WINDOWED:
                result = SDL_SetWindowFullscreen(rc2d_sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                if (result != 0) 
                {
                    rc2d_log(RC2D_LOG_ERROR, "Impossible de passer en mode plein écran : %s dans rc2d_window_setFullscreen().\n", SDL_GetError());
                }
                break;
            case RC2D_FULLSCREEN_HARDWARE:
                result = SDL_SetWindowFullscreen(rc2d_sdl_window, SDL_WINDOW_FULLSCREEN);
                if (result != 0) 
                {
                    rc2d_log(RC2D_LOG_ERROR, "Impossible de passer en mode plein écran : %s dans rc2d_window_setFullscreen().\n", SDL_GetError());
                }
                break;
            default:
                // Gestion d'une valeur invalide de l'énumération
                rc2d_log(RC2D_LOG_ERROR, "Type de plein Ecran invalide.\n");
                break;
        }
    } 
    else 
    {
        result = SDL_SetWindowFullscreen(rc2d_sdl_window, 0); // Quitte le mode plein écran
        if (result != 0) 
        {
            rc2d_log(RC2D_LOG_ERROR, "Impossible de quitter le mode plein écran : %s dans rc2d_window_setFullscreen().\n", SDL_GetError());
        }
    }
}

/**
 * Active ou désactive le mode de synchronisation verticale (VSync) pour le rendu.
 * La synchronisation verticale permet de synchroniser le taux de rafraîchissement de l'affichage avec le taux de rafraîchissement de la fenêtre pour réduire le déchirement de l'image.
 *
 * @param vsync Si vrai, active le VSync. Si faux, le désactive.
 */
void rc2d_window_setVSync(bool vsync)
{
	if (vsync)
    {
        int result = SDL_RenderSetVSync(rc2d_sdl_renderer, 1);
        if (result != 0) 
        {
            rc2d_log(RC2D_LOG_ERROR, "Impossible d'activer VSync, dans rc2d_window_setVSync().\n");
        }
    }
	else
    {
        int result = SDL_RenderSetVSync(rc2d_sdl_renderer, 0);
        if (result != 0) 
        {
            rc2d_log(RC2D_LOG_ERROR, "Impossible de désactiver VSync, dans rc2d_window_setVSync().\n");
        }
    }
}

/**
 * Récupère la hauteur actuelle de la fenêtre de l'application.
 *
 * @return La hauteur de la fenêtre en pixels.
 */
int rc2d_window_getHeight(void)
{
	int width, height;
	SDL_GetWindowSize(rc2d_sdl_window, &width, &height);
	return height;
}

/**
 * Récupère la largeur actuelle de la fenêtre de l'application.
 *
 * @return La largeur de la fenêtre en pixels.
 */
int rc2d_window_getWidth(void)
{
	int width, height;
	SDL_GetWindowSize(rc2d_sdl_window, &width, &height);
	return width;
}

/**
 * Définit la taille de la fenêtre de l'application.
 * Cette fonction ajuste la taille de la fenêtre à la largeur et à la hauteur spécifiées.
 *
 * @param width La nouvelle largeur de la fenêtre en pixels.
 * @param height La nouvelle hauteur de la fenêtre en pixels.
 */
void rc2d_window_setWindowSize(int width, int height)
{
	SDL_SetWindowSize(rc2d_sdl_window, width, height);

	rc2d_sdl_window_width = width;
	rc2d_sdl_window_height = height;
}

/**
 * Obtient la largeur et la hauteur du bureau du moniteur qui contient la fenetre window
 *
 * @param width   La largeur du bureau.
 * @param height  La hauteur du bureau.
 */
void rc2d_window_getDesktopDimensions(int *width, int *height) 
{
    if (width == NULL || height == NULL)
    {
        // Vérifie si les pointeurs sont valides (non NULL)
        rc2d_log(RC2D_LOG_ERROR, "Les paramètres de rc2d_window_getDesktopDimensions() ne peuvent pas être NULL.\n");
        return;
    }

    // Récupère l'index du moniteur qui contient la fenêtre
	int displayIndex = SDL_GetWindowDisplayIndex(rc2d_sdl_window);
    if (displayIndex < 0)
    {
        rc2d_log(RC2D_LOG_ERROR, "Impossible de récupérer l'index du moniteur de la fenêtre : %s\n", SDL_GetError());
        
        // En cas d'échec, les dimensions sont mises à 0
        *width = 0;
        *height = 0;

        return;
    }

    SDL_DisplayMode mode;
    if (SDL_GetDesktopDisplayMode(displayIndex, &mode) != 0) 
	{
        // En cas d'échec, les dimensions sont mises à 0
        *width = 0;
        *height = 0;

        return;
    }
    
    // Attribue les valeurs de largeur et de hauteur
    *width = mode.w;
    *height = mode.h;
}

/**
 * Récupère le nombre de moniteurs connectés.
 *
 * @return Le nombre de moniteurs connectés.
 */
int rc2d_window_getDisplayCount(void) 
{
    // Utilise SDL_GetNumVideoDisplays pour obtenir le nombre de moniteurs connectés
    int numDisplays = SDL_GetNumVideoDisplays();
    if (numDisplays < 1)
    {
        rc2d_log(RC2D_LOG_ERROR, "Impossible de récupérer le nombre de moniteurs connectés : %s dans rc2d_window_getDisplayCount().\n", SDL_GetError());
        return 0;
    }
    
    // Renvoie le nombre de moniteurs connectés
    return numDisplays;
}

/**
 * Récupère l'orientation actuelle du moniteur principal.
 *
 * @return L'orientation actuelle du moniteur principal.
 */
RC2D_DisplayOrientation rc2d_window_getDisplayOrientation(void) 
{
	int displayIndex = SDL_GetWindowDisplayIndex(rc2d_sdl_window);
    if (displayIndex < 0)
    {
        rc2d_log(RC2D_LOG_ERROR, "Impossible de récupérer l'index du moniteur de la fenêtre : %s\n", SDL_GetError());
        return RC2D_ORIENTATION_UNKNOWN;
    }

    SDL_DisplayOrientation orientation = SDL_GetDisplayOrientation(displayIndex);
    switch (orientation) 
	{
        case SDL_ORIENTATION_UNKNOWN:
            return RC2D_ORIENTATION_UNKNOWN;
        case SDL_ORIENTATION_LANDSCAPE:
            return RC2D_ORIENTATION_LANDSCAPE;
        case SDL_ORIENTATION_LANDSCAPE_FLIPPED:
            return RC2D_ORIENTATION_LANDSCAPE_FLIPPED;
        case SDL_ORIENTATION_PORTRAIT:
            return RC2D_ORIENTATION_PORTRAIT;
        case SDL_ORIENTATION_PORTRAIT_FLIPPED:
            return RC2D_ORIENTATION_PORTRAIT_FLIPPED;
        default:
            return RC2D_ORIENTATION_UNKNOWN;
    }
}

/**
 * Récupère le titre de la fenêtre.
 *
 * @return Le titre de la fenêtre.
 */
const char *rc2d_window_getTitle(void) 
{
    return SDL_GetWindowTitle(rc2d_sdl_window);
}

/**
 * Récupère la valeur actuelle de VSync.
 *
 * @return true si VSync est activé, sinon false.
 */
bool rc2d_window_getVSync(void) 
{
    SDL_RendererInfo rendererInfo;

    if (SDL_GetRendererInfo(rc2d_sdl_renderer, &rendererInfo) == 0) 
    {
        // Vérifie si le flag SDL_RENDERER_PRESENTVSYNC est activé
        return (rendererInfo.flags & SDL_RENDERER_PRESENTVSYNC) != 0;
    } 
    else 
    {
        // Gérer l'erreur si SDL_GetRendererInfo échoue
        rc2d_log(RC2D_LOG_ERROR, "Erreur lors de la récupération des informations du renderer: %s\n", SDL_GetError());
        return false;
    }
}

/**
 * Récupère la position de la fenêtre à l'écran.
 *
 * @param x La coordonnée x de la position de la fenêtre.
 * @param y La coordonnée y de la position de la fenêtre.
 */
void rc2d_window_getPosition(int *x, int *y) 
{
    if (x == NULL || y == NULL) 
    {
        // Vérifie si les pointeurs sont valides (non NULL)
        rc2d_log(RC2D_LOG_ERROR, "Les paramètres de rc2d_window_getPosition() ne peuvent pas être NULL.\n");
        return;
    }

    SDL_GetWindowPosition(rc2d_sdl_window, x, y);
}

/**
 * Récupère si la fenêtre est en mode plein écran.
 *
 * @return true si la fenêtre est en mode plein écran, sinon false.
 */
bool rc2d_window_getFullscreen(void) 
{
    Uint32 flags = SDL_GetWindowFlags(rc2d_sdl_window);
    return (flags & SDL_WINDOW_FULLSCREEN) || (flags & SDL_WINDOW_FULLSCREEN_DESKTOP);
}

/**
 * Vérifie si la fenêtre du jeu a le focus du clavier.
 *
 * @return true si la fenêtre a le focus du clavier, sinon false.
 */
bool rc2d_window_hasKeyboardFocus(void) 
{
    return SDL_GetKeyboardFocus() == rc2d_sdl_window;
}

/**
 * Vérifie si la fenêtre du jeu a le focus de la souris.
 *
 * @return true si la fenêtre a le focus de la souris, sinon false.
 */
bool rc2d_window_hasMouseFocus(void) 
{
    return SDL_GetMouseFocus() == rc2d_sdl_window;
}

/**
 * Élevez la fenêtre window au-dessus des autres fenêtres et définissez le focus d’entrée.
 */
void rc2d_window_bringToFrontAndRequestAttention(void) 
{
    SDL_ShowWindow(rc2d_sdl_window);
    SDL_RaiseWindow(rc2d_sdl_window);
}

/**
 * Vérifie si la fenêtre du jeu est visible.
 *
 * @return true si la fenêtre est visible, sinon false.
 */
bool rc2d_window_isVisible(void) 
{
    return SDL_GetWindowFlags(rc2d_sdl_window) & SDL_WINDOW_SHOWN;
}

/**
 * Minimise la fenêtre.
 */
void rc2d_window_minimize(void) 
{
    SDL_MinimizeWindow(rc2d_sdl_window);
}

/**
 * Maximise la fenêtre.
 */
void rc2d_window_maximize(void) 
{
    SDL_MaximizeWindow(rc2d_sdl_window);
}

/**
 * Vérifie si la fenêtre est minimisée.
 *
 * @return true si la fenêtre est minimisée, sinon false.
 */
bool rc2d_window_isMinimized(void) 
{
    return SDL_GetWindowFlags(rc2d_sdl_window) & SDL_WINDOW_MINIMIZED;
}

/**
 * Vérifie si la fenêtre est maximisée.
 *
 * @return true si la fenêtre est maximisée, sinon false.
 */
bool rc2d_window_isMaximized(void) 
{
    return SDL_GetWindowFlags(rc2d_sdl_window) & SDL_WINDOW_MAXIMIZED;
}

/**
 * Restaure la taille et la position de la fenêtre si elle était minimisée ou maximisée.
 */
void rc2d_window_restore(void) 
{
    SDL_RestoreWindow(rc2d_sdl_window);
}

/**
 * Définit la position de la fenêtre sur l'écran.
 * 
 * La position de la fenêtre est dans l'espace de coordonnées du moniteur contenant la fenêtre.
 * 
 * @param x La coordonnée x de la position de la fenêtre.
 * @param y La coordonnée y de la position de la fenêtre.
 */
void rc2d_window_setPosition(int x, int y) 
{
    int displayIndex = SDL_GetWindowDisplayIndex(rc2d_sdl_window);
    if (displayIndex < 0) 
	{
        rc2d_log(RC2D_LOG_ERROR, "Impossible de récupérer l'index du moniteur de la fenêtre : %s\n", SDL_GetError());
        return;
    }

    SDL_Rect displayBounds;
    if (SDL_GetDisplayBounds(displayIndex, &displayBounds) != 0) 
	{
        rc2d_log(RC2D_LOG_ERROR, "Impossible de récupérer les limites de l'affichage %d : %s\n", displayIndex, SDL_GetError());
        return;
    }

    // Vérifie si la position spécifiée est valide par rapport aux limites de l'affichage
    if (x < displayBounds.x || x > displayBounds.x + displayBounds.w ||
        y < displayBounds.y || y > displayBounds.y + displayBounds.h) 
	{
        rc2d_log(RC2D_LOG_ERROR, "La position spécifiée est en dehors des limites de l'affichage %d\n", displayIndex);
        return;
    }

    SDL_SetWindowPosition(rc2d_sdl_window, x, y);
}

/**
 * Affiche une boîte de message personnalisée avec une liste de boutons.
 * 
 * @param title Le titre de la boîte de message.
 * @param message Le texte à l'intérieur de la boîte de message.
 * @param buttonlist Un tableau contenant une liste de noms de boutons à afficher.
 * @param numButtons Le nombre de boutons dans le tableau buttonlist.
 * @param pressedButton Un pointeur vers une variable où l'index du bouton pressé sera stocké.
 * @param type Le type de la boîte de message (erreur, avertissement, information).
 * @param attachToWindow Booléen indiquant si la boîte de message doit être attachée à la fenêtre ou non.
 * @return L'index du bouton pressé par l'utilisateur. Peut être 0 si la boîte de dialogue de message a été fermée sans appuyer sur un bouton.
 */
bool rc2d_window_showMessageBox(const char *title, const char *message, const char *buttonlist[], uint32_t numButtons, uint32_t *pressedButton, RC2D_MessageBoxType type, bool attachToWindow) 
{
    if (title == NULL || message == NULL || buttonlist == NULL || pressedButton == NULL)
    {
        // Vérifie si les pointeurs sont valides (non NULL)
        rc2d_log(RC2D_LOG_ERROR, "Les paramètre de rc2d_window_showMessageBox() ne peuvent pas être NULL.\n");
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
        rc2d_log(RC2D_LOG_ERROR, "Impossible d'allouer de la mémoire pour le tableau de boutons.\n");
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
    messageBoxData.window = attachToWindow ? rc2d_sdl_window : NULL;

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
    free(buttons);

    return true;
}

/**
 * Affiche une boîte de message simple avec un bouton 'OK'.
 * 
 * @param title Le titre de la boîte de message.
 * @param message Le texte à l'intérieur de la boîte de message.
 * @param type Le type de la boîte de message (erreur, avertissement, information).
 * @param attachToWindow Booléen indiquant si la boîte de message doit être attachée à la fenêtre ou non.
 * @return true si la boîte de message a été affichée avec succès, sinon false.
 */
bool rc2d_window_showSimpleMessageBox(const char *title, const char *message, RC2D_MessageBoxType type, bool attachToWindow) {
    if (title == NULL || message == NULL)
    {
        // Vérifie si les pointeurs sont valides (non NULL)
        rc2d_log(RC2D_LOG_ERROR, "Les paramètre de rc2d_window_showSimpleMessageBox() ne peuvent pas être NULL.\n");
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
    if (SDL_ShowSimpleMessageBox(sdlType, title, message, attachToWindow ? rc2d_sdl_window : NULL) == 0) 
	{
        return true;
    } 
	else 
	{
        return false;
    }
}