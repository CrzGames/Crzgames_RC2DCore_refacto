#ifndef RC2D_TOUCH_H
#define RC2D_TOUCH_H

#include <SDL3/SDL_touch.h>

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Données d'un point de contact tactile.
 * 
 * Chaque point représente un doigt ou un stylet actif sur un appareil tactile donné.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_TouchPoint {
    SDL_FingerID fingerID; /**< Identifiant du doigt */
    SDL_TouchID deviceID;  /**< Identifiant de l'appareil tactile */
    float x;               /**< Position X normalisée (0.0 - 1.0) */
    float y;               /**< Position Y normalisée (0.0 - 1.0) */
    float pressure;        /**< Pression normalisée (0.0 - 1.0) */
} RC2D_TouchPoint;

/**
 * \brief Etat tactile global.
 * 
 * Conserve tous les contacts tactiles actifs dans l'application.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_TouchState {
    RC2D_TouchPoint* touches; /**< Liste dynamique de points de contact */
    int numTouches;           /**< Nombre total de contacts actifs */
} RC2D_TouchState;

/**
 * \brief Met à jour l'état tactile avec un nouvel événement SDL.
 * 
 * \param deviceID Identifiant du périphérique tactile.
 * \param fingerID Identifiant du doigt.
 * \param eventType Type d'événement (DOWN, MOTION, UP).
 * \param pressure Pression normale.
 * \param x Position X normalisée.
 * \param y Position Y normalisée.
 * 
 * \since Disponible depuis RC2D 1.0.0.
 */
void rc2d_touch_updateState(SDL_TouchID deviceID, SDL_FingerID fingerID, int eventType, float pressure, float x, float y);

/**
 * \brief Libère tous les contacts tactiles actifs.
 * 
 * \since Disponible depuis RC2D 1.0.0.
 */
void rc2d_touch_freeTouchState(void);

/**
 * \brief Récupère les coordonnées (en pixels) d'un contact actif.
 * 
 * \param fingerID Identifiant du doigt.
 * \param x Pointeur vers X à remplir.
 * \param y Pointeur vers Y à remplir.
 * 
 * \since Disponible depuis RC2D 1.0.0.
 */
void rc2d_touch_getPosition(SDL_FingerID fingerID, float* x, float* y);

/**
 * \brief Récupère la pression d'un contact actif.
 * 
 * \param fingerID Identifiant du doigt.
 * \return Pression entre 0.0 et 1.0 (ou 1.0 si inconnu).
 * 
 * \since Disponible depuis RC2D 1.0.0.
 */
float rc2d_touch_getPressure(SDL_FingerID fingerID);

/**
 * \brief Récupère la liste des identifiants des doigts actifs.
 * 
 * \return {SDL_FingerID*} Tableau dynamique d'identifiants de doigts actifs, ou NULL si aucun doigt n'est actif.
 * 
 * \warning Le tableau retourné doit être libéré par l'appelant avec `rc2d_touch_freeTouches` lorsque les identifiants ne sont plus nécessaires.
 * 
 * \since Disponible depuis RC2D 1.0.0.
 */
SDL_FingerID* rc2d_touch_getTouches(void);

/**
 * \brief Libère un tableau obtenu via rc2d_touch_getTouches.
 * 
 * \param touches Tableau à libérer.
 * 
 * \since Disponible depuis RC2D 1.0.0.
 */
void rc2d_touch_freeTouches(SDL_FingerID* touches);

#ifdef __cplusplus
};
#endif

#endif // RC2D_TOUCH_H