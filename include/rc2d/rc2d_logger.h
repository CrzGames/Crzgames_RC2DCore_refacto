#ifndef RC2D_LOGGER_H
#define RC2D_LOGGER_H

#include <stdarg.h> // Required for : ... (va_list, va_start, va_end, vsnprintf)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enum définissant les niveaux de journalisation personnalisés.
 * @typedef {enum} RC2D_LogLevel
 * @property {number} RC2D_LOG_DEBUG - Messages de débogage.
 * @property {number} RC2D_LOG_INFO - Messages informatifs.
 * @property {number} RC2D_LOG_WARN - Messages d'avertissement.
 * @property {number} RC2D_LOG_ERROR - Messages d'erreur.
 * @property {number} RC2D_LOG_CRITICAL - Messages d'erreur critique.
 */
typedef enum RC2D_LogLevel {
    RC2D_LOG_DEBUG,    
    RC2D_LOG_INFO,      
    RC2D_LOG_WARN,      
    RC2D_LOG_ERROR,    
    RC2D_LOG_CRITICAL   
} RC2D_LogLevel;

void rc2d_log_set_priority(const RC2D_LogLevel logLevel);
void rc2d_log(const RC2D_LogLevel logLevel, const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif // RC2D_LOGGER_H
