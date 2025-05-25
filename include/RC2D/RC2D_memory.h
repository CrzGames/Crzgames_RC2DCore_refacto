#ifndef RC2D_MEMORY_H
#define RC2D_MEMORY_H

#include <RC2D/RC2D_config.h>

#include <SDL3/SDL_stdinc.h>

#if RC2D_MEMORY_DEBUG_ENABLED
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
#define RC2D_malloc(size) rc2d_malloc_debug(size, SDL_FILE, SDL_LINE, SDL_FUNCTION)
#define RC2D_calloc(nmemb, size) rc2d_calloc_debug(nmemb, size, SDL_FILE, SDL_LINE, SDL_FUNCTION)
#define RC2D_realloc(ptr, size) rc2d_realloc_debug(ptr, size, SDL_FILE, SDL_LINE, SDL_FUNCTION)
#define RC2D_free(ptr) rc2d_free_debug(ptr, SDL_FILE, SDL_LINE, SDL_FUNCTION)
#define RC2D_strdup(str) rc2d_strdup_debug(str, SDL_FILE, SDL_LINE, SDL_FUNCTION)
#define RC2D_strndup(str, n) rc2d_strndup_debug(str, n, SDL_FILE, SDL_LINE, SDL_FUNCTION)

/**
 * \brief Alloue dynamiquement un bloc de mémoire avec suivi de débogage.
 *
 * Cette fonction alloue un bloc de mémoire de la taille spécifiée via `RC2D_malloc` et enregistre
 * les informations de débogage (fichier, ligne, fonction) si `RC2D_MEMORY_DEBUG_ENABLED` est activé.
 *
 * \param size Taille du bloc de mémoire à allouer (en octets).
 * \param file Nom du fichier source où l'allocation est effectuée.
 * \param line Numéro de ligne dans le fichier source.
 * \param func Nom de la fonction appelante.
 *
 * \return Pointeur vers le bloc de mémoire alloué, ou NULL en cas d'échec.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void* rc2d_malloc_debug(size_t size, const char* file, int line, const char* func);

/**
 * \brief Alloue et initialise un bloc de mémoire avec suivi de débogage.
 *
 * Cette fonction alloue un bloc de mémoire pour `nmemb` éléments de taille `size` via `SDL_calloc`,
 * initialise tous les octets à zéro, et enregistre les informations de débogage si
 * `RC2D_MEMORY_DEBUG_ENABLED` est activé.
 *
 * \param nmemb Nombre d'éléments à allouer.
 * \param size Taille de chaque élément (en octets).
 * \param file Nom du fichier source où l'allocation est effectuée.
 * \param line Numéro de ligne dans le fichier source.
 * \param func Nom de la fonction appelante.
 *
 * \return Pointeur vers le bloc de mémoire alloué et initialisé, ou NULL en cas d'échec.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void* rc2d_calloc_debug(size_t nmemb, size_t size, const char* file, int line, const char* func);

/**
 * \brief Réalloue un bloc de mémoire avec suivi de débogage.
 *
 * Cette fonction redimensionne un bloc de mémoire précédemment alloué via `SDL_realloc` et met à jour
 * les informations de débogage si `RC2D_MEMORY_DEBUG_ENABLED` est activé. Si `ptr` est NULL, elle
 * se comporte comme `RC2D_malloc_debug`. Si `size` est 0, elle libère le bloc.
 *
 * \param ptr Pointeur vers le bloc de mémoire à réallouer.
 * \param size Nouvelle taille du bloc de mémoire (en octets).
 * \param file Nom du fichier source où l'allocation est effectuée.
 * \param line Numéro de ligne dans le fichier source.
 * \param func Nom de la fonction appelante.
 *
 * \return Pointeur vers le bloc de mémoire réalloué, ou NULL en cas d'échec.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void* rc2d_realloc_debug(void* ptr, size_t size, const char* file, int line, const char* func);

/**
 * \brief Libère un bloc de mémoire avec suivi de débogage.
 *
 * Cette fonction libère un bloc de mémoire précédemment alloué via `RC2D_free` et supprime les
 * informations de débogage associées si `RC2D_MEMORY_DEBUG_ENABLED` est activé. Si `ptr` est NULL,
 * la fonction ne fait rien.
 *
 * \param ptr Pointeur vers le bloc de mémoire à libérer.
 * \param file Nom du fichier source où la libération est effectuée.
 * \param line Numéro de ligne dans le fichier source.
 * \param func Nom de la fonction appelante.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_free_debug(void* ptr, const char* file, int line, const char* func);

/**
 * \brief Duplique une chaîne de caractères avec suivi de débogage.
 *
 * Cette fonction alloue un nouveau bloc de mémoire pour dupliquer la chaîne `str` via `SDL_strdup`
 * et enregistre les informations de débogage si `RC2D_MEMORY_DEBUG_ENABLED` est activé.
 *
 * \param str Chaîne de caractères à dupliquer.
 * \param file Nom du fichier source où l'allocation est effectuée.
 * \param line Numéro de ligne dans le fichier source.
 * \param func Nom de la fonction appelante.
 *
 * \return Pointeur vers la nouvelle chaîne dupliquée, ou NULL en cas d'échec.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
char* rc2d_strdup_debug(const char* str, const char* file, int line, const char* func);

/**
 * \brief Duplique une portion d'une chaîne de caractères avec suivi de débogage.
 *
 * Cette fonction alloue un nouveau bloc de mémoire pour dupliquer jusqu'à `n` caractères de la chaîne
 * `str` via `SDL_strndup` et enregistre les informations de débogage si `RC2D_MEMORY_DEBUG_ENABLED`
 * est activé.
 *
 * \param str Chaîne de caractères à dupliquer.
 * \param n Nombre maximum de caractères à dupliquer.
 * \param file Nom du fichier source où l'allocation est effectuée.
 * \param line Numéro de ligne dans le fichier source.
 * \param func Nom de la fonction appelante.
 *
 * \return Pointeur vers la nouvelle chaîne dupliquée, ou NULL en cas d'échec.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
char* rc2d_strndup_debug(const char* str, size_t n, const char* file, int line, const char* func);
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
void rc2d_memory_report(void);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_MEMORY_H