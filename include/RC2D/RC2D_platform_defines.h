#ifndef RC2D_PLATFORM_DEFINES_H
#define RC2D_PLATFORM_DEFINES_H

#include <SDL3/SDL_platform_defines.h>

/**
 * RC2D Platform Wrappers
 * 
 * Wrapper sur les macros de préprocesseur "SDL_PLATFORM_*" qui sont définies dans SDL3.
 * 
 * Les macros de préprocesseur "RC2D_PLATFORM_*" sont définies pour chaque plateforme supportée par SDL3
 * et qui sont supportées par SDL_GPU.
 */

// --- Linux ---
#if defined(SDL_PLATFORM_LINUX)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour Linux.
     *
     * Notez qu'Android, bien qu'étant ostensiblement un système basé sur Linux, ne définira pas cette macro.
     * Il définit RC2D_PLATFORM_ANDROID à la place.
     *
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     */
    #define RC2D_PLATFORM_LINUX 1
#endif

// --- Android ---
#if defined(SDL_PLATFORM_ANDROID)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour Android.
     *
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     */
    #define RC2D_PLATFORM_ANDROID 1
#endif

// --- Windows ---
#if defined(SDL_PLATFORM_WINDOWS)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour Windows.
     *
     * Cela couvre également plusieurs autres plateformes, comme Microsoft GDK, Xbox,
     * etc. Chacune d'entre elles aura également ses propres macros de plate-forme plus spécifiques.
     *
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     *
     * \sa RC2D_PLATFORM_WIN32
     * \sa RC2D_PLATFORM_XBOXONE
     * \sa RC2D_PLATFORM_XBOXSERIES
     * \sa RC2D_PLATFORM_WINGDK
     * \sa RC2D_PLATFORM_GDK
     */
    #define RC2D_PLATFORM_WINDOWS 1
#endif

#if defined(SDL_PLATFORM_WIN32)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour Windows.
     *
     * Malgré le « 32 », elle couvre également Windows 64 bits, par convention informelle, 
     * sa couche système tend à être encore appelée « l'API Win32 ».
     *
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     *
     * \sa RC2D_PLATFORM_WIN32
     * \sa RC2D_PLATFORM_XBOXONE
     * \sa RC2D_PLATFORM_XBOXSERIES
     * \sa RC2D_PLATFORM_WINGDK
     * \sa RC2D_PLATFORM_GDK
     */
    #define RC2D_PLATFORM_WIN32 1
#endif

#if defined(SDL_PLATFORM_WINGDK)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour Microsoft GDK 
     * pour Windows.
     * 
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     */
    #define RC2D_PLATFORM_WINGDK 1
#endif

#if defined(SDL_PLATFORM_XBOXONE)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour Xbox One.
     * 
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     */
    #define RC2D_PLATFORM_XBOXONE 1
#endif

#if defined(SDL_PLATFORM_XBOXSERIES)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour Xbox Series.
     * 
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     */
    #define RC2D_PLATFORM_XBOXSERIES 1
#endif

#if defined(SDL_PLATFORM_GDK)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour Microsoft GDK 
     * sur n'importe quelle plate-forme.
     * 
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     */
    #define RC2D_PLATFORM_GDK 1
#endif

// --- Apple ---
#if defined(SDL_PLATFORM_MACOS)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour macOS.
     * 
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     * 
     * \sa RC2D_PLATFORM_APPLE
     */
    #define RC2D_PLATFORM_MACOS 1
#endif

#if defined(SDL_PLATFORM_IOS)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour iOS.
     * 
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     * 
     * \sa RC2D_PLATFORM_APPLE
     */
    #define RC2D_PLATFORM_IOS 1
#endif

#if defined(SDL_PLATFORM_TVOS)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour tvOS.
     * 
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     * 
     * \sa RC2D_PLATFORM_APPLE
     */
    #define RC2D_PLATFORM_TVOS 1
#endif

#if defined(SDL_PLATFORM_VISIONOS)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour visionOS.
     * 
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     * 
     * \sa RC2D_PLATFORM_APPLE
     */
    #define RC2D_PLATFORM_VISIONOS 1
#endif

#if defined(SDL_PLATFORM_APPLE)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour les plates-formes Apple.
     * 
     * iOS, macOS, tvOS, visionOS..etc définiront en plus une macro de plateforme plus spécifique.
     *
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     *
     * \sa RC2D_PLATFORM_MACOS
     * \sa RC2D_PLATFORM_IOS
     * \sa RC2D_PLATFORM_TVOS
     * \sa RC2D_PLATFORM_VISIONOS
     */
    #define RC2D_PLATFORM_APPLE 1
#endif

// --- BSD / UNIX / Autres ---
#if defined(SDL_PLATFORM_FREEBSD)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour FreeBSD.
     * 
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     */
    #define RC2D_PLATFORM_FREEBSD 1
#endif

#if defined(SDL_PLATFORM_NETBSD)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour NetBSD.
     * 
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     */
    #define RC2D_PLATFORM_NETBSD 1
#endif

#if defined(SDL_PLATFORM_OPENBSD)
    /**
     * Une macro de préprocesseur qui n'est définie que si l'on compile pour OpenBSD.
     * 
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     */
    #define RC2D_PLATFORM_OPENBSD 1
#endif

#if defined(SDL_PLATFORM_UNIX)
    /**
     * Une macro du préprocesseur qui n'est définie que si l'on compile pour un système de type Unix.
     * 
     * D'autres plates-formes, comme Linux, peuvent la définir en plus de leur définition principale.
     *
     * \since Cette macro est disponible depuis RC2D 1.0.0.
     */
    #define RC2D_PLATFORM_UNIX 1
#endif

#endif // RC2D_PLATFORM_DEFINES_H
