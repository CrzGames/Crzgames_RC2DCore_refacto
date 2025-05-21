#ifndef RC2D_SYSTEM_H
#define RC2D_SYSTEM_H

#include <stdbool.h> // Required for : bool

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumération représentant les différents types de conteneurs de sandboxing.
 * 
 * \since Cette enum est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_Sandbox {
	/**
	 * Aucun conteneur de sandboxing.
	 */
	RC2D_SANDBOX_NONE = 0,

	/**
	 * Conteneur de sandboxing de type AppImage.
	 */
	RC2D_SANDBOX_UNKNOWN_CONTAINER,

	/**
	 * Conteneur de sandboxing de type Flatpak.
	 */
	RC2D_SANDBOX_FLATPAK,

	/**
	 * Conteneur de sandboxing de type Snap.
	 */
	RC2D_SANDBOX_SNAP,

	/**
	 * Conteneur de sandboxing de type macOS. 
	 */
	RC2D_SANDBOX_MACOS,
} RC2D_Sandbox;

/**
 * \brief Vérifie si du texte est actuellement stocké dans le presse-papiers.
 * 
 * \return Retourne vrai si du texte est présent, faux sinon.
 * 
 * \threadsafety Cette fonction ne doit être appelée que sur le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
bool rc2d_system_hasClipboardText(void);

/**
 * \brief Récupère le texte actuellement stocké dans le presse-papiers du système.
 * 
 * \return Un pointeur vers une chaîne de caractères contenant le texte. 
 * 
 * \note Doit être libéré avec rc2d_system_freeClipboardText().
 * 
 * \threadsafety Cette fonction ne doit être appelée que sur le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
char *rc2d_system_getClipboardText(void);

/**
 * \brief Place une chaîne de caractères dans le presse-papiers du système.
 * 
 * \param {const char*} text - Le texte à copier dans le presse-papiers.
 * 
 * \threadsafety Cette fonction ne doit être appelée que sur le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_system_setClipboardText(const char* text);

/**
 * \brief Libère la mémoire allouée par SDL_GetClipboardText().
 * 
 * \param {char*} text - Le texte à libérer.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_system_freeClipboardText(char* text);

/**
 * \brief Ouvre une URL dans le navigateur web par défaut de l'utilisateur 
 * ou dans une autre application externe appropriée.
 * 
 * \param {const char*} url - L'URL à ouvrir.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_system_openURL(const char* url);

/**
 * \brief Obtenez le nombre de cœurs de processeur logiques disponibles.
 *
 * \return {int} Renvoie le nombre total de cœurs logiques du processeur. Sur les processeurs intégrant des technologies 
 * telles que l'hyperthreading, le nombre de cœurs logiques peut être supérieur au nombre de cœurs physiques.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *  
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
int rc2d_system_getNumLogicalCPUCores(void);

/**
 * \brief Obtient la quantité de RAM système installée en Mo.
 * 
 * \return {int} La quantité de RAM système installée en Mo.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
int rc2d_system_getRAM(void);

/**
 * \brief Demander si l'appareil actuel est une tablette.
 * 
 * \return {bool} Retourne vrai si l'appareil est une tablette, faux sinon.
 * 
 * \note Si RC2D ne peut pas déterminer cela, il renverra false.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
bool rc2d_system_isTablet(void);

/**
 * \brief Demander si l'appareil actuel est un téléviseur.
 * 
 * \return {bool} Retourne vrai si l'appareil est un téléviseur, faux sinon.
 * 
 * \note Si RC2D ne peut pas déterminer cela, il renverra false.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
bool rc2d_system_isTV(void);

/**
 * \brief Obtient le type de sandbox de l'application, s'il y en a un.
 * 
 * \return {RC2D_Sandbox} Le type de sandbox de l'application.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
RC2D_Sandbox rc2d_system_getSandbox(void);

/**
 * \brief Joue une vibration sur un appareil haptique.
 * 
 * \param {double} seconds - La durée de la vibration en secondes.
 * \param {float} strength - La force de la vibration (entre 0.0 et 1.0).
 * 
 * \threadsafety Cette fonction doit être appelée depuis le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_system_vibrate(const double seconds, const float strength);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_SYSTEM_H