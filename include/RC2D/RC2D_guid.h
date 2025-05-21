#ifndef RC2D_GUID_H
#define RC2D_GUID_H

#include <SDL3/SDL_guid.h> // Required for: SDL_GUID, SDL_GUIDToString, SDL_StringToGUID

#include <stdbool.h>      // Required for: bool

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Représente un identifiant global unique (GUID) dans RC2D.
 *
 * Ce type encapsule un identifiant de 128 bits (16 octets), permettant de distinguer
 * de façon unique n'importe quelle entité dans un programme (périphérique, session, ressource, etc.).
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_GUID {
    uint8_t data[16];
} RC2D_GUID;

/**
 * \brief Taille minimale requise (en octets) pour stocker un GUID sous forme de chaîne.
 *
 * Un GUID RC2D converti en chaîne ASCII est toujours de 32 caractères + le caractère nul `\0`,
 * soit **33 octets** nécessaires.
 * 
 * \since Cette macro de préprocesseur est disponible depuis RC2D 1.0.0.
 */
#define RC2D_GUID_STRING_LENGTH 33

/**
 * \brief Convertit un `RC2D_GUID` en chaîne ASCII.
 *
 * \note Le résultat est écrit dans `buffer`. Aucun retour.
 *
 * \param {RC2D_GUID} guid - Le GUID à convertir.
 * \param {char *} buffer - Pointeur vers le buffer de destination (doit être au moins de taille `RC2D_GUID_STRING_LENGTH`).
 * 
 * \threadsafety Cette fonction peut être appelée depuis n’importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 *
 * \see rc2d_guid_fromString
 */
void rc2d_guid_toString(RC2D_GUID guid, char *buffer);

/**
 * \brief Convertit une chaîne ASCII de 32 caractères hexadécimaux en `RC2D_GUID`.
 *
 * \note Aucune vérification de validité n’est effectuée sur la chaîne.
 * 
 * \param {const char *} string - Chaîne représentant un GUID valide (ex: "030000005e0400008e02000000000000").
 * \return {RC2D_GUID} - Le GUID correspondant.
 *
 * \threadsafety Cette fonction peut être appelée depuis n’importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 *
 * \see rc2d_guid_toString
 */
RC2D_GUID rc2d_guid_fromString(const char *string);

/**
 * \brief Compare deux GUID pour vérifier s’ils sont identiques.
 *
 * \param {RC2D_GUID} a - Premier GUID à comparer.
 * \param {RC2D_GUID} b - Deuxième GUID à comparer.
 * \return {bool} - `true` si les deux GUID sont identiques, `false` sinon.
 *
 * \threadsafety Cette fonction peut être appelée depuis n’importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
bool rc2d_guid_equals(RC2D_GUID a, RC2D_GUID b);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_GUID_H
