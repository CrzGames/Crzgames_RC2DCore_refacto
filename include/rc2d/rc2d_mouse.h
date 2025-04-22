#ifndef RC2D_MOUSE_H
#define RC2D_MOUSE_H

#include <SDL3/SDL_mouse.h>
#include <stdbool.h> // Required for : bool

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Structure représentant un curseur personnalisé.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct SDL_Cursor RC2D_Cursor;

/**
 * \brief Enum définissant les différents types de curseurs système.
 * 
 * \since Cette enum est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_SystemCursor {
    /**
     * Le curseur par défaut. Généralement une flèche.
     */
    RC2D_SYSTEM_CURSOR_DEFAULT,

    /**
     * Curseur de sélection de texte. 
     * Généralement un I-beam.
     */
    RC2D_SYSTEM_CURSOR_TEXT,

    /**
     * Curseur d'attente. Généralement un sablier, une montre ou une roue tournante.
     */
    RC2D_SYSTEM_CURSOR_WAIT,

    /**
     * Curseur en forme de croix pour des sélections précises.
     */
    RC2D_SYSTEM_CURSOR_CROSSHAIR,

    /**
     * Le programme est occupé mais reste interactif.
     * Généralement une flèche combinée avec un sablier.
     */
    RC2D_SYSTEM_CURSOR_PROGRESS,

    /**
     * Flèche double pointant vers le nord-ouest et le sud-est.
     * Utilisée pour redimensionner les fenêtres.
     */
    RC2D_SYSTEM_CURSOR_NWSE_RESIZE,

    /**
     * Flèche double pointant vers le nord-est et le sud-ouest.
     * Utilisée pour redimensionner les fenêtres.
     */
    RC2D_SYSTEM_CURSOR_NESW_RESIZE,

    /**
     * Flèche double pointant vers l’est et l’ouest.
     * Utilisée pour redimensionner horizontalement.
     */
    RC2D_SYSTEM_CURSOR_EW_RESIZE,

    /**
     * Flèche double pointant vers le nord et le sud.
     * Utilisée pour redimensionner verticalement.
     */
    RC2D_SYSTEM_CURSOR_NS_RESIZE,

    /**
     * Curseur de déplacement. Généralement une croix à quatre flèches.
     */
    RC2D_SYSTEM_CURSOR_MOVE,

    /**
     * Action non permise. Généralement représenté par un cercle barré.
     */
    RC2D_SYSTEM_CURSOR_NOT_ALLOWED,

    /**
     * Curseur indiquant un lien cliquable. 
     * Généralement une main pointant.
     */
    RC2D_SYSTEM_CURSOR_POINTER,

    /**
     * Redimensionnement depuis le coin supérieur gauche.
     */
    RC2D_SYSTEM_CURSOR_NW_RESIZE,

    /**
     * Redimensionnement depuis le haut.
     */
    RC2D_SYSTEM_CURSOR_N_RESIZE,

    /**
     * Redimensionnement depuis le coin supérieur droit.
     */
    RC2D_SYSTEM_CURSOR_NE_RESIZE,

    /**
     * Redimensionnement depuis la droite.
     */
    RC2D_SYSTEM_CURSOR_E_RESIZE,

    /**
     * Redimensionnement depuis le coin inférieur droit.
     */
    RC2D_SYSTEM_CURSOR_SE_RESIZE,

    /**
     * Redimensionnement depuis le bas.
     */
    RC2D_SYSTEM_CURSOR_S_RESIZE,

    /**
     * Redimensionnement depuis le coin inférieur gauche.
     */
    RC2D_SYSTEM_CURSOR_SW_RESIZE,

    /**
     * Redimensionnement depuis la gauche.
     */
    RC2D_SYSTEM_CURSOR_W_RESIZE,

    /**
     * Nombre total de curseurs système définis.
     */
    RC2D_SYSTEM_CURSOR_COUNT
} RC2D_SystemCursor;

/**
 * \brief Enum définissant les différents boutons de la souris.
 * 
 * \since Cette enum est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_MouseButtons {
    /**
     * Bouton gauche de la souris.
     */
    RC2D_MOUSE_BUTTON_LEFT,

    /**
     * Bouton du milieu de la souris.
     */
    RC2D_MOUSE_BUTTON_MIDDLE,

    /**
     * Bouton droit de la souris.
     */
    RC2D_MOUSE_BUTTON_RIGHT
} RC2D_MouseButtons;

RC2D_Cursor rc2d_mouse_newCursor(const char* filePath, const int hotx, const int hoty);
RC2D_Cursor rc2d_mouse_newSystemCursor(const RC2D_SystemCursor systemCursorId);
void rc2d_mouse_freeCursor(RC2D_Cursor* cursor);

bool rc2d_mouse_isVisible(void);
bool rc2d_mouse_isCursorSupported(void);
bool rc2d_mouse_isGrabbed(void);
bool rc2d_mouse_isDown(const RC2D_MouseButtons button);

RC2D_Cursor rc2d_mouse_getCurrentCursor(void);
void rc2d_mouse_getPosition(int* x, int* y);
int rc2d_mouse_getX(void);
int rc2d_mouse_getY(void);
bool rc2d_mouse_getRelativeMode(void);

void rc2d_mouse_setVisible(const bool visible);
void rc2d_mouse_setCursor(const RC2D_Cursor* cursor);
void rc2d_mouse_setPosition(int x, int y);
void rc2d_mouse_setX(int x);
void rc2d_mouse_setY(int y);
void rc2d_mouse_setGrabbed(const bool grabbed);
void rc2d_mouse_setRelativeMode(const bool enabled);

#ifdef __cplusplus
}
#endif

#endif // RC2D_MOUSE_H