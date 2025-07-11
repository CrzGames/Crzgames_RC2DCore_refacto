#include <RC2D/RC2D_local.h>
#include <RC2D/RC2D_logger.h> // Required for : RC2D_log
#include <RC2D/RC2D_memory.h>

#include <SDL3/SDL_stdinc.h> // Required for : SDL_malloc, SDL_free

RC2D_Locale *rc2d_local_getPreferredLocales(void)
{
    // Récupère la liste des locales préférées de l'utilisateur
    int sdl_count = 0;
    SDL_Locale **sdl_locales = SDL_GetPreferredLocales(&sdl_count);

    // Vérifie si la liste est valide et si le nombre de locales est positif
    if (!sdl_locales || sdl_count <= 0) 
    {
        RC2D_log(RC2D_LOG_WARN, "rc2d_local_getPreferredLocales : Aucune locale détectée ou erreur SDL : %s", SDL_GetError());
        return NULL;
    }

    // Alloue de la mémoire pour le tableau de locales RC2D
    RC2D_Locale *result = (RC2D_Locale *)RC2D_malloc(sizeof(RC2D_Locale) * sdl_count);
    if (!result) 
    {
        RC2D_log(RC2D_LOG_ERROR, "rc2d_local_getPreferredLocales : Échec d'allocation mémoire.");
        RC2D_safe_free(sdl_locales);
        return NULL;
    }

    // Remplit le tableau de locales RC2D avec les données de SDL
    for (int i = 0; i < sdl_count; ++i) 
    {
        result[i].language = sdl_locales[i]->language;
        result[i].country = sdl_locales[i]->country;
    }

    // Libère le tableau d'origine de SDL
    RC2D_safe_free(sdl_locales);

    // Retourne le tableau de locales RC2D
    return result;
}

void rc2d_local_freeLocales(RC2D_Locale *locales)
{
    RC2D_safe_free(locales);
}
