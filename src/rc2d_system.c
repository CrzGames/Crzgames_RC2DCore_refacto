#include "rc2d/rc2d_system.h"
#include "rc2d/rc2d_logger.h"

#include <SDL.h> 

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
        rc2d_log(RC2D_LOG_ERROR, "Impossible d'arrêter la vibration : paramètre invalide.");
        return 0;  // Ne pas replanifier le timer si le paramètre est invalide
    }

    SDL_Haptic *haptic = (SDL_Haptic *)param; // Convertit le paramètre en un pointeur vers un périphérique haptique

    int result = SDL_HapticRumbleStop(haptic); // Arrête la vibration du périphérique haptique
    if (result != 0)
    {
        // Affiche un message d'erreur si la vibration n'a pas pu être arrêtée
        rc2d_log(RC2D_LOG_ERROR, "Impossible d'arrêter la vibration : %s.", SDL_GetError());
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
        rc2d_log(RC2D_LOG_WARN, "Impossible de jouer la vibration : paramètres invalides dans rc2d_system_vibrate().");
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
                rc2d_log(RC2D_LOG_ERROR, "Impossible d'initialiser la vibration %s.", SDL_GetError());
            }
            else
            {
                // Jouer la vibration
                result = SDL_HapticRumblePlay(haptic, strength, milliseconds);
                if (result != 0)
                {
                    // Afficher un message d'erreur si la vibration n'a pas pu être jouée
                    rc2d_log(RC2D_LOG_ERROR, "Impossible de jouer la vibration %s.", SDL_GetError());
                }
                else
                {
                    // Planifier l'arrêt de la vibration après la durée spécifiée
                    SDL_TimerID timerID = SDL_AddTimer(milliseconds, stopVibration, haptic);
                    if (timerID == 0)
                    {
                        // Afficher un message d'erreur si la temporisation a échoué
                        rc2d_log(RC2D_LOG_ERROR, "Impossible de planifier l'arrêt de la vibration %s.", SDL_GetError());
                    }
                }
            }
        }
        else 
        {
            // Afficher un message d'erreur si le périphérique ne prend pas en charge la vibration
            rc2d_log(RC2D_LOG_ERROR, "Le périphérique ne prend pas en charge la vibration.");
        }
    } 
    else 
    {
        // Afficher un message d'erreur si le périphérique haptique n'a pas pu être ouvert
        rc2d_log(RC2D_LOG_ERROR, "Impossible d'ouvrir le dispositif haptique %s.", SDL_GetError());
    }
}

/**
 * Vérifie si du texte est actuellement stocké dans le presse-papiers.
 * @return Retourne vrai si du texte est présent, faux sinon.
 */
bool rc2d_system_hasClipboardText(void)
{
	if (SDL_HasClipboardText() == SDL_TRUE)
		return true;
	else
		return false;
}

/**
 * Récupère le texte actuellement stocké dans le presse-papiers du système.
 * @return Un pointeur vers une chaîne de caractères contenant le texte. Doit être libéré avec rc2d_system_freeClipboardText().
 */
char* rc2d_system_getClipboardText(void)
{
	char* clipboardText = SDL_GetClipboardText();
    if (clipboardText == NULL)
    {
        rc2d_log(RC2D_LOG_WARN, "Impossible de récupérer le texte du presse-papiers : %s dans rc2d_system_getClipboardText().", SDL_GetError());
    }

    return clipboardText;
}

/**
 * Place une chaîne de caractères dans le presse-papiers du système.
 * @param text Le texte à stocker dans le presse-papiers.
 */
void rc2d_system_setClipboardText(const char* text)
{
    if (text == NULL)
    {
        rc2d_log(RC2D_LOG_WARN, "Impossible de copier le texte dans le presse-papiers : le texte est NULL dans rc2d_system_setClipboardText().");
        return;
    }

	int result = SDL_SetClipboardText(text);
    if (result != 0)
    {
        rc2d_log(RC2D_LOG_WARN, "Impossible de copier le texte dans le presse-papiers : %s dans rc2d_system_setClipboardText().", SDL_GetError());
    }
}

/**
 * Libère la mémoire allouée par SDL_GetClipboardText().
 * @param text Le pointeur vers la chaîne de caractères à libérer.
 */
void rc2d_system_freeClipboardText(char* text)
{
    if (text != NULL)
    {
        SDL_free(text);
        text = NULL;
    }
}

/**
 * Obtient l'état actuel de l'alimentation du système.
 * @return Un enum représentant l'état de l'alimentation, par exemple batterie faible, en charge, etc.
 */
RC2D_PowerState rc2d_system_getPowerInfo(void)
{
	SDL_PowerState powerState = SDL_GetPowerInfo(NULL, NULL);

    switch (powerState)
    {
        case SDL_POWERSTATE_UNKNOWN:
            return RC2D_POWERSTATE_UNKNOWN;
        case SDL_POWERSTATE_ON_BATTERY:
            return RC2D_POWERSTATE_ON_BATTERY;
        case SDL_POWERSTATE_NO_BATTERY:
            return RC2D_POWERSTATE_NO_BATTERY;
        case SDL_POWERSTATE_CHARGING:
            return RC2D_POWERSTATE_CHARGING;
        case SDL_POWERSTATE_CHARGED:
            return RC2D_POWERSTATE_CHARGED;
        default:
            return RC2D_POWERSTATE_UNKNOWN;
    }
}

/**
 * Récupère le nombre de cœurs de processeur (physiques ou logiques) disponibles sur le système.
 * @return Le nombre de cœurs de processeur disponibles.
 */
int rc2d_system_getProcessorCount(void)
{
	return SDL_GetCPUCount();
}

/**
 * Obtient la quantité de RAM système installée en Mo.
 * @return La quantité de RAM en Mo.
 */
int rc2d_system_getRAM(void)
{
	return SDL_GetSystemRAM();
}

/**
 * Retourne une chaîne identifiant le système d'exploitation sur lequel l'application est en cours d'exécution.
 * @return La chaîne représentant le système d'exploitation (par exemple, "Windows", "Mac OS X", "Linux", "Android", "iOS", "Uknown").
 */
const char* rc2d_system_getOS(void)
{
	return SDL_GetPlatform();
}

/**
 * Ouvre une URL dans le navigateur web par défaut de l'utilisateur ou dans une autre application externe appropriée.
 * @param url L'URL à ouvrir.
 */
void rc2d_system_openURL(const char* url)
{
    if (url == NULL)
    {
        rc2d_log(RC2D_LOG_WARN, "Impossible d'ouvrir l'URL : l'URL est NULL dans rc2d_system_openURL().");
        return;
    }

	int result = SDL_OpenURL(url);
    if (result != 0)
    {
        rc2d_log(RC2D_LOG_WARN, "Impossible d'ouvrir l'URL : %s dans rc2d_system_openURL().", SDL_GetError());
    }
}