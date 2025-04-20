#ifndef RC2D_KEYBOARD_H
#define RC2D_KEYBOARD_H

#include <RC2D/RC2D_scancode.h>
#include <RC2D/RC2D_keycode.h>

#include <stdbool.h> // Required for : bool

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Vérifie si une touche spécifique est actuellement pressée.
 *
 * \param {RC2D_KeyCode} La touche à vérifier, spécifiée en tant que RC2D_KeyCode.
 * \return {bool} true si la touche est pressée, false sinon.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_keyboard_isScancodeDown()
 */
bool rc2d_keyboard_isDown(const RC2D_Keycode key);

/**
 * \brief Vérifie si un scancode spécifique est actuellement pressé.
 *
 * \param {RC2D_Scancode} Le scancode à vérifier, spécifié en tant que RC2D_Scancode.
 * \return {bool} true si le scancode est pressé, false sinon.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_keyboard_isDown()
 */
bool rc2d_keyboard_isScancodeDown(const RC2D_Scancode scancode);

/**
 * \brief Active ou désactive la saisie de texte.
 *
 * \note Sur mobile ou tablette, elle peut faire apparaître le clavier à l’écran automatiquement.
 * La saisie de texte est désactivée par défaut.
 * 
 * \param {bool} enabled Si true, la saisie de texte est activée. Si false, elle est désactivée.
 * 
 * \threadsafety Cette fonction ne doit être appelée que sur le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_keyboard_setTextInput(const bool enabled);

/**
 * \brief Vérifiez si la plate-forme prend en charge le clavier à l'écran.
 *
 * \return true si un clavier à l'écran est supporté, false sinon.
 * 
 * \threadsafety Cette fonction ne doit être appelée que sur le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_keyboard_setTextInput()
 */
bool rc2d_keyboard_hasScreenKeyboardSupport(void);

/**
 * \brief Convertit une touche spécifiée par un RC2D_Keycode en son scancode correspondant.
 * 
 * \param {RC2D_Keycode} La touche à convertir, spécifiée en tant que RC2D_Keycode.
 * \return {RC2D_Scancode} Le scancode correspondant à la touche spécifiée.
 * 
 * \threadsafety Cette fonction ne doit être appelée que sur le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_keyboard_getKeyFromScancode()
 */
RC2D_Scancode rc2d_keyboard_getScancodeFromKey(const RC2D_Keycode key);

/**
 * \brief Convertit un scancode spécifié par un RC2D_Scancode en sa touche correspondante.
 *
 * \param {RC2D_Scancode} Le scancode à convertir, spécifié en tant que RC2D_Scancode.
 * \return {RC2D_Keycode} La touche correspondant au scancode spécifié.
 * 
 * \threadsafety Cette fonction ne doit être appelée que sur le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_keyboard_getScancodeFromKey()
 */
RC2D_Keycode rc2d_keyboard_getKeyFromScancode(const RC2D_Scancode scancode);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_KEYBOARD_H
