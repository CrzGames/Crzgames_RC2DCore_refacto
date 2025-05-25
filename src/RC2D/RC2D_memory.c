#include <RC2D/RC2D_memory.h>
#include <RC2D/RC2D_logger.h>
#include <SDL3/SDL_stdinc.h>

#if RC2D_MEMORY_DEBUG_ENABLED

/* Structure pour stocker les informations d'une allocation */
typedef struct Allocation {
    void* ptr;              /* Pointeur alloué */
    size_t size;            /* Taille de l'allocation */
    const char* file;       /* Fichier source */
    int line;               /* Ligne dans le fichier */
    const char* func;       /* Fonction appelante */
    struct Allocation* next; /* Prochaine allocation */
} Allocation;

/* Liste chaînée des allocations */
static Allocation* allocations = NULL;
static int initialized = 0;

/* Ajouter une allocation à la liste */
static void add_allocation(void* ptr, size_t size, const char* file, int line, const char* func) 
{
    if (!ptr) return;

    Allocation* alloc = (Allocation*)SDL_malloc(sizeof(Allocation));
    if (!alloc) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'allouer la structure Allocation");
        return;
    }

    alloc->ptr = ptr;
    alloc->size = size;
    alloc->file = file;
    alloc->line = line;
    alloc->func = func;
    alloc->next = allocations;
    allocations = alloc;
}

/* Supprimer une allocation de la liste */
static void remove_allocation(void* ptr) 
{
    Allocation* current = allocations;
    Allocation* prev = NULL;

    while (current) 
    {
        if (current->ptr == ptr) 
        {
            if (prev) 
            {
                prev->next = current->next;
            } 
            else 
            {
                allocations = current->next;
            }

            /* Libérer la mémoire de l'allocation */
            SDL_free(current);
            return;
        
        }
        prev = current;
        current = current->next;
    }
}

/**
 * \brief Alloue dynamiquement un bloc de mémoire avec suivi de débogage.
 *
 * Cette fonction alloue un bloc de mémoire de la taille spécifiée via `SDL_malloc` et enregistre
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
void* rc2d_malloc_debug(size_t size, const char* file, int line, const char* func) 
{
    void* ptr = SDL_malloc(size);
    if (ptr) 
    {
        add_allocation(ptr, size, file, line, func);
    }

    return ptr;
}

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
void* rc2d_calloc_debug(size_t nmemb, size_t size, const char* file, int line, const char* func) 
{
    void* ptr = SDL_calloc(nmemb, size);
    if (ptr) 
    {
        add_allocation(ptr, nmemb * size, file, line, func);
    }

    return ptr;
}

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
void* rc2d_realloc_debug(void* ptr, size_t size, const char* file, int line, const char* func) 
{
    if (ptr) 
    {
        remove_allocation(ptr);
    }

    void* new_ptr = SDL_realloc(ptr, size);
    if (new_ptr) 
    {
        add_allocation(new_ptr, size, file, line, func);
    }

    return new_ptr;
}

/**
 * \brief Libère un bloc de mémoire avec suivi de débogage.
 *
 * Cette fonction libère un bloc de mémoire précédemment alloué via `SDL_free` et supprime les
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
void rc2d_free_debug(void* ptr, const char* file, int line, const char* func) 
{
    if (ptr) 
    {
        remove_allocation(ptr);
        SDL_free(ptr);
    }
}

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
char* rc2d_strdup_debug(const char* str, const char* file, int line, const char* func) 
{
    char* ptr = SDL_strdup(str);
    if (ptr) 
    {
        add_allocation(ptr, strlen(str) + 1, file, line, func);
    }

    return ptr;
}

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
char* rc2d_strndup_debug(const char* str, size_t n, const char* file, int line, const char* func) 
{
    char* ptr = SDL_strndup(str, n);
    if (ptr) 
    {
        add_allocation(ptr, strlen(ptr) + 1, file, line, func);
    }

    return ptr;
}

#endif /* RC2D_MEMORY_DEBUG_ENABLED */

/* Afficher le rapport des fuites mémoire */
void rc2d_memory_report(void) 
{
#if RC2D_MEMORY_DEBUG_ENABLED
    if (!allocations) 
    {
        RC2D_log(RC2D_LOG_INFO, "RC2D Memory: Aucune fuite mémoire détectée.");
        return;
    }

    size_t total_leaked = 0;
    int leak_count = 0;

    RC2D_log(RC2D_LOG_ERROR, "RC2D Memory: Rapport des fuites mémoire:");
    RC2D_log(RC2D_LOG_ERROR, "----------------------------------------");

    Allocation* current = allocations;
    while (current) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Fuite: %p, Taille: %zu octets, Fichier: %s, Ligne: %d, Fonction: %s",
                 current->ptr, current->size, current->file, current->line, current->func);
        total_leaked += current->size;
        leak_count++;
        current = current->next;
    }

    RC2D_log(RC2D_LOG_ERROR, "----------------------------------------");
    RC2D_log(RC2D_LOG_ERROR, "Total: %d fuites, %zu octets non libérés", leak_count, total_leaked);

    /* Libérer la liste des allocations */
    while (allocations) 
    {
        Allocation* temp = allocations;
        allocations = allocations->next;
        SDL_free(temp);
    }
#else
    /* Ne rien faire si le suivi de mémoire est désactivé */
#endif
}