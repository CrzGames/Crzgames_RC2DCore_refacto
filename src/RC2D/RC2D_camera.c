#include <RC2D/RC2D_camera.h>
#include <RC2D/RC2D_logger.h>
#include <SDL3/SDL_camera.h>

// Structure interne pour RC2D_Camera
struct RC2D_Camera {
    SDL_Camera *sdl_camera;
};

// Convertit RC2D_CameraPosition en SDL_CameraPosition
static SDL_CameraPosition rc2d_camera_convert_position(RC2D_CameraPosition position) 
{
    switch (position) 
    {
        case RC2D_CAMERA_POSITION_UNKNOWN: return SDL_CAMERA_POSITION_UNKNOWN;
        case RC2D_CAMERA_POSITION_FRONT_FACING: return SDL_CAMERA_POSITION_FRONT_FACING;
        case RC2D_CAMERA_POSITION_BACK_FACING: return SDL_CAMERA_POSITION_BACK_FACING;
        default: return SDL_CAMERA_POSITION_UNKNOWN;
    }
}

// Convertit RC2D_CameraSpec en SDL_CameraSpec
static void rc2d_camera_convert_spec(const RC2D_CameraSpec *rc2d_spec, SDL_CameraSpec *sdl_spec) 
{
    if (!rc2d_spec || !sdl_spec) return;

    // Mappe le format RC2D à SDL
    sdl_spec->format = rc2d_spec->format;
    sdl_spec->width = rc2d_spec->width;
    sdl_spec->height = rc2d_spec->height;
    sdl_spec->framerate_numerator = rc2d_spec->framerate;
    sdl_spec->framerate_denominator = rc2d_spec->framerate ? 1 : 0;
}

RC2D_CameraID *rc2d_camera_getDevices(int *count) 
{
    // Récupère le nombre de caméras disponibles
    RC2D_CameraID *devices = SDL_GetCameras(count);
    if (!devices && count) 
    {
        RC2D_log(RC2D_LOG_ERROR,  "echec de SDL_GetCameras : %s", SDL_GetError());
    }

    return devices;
}

const char *rc2d_camera_getName(RC2D_CameraID instance_id) 
{
    const char *name = SDL_GetCameraName(instance_id);
    if (!name) 
    {
        RC2D_log(RC2D_LOG_ERROR,  "echec de SDL_GetCameraName : %s", SDL_GetError());
    }

    return name;
}

RC2D_Camera *rc2d_camera_open(RC2D_CameraID instance_id, const RC2D_CameraOptions *options) 
{
    if (instance_id == 0) 
    {
        RC2D_log(RC2D_LOG_ERROR, "ID de caméra invalide");
        return NULL;
    }

    // Vérifie la position si spécifiée
    if (options && options->position != RC2D_CAMERA_POSITION_UNKNOWN) 
    {
        SDL_CameraPosition sdl_position = rc2d_camera_convert_position(options->position);
        if (SDL_GetCameraPosition(instance_id) != sdl_position) 
        {
            RC2D_log(RC2D_LOG_WARN, "position demandée (%d) non disponible pour cette caméra", options->position);
            return NULL;
        }
    }

    // Prépare la spécification
    SDL_CameraSpec sdl_spec = {0};
    if (options && options->spec) 
    {
        rc2d_camera_convert_spec(options->spec, &sdl_spec);
    }

    // Ouvre la caméra
    SDL_Camera *sdl_camera = SDL_OpenCamera(instance_id, options && options->spec ? &sdl_spec : NULL);
    if (!sdl_camera) 
    {
        RC2D_log(RC2D_LOG_ERROR,  "echec de SDL_OpenCamera : %s", SDL_GetError());
        return NULL;
    }

    // Alloue la structure RC2D_Camera
    RC2D_Camera *camera = SDL_malloc(sizeof(RC2D_Camera));
    if (!camera) 
    {
        RC2D_log(RC2D_LOG_ERROR,  "echec d'allocation de RC2D_Camera");
        SDL_CloseCamera(sdl_camera);
        return NULL;
    }
    camera->sdl_camera = sdl_camera;

    RC2D_log(RC2D_LOG_INFO, "caméra ouverte, en attente de permission utilisateur");
    return camera;
}

void rc2d_camera_close(RC2D_Camera *camera) 
{
    if (!camera) 
    {
        RC2D_log(RC2D_LOG_WARN,  "rc2d_camera_close : caméra NULL");
        return;
    }

    SDL_CloseCamera(camera->sdl_camera);
    SDL_free(camera);
}

int rc2d_camera_getPermission(RC2D_Camera *camera) 
{
    if (!camera) 
    {
        RC2D_log(RC2D_LOG_WARN,  "rc2d_camera_get_permission : caméra NULL");
        return -1;
    }

    int state = SDL_GetCameraPermissionState(camera->sdl_camera);
    if (state == 0) 
    {
        RC2D_log(RC2D_LOG_DEBUG,"rc2d_camera_get_permission : en attente de permission utilisateur");
    } 
    else if (state == -1) 
    {
        RC2D_log(RC2D_LOG_ERROR,  "rc2d_camera_get_permission : permission refusée par l'utilisateur");
    } 
    else 
    {
        RC2D_log(RC2D_LOG_INFO, "rc2d_camera_get_permission : permission accordée");
    }

    return state;
}

SDL_Surface *rc2d_camera_getFrame(RC2D_Camera *camera, Uint64 *timestamp_ns) 
{
    if (!camera) 
    {
        RC2D_log(RC2D_LOG_WARN,  "rc2d_camera_get_frame : caméra NULL");
        return NULL;
    }

    SDL_Surface *frame = SDL_AcquireCameraFrame(camera->sdl_camera, timestamp_ns);
    if (!frame) 
    {
        // NULL est normal si aucune image n'est disponible
        RC2D_log(RC2D_LOG_DEBUG,"rc2d_camera_get_frame : aucune image disponible");
    }

    return frame;
}

void rc2d_camera_releaseFrame(RC2D_Camera *camera, SDL_Surface *frame)
{
    if (!camera || !frame) 
    {
        RC2D_log(RC2D_LOG_WARN,  "rc2d_camera_release_frame : caméra ou frame NULL");
        return;
    }
    
    SDL_ReleaseCameraFrame(camera->sdl_camera, frame);
    RC2D_log(RC2D_LOG_DEBUG,"rc2d_camera_release_frame : image libérée");
}

bool rc2d_camera_getSpec(RC2D_Camera *camera, RC2D_CameraSpec *spec) 
{
    if (!camera || !spec) 
    {
        RC2D_log(RC2D_LOG_WARN,  "rc2d_camera_get_spec : caméra ou spec NULL");
        return false;
    }

    SDL_CameraSpec sdl_spec;
    if (!SDL_GetCameraFormat(camera->sdl_camera, &sdl_spec)) 
    {
        RC2D_log(RC2D_LOG_ERROR,  "rc2d_camera_get_spec : échec de SDL_GetCameraFormat : %s", SDL_GetError());
        return false;
    }
    spec->format = sdl_spec.format;
    spec->width = sdl_spec.width;
    spec->height = sdl_spec.height;
    spec->framerate = sdl_spec.framerate_numerator;

    return true;
}