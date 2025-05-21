#ifndef RC2D_TIME_H
#define RC2D_TIME_H

#include <stdbool.h> // Required for : bool

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Structure représentant une date et une heure décomposées en éléments humains.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_DateTime {
    /**
     * Année complète (ex: 2025)
     */
    int year;

    /**
     * Mois de l'année (1-12)
     */
    int month;

    /**
     * Jour du mois (1-31)
     */
    int day;

    /**
     * Heure de la journée (0-23)
     */
    int hour;

    /**
     * Minute de l'heure (0-59)
     */
    int minute;

    /**
     * Seconde de la minute (0-60, bissextile)
     */
    int second;

    /**
     * Nanoseconde [0-999999999]
     */
    int nanosecond;

    /**
     * Jour de la semaine [0-6] (0 = dimanche)
     */
    int day_of_week;

    /**
     * Décalage en secondes par rapport à UTC.
     */
    int utc_offset;
} RC2D_DateTime;

/**
 * \brief Enum du format de date préféré de l'utilisateur.
 *
 * Ces valeurs représentent les différents formats d'affichage locaux standards pour une date.
 *
 * \since Cette énumération est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_DateFormat {
    /**
     * Format de date : Année/Mois/Jour
     */
    RC2D_DATE_FORMAT_YYYYMMDD = 0,

    /**
     * Format de date : Jour/Mois/Année
     */
    RC2D_DATE_FORMAT_DDMMYYYY = 1,

    /**
     * Format de date : Mois/Jour/Année
     */
    RC2D_DATE_FORMAT_MMDDYYYY = 2
} RC2D_DateFormat;

/**
 * \brief Enum du format d'heure préféré de l'utilisateur.
 *
 * Représente si le système utilise un format 24h ou 12h (AM/PM).
 *
 * \since Cette énumération est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_TimeFormat {
    /**
     * Format 24 heures
     */
    RC2D_TIME_FORMAT_24HR = 0,

    /**
     * Format 12 heures (AM/PM)
     */
    RC2D_TIME_FORMAT_12HR = 1
} RC2D_TimeFormat;

/**
 * \brief Obtient la valeur actuelle de l'horloge temps réel du système.
 *
 * Obtient la valeur actuelle de l'horloge temps réel du système en nanosecondes depuis 
 * le 1er janvier 1970 en temps universel coordonné (UTC).
 *
 * \param datetime Pointeur vers la structure RC2D_DateTime à remplir.
 * \return true en cas de succès, false sinon.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
bool rc2d_time_getCurrentTime(RC2D_DateTime *datetime);

/**
 * \brief Obtient le format de date et d'heure préféré actuel pour les paramètres régionaux du système.
 *
 * Remplit les pointeurs fournis avec les formats de date et d'heure préférés selon la configuration de la locale système.
 *
 * \param dateFormat Pointeur vers RC2D_DateFormat pour stocker le format de date (peut être NULL).
 * \param timeFormat Pointeur vers RC2D_TimeFormat pour stocker le format d'heure (peut être NULL).
 * \return true en cas de succès, false sinon.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
bool rc2d_time_getDateTimeLocalePreferences(RC2D_DateFormat *dateFormat, RC2D_TimeFormat *timeFormat);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_TIME_H