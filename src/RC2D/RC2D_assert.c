#include <RC2D/RC2D_assert.h>

/**
 * Gestionnaire d'assertions personnalisé pour RC2D
 * 
 * Ce gestionnaire est appelé lorsque des assertions échouent.
 * 
 * Il vérifie la variable d'environnement RC2D_ASSERT pour déterminer le comportement à adopter.
 * 
 * \threadsafety Il est possible d'appeler cette fonction en toute sécurité à partir de n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static SDL_AssertState rc2d_assert_assertionHandler(const SDL_AssertData *data, void *userdata) 
{
    /**
     * Vérifie si la variable d'environnement RC2D_ASSERT est définie.     * 
     * Cela est utile pour le débogage et le développement.
     */
    const char *assert_env = SDL_getenv("RC2D_ASSERT");
    if (assert_env != NULL) 
    {
        if (SDL_strcasecmp(assert_env, "abort") == 0) 
        {
            return SDL_ASSERTION_ABORT;
        } 
        else if (SDL_strcasecmp(assert_env, "break") == 0) 
        {
            SDL_TriggerBreakpoint();
            return SDL_ASSERTION_BREAK;
        } 
        else if (SDL_strcasecmp(assert_env, "retry") == 0) 
        {
            return SDL_ASSERTION_RETRY;
        } 
        else if (SDL_strcasecmp(assert_env, "ignore") == 0) 
        {
            return SDL_ASSERTION_IGNORE;
        } 
        else if (SDL_strcasecmp(assert_env, "always_ignore") == 0) 
        {
            return SDL_ASSERTION_ALWAYS_IGNORE;
        }
    }

    // Appeler le gestionnaire par défaut pour boîte de dialogue (desktop)
    return SDL_GetDefaultAssertionHandler()(data, userdata);
}

void rc2d_assert_init(void) 
{
    SDL_SetAssertionHandler(rc2d_assert_assertionHandler, NULL);
}