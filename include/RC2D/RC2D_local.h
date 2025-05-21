#ifndef RC2D_LOCAL_H
#define RC2D_LOCAL_H

#include <SDL3/SDL_locale.h>  // Required for : SDL_Locale, SDL_GetPreferredLocales
#include <stddef.h>           // Required for : size_t

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Structure représentant une langue locale préférée de l'utilisateur.
 * 
 * Chaque locale combine un code de langue (ex : "fr") et éventuellement un code pays (ex : "FR").
 * Cela peut être utilisé pour adapter dynamiquement les textes, unités ou formats de date/heure à l'utilisateur.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_Locale {
    /**
     * \brief Code de langue selon la norme ISO-639 (ex : "fr", "en", "ja").
     */
    const char *language;

    /**
     * \brief Code de pays selon la norme ISO-3166 (ex : "FR", "US", "JP").
     * Peut être NULL si aucune préférence pays n'est définie.
     */
    const char *country;
} RC2D_Locale;

/**
 * \brief Récupère la liste des locales préférées de l'utilisateur.
 *
 * \note Elle est allouée dynamiquement et doit être libérée avec `rc2d_local_freeLocales()` une fois inutilisée.
 *
 * \return Un tableau de `RC2D_Locale` contenant les locales préférées, ou NULL en cas d'erreur.
 *
 * \threadsafety Cette fonction doit être appelée depuis le thread principal.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 *
 * \see rc2d_local_freeLocales
 */
RC2D_Locale *rc2d_local_getPreferredLocales(void);

/**
 * \brief Libère une liste de locales précédemment allouée avec `rc2d_local_getPreferredLocales()`.
 *
 * \param locales Pointeur vers le tableau à libérer. Ne rien faire si NULL.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 *
 * \see rc2d_local_getPreferredLocales
 */
void rc2d_local_freeLocales(RC2D_Locale *locales);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_LOCAL_H
