#include <RC2D/RC2D_joystick.h>
#include <RC2D/RC2D_logger.h>

#define RC2D_JOYSTICK_AXIS_MIN -32768
#define RC2D_JOYSTICK_AXIS_MAX 32767

/**
 * Normalise la valeur d'un axe du joystick.
 * 
 * @param {number} value - La valeur de l'axe du joystick à normaliser.
 * @returns {float} La valeur normalisée de l'axe du joystick.
 */
static float rc2d_joystick_normalizeAxisValue(int value) 
{
    return (value - RC2D_JOYSTICK_AXIS_MIN) / (float)(RC2D_JOYSTICK_AXIS_MAX - RC2D_JOYSTICK_AXIS_MIN) * 2.0f - 1.0f;
}

// TODO: rc2d_joystick_getAxes() a faire

/**
 * Obtient la direction d'un axe spécifique d'un joystick.
 *
 * @param joystickID L'identifiant du joystick.
 * @param axisIndex L'index de l'axe à interroger.
 * @return La direction de l'axe spécifié, normalisée entre -1.0 et 1.0.
 */
float rc2d_joystick_getAxis(int joystickID, int axisIndex) 
{
    // Ouvre le joystick pour l'accès
    SDL_Joystick* joystick = SDL_JoystickOpen(joystickID);
    if (!joystick) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ouvrir le joystick dans rc2d_joystick_getAxis : %s", SDL_GetError());
        return 0.0f; // Retourne 0.0 si le joystick n'est pas accessible
    }

    // Obtient la valeur actuelle de l'axe
    int axisValue = SDL_JoystickGetAxis(joystick, axisIndex);
    if (axisValue == 0) 
    {
        // Si la valeur es 0 : on failure
        RC2D_log(RC2D_LOG_DEBUG, "SDL_JoystickGetAxis error in rc2d_joystick_getAxis : %s", SDL_GetError());
        return 0.0f;
    }

    // Normalise la valeur de l'axe pour qu'elle soit comprise entre -1.0 et 1.0
    float direction = (float)axisValue / RC2D_JOYSTICK_AXIS_MAX;

    // Ferme le joystick après l'accès
    SDL_JoystickClose(joystick);

    return direction;
}

/**
 * Obtient le nombre d'axes d'un joystick.
 *
 * @param joystickID L'identifiant du joystick.
 * @return Le nombre d'axes du joystick spécifié.
 */
int rc2d_joystick_getAxisCount(const int joystickID) 
{
    // Ouvre le joystick pour l'accès
    SDL_Joystick* joystick = SDL_JoystickOpen(joystickID);
    if (!joystick) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ouvrir le joystick %s", SDL_GetError());
        return 0; // Retourne 0 si le joystick n'est pas accessible
    }

    // Obtient le nombre d'axes du joystick
    int axisCount = SDL_JoystickNumAxes(joystick);
    if (axisCount != 0) 
    {
        RC2D_log(RC2D_LOG_DEBUG, "SDL_JoystickNumAxes error in rc2d_joystick_getAxisCount : ", SDL_GetError());
        return 0;
    }

    // Ferme le joystick après l'accès
    SDL_JoystickClose(joystick);

    return axisCount;
}

/**
 * Obtient le nombre de boutons d'un joystick.
 *
 * Cette fonction ouvre un joystick en utilisant son identifiant,
 * interroge SDL pour connaître le nombre de boutons du joystick,
 * puis ferme le joystick et retourne ce nombre.
 *
 * @param joystickID L'identifiant du joystick.
 * @return Le nombre de boutons du joystick spécifié.
 */
int rc2d_joystick_getButtonCount(const int joystickID) 
{
    // Ouvre le joystick pour l'accès
    SDL_Joystick* joystick = SDL_JoystickOpen(joystickID);
    if (!joystick) 
    {
        // Si le joystick ne peut pas être ouvert, log une erreur
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ouvrir le joystick dans rc2d_joystick_getButtonCount : %s", SDL_GetError());
        return 0; // Retourne 0 si le joystick n'est pas accessible
    }

    // Obtient le nombre de boutons du joystick
    int buttonCount = SDL_JoystickNumButtons(joystick);
    if (buttonCount < 0) 
    {
        RC2D_log(RC2D_LOG_DEBUG, "SDL_JoystickNumButtons error in rc2d_joystick_getButtonCount : %s", SDL_GetError());
        return 0;
    }

    // Ferme le joystick après l'accès
    SDL_JoystickClose(joystick);

    return buttonCount;
}

/**
 * Obtient les informations de l'appareil pour un joystick spécifié.
 *
 * Cette fonction ouvre un joystick en utilisant son identifiant,
 * interroge SDL pour obtenir l'ID du vendeur USB, l'ID du produit
 * et le numéro de version du produit, puis ferme le joystick.
 *
 * @param joystickID L'identifiant du joystick.
 * @param vendorID Pointeur vers la variable où l'ID du vendeur sera stocké.
 * @param productID Pointeur vers la variable où l'ID du produit sera stocké.
 * @param productVersion Pointeur vers la variable où le numéro de version du produit sera stocké.
 */
void rc2d_joystick_getDeviceInfo(int joystickID, int* vendorID, int* productID, int* productVersion) 
{
    if (vendorID == NULL || productID == NULL || productVersion == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Invalid parameters in rc2d_joystick_getDeviceInfo");
        return;
    }

    // Ouvre le joystick pour l'accès
    SDL_Joystick* joystick = SDL_JoystickOpen(joystickID);
    if (!joystick) 
    {
        // Si le joystick ne peut pas être ouvert, log une erreur
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ouvrir le joystick dans rc2d_joystick_getDeviceInfo : %s", SDL_GetError());

        *vendorID = 0;
        *productID = 0;
        *productVersion = 0;

        return; // Retourne des zéros si le joystick n'est pas accessible
    }

    // Obtient l'ID du vendeur, l'ID du produit, et le numéro de version du produit
    *vendorID = SDL_JoystickGetVendor(joystick);
    *productID = SDL_JoystickGetProduct(joystick);
    *productVersion = SDL_JoystickGetProductVersion(joystick);

    // Ferme le joystick après l'accès
    SDL_JoystickClose(joystick);
}


// TODO: A faire -> getGUID

// TODO: A faire -> getGamepadAxis

/**
 * Obtient la direction d'un chapeau (hat) spécifique du joystick.
 *
 * @param joystickID L'identifiant du joystick.
 * @param hatIndex L'index du chapeau à vérifier.
 * @return La direction vers laquelle le chapeau est poussé, utilisant l'énumération RC2D_JoystickHat.
 */
RC2D_JoystickHat rc2d_joystick_getHat(const int joystickID, const int hatIndex) 
{
    // Ouvre le joystick pour l'accès
    SDL_Joystick* joystick = SDL_JoystickOpen(joystickID);
    if (!joystick) 
    {
        // Enregistre une erreur si le joystick ne peut pas être ouvert
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ouvrir le joystick dans rc2d_joystick_getHat : %s", SDL_GetError());
        return RC2D_JOYSTICK_HAT_CENTERED; // Retourne centré si le joystick n'est pas accessible
    }

    // Obtient la valeur actuelle du chapeau
    Uint8 hatValue = SDL_JoystickGetHat(joystick, hatIndex);
    
    // Ferme le joystick après l'accès
    SDL_JoystickClose(joystick);
    
    // Convertit la valeur du chapeau en énumération RC2D_JoystickHat et retourne
    switch (hatValue)
    {
        case SDL_HAT_UP:
            return RC2D_JOYSTICK_HAT_UP;
        case SDL_HAT_RIGHT:
            return RC2D_JOYSTICK_HAT_RIGHT;
        case SDL_HAT_DOWN:
            return RC2D_JOYSTICK_HAT_DOWN;
        case SDL_HAT_LEFT:
            return RC2D_JOYSTICK_HAT_LEFT;
        case SDL_HAT_RIGHTUP:
            return RC2D_JOYSTICK_HAT_RIGHTUP;
        case SDL_HAT_RIGHTDOWN:
            return RC2D_JOYSTICK_HAT_RIGHTDOWN;
        case SDL_HAT_LEFTUP:
            return RC2D_JOYSTICK_HAT_LEFTUP;
        case SDL_HAT_LEFTDOWN:
            return RC2D_JOYSTICK_HAT_LEFTDOWN;
        default:
            return RC2D_JOYSTICK_HAT_CENTERED;
    }
}

/**
 * Obtient le nombre de chapeaux (hats) d'un joystick.
 *
 * @param joystickID L'identifiant du joystick.
 * @return Le nombre de chapeaux sur le joystick spécifié.
 */
int rc2d_joystick_getHatCount(const int joystickID) 
{
    // Ouvre le joystick pour l'accès
    SDL_Joystick* joystick = SDL_JoystickOpen(joystickID);
    if (!joystick) 
    {
        // Enregistre une erreur si le joystick ne peut pas être ouvert
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ouvrir le joystick dans rc2d_joystick_getHatCount : %s", SDL_GetError());
        return 0; // Retourne 0 si le joystick n'est pas accessible
    }

    // Obtient le nombre de chapeaux du joystick
    int hatCount = SDL_JoystickNumHats(joystick);
    if (hatCount < 0) 
    {
        RC2D_log(RC2D_LOG_DEBUG, "SDL_JoystickNumHats error in rc2d_joystick_getHatCount : %s", SDL_GetError());
        return 0;
    }

    // Ferme le joystick après l'accès
    SDL_JoystickClose(joystick);

    return hatCount;
}

/**
 * Obtient l'identifiant d'instance d'un joystick.
 *
 * @param joystickID L'index du joystick, servant à identifier le joystick concerné.
 * @return L'identifiant d'instance du joystick, ou -1 si le joystick n'est pas connecté ou non reconnu.
 */
int rc2d_joystick_getInstanceId(const int joystickID) 
{
    SDL_Joystick* joystick = SDL_JoystickOpen(joystickID);
    if (!joystick) 
    {
        // Enregistre une erreur si le joystick ne peut pas être ouvert
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ouvrir le joystick dans rc2d_joystick_getInstanceId : %s", SDL_GetError());
        return -1; // Indique que le joystick n'est pas connecté ou non reconnu
    }

    // Obtient l'identifiant d'instance du joystick
    int instanceId = SDL_JoystickInstanceID(joystick);
    if (instanceId < 0) 
    {
        RC2D_log(RC2D_LOG_DEBUG, "SDL_JoystickInstanceID error in rc2d_joystick_getInstanceId : %s", SDL_GetError());
        return -1;
    }

    // Ferme le joystick après l'accès
    SDL_JoystickClose(joystick);

    return instanceId;
}

/**
 * Obtient le nom d'un joystick.
 *
 * @param joystickID L'index du joystick, servant à identifier le joystick concerné.
 * @return Le nom du joystick, ou NULL si le joystick n'est pas connecté ou non reconnu.
 */
const char* rc2d_joystick_getName(const int joystickID) 
{
    SDL_Joystick* joystick = SDL_JoystickOpen(joystickID);
    if (!joystick) 
    {
        // Enregistre une erreur si le joystick ne peut pas être ouvert
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ouvrir le joystick dans rc2d_joystick_getName : %s", SDL_GetError());
        return NULL; // Indique que le joystick n'est pas connecté ou non reconnu
    }

    const char* name = SDL_JoystickName(joystick);
    if (name == NULL) 
    {
        RC2D_log(RC2D_LOG_DEBUG, "SDL_JoystickName error in rc2d_joystick_getName : %s", SDL_GetError());
        return NULL;
    }

    // Ferme le joystick après l'accès
    SDL_JoystickClose(joystick);

    return name;
}


// TODO: getVibration a faire


/**
 * Vérifie si un joystick est connecté.
 *
 * @param joystickID L'identifiant du joystick.
 * @return true si le joystick est actuellement connecté, false sinon.
 */
bool rc2d_joystick_isConnected(const int joystickID) 
{
    SDL_Joystick* joystick = SDL_JoystickOpen(joystickID);
    if (!joystick) 
    {
        // Si le joystick ne peut pas être ouvert, considère qu'il n'est pas connecté
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ouvrir le joystick dans rc2d_joystick_isConnected : %s", SDL_GetError());
        return false;
    }

    bool isConnected = SDL_JoystickGetAttached(joystick);
    if (!isConnected) 
    {
        // Si le joystick est ouvert mais n'est pas attaché, considère qu'il n'est pas connecté
        RC2D_log(RC2D_LOG_WARN, "Le joystick n'est pas attaché dans rc2d_joystick_isConnected : %s", SDL_GetError());
    }
    
    // Ferme le joystick après vérification pour ne pas garder la ressource ouverte inutilement
    SDL_JoystickClose(joystick);

    return isConnected;
}

/**
 * Vérifie si la vibration est prise en charge par le joystick.
 *
 * @param joystickID L'identifiant du joystick.
 * @return true si la vibration est prise en charge, false sinon.
 */
bool rc2d_joystick_isVibrationSupported(const int joystickID) 
{
    SDL_Joystick* joystick = SDL_JoystickOpen(joystickID);
    if (!joystick) 
    {
        // Si le joystick ne peut pas être ouvert, considère qu'il n'est pas connecté
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ouvrir le joystick dans rc2d_joystick_isVibrationSupported : %s", SDL_GetError());
        return false;
    }

    // Tente d'initialiser le système Haptic pour ce joystick
    SDL_Haptic* haptic = SDL_HapticOpenFromJoystick(joystick);
    if (!haptic) 
    {
        // Si le joystick ne peut pas ouvrir un système Haptic, la vibration n'est pas prise en charge
        RC2D_log(RC2D_LOG_WARN, "Le joystick ne prend pas en charge la vibration dans rc2d_joystick_isVibrationSupported : %s", SDL_GetError());
        SDL_JoystickClose(joystick);
        return false;
    }

    // La vibration est prise en charge, ferme le système Haptic et le joystick
    SDL_HapticClose(haptic);
    SDL_JoystickClose(joystick);

    return true;
}


// TODO: setVibration a faire


/**
 * Obtient le nombre de joysticks connectés.
 * 
 * @returns {number} Le nombre de joysticks connectés.
 */
int rc2d_joystick_getJoystickCount(void) 
{
    int count = SDL_NumJoysticks();
    if (count < 0) 
    {
        RC2D_log(RC2D_LOG_ERROR, "SDL_NumJoysticks error in rc2d_joystick_getJoystickCount : %s", SDL_GetError());
        return -1;
    }
}

/*static void rc2d_joystick_loadGamePadMappingFromFile(const char) 
{
	/*
	La base de donnee des gamecontroller ce trouve : 
	https://github.com/mdqinc/SDL_GameControllerDB/blob/master/gamecontrollerdb.txt

	Elle est mis a jour regulierement par la communaute
	/*
	int result = SDL_GameControllerAddMappingsFromRW(pathFileRRES);
	if (result < 0) 
	{
		RC2D_log(RC2D_LOG_ERROR, "Error rc2d_joystick_loadGamePadMapping for mappingString", SDL_GetError());
	}
}*/

/**
 * Obtient une chaîne de mappage de gamepad à partir d'une chaîne de GUID de joystick.
 * 
 * @param {string} guidString - La chaîne de GUID du joystick.
 * @returns {string} La chaîne de mappage du gamepad correspondant au GUID du joystick.
 */
char* rc2d_joystick_getGamepadMappingString(const char* guidString) 
{
    if (guidString == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "Error rc2d_joystick_getGamepadMappingString for GUID string is NULL\n");
        return NULL;
    }

    SDL_JoystickGUID guid = SDL_JoystickGetGUIDFromString(guidString);
    char* mapping = SDL_GameControllerMappingForGUID(guid);
    if (mapping == NULL) 
	{
        // Sinon, le GUID n'est pas reconnu comme un gamepad par SDL
		RC2D_log(RC2D_LOG_ERROR, "Error rc2d_joystick_getGamepadMappingString for guid : %s", SDL_GetError());
        return NULL;
    } 
	
    return mapping;
}

/**
 * Libère la mémoire allouée pour une chaîne de mappage de gamepad.
 * 
 * @param {string} mapping - La chaîne de mappage du gamepad à libérer.
 */
void rc2d_joystick_freeGamepadMappingString(char* mapping) 
{
	if (mapping != NULL)
	{
		SDL_free(mapping);
		mapping = NULL;
	}
}





/***********************************************/


/*          INSTANCE JOYSTICK                  */


/***********************************************/

/**
 * Obtient une liste des structures de joystick pour tous les joysticks connectés.
 * 
 * @returns {RC2D_Joystick[]} Un tableau de structures RC2D_Joystick pour tous les joysticks connectés.
 */
RC2D_Joystick** rc2d_joystick_getJoysticks(void) 
{
    int joystickCount = rc2d_joystick_getJoystickCount();
    if (joystickCount == 0) 
	{
        RC2D_log(RC2D_LOG_WARN, "Aucun joystick connecté dans rc2d_joystick_getJoysticks\n");
        return NULL; // Aucun joystick connecté
    }

    RC2D_Joystick** joysticks = (RC2D_Joystick**)malloc(sizeof(RC2D_Joystick*) * joystickCount);
    if (!joysticks) 
	{
        RC2D_log(RC2D_LOG_ERROR, "Malloc failed in rc2d_joystick_getJoysticks");  
        return NULL;
    }

    for (int i = 0; i < joystickCount; ++i) 
	{
        joysticks[i] = (RC2D_Joystick*)malloc(sizeof(RC2D_Joystick));

        if (!joysticks[i]) 
		{
            RC2D_log(RC2D_LOG_ERROR, "Malloc failed for joystick structure in rc2d_joystick_getJoysticks");

            // Libérer la mémoire allouée précédemment
            for (int j = 0; j < i; ++j) 
			{
                free(joysticks[j]);
            }
			
            free(joysticks);

            return NULL;
        }

        joysticks[i]->sdlJoystick = SDL_JoystickOpen(i);
        if (!joysticks[i]->sdlJoystick)
        {
            RC2D_log(RC2D_LOG_ERROR, "SDL_JoystickOpen error in rc2d_joystick_getJoysticks : %s", SDL_GetError());

            free(joysticks[i]);
            free(joysticks);

            return NULL;
        }

        joysticks[i]->joystickId = SDL_JoystickInstanceID(joysticks[i]->sdlJoystick);
        if (joysticks[i]->joystickId < 0)
        {
            RC2D_log(RC2D_LOG_ERROR, "SDL_JoystickInstanceID error in rc2d_joystick_getJoysticks : %s", SDL_GetError());

            SDL_JoystickClose(joysticks[i]->sdlJoystick);
            free(joysticks[i]);
            free(joysticks);

            return NULL;
        }
    }

    return joysticks;
}

/**
 * Libère la mémoire allouée pour les structures de joystick.
 * 
 * @param {RC2D_Joystick[]} joysticks - Le tableau de structures de joystick à libérer.
 * @param {number} count - Le nombre de structures de joystick dans le tableau.
 */
void rc2d_joystick_freeJoysticks(RC2D_Joystick** joysticks, int count) 
{
    if (!joysticks)
    {
        RC2D_log(RC2D_LOG_WARN, "Attempt to free NULL joystick array in rc2d_joystick_freeJoysticks");
        return;
    }

    for (int i = 0; i < count; ++i) 
	{
        if (joysticks[i]) 
		{
            if (joysticks[i]->sdlJoystick) 
			{
                SDL_JoystickClose(joysticks[i]->sdlJoystick);
            }

            free(joysticks[i]);
        }
    }

    free(joysticks);
}