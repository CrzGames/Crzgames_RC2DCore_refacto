#include <RC2D/RC2D_window.h>
#include <RC2D/RC2D_internal.h>
#include <RC2D/RC2D_logger.h>
#include <RC2D/RC2D_math.h>

static bool rc2d_is_current_fullscreen = false;
static RC2D_FullscreenType rc2d_current_fullscreen_type = RC2D_FULLSCREEN_NONE;

static bool rc2d_window_hasFlag(SDL_WindowFlags flag)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour interroger les drapeaux.\n");
        return false;
    }

    // Récupère les drapeaux de la fenêtre
    SDL_WindowFlags flags = SDL_GetWindowFlags(rc2d_engine_state.window);

    // Vérifie si le drapeau spécifié est présent dans les drapeaux de la fenêtre
    return (flags & flag) != 0;
}

SDL_Window* rc2d_window_getWindow(void)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour récupérer la fenêtre.\n");
        return NULL;
    }

    // Retourne le pointeur vers la fenêtre
    return rc2d_engine_state.window;
}

void rc2d_window_setMinimumSize(int width, int height)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour définir la taille minimale.\n");
        return;
    }

    // Définit la taille minimale de la fenêtre
    SDL_SetWindowMinimumSize(rc2d_engine_state.window, width, height);
}

void rc2d_window_setMaximumSize(int width, int height)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour définir la taille maximale.\n");
        return;
    }

    // Définit la taille maximale de la fenêtre
    SDL_SetWindowMaximumSize(rc2d_engine_state.window, width, height);
}

void rc2d_window_setTitle(const char *title)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour définir le titre.\n");
        return;
    }

    // Si le titre est NULL, on log une erreur
    if (title == NULL)
    {
        // Vérifie si le titre de la fenetre est valide (non NULL)
        RC2D_log(RC2D_LOG_ERROR, "rc2d_window_setTitle : Le titre de la fenetre ne peut pas etre NULL.\n");
        return;
    }

    // Définit le titre de la fenêtre
	if (!SDL_SetWindowTitle(rc2d_engine_state.window, title))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de définir le titre de la fenêtre : %s\n", SDL_GetError());
    }
}

void rc2d_window_setFullscreen(const bool fullscreen, const RC2D_FullscreenType type, const bool syncWindow)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour définir le mode plein écran.\n");
        return;
    }

    /**
     * Si fullscreen est vrai, on active le mode plein écran 
     * soit RC2D_FULLSCREEN_BORDERLESS ou RC2D_FULLSCREEN_EXCLUSIVE
     */
    if (fullscreen) 
    {
        bool result = false;
        switch (type) 
        {
            // Mode plein écran fenêtré : utiliser la résolution actuelle du bureau
            case RC2D_FULLSCREEN_BORDERLESS:
                /**
                 * On choisit le mode plein écran fenêtré (borderless) pour la fenêtre.
                 * 
                 * Cette fonction n'applique pas encore le fullscreen à l'écran, 
                 * mais seulement le mode de présentation.
                 */
                result = SDL_SetWindowFullscreenMode(rc2d_engine_state.window, NULL);
                if (!result) 
                {
                    RC2D_log(RC2D_LOG_ERROR, "rc2d_window_setFullscreen: Impossible de définir le mode plein écran : %s\n", SDL_GetError());
                    return;
                }

                /**
                 * On applique le mode plein écran sur la fenêtre.
                 * 
                 * Utilise la résolution actuelle du bureau sans changer la résolution réelle de l'affichage,
                 * puisque le mode plein écran est borderless.
                 */
                result &= SDL_SetWindowFullscreen(rc2d_engine_state.window, true);
                if (!result) 
                {
                    RC2D_log(RC2D_LOG_ERROR, "rc2d_window_setFullscreen: Impossible de définir le mode plein écran : %s\n", SDL_GetError());
                    return;
                }
                break;

            // Mode plein écran exclusif : utiliser le meilleur mode disponible
            case RC2D_FULLSCREEN_EXCLUSIVE: {
                /**
                 * Récupérer l'index du moniteur qui contient la fenêtre
                 */
                SDL_DisplayID displayID = SDL_GetDisplayForWindow(rc2d_engine_state.window);
                if (displayID == 0)
                {
                    RC2D_log(RC2D_LOG_ERROR, "rc2d_window_setFullscreen: Impossible de récupérer l'index du moniteur de la fenêtre : %s\n", SDL_GetError());
                    return;
                }

                /**
                 * Récupérer les modes d'affichage disponibles pour le 
                 * moniteur qui contient la fenêtre.
                 */
                int count = 0;
                SDL_DisplayMode **modes = SDL_GetFullscreenDisplayModes(displayID, &count);
                if (modes == NULL || count <= 0) 
                {
                    RC2D_log(RC2D_LOG_ERROR, "rc2d_window_setFullscreen: Impossible de récupérer les modes plein écran : %s\n", SDL_GetError());
                    if (modes != NULL) 
                    {
                        SDL_free(modes);
                    }
                    return;
                }

                /**
                 * On choisit le meilleur mode disponible pour le fullscreen (le premier de la liste).
                 * Qui aura la meilleure résolution, le meilleur taux de rafraîchissement..etc.
                 * 
                 * Cette fonction n'applique pas encore le fullscreen à l'écran,
                 * mais seulement le mode de présentation.
                 */
                result = SDL_SetWindowFullscreenMode(rc2d_engine_state.window, modes[0]);
                if (!result) 
                {
                    RC2D_log(RC2D_LOG_ERROR, "rc2d_window_setFullscreen: Impossible de définir le mode plein écran : %s\n", SDL_GetError());
                    SDL_free(modes);
                    return;
                }
                
                /**
                 * On applique le mode plein écran sur la fenêtre.
                 * Cette fonction va changer la résolution de l'affichage et le mode de présentation.
                 */
                result &= SDL_SetWindowFullscreen(rc2d_engine_state.window, true);
                if (!result) 
                {
                    RC2D_log(RC2D_LOG_ERROR, "rc2d_window_setFullscreen: Impossible de définir le mode plein écran : %s\n", SDL_GetError());
                    SDL_free(modes);
                    return;
                }
                break;
            }
            
            default:
                RC2D_log(RC2D_LOG_ERROR, "rc2d_window_setFullscreen: Type de plein ecran invalide.\n");
                return;
        }

        if (!result) 
        {
            RC2D_log(RC2D_LOG_ERROR, "rc2d_window_setFullscreen: Echec de l activation du plein ecran : %s.\n", SDL_GetError());
        }
        else
        {
            // Met à jour l'état du mode plein écran, indiquant que la fenêtre est en plein écran
            rc2d_is_current_fullscreen = true;
            rc2d_current_fullscreen_type = type;
        }
    } 
    else 
    {
        // Quitter le mode plein écran (retour fenêtré standard)
        if (!SDL_SetWindowFullscreen(rc2d_engine_state.window, false)) 
        {
            RC2D_log(RC2D_LOG_ERROR, "rc2d_window_setFullscreen : Impossible de quitter le mode plein ecran : %s.\n", SDL_GetError());
        }
        else
        {
            // Met à jour l'état du mode plein écran, indiquant que la fenêtre n'est pas en plein écran
            rc2d_is_current_fullscreen = false;
            rc2d_current_fullscreen_type = RC2D_FULLSCREEN_NONE;
        }
    }

    // Attendre que le changement soit pris en compte
   if (syncWindow)
   {
        SDL_SyncWindow(rc2d_engine_state.window);
   }
}

void rc2d_window_setVSync(const bool vsync)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour définir le mode VSync.\n");
        return;
    }

    /**
     * Si "vsync" est true, on utilise le mode de présentation SDL_GPU_PRESENTMODE_MAILBOX,
     * si il n'est pas supporté, on utilise le mode VSync classique (SDL_GPU_PRESENTMODE_VSYNC).
     * Dans les deux cas, préférer le mode SDL_GPU_PRESENTMODE_MAILBOX si il est supporté.
     *
     * Si "vsync" est false, on utilise le mode SDL_GPU_PRESENTMODE_IMMEDIATE, si il est supporté.
     * 
     * SDL_GPU_PRESENTMODE_MAILBOX et SDL_GPU_PRESENTMODE_IMMEDIATE peuvent ne pas être supportés sur tous les systèmes.
     * SDL_GPU_PRESENTMODE_VSYNC sera toujours pris en charge.
     */
    const SDL_GPUPresentMode preferredMode = vsync ? SDL_GPU_PRESENTMODE_MAILBOX : SDL_GPU_PRESENTMODE_IMMEDIATE;
    const SDL_GPUPresentMode fallbackMode  = SDL_GPU_PRESENTMODE_VSYNC;

    /**
     * Le mode de présentation du GPU, actuellement sélectionné, est celui qui sera utilisé pour le rendu.
     */
    SDL_GPUPresentMode selectedMode = preferredMode;

    /**
     * Si le mode préféré n’est pas supporté, fallback sur VSYNC classique (SDL_GPU_PRESENTMODE_VSYNC).
     */
    if (!SDL_WindowSupportsGPUPresentMode(rc2d_engine_state.gpu_device, rc2d_engine_state.window, selectedMode)) {
        if (vsync && SDL_WindowSupportsGPUPresentMode(rc2d_engine_state.gpu_device, rc2d_engine_state.window, fallbackMode)) {
            selectedMode = fallbackMode;
        } else {
            RC2D_log(RC2D_LOG_ERROR, "Aucun mode GPU compatible trouvé pour VSync = %s", vsync ? "true" : "false");
            return;
        }
    }

    // Set le mode de présentation du GPU pour la fenêtre
    rc2d_engine_state.gpu_present_mode = selectedMode;

    // Définit le mode de présentation du GPU pour la fenêtre
    if (!SDL_SetGPUSwapchainParameters(rc2d_engine_state.gpu_device, rc2d_engine_state.window, rc2d_engine_state.gpu_swapchain_composition, rc2d_engine_state.gpu_present_mode)) {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de définir le mode GPU : %s", SDL_GetError());
    }
}

int rc2d_window_getHeight(void)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour récupérer la hauteur.\n");
        return -1;
    }

    // Récupère la hauteur de la fenêtre
	int height;
	if (!SDL_GetWindowSize(rc2d_engine_state.window, NULL, &height))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de récupérer la hauteur de la fenêtre : %s\n", SDL_GetError());
        return -1;
    }

    // Renvoie la hauteur de la fenêtre
	return height;
}

int rc2d_window_getWidth(void)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour récupérer la largeur.\n");
        return -1;
    }

    // Récupère la largeur de la fenêtre
	int width;
	if (!SDL_GetWindowSize(rc2d_engine_state.window, &width, NULL))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de récupérer la largeur de la fenêtre : %s\n", SDL_GetError());
        return -1;
    }

    // Renvoie la largeur de la fenêtre
	return width;
}

void rc2d_window_setSize(const int width, const int height)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour définir la taille.\n");
        return;
    }

    // Vérifie si la largeur et la hauteur sont valides (supérieures à 0)
    if (width <= 0 || height <= 0) 
    {
        RC2D_log(RC2D_LOG_ERROR, "La largeur et la hauteur de la fenêtre doivent être supérieures à 0.\n");
        return;
    }

    // Définit la taille de la fenêtre
	if (!SDL_SetWindowSize(rc2d_engine_state.window, width, height))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de définir la taille de la fenêtre : %s\n", SDL_GetError());
    }
}

int rc2d_window_getDisplayCount(void) 
{
    // Utilise SDL_GetDisplays pour obtenir le nombre de moniteurs connectés
    int numDisplays = 0;
    SDL_DisplayID *displays = SDL_GetDisplays(&numDisplays);
    if (displays == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de récupérer le nombre de moniteurs connectés : %s dans rc2d_window_getDisplayCount().\n", SDL_GetError());
        return 0;
    }
    
    // Renvoie le nombre de moniteurs connectés
    return numDisplays;
}

const char *rc2d_window_getDisplayName(int displayID)
{
    // Vérifie si l'ID du moniteur est valide
    if (displayID < 0 || displayID >= rc2d_window_getDisplayCount())
    {
        RC2D_log(RC2D_LOG_ERROR, "ID de moniteur invalide : %d dans rc2d_window_getDisplayName().\n", displayID);
        return NULL;
    }

    // Récupère le nom du moniteur
    const char *displayName = SDL_GetDisplayName(displayID);
    if (displayName == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de récupérer le nom du moniteur : %s dans rc2d_window_getDisplayName().\n", SDL_GetError());
        return NULL;
    }

    // Renvoie le nom du moniteur
    return displayName;
}

RC2D_DisplayID rc2d_window_getDisplayForWindow(void)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour récupérer l'ID de l'affichage.\n");
        return -1;
    }

    // Récupère l'index du moniteur qui contient la fenêtre
    SDL_DisplayID displayID = SDL_GetDisplayForWindow(rc2d_engine_state.window);
    if (displayID == 0)
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de récupérer l'index du moniteur de la fenêtre : %s\n", SDL_GetError());
        return 0;
    }

    // Mappe l'index du moniteur à un ID de moniteur RC2D
    RC2D_DisplayID rc2dDisplayID = (RC2D_DisplayID)displayID;

    // Renvoie l'ID de l'affichage
    return rc2dDisplayID;
}

RC2D_DisplayOrientation rc2d_window_getDisplayOrientation(void) 
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour récupérer l'orientation.\n");
        return RC2D_ORIENTATION_UNKNOWN;
    }

    // Récupère l'index du moniteur qui contient la fenêtre
	SDL_DisplayID displayID = SDL_GetDisplayForWindow(rc2d_engine_state.window);
    if (displayID == 0)
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de récupérer l'index du moniteur de la fenêtre : %s\n", SDL_GetError());
        return RC2D_ORIENTATION_UNKNOWN;
    }

    // Récupère l'orientation actuelle du moniteur
    SDL_DisplayOrientation orientation = SDL_GetCurrentDisplayOrientation(displayID);
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

const char *rc2d_window_getTitle(void) 
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour récupérer le titre.\n");
        return NULL;
    }

    // Récupère le titre de la fenêtre
    return SDL_GetWindowTitle(rc2d_engine_state.window);
}

bool rc2d_window_getVSync(void) 
{
    /**
     * True si le mode de présentation GPU est synchrone (MAILBOX ou VSYNC) est actif, 
     * sinon false (IMMEDIATE).
     */
    return rc2d_engine_state.gpu_present_mode != SDL_GPU_PRESENTMODE_IMMEDIATE;
}

RC2D_FullscreenInfo rc2d_window_getFullscreen(void)
{
    RC2D_FullscreenInfo fullscreenInfo = {0};

    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour vérifier le mode plein écran.\n");
        fullscreenInfo.is_fullscreen = false;
        fullscreenInfo.type = RC2D_FULLSCREEN_NONE;
        return fullscreenInfo;
    }

    fullscreenInfo.is_fullscreen = rc2d_is_current_fullscreen;
    fullscreenInfo.type = rc2d_current_fullscreen_type;

    // Retourne l'état actuel stocké
    return fullscreenInfo;
}

bool rc2d_window_hasKeyboardFocus(void) 
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour vérifier le focus du clavier.\n");
        return false;
    }

    // Vérifie si la fenêtre a le focus du clavier
    return SDL_GetKeyboardFocus() == rc2d_engine_state.window;
}

bool rc2d_window_hasMouseFocus(void) 
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour vérifier le focus de la souris.\n");
        return false;
    }

    // Vérifie si la fenêtre a le focus de la souris
    return SDL_GetMouseFocus() == rc2d_engine_state.window;
}

bool rc2d_window_isVisible(void)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour vérifier si elle est visible.\n");
        return false;
    }

    // Vérifie si la fenêtre est visible
    return rc2d_window_hasFlag(SDL_WINDOW_HIDDEN);
}


void rc2d_window_minimize(void) 
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour minimiser.\n");
        return;
    }

    if (!SDL_MinimizeWindow(rc2d_engine_state.window))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de minimiser la fenêtre : %s\n", SDL_GetError());
    }
}

/**
 * Maximise la fenêtre.
 */
void rc2d_window_maximize(void) 
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour maximiser.\n");
        return;
    }

    if (!SDL_MaximizeWindow(rc2d_engine_state.window))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de maximiser la fenêtre : %s\n", SDL_GetError());
    }
}

bool rc2d_window_isMinimized(void)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour vérifier si elle est minimisée.\n");
        return false;
    }

    return rc2d_window_hasFlag(SDL_WINDOW_MINIMIZED);
}

bool rc2d_window_isMaximized(void) 
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour vérifier si elle est maximisée.\n");
        return false;
    }

    return rc2d_window_hasFlag(SDL_WINDOW_MAXIMIZED);
}

void rc2d_window_restore(void) 
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour restaurer.\n");
        return;
    }

    /**
     * Restaure la fenêtre à sa taille et position d'origine 
     * si elle était minimisée ou maximisée.
     */
    if (!SDL_RestoreWindow(rc2d_engine_state.window))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de restaurer la fenêtre : %s\n", SDL_GetError());
    }
}

float rc2d_window_getPixelDensity(void)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour obtenir le facteur DPI.\n");
        return 0.0f;
    }

    // Récupère la densité de pixels (dpi) de la fenêtre
    float pixelDensity = SDL_GetWindowPixelDensity(rc2d_engine_state.window);
    if (pixelDensity == 0.0f)
    {
        RC2D_log(RC2D_LOG_ERROR, "Échec de récupération de la densité de pixels de la fenêtre.\n", SDL_GetError());
        return 0.0f;
    }

    // Renvoie la densité de pixels
    return pixelDensity;
}

void rc2d_window_getSize(int *width, int *height)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour récupérer la taille.");
        return;
    }

    // Récupère la taille de la fenêtre en unités logiques
    if (!SDL_GetWindowSize(rc2d_engine_state.window, width, height))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de récupérer la taille de la fenêtre : %s", SDL_GetError());
    }
}

void rc2d_window_getSizeInPixels(int *width, int *height)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour récupérer la taille en pixels.");
        return;
    }

    // Récupère la taille de la fenêtre en pixels physiques (HiDPI)
    if (!SDL_GetWindowSizeInPixels(rc2d_engine_state.window, width, height))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de récupérer la taille en pixels de la fenêtre : %s", SDL_GetError());
    }
}

float rc2d_window_getDisplayScale(void)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour obtenir le display scale.");
        return 0.0f;
    }

    // Récupère le facteur d'échelle de la fenêtre
    float scale = SDL_GetWindowDisplayScale(rc2d_engine_state.window);
    if (scale == 0.0f)
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de récupérer le display scale de la fenêtre : %s", SDL_GetError());
        return 0.0f;
    }

    // Renvoie le facteur d'échelle
    return scale;
}

float rc2d_window_getContentScale(void)
{
    float pixel_density = rc2d_window_getPixelDensity();
    float display_scale = rc2d_window_getDisplayScale();

    return pixel_density * display_scale;
}

void rc2d_window_getSafeArea(RC2D_Rect *rect)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour obtenir la zone sûre (safe area).\n");
        return;
    }

    // Vérifie si le rectangle est valide (non NULL)
    if (rect == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Pointeur 'rect' invalide passé à rc2d_window_getSafeArea.\n");
        return;
    }

    // Récupère la zone sûre (safe area) de la fenêtre
    SDL_Rect rectSDL;
    if (!SDL_GetWindowSafeArea(rc2d_engine_state.window, &rectSDL))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de récupérer la safe area : %s\n", SDL_GetError());
        return;
    }
    else
    {
        rect->x = rectSDL.x;
        rect->y = rectSDL.y;
        rect->width = rectSDL.w;
        rect->height = rectSDL.h;
    }
}

void rc2d_window_setResizable(bool resizable) 
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour définir la redimensionnabilité.\n");
        return;
    }

    // Set la redimensionnabilité de la fenêtre
    if (!SDL_SetWindowResizable(rc2d_engine_state.window, resizable))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de définir la redimensionnabilité de la fenêtre : %s\n", SDL_GetError());
    }
}

bool rc2d_window_isResizable(void) 
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Aucune fenêtre active pour vérifier la redimensionnabilité.\n");
        return false;
    }

    // Vérifie si la fenêtre est redimensionnable
    return rc2d_window_hasFlag(SDL_WINDOW_RESIZABLE);
}

void rc2d_window_setAlwaysOnTop(bool enable)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL) {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de modifier l'état always-on-top : aucune fenêtre active.\n");
        return;
    }

    // Définit l'état always-on-top de la fenêtre
    if (!SDL_SetWindowAlwaysOnTop(rc2d_engine_state.window, enable))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de définir l'état always-on-top de la fenêtre : %s\n", SDL_GetError());
    }
}

void rc2d_window_setMouseGrabbed(bool grabbed)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de modifier la capture de souris : aucune fenêtre active.\n");
        return;
    }

    // Définit la capture de souris de la fenêtre
    if (!SDL_SetWindowMouseGrab(rc2d_engine_state.window, grabbed))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de définir la capture de souris : %s\n", SDL_GetError());
    }
}

bool rc2d_window_isMouseGrabbed(void)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de vérifier la capture de souris : aucune fenêtre active.\n");
        return false;
    }

    // Vérifie si la fenêtre a la capture de souris
    return rc2d_window_hasFlag(SDL_WINDOW_MOUSE_GRABBED);
}

void rc2d_window_setKeyboardGrabbed(bool grabbed)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de modifier la capture du clavier : aucune fenêtre active.\n");
        return;
    }

    // Définit la capture du clavier de la fenêtre
    if (!SDL_SetWindowKeyboardGrab(rc2d_engine_state.window, grabbed))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de définir la capture du clavier : %s\n", SDL_GetError());
    }
}

bool rc2d_window_isKeyboardGrabbed(void)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de vérifier la capture du clavier : aucune fenêtre active.\n");
        return false;
    }

    // Vérifie si la fenêtre a la capture du clavier
    return rc2d_window_hasFlag(SDL_WINDOW_KEYBOARD_GRABBED);
}

bool rc2d_window_isOccluded(void)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de vérifier l'état d'occlusion : aucune fenêtre active.\n");
        return false;
    }

    // Vérifie si la fenêtre est occluse
    return rc2d_window_hasFlag(SDL_WINDOW_OCCLUDED);
}

bool rc2d_window_isMouseCaptured(void)
{
    // Vérifie si la fenêtre est en mode de capture de la souris
    return rc2d_window_hasFlag(SDL_WINDOW_MOUSE_CAPTURE);
}

void rc2d_window_setMouseCaptured(bool capture)
{
    // Active ou désactive la capture de la souris pour la fenêtre
    if (!SDL_CaptureMouse(capture))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de définir la capture de la souris : %s\n", SDL_GetError());
    }
}

bool rc2d_window_isMouseInRelativeMode(void)
{
    // Vérifie si la fenêtre est en mode relatif de la souris
    return rc2d_window_hasFlag(SDL_WINDOW_MOUSE_RELATIVE_MODE);
}

void rc2d_window_setMouseRelativeMode(bool enabled)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de modifier le mode relatif de la souris : aucune fenêtre active.\n");
        return;
    }

    // Définit le mode relatif de la souris de la fenêtre
    if (!SDL_SetWindowRelativeMouseMode(rc2d_engine_state.window, enabled))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de définir le mode relatif de la souris : %s\n", SDL_GetError());
    }
}

bool rc2d_window_isNotFocusable(void)
{
    // Vérifie si la fenêtre est non focusable
    return rc2d_window_hasFlag(SDL_WINDOW_NOT_FOCUSABLE);
}
