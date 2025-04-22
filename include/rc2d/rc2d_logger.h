#ifndef RC2D_LOGGER_H
#define RC2D_LOGGER_H

#include <RC2D/RC2D_assert.h>

#include <SDL3/SDL_stdinc.h>

#include <stdarg.h> // Required for : ... (va_list, va_start, va_end)

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Macro pour afficher un message de log avec le niveau de priorité spécifié.
 * 
 * Exemple d'utilisation :
 * RC2D_log(RC2D_LOG_INFO, "La propriété GPU est NULL !");
 *
 * Exemple d'affichage :
 * [info:rc2d_gpu.c:42:rc2d_gpu_getInfo] La propriété GPU est NULL !
 * 
 * \param {RC2D_LogLevel} level - Le niveau de priorité du message.
 * \param {const char*} format - Le format du message, suivant la syntaxe de printf.
 * \param {...} - Les arguments à insérer dans le format du message.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette macro est disponible depuis RC2D 1.0.0.
 */
#define RC2D_log(level, format, ...) \
    rc2d_log((level), SDL_FILE, SDL_LINE, SDL_FUNCTION, (format), ##__VA_ARGS__)

/**
 * \brief Cette enum est utilisée pour définir le niveau de priorité des messages de log.
 * 
 * \since Cette enum est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_LogLevel {
    /**
     * Messages de débogage.
     */
    RC2D_LOG_DEBUG,

    /**
     * Messages informatifs.
     */
    RC2D_LOG_INFO,
    
    /**
     * Messages d'avertissement.
     */
    RC2D_LOG_WARN,
    
    /**
     * Messages d'erreur.
     */
    RC2D_LOG_ERROR,
    
    /**
     * Messages d'erreur critique.
     */
    RC2D_LOG_CRITICAL   
} RC2D_LogLevel;

/**
 * \brief Récupère le niveau de priorité actuel pour les logs RC2D.
 *
 * \return Le niveau de priorité courant.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
RC2D_LogLevel rc2d_log_get_priority(void);

/**
 * \brief Définit le niveau de priorité des messages de log.
 *
 * Cette fonction ajuste le niveau de priorité global pour les messages de log,
 * permettant de filtrer les messages moins importants selon le niveau spécifié.
 * Les messages ayant un niveau de priorité inférieur au niveau spécifié seront ignorés.
 * 
 * \param {RC2D_LogLevel} logLevel - Le niveau de log à définir.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_log_set_priority(const RC2D_LogLevel logLevel);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_LOGGER_H
