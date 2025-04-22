#include <RC2D/RC2D_mouse.h>
#include <RC2D/RC2D_internal.h> // Required for : rc2d_engine_state.window (SDL_Window*)
#include <RC2D/RC2D_logger.h> // Required for : RC2D_log

#include <SDL3_image/SDL_image.h>

static RC2D_Cursor* currentCursor = NULL;

/**
 * Vérifie si le curseur est pris en charge par la plateforme actuelle.
 * Sur certaines plateformes mobiles comme Android et iOS, les curseurs ne sont généralement pas pris en charge.
 * 
 * @return True si le curseur est pris en charge, False sinon.
 */
bool rc2d_mouse_isCursorSupported(void) 
{
#if defined(RC2D_PLATFORM_IOS) || defined(RC2D_PLATFORM_ANDROID)
    return false;
#else
    // Pour les plateformes de bureau et html5 supposez que le support de curseur existe.
    return true;
#endif
}

/**
 * Vérifie si la souris est capturée par la fenêtre. Lorsque la souris est capturée,
 * les mouvements du curseur sont limités à l'intérieur de la fenêtre de l'application.
 * 
 * @return True si la souris est capturée, False sinon.
 */
bool rc2d_mouse_isGrabbed(void) 
{
    return SDL_GetWindowMouseGrab(rc2d_engine_state.window);
}

/**
 * Définit la visibilité du curseur de la souris.
 * 
 * @param visible Détermine si le curseur doit être visible (true) ou caché (false).
 */
void rc2d_mouse_setVisible(const bool visible)
{
	if (visible)
    {
        if (!SDL_ShowCursor())
        {
            RC2D_log(RC2D_LOG_ERROR, "SDL_ShowCursor failed in rc2d_mouse_setVisible: %s\n", SDL_GetError());
        }
    }
	else 
    {
        if (!SDL_HideCursor())
        {
            RC2D_log(RC2D_LOG_ERROR, "SDL_ShowCursor failed in rc2d_mouse_setVisible: %s\n", SDL_GetError());
        }
    }
}

/**
 * Crée un nouveau curseur à partir d'une image spécifiée.
 * 
 * @param filePath Chemin vers l'image qui sera utilisée comme curseur.
 * @param hotx La position x du point chaud du curseur dans l'image.
 * @param hoty La position y du point chaud du curseur dans l'image.
 * @return Le curseur créé.
 */
RC2D_Cursor* rc2d_mouse_newCursor(const char* filePath, const int hotx, const int hoty)
{
    if (filePath == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Le chemin du fichier image est NULL dans rc2d_mouse_newCursor().");
        return NULL;
    }

	RC2D_Cursor* cursor = NULL;

	SDL_Surface* cursorImage = NULL;
	cursorImage = IMG_Load(filePath);

	if (cursorImage == NULL)
	{
		RC2D_log(RC2D_LOG_ERROR, "SDL_Surface failed in rc2d_mouse_newCursor: %s\n", SDL_GetError());
	}
	else
	{
		if (SDL_CreateColorCursor(cursorImage, hotx, hoty) == NULL)
		{
			RC2D_log(RC2D_LOG_ERROR, "SDL_CreateColorCursor failed in rc2d_mouse_newCursor: %s\n", SDL_GetError());
		}
		else
		{
			cursor = SDL_CreateColorCursor(cursorImage, hotx, hoty);

			SDL_DestroySurface(cursorImage);
			cursorImage = NULL;
		}
	}

	return cursor;
}

/**
 * \brief Crée un nouveau curseur système en fonction de l'identifiant spécifié.
 * 
 * Cette fonction permet d’obtenir un curseur système standard
 * correspondant à l’identifiant RC2D fourni.
 * 
 * @param systemCursorId Identifiant du curseur système souhaité.
 * @return Le curseur système créé. Si la création échoue, le curseur retourné sera invalide (NULL).
 * 
 * @since Cette fonction est disponible depuis RC2D 1.0.0.
 */
RC2D_Cursor* rc2d_mouse_newSystemCursor(const RC2D_SystemCursor systemCursorId) 
{
    SDL_SystemCursor sdlCursorId;
    switch (systemCursorId) 
    {
        case RC2D_SYSTEM_CURSOR_DEFAULT:       sdlCursorId = SDL_SYSTEM_CURSOR_DEFAULT; break;
        case RC2D_SYSTEM_CURSOR_TEXT:          sdlCursorId = SDL_SYSTEM_CURSOR_TEXT; break;
        case RC2D_SYSTEM_CURSOR_WAIT:          sdlCursorId = SDL_SYSTEM_CURSOR_WAIT; break;
        case RC2D_SYSTEM_CURSOR_CROSSHAIR:     sdlCursorId = SDL_SYSTEM_CURSOR_CROSSHAIR; break;
        case RC2D_SYSTEM_CURSOR_PROGRESS:      sdlCursorId = SDL_SYSTEM_CURSOR_PROGRESS; break;
        case RC2D_SYSTEM_CURSOR_NWSE_RESIZE:   sdlCursorId = SDL_SYSTEM_CURSOR_NWSE_RESIZE; break;
        case RC2D_SYSTEM_CURSOR_NESW_RESIZE:   sdlCursorId = SDL_SYSTEM_CURSOR_NESW_RESIZE; break;
        case RC2D_SYSTEM_CURSOR_EW_RESIZE:     sdlCursorId = SDL_SYSTEM_CURSOR_EW_RESIZE; break;
        case RC2D_SYSTEM_CURSOR_NS_RESIZE:     sdlCursorId = SDL_SYSTEM_CURSOR_NS_RESIZE; break;
        case RC2D_SYSTEM_CURSOR_MOVE:          sdlCursorId = SDL_SYSTEM_CURSOR_MOVE; break;
        case RC2D_SYSTEM_CURSOR_NOT_ALLOWED:   sdlCursorId = SDL_SYSTEM_CURSOR_NOT_ALLOWED; break;
        case RC2D_SYSTEM_CURSOR_POINTER:       sdlCursorId = SDL_SYSTEM_CURSOR_POINTER; break;
        case RC2D_SYSTEM_CURSOR_NW_RESIZE:     sdlCursorId = SDL_SYSTEM_CURSOR_NW_RESIZE; break;
        case RC2D_SYSTEM_CURSOR_N_RESIZE:      sdlCursorId = SDL_SYSTEM_CURSOR_N_RESIZE; break;
        case RC2D_SYSTEM_CURSOR_NE_RESIZE:     sdlCursorId = SDL_SYSTEM_CURSOR_NE_RESIZE; break;
        case RC2D_SYSTEM_CURSOR_E_RESIZE:      sdlCursorId = SDL_SYSTEM_CURSOR_E_RESIZE; break;
        case RC2D_SYSTEM_CURSOR_SE_RESIZE:     sdlCursorId = SDL_SYSTEM_CURSOR_SE_RESIZE; break;
        case RC2D_SYSTEM_CURSOR_S_RESIZE:      sdlCursorId = SDL_SYSTEM_CURSOR_S_RESIZE; break;
        case RC2D_SYSTEM_CURSOR_SW_RESIZE:     sdlCursorId = SDL_SYSTEM_CURSOR_SW_RESIZE; break;
        case RC2D_SYSTEM_CURSOR_W_RESIZE:      sdlCursorId = SDL_SYSTEM_CURSOR_W_RESIZE; break;
        default:                                sdlCursorId = SDL_SYSTEM_CURSOR_DEFAULT; break; // Fallback
    }

    RC2D_Cursor* cursor = NULL;
    cursor = SDL_CreateSystemCursor(sdlCursorId);
    if (cursor == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "SDL_CreateSystemCursor failed in rc2d_mouse_newSystemCursor: %s", SDL_GetError());
        return NULL; // Erreur lors de la création du curseur
    }

    return cursor;
}

/**
 * Définit si la souris doit être capturée par la fenêtre.
 * 
 * @param grabbed Détermine si la souris doit être capturée (true) ou relâchée (false).
 */
void rc2d_window_setGrabbed(bool grabbed)
{
    // Vérifie si la fenêtre est valide
    if (rc2d_engine_state.window == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de modifier la capture de souris : aucune fenêtre active.\n");
        return;
    }

    // Définit la capture de souris de la fenêtre
    (!SDL_SetWindowMouseGrab(rc2d_engine_state.window, grabbed))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de définir la capture de souris : %s\n", SDL_GetError());
    }
}

/**
 * Récupère le mode relatif de la souris, indiquant si les mouvements de la souris sont rapportés relativement à la position précédente.
 * 
 * @return True si le mode relatif est activé, False sinon.
 */
bool rc2d_mouse_getRelativeMode(void) 
{
    return SDL_GetRelativeMouseMode();
}

/**
 * Récupère le curseur actuellement utilisé.
 * 
 * @return Le curseur actuellement en utilisation.
 */
RC2D_Cursor rc2d_mouse_getCurrentCursor(void) 
{
    if (currentCursor != NULL) 
	{
        return *currentCursor;
    }
    
    // Retourner un curseur vide ou par défaut si aucun curseur n'est actuellement défini
    RC2D_Cursor emptyCursor;
	emptyCursor.sdl_cursor = NULL;

    return emptyCursor;
}

/**
 * Vérifie si un bouton de la souris est actuellement enfoncé.
 * 
 * @param button Le bouton de la souris à vérifier.
 * @return True si le bouton spécifié est enfoncé, False sinon.
 */
bool rc2d_mouse_isDown(const RC2D_MouseButtons button) 
{
    Uint32 sdlButtonMask = SDL_GetMouseState(NULL, NULL);
	
    switch (button) 
	{
        case RC2D_MOUSE_BUTTON_LEFT:
            return sdlButtonMask & SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
        case RC2D_MOUSE_BUTTON_MIDDLE:
            return sdlButtonMask & SDL_BUTTON_MASK(SDL_BUTTON_MIDDLE);
        case RC2D_MOUSE_BUTTON_RIGHT:
            return sdlButtonMask & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);
        default:
            return false;
    }
}

/**
 * Récupère la position x de la souris.
 * 
 * @return La position x actuelle de la souris.
 */
int rc2d_mouse_getX(void)
{
    int x;
    SDL_GetMouseState(&x, NULL);

    return x;
}

/**
 * Récupère la position y de la souris.
 * 
 * @return La position y actuelle de la souris.
 */
int rc2d_mouse_getY(void)
{
    int y;
    SDL_GetMouseState(NULL, &y);

    return y;
}

/**
 * Récupère à la fois les positions x et y de la souris.
 * 
 * @param x Pointeur vers une variable où stocker la position x actuelle de la souris.
 * @param y Pointeur vers une variable où stocker la position y actuelle de la souris.
 */
void rc2d_mouse_getPosition(int* x, int* y) 
{
    if (x == NULL || y == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Les pointeurs x et y sont NULL dans rc2d_mouse_getPosition().");
        return;
    }

    SDL_GetMouseState(x, y);
}

/**
 * Active ou désactive le mode relatif pour la souris.
 * 
 * @param enabled Détermine si le mode relatif doit être activé (true) ou désactivé (false).
 */
void rc2d_mouse_setRelativeMode(const bool enabled) 
{
    SDL_bool mode = enabled ? SDL_TRUE : SDL_FALSE;
    
    int result = SDL_SetRelativeMouseMode(mode);
    if (result < 0)
    {
        RC2D_log(RC2D_LOG_ERROR, "SDL_SetRelativeMouseMode failed in rc2d_mouse_setRelativeMode: %s\n", SDL_GetError());
    }
}

/**
 * Définit la position X de la souris dans la fenêtre actuelle.
 * Cette fonction déplace le curseur de la souris à la position X spécifiée, en conservant la position Y actuelle.
 * 
 * @param x La nouvelle position X pour le curseur de la souris.
 */
void rc2d_mouse_setX(int x)
{
    int currentMouseY;
    SDL_GetMouseState(NULL, &currentMouseY);
    SDL_WarpMouseInWindow(rc2d_sdl_window, x, currentMouseY);
}

/**
 * Définit la position Y de la souris dans la fenêtre actuelle.
 * Cette fonction déplace le curseur de la souris à la position Y spécifiée, en conservant la position X actuelle.
 * 
 * @param y La nouvelle position Y pour le curseur de la souris.
 */
void rc2d_mouse_setY(int y)
{
    int currentMouseX;
    SDL_GetMouseState(&currentMouseX, NULL);
    SDL_WarpMouseInWindow(rc2d_sdl_window, currentMouseX, y);
}

/**
 * Vérifie si le curseur de la souris est actuellement visible.
 * 
 * @return True si le curseur est visible, False s'il est caché.
 */
bool rc2d_mouse_isVisible(void)
{
    int result = SDL_ShowCursor(SDL_QUERY);
    if (result < 0)
    {
        RC2D_log(RC2D_LOG_ERROR, "SDL_ShowCursor failed in rc2d_mouse_isVisible: %s\n", SDL_GetError());
    }

    return result == SDL_ENABLE;
}

/**
 * Libère les ressources associées à un curseur.
 * 
 * @param cursor Le curseur à libérer.
 */
void rc2d_mouse_freeCursor(RC2D_Cursor* cursor)
{
	if (cursor->sdl_cursor != NULL)
	{
		SDL_FreeCursor(cursor->sdl_cursor);
		cursor->sdl_cursor = NULL;
	}
}

/**
 * Définit le curseur actuellement utilisé.
 * 
 * @param cursor Le nouveau curseur à utiliser.
 */
void rc2d_mouse_setCursor(const RC2D_Cursor* cursor) 
{
    if (cursor->sdl_cursor != NULL) 
	{
        SDL_SetCursor(cursor->sdl_cursor);
        
        // Mettre à jour le curseur current
        currentCursor = cursor;
    }
}

/**
 * Déplace le curseur à une position spécifique.
 * 
 * @param x La position x cible pour le curseur.
 * @param y La position y cible pour le curseur.
 */
void rc2d_mouse_setPosition(int x, int y) 
{
    // Récupérer le mode de rendu logique actuel
    RC2D_RenderLogicalMode mode = rc2d_getRenderLogicalSizeMode();

	if (mode != RC2D_RENDER_LOGICAL_SIZE_MODE_NONE)
	{
		// Récupérer les facteurs de mise à l'échelle
		double scaleX, scaleY;
		rc2d_getAspectRatioScales(&scaleX, &scaleY);

		// Récupérer les décalages pour l'overscan si nécessaire
		double offsetX, offsetY;
		rc2d_getOverscanOffsets(&offsetX, &offsetY);

		// Appliquer les ajustements basés sur le mode de rendu logique
		if (mode == RC2D_RENDER_LOGICAL_SIZE_MODE_LETTERBOX) 
		{
			// Ajuster les coordonnées en fonction des échelles et des décalages récupérés
			x = (int)(x * scaleX);
			y = (int)(y * scaleY);
		} 
		else if (mode == RC2D_RENDER_LOGICAL_SIZE_MODE_OVERSCAN)
		{
			// Ajuster les coordonnées en fonction des échelles et des décalages récupérés
			x = (int)(x * scaleX + offsetX);
			y = (int)(y * scaleY + offsetY);
		}
	}

    // Déplacer le curseur de la souris à la position ajustée
    SDL_WarpMouseInWindow(rc2d_sdl_window, x, y);
}