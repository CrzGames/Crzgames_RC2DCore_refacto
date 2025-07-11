#ifndef RC2D_CAMERA_H
#define RC2D_CAMERA_H

#include <SDL3/SDL_camera.h> // Requis pour : SDL_Camera, SDL_CameraID, SDL_Surface
#include <stdbool.h>         // Requis pour : bool

/* Configuration pour les définitions de fonctions C, même en C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Structure opaque représentant une caméra ouverte dans RC2D.
 *
 * Encapsule SDL_Camera pour fournir une interface de haut niveau.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_Camera RC2D_Camera;

/**
 * \brief Identifiant unique d'une caméra connectée.
 *
 * Correspond à SDL_CameraID, utilisé pour énumérer et ouvrir les caméras.
 *
 * \since Ce type est disponible depuis RC2D 1.0.0.
 */
typedef SDL_CameraID RC2D_CameraID;

/**
 * \brief Position de la caméra par rapport à l'appareil.
 *
 * Indique si la caméra est à l'avant (selfie) ou à l'arrière.
 *
 * \since Cette énumération est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_CameraPosition {
    /**
     * Position inconnue (par défaut sur la plupart des plateformes).
     */
    RC2D_CAMERA_POSITION_UNKNOWN = 0,

    /**
     * Caméra à l'avant (selfie).
     */
    RC2D_CAMERA_POSITION_FRONT_FACING = 1,

    /**
     * Caméra à l'arrière.
     */
    RC2D_CAMERA_POSITION_BACK_FACING = 2
} RC2D_CameraPosition;

/**
 * \brief Spécification pour le format et la résolution d'une caméra.
 *
 * Définit le format de pixel, la largeur, la hauteur et le framerate souhaités.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_CameraSpec {
    /**
     * Format de pixel (ex. : SDL_PIXELFORMAT_YUY2, SDL_PIXELFORMAT_RGB24).
     */
    Uint32 format;

    /**
     * Largeur de l'image en pixels.
     */
    int width;

    /**
     * Hauteur de l'image en pixels.
     */
    int height;

    /**
     * Framerate souhaité en images par seconde (0 pour le défaut).
     */
    int framerate;
} RC2D_CameraSpec;

/**
 * \brief Options pour l'ouverture d'une caméra.
 *
 * Permet de spécifier le format, la position et d'autres paramètres.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_CameraOptions {
    /**
     * Spécification souhaitée pour le format et la résolution (peut être NULL pour le défaut).
     */
    const RC2D_CameraSpec *spec;

    /**
     * Position préférée de la caméra (peut être UNKNOWN pour ignorer).
     */
    RC2D_CameraPosition position;
} RC2D_CameraOptions;

/**
 * \brief Récupère la liste des caméras connectées.
 *
 * \param count Pointeur pour stocker le nombre de caméras (peut être NULL).
 * \return Tableau d'ID de caméras terminé par 0, ou NULL en cas d'erreur.
 * 
 * \warning Le tableau retourné doit être libéré par l'appelant avec `RC2D_safe_free()`.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see RC2D_safe_free
 * 
 */
RC2D_CameraID *rc2d_camera_getDevices(int *count);

/**
 * \brief Récupère le nom d'une caméra.
 *
 * \param instance_id ID de la caméra.
 * \return Nom de la caméra en UTF-8, ou NULL en cas d'erreur.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
const char *rc2d_camera_getName(RC2D_CameraID instance_id);

/**
 * \brief Ouvre une caméra avec les options spécifiées.
 *
 * \param instance_id ID de la caméra à ouvrir.
 * \param options Options de configuration (peut être NULL pour les valeurs par défaut).
 * \return Pointeur vers RC2D_Camera en cas de succès, ou NULL en cas d'erreur.
 * 
 * \warning Le pointeur retourné doit être libéré par l'appelant avec `rc2d_camera_close()`.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
RC2D_Camera *rc2d_camera_open(RC2D_CameraID instance_id, const RC2D_CameraOptions *options);

/**
 * \brief Ferme une caméra ouverte.
 *
 * \param camera Caméra à fermer.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_camera_close(RC2D_Camera *camera);

/**
 * \brief Vérifie si l'accès à la caméra a été approuvé par l'utilisateur.
 *
 * \param camera Caméra ouverte à interroger.
 * \return 1 si approuvé, -1 si refusé, 0 si en attente.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
int rc2d_camera_getPermission(RC2D_Camera *camera);

/**
 * \brief Récupère une nouvelle image de la caméra.
 *
 * \param camera Caméra ouverte.
 * \param timestamp_ns Pointeur pour stocker l'horodatage de l'image en nanosecondes (peut être NULL).
 * \return Surface SDL contenant l'image, ou NULL si aucune image n'est disponible ou en cas d'erreur.
 * 
 * \warning Le pointeur retourné doit être libéré par l'appelant avec `rc2d_camera_releaseFrame()`.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
SDL_Surface *rc2d_camera_getFrame(RC2D_Camera *camera, Uint64 *timestamp_ns);

/**
 * \brief Libère une image récupérée de la caméra.
 *
 * \param camera Caméra ouverte.
 * \param frame Surface à libérer (obtenue via rc2d_camera_get_frame).
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_camera_releaseFrame(RC2D_Camera *camera, SDL_Surface *frame);

/**
 * \brief Récupère la spécification actuelle de la caméra.
 *
 * \param camera Caméra ouverte.
 * \param spec Pointeur pour stocker la spécification.
 * \return true en cas de succès, false sinon.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
bool rc2d_camera_getSpec(RC2D_Camera *camera, RC2D_CameraSpec *spec);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_CAMERA_H