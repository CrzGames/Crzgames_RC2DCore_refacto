#ifndef RC2D_ASSERT_H
#define RC2D_ASSERT_H

/**
 * \brief Cette valeur change en fonction des options du compilateur et d'autres définitions du préprocesseur.
 *
 * 0 : toutes les macros d’assertion RC2D sont désactivées.
 * 1 : Paramètres de Release : RC2D_assert désactivé, RC2D_assert_release activé.
 * 2 : Paramètres de Debug : RC2D_assert et RC2DL_assert_release activés.
 * 3 : Paramètres Paranoid : RC2D_assert, RC2D_assert_release et RC2D_assert_paranoid activés.
 * 
 * \note Par défaut, cette valeur est définie sur 2, mais vous pouvez la modifier en fonction de vos besoins.
 * 
 * \threadsafety Il est possible d'appeler cette macro en toute sécurité à partir de n'importe quel thread.
 * 
 * \since Cette macro est disponible depuis RC2D 1.0.0.
 */
#ifndef RC2D_ASSERT_LEVEL
    #define RC2D_ASSERT_LEVEL 3
#endif

/**
 * Synchronise avec SDL_ASSERT_LEVEL *avant* l'include de : SDL_assert.h
 */
#ifndef SDL_ASSERT_LEVEL
    #define SDL_ASSERT_LEVEL RC2D_ASSERT_LEVEL
#endif

#include <RC2D/RC2D_logger.h>

#include <SDL3/SDL_assert.h>

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Assertion de RC2D de niveau 1 (RC2D_assert_release).
 * 
 * Cette macro est activée lorsque RC2D_ASSERT_LEVEL est >= 1, sinon, elle est désactivée. 
 * 
 * Elle est destinée aux tests peu coûteux à réaliser et dont l'échec est extrêmement improbable, 
 * il est généralement mal vu d'avoir une assertion défaillante lors d'une version de publication. 
 * Ces assertions doivent donc être d'une importance vitale pour pouvoir être déclenchées. 
 * Il est conseillé de gérer ces cas avec plus de souplesse que ne le permet une assertion.
 * 
 * Vous pouvez définir la variable d'environnement « RC2D_ASSERT » sur l'une des chaînes suivantes : 
 * « abort », « break », « retry », « ignore », « always_ignore ») pour forcer un comportement par défaut, 
 * ce qui peut être utile à des fins d'automatisation. 
 * 
 * Si votre plateforme nécessite que les interfaces graphiques soient exécutées sur le thread principal, 
 * mais que vous déboguez une assertion dans un thread d'arrière-plan, il peut être judicieux de définir 
 * cette variable sur « break » afin que votre débogueur prenne le contrôle dès le déclenchement de l'assertion, 
 * au lieu de risquer une mauvaise interaction avec l'interface utilisateur (blocage, etc.) dans l'application.
 * 
 * Exemple d'utilisation :
 * RC2D_assert_release(condition, RC2D_LOG_CRITICAL, "La propriété GPU est NULL !");
 * 
 * Exemple d'affichage :
 * [critical:rc2d_gpu.c:42:rc2d_gpu_getInfo] La propriété GPU est NULL !
 * 
 * \param condition La condition à vérifier.
 * \param message Le message d'erreur à afficher si la condition échoue.
 * \param {RC2D_LogLevel} logLevel Le niveau de log à utiliser pour afficher le message d'erreur.
 * 
 * \threadsafety Il est possible d'appeler cette macro en toute sécurité à partir de n'importe quel thread.
 * 
 * \since Cette macro est disponible depuis RC2D 1.0.0.
 * 
 * \see RC2D_assert
 * \see RC2D_assert_paranoid
 */
#if RC2D_ASSERT_LEVEL >= 1
#define RC2D_assert_release(condition, logLevel, format, ...) do { \
    if (!(condition)) { \
        SDL_assert_release((condition)); \
        RC2D_log((logLevel), format, ##__VA_ARGS__); \
    } \
} while (0)
#else
#define RC2D_assert_release(condition, logLevel, format, ...) SDL_disabled_assert(condition)
#endif

/**
 * \brief Assertion de RC2D de niveau 2 (RC2D_assert).
 * 
 * Cette macro est activée lorsque RC2D_ASSERT_LEVEL est >= 2, sinon, elle est désactivée. 
 * 
 * Elle est destinée à être utilisée pour les tests d'assertion qui doivent être effectués 
 * dans les versions de débogage. Ces assertions sont généralement utilisées pour vérifier 
 * des conditions critiques qui ne devraient jamais échouer dans un code bien écrit.
 * 
 * Vous pouvez définir la variable d'environnement « RC2D_ASSERT » sur l'une des chaînes suivantes : 
 * « abort », « break », « retry », « ignore », « always_ignore ») pour forcer un comportement par défaut, 
 * ce qui peut être utile à des fins d'automatisation. 
 * 
 * Si votre plateforme nécessite que les interfaces graphiques soient exécutées sur le thread principal, 
 * mais que vous déboguez une assertion dans un thread d'arrière-plan, il peut être judicieux de définir 
 * cette variable sur « break » afin que votre débogueur prenne le contrôle dès le déclenchement de l'assertion, 
 * au lieu de risquer une mauvaise interaction avec l'interface utilisateur (blocage, etc.) dans l'application.
 * 
 * Exemple d'utilisation :
 * RC2D_assert(condition, RC2D_LOG_CRITICAL, "La propriété GPU est NULL !");
 * 
 * Exemple d'affichage :
 * [critical:rc2d_gpu.c:42:rc2d_gpu_getInfo] La propriété GPU est NULL !
 * 
 * \param condition La condition à vérifier.
 * \param message Le message d'erreur à afficher si la condition échoue.
 * \param {RC2D_LogLevel} logLevel Le niveau de log à utiliser pour afficher le message d'erreur.
 * 
 * \threadsafety Il est possible d'appeler cette macro en toute sécurité à partir de n'importe quel thread.
 * 
 * \since Cette macro est disponible depuis RC2D 1.0.0.
 * 
 * \see RC2D_assert_release
 * \see RC2D_assert_paranoid
 */
#if RC2D_ASSERT_LEVEL >= 2
#define RC2D_assert(condition, logLevel, format, ...) do { \
    if (!(condition)) { \
        SDL_assert((condition)); \
        RC2D_log((logLevel), format, ##__VA_ARGS__); \
    } \
} while (0)
#else
#define RC2D_assert(condition, logLevel, format, ...) SDL_disabled_assert(condition)
#endif

/**
 * \brief Assertion de RC2D de niveau 3 (RC2D_assert_paranoid).
 * 
 * Cette macro est activée lorsque RC2D_ASSERT_LEVEL est >= 3, sinon, elle est désactivée. 
 * 
 * Elle est destinée à être utilisée pour les tests d'assertion qui doivent toujours être effectués, 
 * même dans les versions de publication. Ces assertions sont généralement utilisées pour vérifier 
 * des conditions critiques qui ne devraient jamais échouer dans un code bien écrit.
 * 
 * Vous pouvez définir la variable d'environnement « RC2D_ASSERT » sur l'une des chaînes suivantes : 
 * « abort », « break », « retry », « ignore », « always_ignore ») pour forcer un comportement par défaut, 
 * ce qui peut être utile à des fins d'automatisation. 
 * 
 * Si votre plateforme nécessite que les interfaces graphiques soient exécutées sur le thread principal, 
 * mais que vous déboguez une assertion dans un thread d'arrière-plan, il peut être judicieux de définir 
 * cette variable sur « break » afin que votre débogueur prenne le contrôle dès le déclenchement de l'assertion, 
 * au lieu de risquer une mauvaise interaction avec l'interface utilisateur (blocage, etc.) dans l'application.
 * 
 * Exemple d'utilisation :
 * RC2D_assert_paranoid(condition, RC2D_LOG_CRITICAL, "La propriété GPU est NULL !");
 * 
 * Exemple d'affichage :
 * [critical:rc2d_gpu.c:42:rc2d_gpu_getInfo] La propriété GPU est NULL !
 * 
 * \param condition La condition à vérifier.
 * \param message Le message d'erreur à afficher si la condition échoue.
 * \param {RC2D_LogLevel} logLevel Le niveau de log à utiliser pour afficher le message d'erreur.
 * 
 * \threadsafety Il est possible d'appeler cette macro en toute sécurité à partir de n'importe quel thread.
 * 
 * \since Cette macro est disponible depuis RC2D 1.0.0.
 * 
 * \see RC2D_assert
 * \see RC2D_assert_release
 */
#if RC2D_ASSERT_LEVEL >= 3
#define RC2D_assert_paranoid(condition, logLevel, format, ...) do { \
    if (!(condition)) { \
        SDL_assert_paranoid((condition)); \
        RC2D_log((logLevel), format, ##__VA_ARGS__); \
    } \
} while (0)
#else
#define RC2D_assert_paranoid(condition, logLevel, format, ...) SDL_disabled_assert(condition)
#endif

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_ASSERT_H