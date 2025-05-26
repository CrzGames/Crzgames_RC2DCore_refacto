#include <RC2D/RC2D_touch.h>
#include <RC2D/RC2D_memory.h>
#include <RC2D/RC2D_logger.h>

#include <SDL3/SDL_events.h>

/**
 * \brief Structure pour stocker l'état des points de contact tactiles.
 * 
 * Cette structure contient un tableau dynamique de points de contact
 * et le nombre total de contacts actifs.
 */
static RC2D_TouchState* touchState = NULL;

/**
 * \brief Initialise le gestionnaire tactile interne (si nécessaire).
 */
static void rc2d_touch_init_if_needed(void)
{
    if (!touchState) 
    {
        // Allocation de la structure principale pour les touch points
        touchState = RC2D_malloc(sizeof(RC2D_TouchState));
        if (!touchState) 
        {
            RC2D_log(RC2D_LOG_ERROR, "Échec de l'allocation de RC2D_TouchState");
            return;
        }

        // Initialisation des champs
        touchState->touches = NULL;
        touchState->numTouches = 0;
    }
}

void rc2d_touch_updateState(SDL_TouchID deviceID, SDL_FingerID fingerID, int eventType, float pressure, float x, float y)
{
    rc2d_touch_init_if_needed();
    if (!touchState) return;

    // Recherche de l'indice du doigt s'il est déjà présent dans la liste
    int index = -1;
    for (int i = 0; i < touchState->numTouches; ++i) 
    {
        if (touchState->touches[i].fingerID == fingerID) 
        {
            index = i;
            break;
        }
    }

    if (eventType == SDL_EVENT_FINGER_DOWN || eventType == SDL_EVENT_FINGER_MOTION) 
    {
        if (index >= 0) 
        {
            // Mise à jour des informations d'un doigt déjà enregistré
            touchState->touches[index].x = x;
            touchState->touches[index].y = y;
            touchState->touches[index].pressure = pressure;
        } 
        else 
        {
            // Ajout d'un nouveau doigt à la structure
            touchState->numTouches++;
            touchState->touches = RC2D_realloc(touchState->touches, sizeof(RC2D_TouchPoint) * touchState->numTouches);
            if (!touchState->touches) 
            {
                RC2D_log(RC2D_LOG_ERROR, "Échec de realloc pour RC2D_TouchPoint");
                return;
            }

            // Remplissage des données pour le nouveau doigt
            touchState->touches[touchState->numTouches - 1] = (RC2D_TouchPoint){
                .fingerID = fingerID,
                .deviceID = deviceID,
                .x = x,
                .y = y,
                .pressure = pressure
            };
        }
    } 
    else if (eventType == SDL_EVENT_FINGER_UP || eventType == SDL_EVENT_FINGER_CANCELED) {
        if (index >= 0) 
        {
            // Suppression du doigt de la liste
            for (int j = index; j < touchState->numTouches - 1; ++j)
             {
                touchState->touches[j] = touchState->touches[j + 1];
            }

            // Mise à jour du nombre total
            touchState->numTouches--;

            // Réallocation ou libération mémoire selon s'il reste des touches
            if (touchState->numTouches > 0) 
            {
                touchState->touches = RC2D_realloc(touchState->touches, sizeof(RC2D_TouchPoint) * touchState->numTouches);
            } 
            else 
            {
                RC2D_free(touchState->touches);
                touchState->touches = NULL;
            }
        }
    }
}

void rc2d_touch_freeTouchState(void)
{
    if (touchState)
    {
        // Libération du tableau des touches si alloué
        if (touchState->touches) 
        {
            RC2D_free(touchState->touches);
        }

        // Libération de la structure principale
        RC2D_free(touchState);
        touchState = NULL;
    }
}

void rc2d_touch_getPosition(SDL_FingerID fingerID, float* x, float* y)
{
    *x = 0.0f;
    *y = 0.0f;
    if (!touchState || !touchState->touches) return;

    // Recherche du doigt et calcul des coordonnées écran en pixels
    for (int i = 0; i < touchState->numTouches; ++i) 
    {
        if (touchState->touches[i].fingerID == fingerID) 
        {
            *x = touchState->touches[i].x * rc2d_window_getWidth();
            *y = touchState->touches[i].y * rc2d_window_getHeight();
            return;
        }
    }
}

float rc2d_touch_getPressure(SDL_FingerID fingerID)
{
    if (!touchState || !touchState->touches) return 1.0f;

    // Recherche du doigt et retour de sa pression
    for (int i = 0; i < touchState->numTouches; ++i)
    {
        if (touchState->touches[i].fingerID == fingerID) 
        {
            return touchState->touches[i].pressure;
        }
    }

    // Si le doigt n'est pas trouvé, on retourne une pression par défaut
    return 1.0f;
}

SDL_FingerID* rc2d_touch_getTouches(void)
{
    if (!touchState || !touchState->touches || touchState->numTouches == 0)
        return NULL;

    // Allocation du tableau de FingerID à retourner
    SDL_FingerID* result = RC2D_malloc(sizeof(SDL_FingerID) * touchState->numTouches);
    if (!result) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Échec de l'allocation de touches[]");
        return NULL;
    }

    // Copie des identifiants depuis la structure interne
    for (int i = 0; i < touchState->numTouches; ++i) 
    {
        result[i] = touchState->touches[i].fingerID;
    }
    return result;
}

void rc2d_touch_freeTouches(SDL_FingerID* touches)
{
    if (touches) RC2D_free(touches);
}
