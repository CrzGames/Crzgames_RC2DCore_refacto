#include "rc2d/rc2d_logger.h"

#include <SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

static RC2D_LogLevel currentLogLevel = RC2D_LOG_DEBUG; // Default log level

/**
 * Définit le niveau de priorité des messages de log.
 *
 * Cette fonction ajuste le niveau de priorité global pour les messages de log,
 * permettant de filtrer les messages moins importants selon le niveau spécifié.
 * Les messages ayant un niveau de priorité inférieur au niveau spécifié seront ignorés.
 *
 * @param logLevel Le niveau de log à définir, déterminant quels messages doivent être affichés.
 */
void rc2d_log_set_priority(const RC2D_LogLevel logLevel) 
{
    // Enregistre le niveau de log actuel
    currentLogLevel = logLevel;

    // Convertit le niveau de log de RC2D en priorite SDL correspondante.
    SDL_LogPriority logPriority;
    switch(logLevel) 
    {
        case RC2D_LOG_DEBUG: logPriority = SDL_LOG_PRIORITY_DEBUG; break;
        case RC2D_LOG_INFO: logPriority = SDL_LOG_PRIORITY_INFO; break;
        case RC2D_LOG_WARN: logPriority = SDL_LOG_PRIORITY_WARN; break;
        case RC2D_LOG_ERROR: logPriority = SDL_LOG_PRIORITY_ERROR; break;
        case RC2D_LOG_CRITICAL: logPriority = SDL_LOG_PRIORITY_CRITICAL; break;
    }

    // Definit la priorite de log pour toutes les categories
    SDL_LogSetAllPriority(logPriority);
}

/**
 * Affiche un message de log selon le format et les arguments spécifiés.
 *
 * Cette fonction affiche un message de log, en utilisant le formatage printf,
 * si son niveau de priorité est supérieur ou égal au niveau de log actuel.
 *
 * @param logLevel Le niveau de priorité du message.
 * @param format Le format du message, suivant la syntaxe de printf.
 * @param ... Les arguments à insérer dans le format du message.
 */
void rc2d_log(const RC2D_LogLevel logLevel, const char* format, ...) 
{
    if (logLevel < currentLogLevel) return;

    va_list args;
    va_start(args, format);

#ifdef __EMSCRIPTEN__
    // Convertir le format et les arguments en une chaîne
    char message[256];
    vsnprintf(message, sizeof(message), format, args);

    // Choisir la fonction de console appropriée en fonction du niveau de log
    switch (logLevel)
    {
        case RC2D_LOG_DEBUG:
            // Utiliser console.debug pour les messages de débogage
            emscripten_log(EM_LOG_CONSOLE | EM_LOG_NO_PATHS, "%s", message);
            break;
        case RC2D_LOG_INFO:
            // Utiliser console.info pour les messages d'information
            emscripten_log(EM_LOG_CONSOLE | EM_LOG_NO_PATHS, "%s", message);
            break;
        case RC2D_LOG_WARN:
            // Utiliser console.warn pour les avertissements
            emscripten_log(EM_LOG_WARN | EM_LOG_NO_PATHS, "%s", message);
            break;
        case RC2D_LOG_ERROR:
            // Utiliser console.error pour les erreurs
            emscripten_log(EM_LOG_ERROR | EM_LOG_NO_PATHS, "%s", message);
            break;
        case RC2D_LOG_CRITICAL:
            // Utiliser console.error pour les erreurs critiques aussi, ou envisager une stratégie différente si nécessaire
            emscripten_log(EM_LOG_ERROR | EM_LOG_NO_PATHS, "%s", message);
            break;
    }
#else
    // Convertit le niveau de log de RC2D en priorite SDL correspondante.
    SDL_LogPriority logPriority;
    switch(logLevel) 
    {
        case RC2D_LOG_DEBUG: logPriority = SDL_LOG_PRIORITY_DEBUG; break;
        case RC2D_LOG_INFO: logPriority = SDL_LOG_PRIORITY_INFO; break;
        case RC2D_LOG_WARN: logPriority = SDL_LOG_PRIORITY_WARN; break;
        case RC2D_LOG_ERROR: logPriority = SDL_LOG_PRIORITY_ERROR; break;
        case RC2D_LOG_CRITICAL: logPriority = SDL_LOG_PRIORITY_CRITICAL; break;
    }

    // Utiliser SDL_LogMessageV pour les plateformes natives
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, logPriority, format, args);
#endif

    va_end(args);
}