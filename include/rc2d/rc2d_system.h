#ifndef RC2D_SYSTEM_H
#define RC2D_SYSTEM_H

#include <stdbool.h> // Required for : bool

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumération représentant les différents états de l'alimentation.
 * @typedef {enum} RC2D_PowerState
 * @property {number} RC2D_POWERSTATE_UNKNOWN - Impossible de déterminer l'état de l'alimentation
 * @property {number} RC2D_POWERSTATE_ON_BATTERY - Non branché, fonctionne sur batterie
 * @property {number} RC2D_POWERSTATE_NO_BATTERY - Branché, pas de batterie disponible
 * @property {number} RC2D_POWERSTATE_CHARGING - Branché, en train de charger la batterie
 * @property {number} RC2D_POWERSTATE_CHARGED - Branché, batterie complètement chargée
 */
typedef enum RC2D_PowerState {
	RC2D_POWERSTATE_UNKNOWN,
 	RC2D_POWERSTATE_ON_BATTERY,
	RC2D_POWERSTATE_NO_BATTERY,
	RC2D_POWERSTATE_CHARGING,
	RC2D_POWERSTATE_CHARGED
} RC2D_PowerState;

bool rc2d_system_hasClipboardText(void);
char *rc2d_system_getClipboardText(void); // Doit etre libere avec rc2d_system_freeClipboardText
void rc2d_system_setClipboardText(const char* text);
void rc2d_system_freeClipboardText(char* text);

void rc2d_system_openURL(const char* url);
const char* rc2d_system_getOS(void);
int rc2d_system_getProcessorCount(void);
int rc2d_system_getRAM(void);

void rc2d_system_vibrate(const double seconds, const float strength);

#ifdef __cplusplus
}
#endif

#endif // RC2D_SYSTEM_H