#ifndef RC2D_KEYBOARD_H
#define RC2D_KEYBOARD_H

#include "rc2d/rc2d_keycode_and_scancode.h" // Required for : RC2D_KeyCode, RC2D_Scancode
#include <stdbool.h> // Required for : bool

#ifdef __cplusplus
extern "C" {
#endif

bool rc2d_keyboard_isDown(const RC2D_KeyCode key);
bool rc2d_keyboard_isScancodeDown(const RC2D_Scancode scancode);

void rc2d_keyboard_setTextInput(const bool enabled);

bool rc2d_keyboard_hasScreenKeyboard(void);

RC2D_Scancode rc2d_keyboard_getScancodeFromKey(const RC2D_KeyCode key);
RC2D_KeyCode rc2d_keyboard_getKeyFromScancode(const RC2D_Scancode scancode);

#ifdef __cplusplus
}
#endif

#endif // RC2D_KEYBOARD_H
