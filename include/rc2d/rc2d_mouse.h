#ifndef RC2D_MOUSE_H
#define RC2D_MOUSE_H

#include <SDL.h>
#include <stdbool.h> // Required for : bool

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure représentant un curseur personnalisé.
 * @typedef {Object} RC2D_Cursor
 * @property {SDL_Cursor} sdl_cursor - Pointeur vers le curseur SDL.
 */
typedef struct RC2D_Cursor {
	SDL_Cursor* sdl_cursor;
} RC2D_Cursor;

/**
 * Enum définissant les différents types de curseurs système.
 * @typedef {enum} RC2D_SystemCursor
 * @property {number} RC2D_CURSOR_ARROW - Curseur Flèche.
 * @property {number} RC2D_CURSOR_IBEAM - Curseur I-beam (curseur de texte).
 * @property {number} RC2D_CURSOR_WAIT - Curseur d'attente (sablier).
 * @property {number} RC2D_CURSOR_CROSSHAIR - Curseur Viseur.
 * @property {number} RC2D_CURSOR_WAITARROW - Curseur d'attente avec flèche.
 * @property {number} RC2D_CURSOR_SIZENWSE - Curseur Redimensionner NW-SE.
 * @property {number} RC2D_CURSOR_SIZENESW - Curseur Redimensionner NE-SW.
 * @property {number} RC2D_CURSOR_SIZEWE - Curseur Redimensionner horizontalement.
 * @property {number} RC2D_CURSOR_SIZENS - Curseur Redimensionner verticalement.
 * @property {number} RC2D_CURSOR_SIZEALL - Curseur Redimensionner toutes les directions.
 * @property {number} RC2D_CURSOR_NO - Curseur interdit.
 * @property {number} RC2D_CURSOR_HAND - Curseur Main.
 */
typedef enum RC2D_SystemCursor {
    RC2D_CURSOR_ARROW,      
    RC2D_CURSOR_IBEAM,       
    RC2D_CURSOR_WAIT,       
    RC2D_CURSOR_CROSSHAIR,  
    RC2D_CURSOR_WAITARROW,   
    RC2D_CURSOR_SIZENWSE,    
    RC2D_CURSOR_SIZENESW,
    RC2D_CURSOR_SIZEWE,      
    RC2D_CURSOR_SIZENS,     
    RC2D_CURSOR_SIZEALL,     
    RC2D_CURSOR_NO,          
    RC2D_CURSOR_HAND
} RC2D_SystemCursor;

/**
 * Enum définissant les différents boutons de la souris.
 * @typedef {enum} RC2D_MouseButtons
 * @property {number} RC2D_MOUSE_BUTTON_LEFT - Bouton gauche de la souris.
 * @property {number} RC2D_MOUSE_BUTTON_MIDDLE - Bouton central de la souris.
 * @property {number} RC2D_MOUSE_BUTTON_RIGHT - Bouton droit de la souris.
 */
typedef enum RC2D_MouseButtons {
    RC2D_MOUSE_BUTTON_LEFT,
    RC2D_MOUSE_BUTTON_MIDDLE,
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