#ifndef RC2D_POWER_H
#define RC2D_POWER_H

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumération représentant les différents états de l'alimentation.
 * @typedef {enum} RC2D_PowerState
 * @property {number} RC2D_POWERSTATE_ERROR - En cas de panne
 * @property {number} RC2D_POWERSTATE_UNKNOWN - Impossible de déterminer l'état de l'alimentation
 * @property {number} RC2D_POWERSTATE_ON_BATTERY - Non branché, fonctionne sur batterie
 * @property {number} RC2D_POWERSTATE_NO_BATTERY - Branché, pas de batterie disponible
 * @property {number} RC2D_POWERSTATE_CHARGING - Branché, en train de charger la batterie
 * @property {number} RC2D_POWERSTATE_CHARGED - Branché, batterie complètement chargée
 */
typedef enum RC2D_PowerState {
	RC2D_POWERSTATE_ERROR,
	RC2D_POWERSTATE_UNKNOWN,
 	RC2D_POWERSTATE_ON_BATTERY,
	RC2D_POWERSTATE_NO_BATTERY,
	RC2D_POWERSTATE_CHARGING,
	RC2D_POWERSTATE_CHARGED
} RC2D_PowerState;


typedef struct RC2D_PowerInfo {
	RC2D_PowerState state;
	int batteryLevel; // En pourcentage (0-100)
	int batteryTimeSeconds; // Temps restant en secondes
} RC2D_PowerInfo;

/**
 * \brief Obtient l'état actuel de l'alimentation du système et les informations sur la batterie.
 * 
 * \return {RC2D_PowerInfo} Une structure contenant l'état de l'alimentation, le niveau de batterie et le temps restant.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
RC2D_PowerInfo rc2d_system_getPowerInfo(void);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_POWER_H