#ifndef RC2D_VERSION_H
#define RC2D_VERSION_H

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Version majeure de la bibliothèque RC2D.
 * 
 * \since Cette macro est disponible depuis RC2D 1.0.0.
 * 
 * \see RC2D_VERSION_MINOR
 * \see RC2D_VERSION_PATCH
 */
#define RC2D_VERSION_MAJOR 1

/**
 * \brief Version mineure de la bibliothèque RC2D.
 * 
 * \since Cette macro est disponible depuis RC2D 1.0.0.
 * 
 * \see RC2D_VERSION_MAJOR
 * \see RC2D_VERSION_PATCH
 */
#define RC2D_VERSION_MINOR 0

/**
 * \brief Version de correction de la bibliothèque RC2D.
 * 
 * \since Cette macro est disponible depuis RC2D 1.0.0.
 * 
 * \see RC2D_VERSION_MAJOR
 * \see RC2D_VERSION_MINOR
 */
#define RC2D_VERSION_PATCH 0

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_VERSION_H