#include <RC2D/RC2D_system.h>
#include <RC2D/RC2D_logger.h>

#include <SDL3/SDL.h>

/**
 * Fonction de rappel pour arrêter la vibration.
 *
 * Cette fonction arrête la vibration d'un périphérique haptique et ferme le gestionnaire haptique.

 * @param interval La durée de la vibration en millisecondes.
 * @param param Un pointeur vers le périphérique haptique (SDL_Haptic) à arrêter.
 */
static Uint32 stopVibration(Uint32 interval, void *param)
{
    // TODO: interval jamais utilisé a checker
    
    if (param == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'arrêter la vibration : paramètre invalide.");
        return 0;  // Ne pas replanifier le timer si le paramètre est invalide
    }

    SDL_Haptic *haptic = (SDL_Haptic *)param; // Convertit le paramètre en un pointeur vers un périphérique haptique

    int result = SDL_HapticRumbleStop(haptic); // Arrête la vibration du périphérique haptique
    if (result != 0)
    {
        // Affiche un message d'erreur si la vibration n'a pas pu être arrêtée
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'arrêter la vibration : %s.", SDL_GetError());
    }

    SDL_HapticClose(haptic); // Ferme le gestionnaire haptique

    return 0; // Retourner 0 pour que le timer ne se replanifie pas automatiquement
}


/**
 * Permet de faire vibrer l'appareil pendant une durée et avec une intensité spécifiée.
 * Utilise la première manette haptique disponible pour générer une vibration.
 * @param seconds La durée de la vibration en secondes.
 * @param strength L'intensité de la vibration, entre 0 (aucune vibration) et 1 (vibration maximale).
 */
void rc2d_system_vibrate(const double seconds, const float strength) 
{
    if (seconds <= 0 || strength < 0 || strength > 1)
    {
        RC2D_log(RC2D_LOG_WARN, "Impossible de jouer la vibration : paramètres invalides dans rc2d_system_vibrate().");
        return;
    }

    // Convertir la durée de la vibration en millisecondes
    Uint32 milliseconds = (Uint32)(seconds * 1000);

    // Si la plateforme est iOS, ajuster la durée de vibration
    #ifdef __IPHONEOS__
        milliseconds = 500; // 0.5 seconde en millisecondes (durée fixe sur iOS)
    #endif

    // Ouvrir un périphérique haptique
    SDL_Haptic *haptic = SDL_HapticOpen(0);

    // Vérifier si le périphérique haptique a été ouvert avec succès
    if (haptic != NULL) 
    {
        // Vérifier si le périphérique prend en charge la vibration
        if (SDL_HapticQuery(haptic) & SDL_HAPTIC_CONSTANT) 
        {
            // Initialiser la vibration
            int result = SDL_HapticRumbleInit(haptic);
            if (result != 0)
            {
                // Afficher un message d'erreur si la vibration n'a pas pu être initialisée
                RC2D_log(RC2D_LOG_ERROR, "Impossible d'initialiser la vibration %s.", SDL_GetError());
            }
            else
            {
                // Jouer la vibration
                result = SDL_HapticRumblePlay(haptic, strength, milliseconds);
                if (result != 0)
                {
                    // Afficher un message d'erreur si la vibration n'a pas pu être jouée
                    RC2D_log(RC2D_LOG_ERROR, "Impossible de jouer la vibration %s.", SDL_GetError());
                }
                else
                {
                    // Planifier l'arrêt de la vibration après la durée spécifiée
                    SDL_TimerID timerID = SDL_AddTimer(milliseconds, stopVibration, haptic);
                    if (timerID == 0)
                    {
                        // Afficher un message d'erreur si la temporisation a échoué
                        RC2D_log(RC2D_LOG_ERROR, "Impossible de planifier l'arrêt de la vibration %s.", SDL_GetError());
                    }
                }
            }
        }
        else 
        {
            // Afficher un message d'erreur si le périphérique ne prend pas en charge la vibration
            RC2D_log(RC2D_LOG_ERROR, "Le périphérique ne prend pas en charge la vibration.");
        }
    } 
    else 
    {
        // Afficher un message d'erreur si le périphérique haptique n'a pas pu être ouvert
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ouvrir le dispositif haptique %s.", SDL_GetError());
    }
}

bool rc2d_system_hasClipboardText(void)
{
	if (SDL_HasClipboardText() == true)
		return true;
	else
		return false;
}

char* rc2d_system_getClipboardText(void)
{
    // Récupère le texte du presse-papiers
	char* clipboardText = SDL_GetClipboardText();
    if (clipboardText == NULL)
    {
        RC2D_log(RC2D_LOG_WARN, "Impossible de récupérer le texte du presse-papiers : %s dans rc2d_system_getClipboardText().", SDL_GetError());
        return NULL; // Retourne NULL si la récupération échoue
    }

    return clipboardText;
}

void rc2d_system_setClipboardText(const char* text)
{
    // Si le texte est NULL, on ne fait rien et on affiche un avertissement
    if (text == NULL)
    {
        RC2D_log(RC2D_LOG_WARN, "Impossible de copier le texte dans le presse-papiers : le texte est NULL dans rc2d_system_setClipboardText().");
        return;
    }

    // Set le texte dans le presse-papiers
    if (!SDL_SetClipboardText(text))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de copier le texte dans le presse-papiers : %s dans rc2d_system_setClipboardText().", SDL_GetError());
    }
}

void rc2d_system_freeClipboardText(char* text)
{
    if (text != NULL)
    {
        SDL_free(text);
        text = NULL;
    }
}

int rc2d_system_getNumLogicalCPUCores(void)
{
	return SDL_GetNumLogicalCPUCores();
}

int rc2d_system_getRAM(void)
{
	return SDL_GetSystemRAM();
}

void rc2d_system_openURL(const char* url)
{
    if (url == NULL)
    {
        RC2D_log(RC2D_LOG_WARN, "Impossible d'ouvrir l'URL : l'URL est NULL dans rc2d_system_openURL().");
        return;
    }

    if (!SDL_OpenURL(url))
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ouvrir l'URL : %s dans rc2d_system_openURL().", SDL_GetError());
    }
}

bool rc2d_system_isTablet(void)
{
    return SDL_IsTablet();
}

bool rc2d_system_isTV(void)
{
    return SDL_IsTV();
}

/**
 * \brief Obtient le type de sandbox de l'application, s'il y en a un.
 * 
 * \return {RC2D_Sandbox} Le type de sandbox de l'application.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
RC2D_Sandbox rc2d_system_getSandbox(void)
{
    RC2D_Sandbox sandbox = RC2D_SANDBOX_NONE;

    const SDL_Sandbox sdlSandbox = SDL_GetSandbox();
    switch (sdlSandbox)
    {
        case SDL_SANDBOX_NONE:
            sandbox = RC2D_SANDBOX_NONE;
            break;
        case SDL_SANDBOX_UNKNOWN_CONTAINER:
            sandbox = RC2D_SANDBOX_UNKNOWN_CONTAINER;
            break;
        case SDL_SANDBOX_FLATPAK:
            sandbox = RC2D_SANDBOX_FLATPAK;
            break;
        case SDL_SANDBOX_SNAP:
            sandbox = RC2D_SANDBOX_SNAP;
            break;
        case SDL_SANDBOX_MACOS:
            sandbox = RC2D_SANDBOX_MACOS;
            break;
        default:
            RC2D_log(RC2D_LOG_ERROR, "Type de sandbox inconnu : %d dans rc2d_system_getSandbox().", sdlSandbox);
            break;
    }

    return sandbox;
}
