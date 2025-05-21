#ifndef RC2D_PLATFORM_H
#define RC2D_PLATFORM_H

#include <RC2D/RC2D_platform_defines.h>

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Retourne une chaîne identifiant la plateforme de compilation actuelle.
 * 
 * \details Cette fonction utilise les macros de préprocesseur RC2D_PLATFORM_* pour détecter 
 * la plateforme exacte.
 * 
 * \return Chaîne constante représentant le nom du système (ex: "Windows", "Xbox Series", "visionOS", etc.)
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
const char* rc2d_system_getPlatform(void);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_PLATFORM_DEFINES_H
