#ifndef RC2D_JOYSTICK_H
#define RC2D_JOYSTICK_H

#include <SDL.h>
#include <stdbool.h> // Required for : bool

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumérations pour les axes du gamepad.
 * Les valeurs correspondent aux axes du gamepad SDL.
 * @typedef {enum} RC2D_GamepadAxis
 * @property {number} RC2D_GAMEPAD_AXIS_LEFT_X - Axe gauche X.
 * @property {number} RC2D_GAMEPAD_AXIS_LEFT_Y - Axe gauche Y.
 * @property {number} RC2D_GAMEPAD_AXIS_RIGHT_X - Axe droit X.
 * @property {number} RC2D_GAMEPAD_AXIS_RIGHT_Y - Axe droit Y.
 * @property {number} RC2D_GAMEPAD_AXIS_TRIGGER_LEFT - Gâchette gauche.
 * @property {number} RC2D_GAMEPAD_AXIS_TRIGGER_RIGHT - Gâchette droite.
 */
typedef enum RC2D_GamepadAxis {
    RC2D_GAMEPAD_AXIS_LEFT_X = SDL_CONTROLLER_AXIS_LEFTX,
    RC2D_GAMEPAD_AXIS_LEFT_Y = SDL_CONTROLLER_AXIS_LEFTY,
    RC2D_GAMEPAD_AXIS_RIGHT_X = SDL_CONTROLLER_AXIS_RIGHTX,
    RC2D_GAMEPAD_AXIS_RIGHT_Y = SDL_CONTROLLER_AXIS_RIGHTY,
    RC2D_GAMEPAD_AXIS_TRIGGER_LEFT = SDL_CONTROLLER_AXIS_TRIGGERLEFT,
    RC2D_GAMEPAD_AXIS_TRIGGER_RIGHT = SDL_CONTROLLER_AXIS_TRIGGERRIGHT
} RC2D_GamepadAxis;

/**
 * Enumérations pour les boutons du gamepad.
 * Les valeurs correspondent aux boutons du gamepad SDL.
 * @typedef {enum} RC2D_GamepadButton
 * @property {number} RC2D_GAMEPAD_BUTTON_INVALID - Bouton invalide.
 * @property {number} RC2D_GAMEPAD_BUTTON_A - Bouton A (face inférieure).
 * @property {number} RC2D_GAMEPAD_BUTTON_B - Bouton B (face droite).
 * @property {number} RC2D_GAMEPAD_BUTTON_X - Bouton X (face gauche).
 * @property {number} RC2D_GAMEPAD_BUTTON_Y - Bouton Y (face supérieure).
 * @property {number} RC2D_GAMEPAD_BUTTON_BACK - Bouton Retour.
 * @property {number} RC2D_GAMEPAD_BUTTON_GUIDE - Bouton Guide.
 * @property {number} RC2D_GAMEPAD_BUTTON_START - Bouton Démarrer.
 * @property {number} RC2D_GAMEPAD_BUTTON_LEFT_STICK - Clic sur le stick gauche.
 * @property {number} RC2D_GAMEPAD_BUTTON_RIGHT_STICK - Clic sur le stick droit.
 * @property {number} RC2D_GAMEPAD_BUTTON_LEFT_SHOULDER - Bouton bumper gauche.
 * @property {number} RC2D_GAMEPAD_BUTTON_RIGHT_SHOULDER - Bouton bumper droit.
 * @property {number} RC2D_GAMEPAD_BUTTON_DPAD_UP - D-pad haut.
 * @property {number} RC2D_GAMEPAD_BUTTON_DPAD_DOWN - D-pad bas.
 * @property {number} RC2D_GAMEPAD_BUTTON_DPAD_LEFT - D-pad gauche.
 * @property {number} RC2D_GAMEPAD_BUTTON_DPAD_RIGHT - D-pad droit.
 * @property {number} RC2D_GAMEPAD_BUTTON_MISC1 - Bouton de partage de la manette Xbox Series X, bouton micro de la manette PS5 et bouton de capture de la manette Switch Pro.
 * @property {number} RC2D_GAMEPAD_BUTTON_PADDLE1 - Premier bouton paddle.
 * @property {number} RC2D_GAMEPAD_BUTTON_PADDLE2 - Deuxième bouton paddle.
 * @property {number} RC2D_GAMEPAD_BUTTON_PADDLE3 - Troisième bouton paddle.
 * @property {number} RC2D_GAMEPAD_BUTTON_PADDLE4 - Quatrième bouton paddle.
 * @property {number} RC2D_GAMEPAD_BUTTON_TOUCHPAD - Bouton pression touchpad / PS4/PS5 touchpad button.
 * @property {number} RC2D_GAMEPAD_BUTTON_MAX - Nombre maximum de boutons.
 */
typedef enum RC2D_GamepadButton {
    RC2D_GAMEPAD_BUTTON_INVALID = SDL_CONTROLLER_BUTTON_INVALID, 
    RC2D_GAMEPAD_BUTTON_A = SDL_CONTROLLER_BUTTON_A, 
    RC2D_GAMEPAD_BUTTON_B = SDL_CONTROLLER_BUTTON_B, 
    RC2D_GAMEPAD_BUTTON_X = SDL_CONTROLLER_BUTTON_X, 
    RC2D_GAMEPAD_BUTTON_Y = SDL_CONTROLLER_BUTTON_Y, 
    RC2D_GAMEPAD_BUTTON_BACK = SDL_CONTROLLER_BUTTON_BACK, 
    RC2D_GAMEPAD_BUTTON_GUIDE = SDL_CONTROLLER_BUTTON_GUIDE, 
    RC2D_GAMEPAD_BUTTON_START = SDL_CONTROLLER_BUTTON_START, 
    RC2D_GAMEPAD_BUTTON_LEFT_STICK = SDL_CONTROLLER_BUTTON_LEFTSTICK, 
    RC2D_GAMEPAD_BUTTON_RIGHT_STICK = SDL_CONTROLLER_BUTTON_RIGHTSTICK, 
    RC2D_GAMEPAD_BUTTON_LEFT_SHOULDER = SDL_CONTROLLER_BUTTON_LEFTSHOULDER, 
    RC2D_GAMEPAD_BUTTON_RIGHT_SHOULDER = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, 
    RC2D_GAMEPAD_BUTTON_DPAD_UP = SDL_CONTROLLER_BUTTON_DPAD_UP, 
    RC2D_GAMEPAD_BUTTON_DPAD_DOWN = SDL_CONTROLLER_BUTTON_DPAD_DOWN, 
    RC2D_GAMEPAD_BUTTON_DPAD_LEFT = SDL_CONTROLLER_BUTTON_DPAD_LEFT, 
    RC2D_GAMEPAD_BUTTON_DPAD_RIGHT = SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
    RC2D_GAMEPAD_BUTTON_MISC1 = SDL_CONTROLLER_BUTTON_MISC1, 
    RC2D_GAMEPAD_BUTTON_PADDLE1 = SDL_CONTROLLER_BUTTON_PADDLE1,
    RC2D_GAMEPAD_BUTTON_PADDLE2 = SDL_CONTROLLER_BUTTON_PADDLE2, 
    RC2D_GAMEPAD_BUTTON_PADDLE3 = SDL_CONTROLLER_BUTTON_PADDLE3, 
    RC2D_GAMEPAD_BUTTON_PADDLE4 = SDL_CONTROLLER_BUTTON_PADDLE4, 
    RC2D_GAMEPAD_BUTTON_TOUCHPAD = SDL_CONTROLLER_BUTTON_TOUCHPAD, 
    RC2D_GAMEPAD_BUTTON_MAX = SDL_CONTROLLER_BUTTON_MAX
} RC2D_GamepadButton;

/**
 * Enumérations pour les positions du chapeau du joystick.
 * Les valeurs correspondent aux positions du chapeau du joystick SDL.
 * @typedef {enum} RC2D_JoystickHat
 * @property {number} RC2D_JOYSTICK_HAT_CENTERED - Position centrale.
 * @property {number} RC2D_JOYSTICK_HAT_UP - Position haut.
 * @property {number} RC2D_JOYSTICK_HAT_RIGHT - Position droite.
 * @property {number} RC2D_JOYSTICK_HAT_DOWN - Position bas.
 * @property {number} RC2D_JOYSTICK_HAT_LEFT - Position gauche.
 * @property {number} RC2D_JOYSTICK_HAT_RIGHTUP - Position diagonale haut-droite.
 * @property {number} RC2D_JOYSTICK_HAT_RIGHTDOWN - Position diagonale bas-droite.
 * @property {number} RC2D_JOYSTICK_HAT_LEFTUP - Position diagonale haut-gauche.
 * @property {number} RC2D_JOYSTICK_HAT_LEFTDOWN - Position diagonale bas-gauche.
 */
typedef enum RC2D_JoystickHat {
    RC2D_JOYSTICK_HAT_CENTERED = SDL_HAT_CENTERED,
    RC2D_JOYSTICK_HAT_UP = SDL_HAT_UP,
    RC2D_JOYSTICK_HAT_RIGHT = SDL_HAT_RIGHT,
    RC2D_JOYSTICK_HAT_DOWN = SDL_HAT_DOWN,
    RC2D_JOYSTICK_HAT_LEFT = SDL_HAT_LEFT,
    RC2D_JOYSTICK_HAT_RIGHTUP = SDL_HAT_RIGHTUP,
    RC2D_JOYSTICK_HAT_RIGHTDOWN = SDL_HAT_RIGHTDOWN,
    RC2D_JOYSTICK_HAT_LEFTUP = SDL_HAT_LEFTUP,
    RC2D_JOYSTICK_HAT_LEFTDOWN = SDL_HAT_LEFTDOWN
} RC2D_JoystickHat;

/**
 * Enumérations pour les types d'entrée du joystick.
 * Indique si l'entrée est un axe analogique, un bouton digital ou une valeur de chapeau 8-directionnel.
 * @typedef {enum} RC2D_JoystickInputType
 * @property {string} RC2D_JOYSTICK_INPUT_AXIS - Axe analogique.
 * @property {string} RC2D_JOYSTICK_INPUT_BUTTON - Bouton digital.
 * @property {string} RC2D_JOYSTICK_INPUT_HAT - Valeur de chapeau 8-directionnel.
 */
typedef enum RC2D_JoystickInputType {
    RC2D_JOYSTICK_INPUT_AXIS,
    RC2D_JOYSTICK_INPUT_BUTTON, 
    RC2D_JOYSTICK_INPUT_HAT
} RC2D_JoystickInputType;

/**
 * Structure pour représenter un joystick.
 * Contient un pointeur vers le joystick SDL et l'identifiant du joystick.
 * @typedef {Object} RC2D_Joystick
 * @property {SDL_Joystick} sdlJoystick - Pointeur vers le joystick SDL.
 * @property {number} joystickId - Identifiant du joystick.
 */
typedef struct RC2D_Joystick {
    SDL_Joystick* sdlJoystick;
    int joystickId;
} RC2D_Joystick;

// Fonctions pour les joysticks
float rc2d_joystick_getAxis(const int joystickID, const int axisIndex);
int rc2d_joystick_getAxisCount(const int joystickID);

int rc2d_joystick_getButtonCount(const int joystickID);
void rc2d_joystick_getDeviceInfo(const int joystickID, int* vendorID, int* productID, int* productVersion);
int rc2d_joystick_getInstanceId(const int joystickID);
int rc2d_joystick_getJoystickCount(void);
const char* rc2d_joystick_getName(const int joystickID);

bool rc2d_joystick_isConnected(const int joystickID);
bool rc2d_joystick_isVibrationSupported(const int joystickID);

RC2D_JoystickHat rc2d_joystick_getHat(const int joystickID, const int hatIndex);
int rc2d_joystick_getHatCount(const int joystickID);

// Fonctions pour les gamepads
char* rc2d_joystick_getGamepadMappingString(const char* guidString);
void rc2d_joystick_freeGamepadMappingString(char* mapping);

// Instance de joystick
RC2D_Joystick** rc2d_joystick_getJoysticks(void);

#ifdef __cplusplus
}
#endif

#endif // RC2D_JOYSTICK_H