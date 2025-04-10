#ifndef RC2D_WINDOW_H
#define RC2D_WINDOW_H

#include <stdbool.h> // Required for : bool
#include <stdint.h> // Required for : uint32_t
 
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumération représentant les orientations d'affichage.
 * @typedef {enum} RC2D_DISPLAY_ORIENTATION
 * @property {number} RC2D_ORIENTATION_UNKNOWN - Orientation inconnue.
 * @property {number} RC2D_ORIENTATION_LANDSCAPE - Orientation paysage.
 * @property {number} RC2D_ORIENTATION_LANDSCAPE_FLIPPED - Orientation paysage inversée.
 * @property {number} RC2D_ORIENTATION_PORTRAIT - Orientation portrait.
 * @property {number} RC2D_ORIENTATION_PORTRAIT_FLIPPED - Orientation portrait inversée.
 */
typedef enum RC2D_DisplayOrientation {
    RC2D_ORIENTATION_UNKNOWN,
    RC2D_ORIENTATION_LANDSCAPE,
    RC2D_ORIENTATION_LANDSCAPE_FLIPPED,
    RC2D_ORIENTATION_PORTRAIT,
    RC2D_ORIENTATION_PORTRAIT_FLIPPED
} RC2D_DisplayOrientation;

/**
 * Enumération représentant les types de boîtes de message.
 * @typedef {enum} RC2D_MESSAGEBOX_TYPE
 * @property {number} RC2D_MESSAGEBOX_ERROR - Boîte de message d'erreur.
 * @property {number} RC2D_MESSAGEBOX_WARNING - Boîte de message d'avertissement.
 * @property {number} RC2D_MESSAGEBOX_INFORMATION - Boîte de message d'information.
 */
typedef enum RC2D_MessageBoxType {
    RC2D_MESSAGEBOX_ERROR,
    RC2D_MESSAGEBOX_WARNING,
    RC2D_MESSAGEBOX_INFORMATION
} RC2D_MessageBoxType;

/**
 * Enumération représentant les types de plein écran.
 * @typedef {enum} RC2D_FULLSCREEN_TYPE
 * @property {number} RC2D_FULLSCREEN_WINDOWED
 * - Plein écran adaptatif, utilisant la résolution actuelle du bureau tout en restant dans une fenêtre. 
 * - Cela permet une transition plus fluide entre les applications et le bureau, sans modifier la résolution native de l'écran.
 * @property {number} RC2D_FULLSCREEN_HARDWARE 
 * - Plein écran exclusif, prenant le contrôle total de l'affichage et permettant potentiellement de changer la résolution de l'écran pour 
 *   celle de l'application. Ce mode est optimisé pour les performances, en offrant un accès direct au matériel graphique.
 */
typedef enum RC2D_FullscreenType {
    RC2D_FULLSCREEN_WINDOWED,
    RC2D_FULLSCREEN_HARDWARE
} RC2D_FullscreenType;

void rc2d_window_setTitle(const char *title);
void rc2d_window_setFullscreen(const bool fullscreen, const RC2D_FullscreenType type);
void rc2d_window_setVSync(const bool vsync);
void rc2d_window_setWindowSize(const int width, const int height);
void rc2d_window_setPosition(const int x, const int y);

void rc2d_window_getDesktopDimensions(int *width, int *height);
int rc2d_window_getDisplayCount(void);
RC2D_DisplayOrientation rc2d_window_getDisplayOrientation(void);
const char *rc2d_window_getTitle(void);
bool rc2d_window_getVSync(void);
void rc2d_window_getPosition(int *x, int *y);
bool rc2d_window_getFullscreen(void);
int rc2d_window_getHeight(void);
int rc2d_window_getWidth(void);

void rc2d_window_restore(void);
void rc2d_window_bringToFrontAndRequestAttention(void);

bool rc2d_window_isMaximized(void);
bool rc2d_window_isMinimized(void);
bool rc2d_window_isVisible(void);

void rc2d_window_maximize(void);
void rc2d_window_minimize(void);

bool rc2d_window_hasKeyboardFocus(void);
bool rc2d_window_hasMouseFocus(void);

bool rc2d_window_showMessageBox(const char *title, const char *message, const char *buttonlist[], uint32_t numButtons, uint32_t *pressedButton, RC2D_MessageBoxType type, bool attachToWindow);
bool rc2d_window_showSimpleMessageBox(const char *title, const char *message, RC2D_MessageBoxType type, bool attachToWindow);

#ifdef __cplusplus
}
#endif

#endif // RC2D_WINDOW_H