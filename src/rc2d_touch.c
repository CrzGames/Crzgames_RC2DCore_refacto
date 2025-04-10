#include "rc2d/rc2d_touch.h"
#include "rc2d/rc2d_graphics.h"

#include <SDL.h> // Required for : SDL_FINGERDOWN, SDL_FINGERUP

#include <stdlib.h> // Required for : malloc, free, realloc

static RC2D_TouchState* touchState = NULL;

/**
 * @brief Initialise la structure de données pour la gestion des pressions tactiles.
 */
static void initTouchState(void) 
{
    touchState = malloc(sizeof(RC2D_TouchState));
    if (touchState == NULL) 
    {
        return; // Gestion de l'erreur de mémoire
    }

    touchState->touches = NULL;
    touchState->pressures = NULL;
    touchState->x = NULL;
    touchState->y = NULL;
    touchState->numTouches = 0;
}

/**
 * @brief Libère la mémoire allouée pour la gestion des pressions tactiles.
 */
void rc2d_touch_freeTouchState(void) 
{
    if (touchState != NULL)
    {
        if (touchState->numTouches > 0)
        {
            free(touchState->touches);
        }

        if (touchState->pressures != NULL)
        {
            free(touchState->pressures);
        }

        if (touchState->x != NULL)
        {
            free(touchState->x);
        }

        if (touchState->y != NULL)
        {
            free(touchState->y);
        }

        free(touchState);
    }
}

/**
 * @brief Met à jour l'état des pressions tactiles en fonction des événements SDL.
 *
 * @param fingerId L'identifiant de la pression tactile.
 * @param eventType Le type d'événement SDL.
 * @param pressure La valeur de pression.
 * @param x La coordonnée x.
 * @param y La coordonnée y.
 */
void rc2d_touch_updateState(const int fingerId, const int eventType, const float pressure, const float x, const float y) 
{
    if (touchState == NULL) 
    {
        initTouchState(); 

        if (touchState == NULL) 
        {
            return; // Gestion de l'erreur de mémoire
        }
    }

    // Mettre à jour l'état des pressions tactiles en fonction du type d'événement
    if (eventType == SDL_FINGERDOWN || eventType == SDL_FINGERMOTION) 
    {
        // Allouer de la mémoire pour un nouvel identifiant de pression tactile
        touchState->numTouches++;
        touchState->touches = realloc(touchState->touches, sizeof(int) * touchState->numTouches);
        touchState->pressures = realloc(touchState->pressures, sizeof(double) * touchState->numTouches); // Allouer de la mémoire pour les pressions
        touchState->x = realloc(touchState->x, sizeof(double) * touchState->numTouches); // Allouer de la mémoire pour les coordonnées x
        touchState->y = realloc(touchState->y, sizeof(double) * touchState->numTouches); // Allouer de la mémoire pour les coordonnées y
        if (touchState->touches != NULL && touchState->pressures != NULL && touchState->x != NULL && touchState->y != NULL) 
        {
            // Ajouter le nouvel identifiant à la liste
            touchState->touches[touchState->numTouches - 1] = fingerId;
            touchState->pressures[touchState->numTouches - 1] = pressure; // Stocker la pression correspondante
            touchState->x[touchState->numTouches - 1] = x; // Stocker la coordonnée x correspondante
            touchState->y[touchState->numTouches - 1] = y; // Stocker la coordonnée y correspondante
        }
    } 
    else if (eventType == SDL_FINGERUP) 
    {
        // Supprimer l'identifiant de pression tactile libéré de la liste
        for (int i = 0; i < touchState->numTouches; i++) 
        {
            if (touchState->touches[i] == fingerId) 
            {
                // Déplacer les éléments restants pour remplir l'espace libéré
                for (int j = i; j < touchState->numTouches - 1; j++) 
                {
                    touchState->touches[j] = touchState->touches[j + 1];
                }
                // Réduire la taille du tableau et libérer la mémoire
                touchState->numTouches--;
                touchState->touches = realloc(touchState->touches, sizeof(int) * touchState->numTouches);
                break;
            }
        }
    }
}

/**
 * @brief Récupère la liste des identifiants de pressions tactiles actives.
 *
 * @return Un tableau des identifiants de pressions tactiles.
 */
int* rc2d_touch_getTouches(void) 
{
    // Retourner la liste des touches
    int* touches = malloc(sizeof(int) * touchState->numTouches);
    if (touches == NULL) 
    {
        return NULL; // Gestion de l'erreur de mémoire
    }

    memcpy(touches, touchState->touches, sizeof(int) * touchState->numTouches);

    return touches;
}

/**
 * @brief Libère la mémoire allouée pour la liste des identifiants de pressions tactiles.
 *
 * @param touches Le tableau des identifiants de pressions tactiles.
 */
void rc2d_touch_freeTouches(int* touches) 
{
    if (touches != NULL) 
    {
        free(touches);
    }
}

/**
 * @brief Récupère la valeur de pression pour une pression tactile spécifique.
 *
 * @param fingerId L'identifiant de la pression tactile.
 * @return La valeur de pression.
 */
float rc2d_touch_getPressure(const int fingerId) 
{
    if (touchState == NULL || touchState->numTouches == 0 || touchState->touches == NULL || touchState->pressures == NULL) 
    {
        return 1.0; // Aucune pression enregistrée
    }

    // Recherche de l'indice correspondant à l'identifiant de pression tactile donné
    int index = -1;
    for (int i = 0; i < touchState->numTouches; i++) 
    {
        if (touchState->touches[i] == fingerId) 
        {
            index = i;
            break;
        }
    }

    // Si l'indice est valide, retourner la pression correspondante, sinon, retourner 1.0
    if (index != -1) 
    {
        return touchState->pressures[index];
    } 
    else 
    {
        // La pression du toucher-presse. 
        // La plupart des écrans tactiles ne sont pas sensibles à la pression, auquel cas la pression sera de 1.0
        return 1.0; // Identifiant de pression tactile non trouvé
    }
}

/**
 * @brief Récupère les coordonnées d'une pression tactile spécifique.
 *
 * @param fingerId L'identifiant de la pression tactile.
 * @param x Pointeur pour stocker la coordonnée x.
 * @param y Pointeur pour stocker la coordonnée y.
 */
void rc2d_touch_getPosition(const int fingerId, float* x, float* y) 
{
    // Initialiser les positions x et y à 0.0 par défaut
    *x = 0.0f;
    *y = 0.0f;

    // Vérifier si l'état des pressions tactiles est valide
    if (touchState == NULL || touchState->numTouches == 0 || touchState->touches == NULL) 
    {
        return; // Aucune pression enregistrée, retourner les positions par défaut (0.0, 0.0)
    }

    // Rechercher l'indice correspondant à l'identifiant de pression tactile donné
    int index = -1;
    for (int i = 0; i < touchState->numTouches; i++) 
    {
        if (touchState->touches[i] == fingerId) 
        {
            index = i;
            break;
        }
    }

    // Si l'indice est valide, récupérer les positions correspondantes
    if (index != -1) 
    {
        // Récupérer les positions en pixels
        *x = touchState->x[index] * (float)rc2d_graphics_getWidth();
        *y = touchState->y[index] * (float)rc2d_graphics_getHeight();
    }
}