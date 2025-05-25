#include <RC2D/RC2D_memory.h>
#include <RC2D/RC2D_logger.h>

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

void* rc2d_malloc_debug(size_t size, const char* file, int line, const char* func) 
{
    void* ptr = SDL_malloc(size);
    if (ptr) 
    {
        add_allocation(ptr, size, file, line, func);
    }

    return ptr;
}

void* rc2d_calloc_debug(size_t nmemb, size_t size, const char* file, int line, const char* func) 
{
    void* ptr = SDL_calloc(nmemb, size);
    if (ptr) 
    {
        add_allocation(ptr, nmemb * size, file, line, func);
    }

    return ptr;
}

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

void rc2d_free_debug(void* ptr, const char* file, int line, const char* func) 
{
    if (ptr) 
    {
        remove_allocation(ptr);
        SDL_free(ptr);
    }
}

char* rc2d_strdup_debug(const char* str, const char* file, int line, const char* func) 
{
    char* ptr = SDL_strdup(str);
    if (ptr) 
    {
        add_allocation(ptr, strlen(str) + 1, file, line, func);
    }

    return ptr;
}

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