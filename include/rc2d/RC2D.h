#ifndef RC2D_H
#define RC2D_H

#include <SDL.h>

#ifndef __EMSCRIPTEN__
#include "rc2d/rc2d_data.h"
#include "rc2d/rc2d_net.h"
#endif // __EMSCRIPTEN__
#include "rc2d/rc2d_audio.h"
#include "rc2d/rc2d_collision.h"
#include "rc2d/rc2d_event.h"
#include "rc2d/rc2d_filesystem.h"
#include "rc2d/rc2d_graphics.h"
#include "rc2d/rc2d_joystick.h"
#include "rc2d/rc2d_keyboard.h"
#include "rc2d/rc2d_logger.h"
#include "rc2d/rc2d_math.h"
#include "rc2d/rc2d_mouse.h"
#include "rc2d/rc2d_rres.h"
#include "rc2d/rc2d_spine.h"
#include "rc2d/rc2d_system.h"
#include "rc2d/rc2d_thread.h"
#include "rc2d/rc2d_timer.h"
#include "rc2d/rc2d_touch.h"
#include "rc2d/rc2d_tweening.h"
#include "rc2d/rc2d_window.h"

#ifdef __cplusplus
extern "C" {
#endif

// Instance Window for SDL2 and width/height
extern SDL_Window* rc2d_sdl_window;
extern int rc2d_sdl_window_width;
extern int rc2d_sdl_window_height;

// Instance Renderer for SDL2 and width/height
extern SDL_Renderer* rc2d_sdl_renderer;
extern int rc2d_sdl_renderer_width;
extern int rc2d_sdl_renderer_height;
#ifdef __EMSCRIPTEN__
extern double rc2d_devicePixelRatioCanvas;
#endif

// Event for Game Loop
extern SDL_Event rc2d_event;

// DeltaTime - FrameRates
extern int FPS;
extern double rc2d_deltaTime;

// Game Loop - Running
extern bool gameIsRunning;

/**
 * Structure représentant les fonctions de rappel pour l'API Engine.
 * @typedef {struct} RC2D_Callbacks
 * @property {function} rc2d_unload - Fonction appelée lors du déchargement du jeu.
 * @property {function} rc2d_load - Fonction appelée lors du chargement du jeu.
 * @property {function} rc2d_update - Fonction appelée pour mettre à jour le jeu.
 * @property {function} rc2d_draw - Fonction appelée pour dessiner le jeu.
 * @property {function} rc2d_keypressed - Fonction appelée lorsqu'une touche est enfoncée.
 * @property {function} rc2d_keyreleased - Fonction appelée lorsqu'une touche est relâchée.
 * @property {function} rc2d_mousemoved - Fonction appelée lorsque la souris est déplacée.
 * @property {function} rc2d_mousepressed - Fonction appelée lorsqu'un bouton de la souris est enfoncé.
 * @property {function} rc2d_mousereleased - Fonction appelée lorsqu'un bouton de la souris est relâché.
 * @property {function} rc2d_wheelmoved - Fonction appelée lorsqu'une molette de la souris est déplacée.
 * @property {function} rc2d_touchmoved - Fonction appelée lorsqu'un toucher est déplacé.
 * @property {function} rc2d_touchpressed - Fonction appelée lorsqu'un toucher est enfoncé.
 * @property {function} rc2d_touchreleased - Fonction appelée lorsqu'un toucher est relâché.
 * @property {function} rc2d_gamepadaxis - Fonction appelée lorsqu'un axe de la manette est déplacé.
 * @property {function} rc2d_gamepadpressed - Fonction appelée lorsqu'un bouton de la manette est enfoncé.
 * @property {function} rc2d_gamepadreleased - Fonction appelée lorsqu'un bouton de la manette est relâché.
 * @property {function} rc2d_joystickaxis - Fonction appelée lorsqu'un axe du joystick est déplacé.
 * @property {function} rc2d_joystickhat - Fonction appelée lorsqu'un chapeau du joystick est déplacé.
 * @property {function} rc2d_joystickpressed - Fonction appelée lorsqu'un bouton du joystick est enfoncé.
 * @property {function} rc2d_joystickreleased - Fonction appelée lorsqu'un bouton du joystick est relâché.
 * @property {function} rc2d_joystickadded - Fonction appelée lorsqu'un joystick est ajouté.
 * @property {function} rc2d_joystickremoved - Fonction appelée lorsqu'un joystick est retiré.
 * @property {function} rc2d_dropfile - Fonction appelée lorsqu'un fichier est déposé.
 * @property {function} rc2d_droptext - Fonction appelée lorsqu'un texte est déposé.
 * @property {function} rc2d_dropbegin - Fonction appelée au début du dépôt.
 * @property {function} rc2d_dropcomplete - Fonction appelée à la fin du dépôt.
 * @property {function} rc2d_windowresized - Fonction appelée lorsque la fenêtre est redimensionnée.
 * @property {function} rc2d_windowmoved - Fonction appelée lorsque la fenêtre est déplacée.
 * @property {function} rc2d_windowsizedchanged - Fonction appelée lorsque la taille de la fenêtre est modifiée.
 * @property {function} rc2d_windowexposed - Fonction appelée lorsque la fenêtre est exposée.
 * @property {function} rc2d_windowminimized - Fonction appelée lorsque la fenêtre est réduite.
 * @property {function} rc2d_windowmaximized - Fonction appelée lorsque la fenêtre est maximisée.
 * @property {function} rc2d_windowrestored - Fonction appelée lorsque la fenêtre est restaurée.
 * @property {function} rc2d_mouseenteredwindow - Fonction appelée lorsque la souris entre dans la fenêtre.
 * @property {function} rc2d_mouseleftwindow - Fonction appelée lorsque la souris quitte la fenêtre.
 * @property {function} rc2d_keyboardfocusgained - Fonction appelée lorsque le focus clavier est obtenu.
 * @property {function} rc2d_keyboardfocuslost - Fonction appelée lorsque le focus clavier est perdu.
 * @property {function} rc2d_windowclosed - Fonction appelée lorsque la fenêtre est fermée.
 * @property {function} rc2d_windowtakefocus - Fonction appelée lorsque la fenêtre prend le focus.
 * @property {function} rc2d_windowhittest - Fonction appelée pour le test de collision de la fenêtre.
 */
typedef struct RC2D_Callbacks {
    // Game Loop Callbacks
    void (*rc2d_unload)(void);
    void (*rc2d_load)(void);
    void (*rc2d_update)(double dt);
    void (*rc2d_draw)(void);

    // Keyboard Callbacks
    void (*rc2d_keypressed)(const char* key, bool isrepeat);
    void (*rc2d_keyreleased)(const char* key);

    // Mouse Callbacks
    void (*rc2d_mousemoved)(int x, int y);
    void (*rc2d_mousepressed)(int x, int y, const char* button, int presses);
    void (*rc2d_mousereleased)(int x, int y, const char* button, int presses);
    void (*rc2d_wheelmoved)(const char* scroll);

    // Touch Callbacks
    void (*rc2d_touchmoved)(SDL_TouchID touchID, SDL_FingerID fingerID, float x, float y, float dx, float dy);
    void (*rc2d_touchpressed)(SDL_TouchID touchID, SDL_FingerID fingerID, float x, float y, float pressure);
    void (*rc2d_touchreleased)(SDL_TouchID touchID, SDL_FingerID fingerID, float x, float y, float pressure);

    // Gamepad and Joystick Callbacks
    void (*rc2d_gamepadaxis)(SDL_JoystickID joystick, Uint8 axis, float value);
    void (*rc2d_gamepadpressed)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_gamepadreleased)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_joystickaxis)(SDL_JoystickID joystick, Uint8 axis, float value);
    void (*rc2d_joystickhat)(SDL_JoystickID joystick, Uint8 hat, Uint8 value);
    void (*rc2d_joystickpressed)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_joystickreleased)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_joystickadded)(Sint32 joystick);
    void (*rc2d_joystickremoved)(Sint32 joystick);

    // Drag and Drop Callbacks
    void (*rc2d_dropfile)(const char* pathFile);
    void (*rc2d_droptext)(const char* pathFile);
    void (*rc2d_dropbegin)(const char* pathFile);
    void (*rc2d_dropcomplete)(const char* pathFile);

    // Window Callbacks
    void (*rc2d_windowresized)(int newWidth, int newHeight);
    void (*rc2d_windowmoved)(int x, int y);
    void (*rc2d_windowsizedchanged)(int newWidth, int newHeight);
    void (*rc2d_windowexposed)(void);
    void (*rc2d_windowminimized)(void);
    void (*rc2d_windowmaximized)(void);
    void (*rc2d_windowrestored)(void);
    void (*rc2d_mouseenteredwindow)(void);
    void (*rc2d_mouseleftwindow)(void);
    void (*rc2d_keyboardfocusgained)(void);
    void (*rc2d_keyboardfocuslost)(void);
    void (*rc2d_windowclosed)(void);
    void (*rc2d_windowtakefocus)(void);
    void (*rc2d_windowhittest)(void);

    // Logical Size OvernScan callbacks
    void (*rc2d_overscanadjusted)(int offsetX, int offsetY);
} RC2D_Callbacks;

/**
 * Enumération représentant les modes de taille logique de rendu.
 * @typedef {enum} RC2D_RenderLogicalMode
 * @property {number} RC2D_RENDER_LOGICAL_SIZE_MODE_NONE - Aucun mode de taille logique.
 * @property {number} RC2D_RENDER_LOGICAL_SIZE_MODE_LETTERBOX - Mode de taille logique avec barres noires.
 * @property {number} RC2D_RENDER_LOGICAL_SIZE_MODE_OVERSCAN - Mode de taille logique avec surbalayage.
 */
typedef enum RC2D_RenderLogicalMode {
    RC2D_RENDER_LOGICAL_SIZE_MODE_NONE,
    RC2D_RENDER_LOGICAL_SIZE_MODE_LETTERBOX,
    RC2D_RENDER_LOGICAL_SIZE_MODE_OVERSCAN
} RC2D_RenderLogicalMode;

// API General for GameEngine 
int rc2d_run(RC2D_Callbacks* callbacksUser, int windowWidth, int windowHeight, int rendererWidth, int rendererHeight, RC2D_RenderLogicalMode renderLogicalMode, RC2D_Image* image);

void rc2d_getOverscanOffsets(double* offsetX, double* offsetY);
void rc2d_getAspectRatioScales(double* scaleX, double* scaleY);
RC2D_RenderLogicalMode rc2d_getRenderLogicalSizeMode(void);
void rc2d_getRendererVirtualSize(int* width, int* height);

#ifdef __cplusplus
}
#endif

#endif // RC2D_H