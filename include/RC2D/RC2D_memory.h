#ifndef RC2D_MEMORY_H
#define RC2D_MEMORY_H

#include <RC2D/RC2D_config.h>

#include <SDL3/SDL_stdinc.h>

/**
 * \brief Macros de préprocesseur pour les fonctions de gestion de mémoire.
 *
 * Ces macros redirigent les appels vers les fonctions de débogage (`RC2D_malloc_debug`, etc.) si
 * `RC2D_MEMORY_DEBUG_ENABLED` est défini à 1, ou vers les fonctions SDL standard (`SDL_malloc`, etc.)
 * sinon. Elles garantissent que les informations de débogage (fichier, ligne, fonction) sont capturées
 * automatiquement lors des allocations lorsque le suivi est activé.
 *
 * \note Utilisez ces macros (`RC2D_malloc`, `RC2D_free`, etc.) dans votre code au lieu des fonctions SDL
 * standard ou C standard pour bénéficier du suivi des fuites mémoire lorsque `RC2D_MEMORY_DEBUG_ENABLED` est activé.
 *
 * \since Ces macros sont disponibles depuis RC2D 1.0.0.
 */
#if RC2D_MEMORY_DEBUG_ENABLED
#define RC2D_malloc(size) RC2D_malloc_debug(size, __FILE__, __LINE__, __FUNCTION__)
#define RC2D_calloc(nmemb, size) RC2D_calloc_debug(nmemb, size, __FILE__, __LINE__, __FUNCTION__)
#define RC2D_realloc(ptr, size) RC2D_realloc_debug(ptr, size, __FILE__, __LINE__, __FUNCTION__)
#define RC2D_free(ptr) RC2D_free_debug(ptr, __FILE__, __LINE__, __FUNCTION__)
#define RC2D_strdup(str) RC2D_strdup_debug(str, __FILE__, __LINE__, __FUNCTION__)
#define RC2D_strndup(str, n) RC2D_strndup_debug(str, n, __FILE__, __LINE__, __FUNCTION__)
#else
#define RC2D_malloc(size) SDL_malloc(size)
#define RC2D_calloc(nmemb, size) SDL_calloc(nmemb, size)
#define RC2D_realloc(ptr, size) SDL_realloc(ptr, size)
#define RC2D_free(ptr) SDL_free(ptr)
#define RC2D_strdup(str) SDL_strdup(str)
#define RC2D_strndup(str, n) SDL_strndup(str, n)
#endif

/**
 * \brief Affiche un rapport des fuites mémoire détectées.
 *
 * Cette fonction génère un rapport détaillé des blocs de mémoire alloués via les fonctions RC2D
 * (`RC2D_malloc`, etc.) qui n'ont pas été libérés. Le rapport inclut l'adresse du pointeur, la taille
 * allouée, le fichier source, la ligne, et la fonction où l'allocation a été effectuée. Si aucune fuite
 * n'est détectée, un message indiquant l'absence de fuites est affiché.
 *
 * Le rapport est automatiquement généré à la fin de l'exécution du programme si
 * `RC2D_MEMORY_DEBUG_ENABLED` est activé, mais cette fonction peut être appelée manuellement pour
 * inspecter les fuites à un moment précis.
 *
 * \note Cette fonction n'a aucun effet si `RC2D_MEMORY_DEBUG_ENABLED` est défini à 0.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void RC2D_memory_report(void);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_MEMORY_H