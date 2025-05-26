#ifndef RC2D_ENGINE_H
#define RC2D_ENGINE_H

#include <RC2D/RC2D_gpu.h>
#include <RC2D/RC2D_window.h>
#include <RC2D/RC2D_local.h>
#include <RC2D/RC2D_touch.h>
#include <RC2D/RC2D_camera.h>

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Modes d'affichage pour le contenu du letterbox.
 * 
 * Définit comment les zones de letterbox (marges visibles autour du rendu principal)
 * doivent être remplies lorsque la résolution logique n'occupe pas toute la fenêtre.
 * 
 * \note Ces zones apparaissent notamment en cas de mise à l’échelle non proportionnelle
 * ou de ratio d’aspect incompatible (par ex. 16:9 dans une fenêtre 4:3).
 * Ce système permet d’améliorer l’esthétique du rendu sur tous les écrans.
 * 
 * \since Cette enum est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_LetterboxMode {
    /**
     * Aucune texture ni remplissage : les marges ne sont pas dessinées, donc noires.
     * 
     * Cela signifie que si le contenu ne remplit pas l'écran, les zones de letterbox
     * seront noires par défaut.
     */
    RC2D_LETTERBOX_NONE,

    /**
     * Une seule texture est utilisée pour tous les bords.
     * 
     * Cela signifie que la même texture sera appliquée sur le haut, le bas, la gauche
     * et la droite de l'écran, créant un effet uniforme.
     */
    RC2D_LETTERBOX_UNIFORM,

    /**
     * Chaque bord (haut, bas, gauche, droite) peut avoir sa propre texture.
     * 
     * Cela permet une personnalisation plus poussée, où chaque bord peut afficher
     * une texture différente pour un effet visuel unique.
     */
    RC2D_LETTERBOX_PER_SIDE,

    /**
     * Une grande texture unique est affichée derrière toute la scène, 
     * incluant les zones letterbox et le rendu principal.
     * 
     * Cela signifie que la texture remplira l'arrière-plan de l'écran, y compris les zones de letterbox,
     * créant un effet d'immersion totale.
     */
    RC2D_LETTERBOX_BACKGROUND_FULL
} RC2D_LetterboxMode;

/**
 * \brief Textures de remplissage pour les marges du letterbox/pillarbox.
 * 
 * Cette structure permet de définir le comportement visuel des marges 
 * (haut, bas, gauche, droite) en cas de mise à l’échelle logique avec letterboxing.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_LetterboxTextures {
    /**
     * Mode d’affichage utilisé pour remplir les zones de letterbox.
     * 
     * - `RC2D_LETTERBOX_NONE` : pas de remplissage, marges noires.
     * - `RC2D_LETTERBOX_UNIFORM` : une seule texture sur tous les bords.
     * - `RC2D_LETTERBOX_PER_SIDE` : textures différentes pour chaque bord.
     * - `RC2D_LETTERBOX_BACKGROUND_FULL` : texture géante en arrière-plan.
     */
    RC2D_LetterboxMode mode;

    /**
     * Texture unique utilisée sur tous les côtés (mode `RC2D_LETTERBOX_UNIFORM`).
     */
    RC2D_GPUTexture* uniform;

    /**
     * Texture spécifique pour chaque côtés (mode `RC2D_LETTERBOX_PER_SIDE`).
     */
    RC2D_GPUTexture* top;
    RC2D_GPUTexture* bottom;
    RC2D_GPUTexture* left;
    RC2D_GPUTexture* right;

    /**
     * Texture géante utilisée en arrière-plan total (mode `RC2D_LETTERBOX_BACKGROUND_FULL`).
     */
    RC2D_GPUTexture* background;
} RC2D_LetterboxTextures;

/**
 * \brief Définit comment le contenu du jeu est affiché à l'écran.
 * 
 * \since Cette enum est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_PresentationMode {
    /**
     * Mode pixel art : mise à l’échelle entière (INTEGER_SCALE) + letterbox.
     * Idéal pour les jeux rétro, garantissant des pixels nets et une esthétique pixelisée.
     */
    RC2D_PRESENTATION_PIXELART,

    /**
     * Mode classique : mise à l’échelle fluide + letterbox.
     * Idéal pour les jeux modernes, s’adaptant à l’écran tout en préservant le ratio d’aspect.
     */
    RC2D_PRESENTATION_CLASSIC
} RC2D_PresentationMode;

/**
 * \brief Informations sur l'application.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_AppInfo {
    /**
     * Nom de l'application.
     */
    const char* name;

    /**
     * Version de l'application.
     */
    const char* version;

    /**
     * Identifiant de l'application.
     */
    const char* identifier;
} RC2D_AppInfo;

/**
 * \brief Informations relatives à un événement de mise à jour du presse-papiers.
 * 
 * Cette structure contient les informations transmises à la callback `rc2d_clipboardupdated`
 * lorsqu'un changement du presse-papiers est détecté.
 * Elle permet à l'utilisateur de savoir si le changement vient de son propre programme,
 * et d'accéder à la liste des types MIME actuellement disponibles dans le presse-papiers.
 * 
 * \note Cette structure est construite à partir de `SDL_ClipboardEvent` introduite dans SDL 3.2.0.
 * Elle n’inclut que les champs les plus utiles pour l’utilisateur final.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_clipboardupdated
 */
typedef struct RC2D_ClipboardEventInfo {
    /**
     * Indique si l’application actuelle est à l’origine de la mise à jour du presse-papiers.
     * 
     * - `true` : la mise à jour du presse-papiers provient de l’application elle-même
     * - `false` : la mise à jour provient d’un autre programme (copie externe)
     */
    bool is_owner;

    /**
     * Nombre total de types MIME actuellement disponibles dans le presse-papiers.
     * 
     * Par exemple :
     * - 1 pour `"text/plain"`
     * - 2 pour `"text/plain"` + `"text/html"`
     */
    int num_mime_types;

    /**
     * Tableau de chaînes représentant les types MIME disponibles dans le presse-papiers.
     * 
     * Ce tableau est uniquement valide pendant la durée de l’événement.
     * Il ne doit **pas** être modifié ou libéré par l’utilisateur.
     *
     * \note Ce tableau peut être NULL si `num_mime_types` vaut 0.
     *
     * Exemples de valeurs possibles :
     * - `"text/plain"`
     * - `"text/html"`
     * - `"image/png"`
     */
    const char** mime_types;
} RC2D_ClipboardEventInfo;

/**
 * \brief Informations relatives à un événement concernant un appareil photo.
 * 
 * Cette structure est transmise aux callbacks suivantes :
 * - `rc2d_cameraadded`
 * - `rc2d_cameraremoved`
 * - `rc2d_cameraapproved`
 * - `rc2d_cameradenied`
 * 
 * Elle permet d’identifier précisément l’appareil photo concerné par l’événement.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_cameraadded
 * \see rc2d_cameraremoved
 * \see rc2d_cameraapproved
 * \see rc2d_cameradenied
 */
typedef struct RC2D_CameraEventInfo {
    /**
     * Identifiant unique de l’appareil photo concerné.
     * 
     * \note Cet identifiant est stable tant que l'appareil reste connecté.
     */
    SDL_CameraID deviceID;
} RC2D_CameraEventInfo;

/**
 * \brief Informations relatives à un événement tactile.
 * 
 * Cette structure représente les données d’un toucher (appui, déplacement, relâchement, annulation),
 * en provenance d’un écran tactile ou d’un trackpad.
 * 
 * Les coordonnées sont normalisées dans la fenêtre entre 0.0f et 1.0f.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_TouchEventInfo {
    /**
     * Identifiant du périphérique tactile (écran, trackpad...).
     */
    SDL_TouchID touchID;

    /**
     * Identifiant du doigt ou point de contact.
     */
    SDL_FingerID fingerID;

    /**
     * Position X normalisée dans la fenêtre (0.0f = gauche, 1.0f = droite).
     */
    float x;

    /**
     * Position Y normalisée dans la fenêtre (0.0f = haut, 1.0f = bas).
     */
    float y;

    /**
     * Déplacement relatif en X depuis le dernier événement.
     * Valeur normalisée entre -1.0f et 1.0f.
     */
    float dx;

    /**
     * Déplacement relatif en Y depuis le dernier événement.
     * Valeur normalisée entre -1.0f et 1.0f.
     */
    float dy;

    /**
     * Pression normalisée (entre 0.0f et 1.0f).
     */
    float pressure;
} RC2D_TouchEventInfo;

/**
 * Structure représentant les fonctions de rappel pour l'API Engine.
 * @typedef {struct} RC2D_Callbacks
 * @property {function} rc2d_unload - Fonction appelée lors du déchargement du jeu.
 * @property {function} rc2d_load - Fonction appelée lors du chargement du jeu.
 * @property {function} rc2d_update - Fonction appelée pour mettre à jour le jeu.
 * @property {function} rc2d_draw - Fonction appelée pour dessiner le jeu.
 * @property {function} rc2d_keypressed - Fonction appelée lorsqu'une touche est enfoncée.
 * @property {function} rc2d_keyreleased - Fonction appelée lorsqu'une touche est relâchée.
 * @property {function} rc2d_mousemoved - Fonction appelée lorsque la souris est déplacée.
 * @property {function} rc2d_mousepressed - Fonction appelée lorsqu'un bouton de la souris est enfoncé.
 * @property {function} rc2d_mousereleased - Fonction appelée lorsqu'un bouton de la souris est relâché.
 * @property {function} rc2d_wheelmoved - Fonction appelée lorsqu'une molette de la souris est déplacée.
 * @property {function} rc2d_touchmoved - Fonction appelée lorsqu'un toucher est déplacé.
 * @property {function} rc2d_touchpressed - Fonction appelée lorsqu'un toucher est enfoncé.
 * @property {function} rc2d_touchreleased - Fonction appelée lorsqu'un toucher est relâché.
 * @property {function} rc2d_gamepadaxis - Fonction appelée lorsqu'un axe de la manette est déplacé.
 * @property {function} rc2d_gamepadpressed - Fonction appelée lorsqu'un bouton de la manette est enfoncé.
 * @property {function} rc2d_gamepadreleased - Fonction appelée lorsqu'un bouton de la manette est relâché.
 * @property {function} rc2d_joystickaxis - Fonction appelée lorsqu'un axe du joystick est déplacé.
 * @property {function} rc2d_joystickhat - Fonction appelée lorsqu'un chapeau du joystick est déplacé.
 * @property {function} rc2d_joystickpressed - Fonction appelée lorsqu'un bouton du joystick est enfoncé.
 * @property {function} rc2d_joystickreleased - Fonction appelée lorsqu'un bouton du joystick est relâché.
 * @property {function} rc2d_joystickadded - Fonction appelée lorsqu'un joystick est ajouté.
 * @property {function} rc2d_joystickremoved - Fonction appelée lorsqu'un joystick est retiré.
 * @property {function} rc2d_dropfile - Fonction appelée lorsqu'un fichier est déposé.
 * @property {function} rc2d_droptext - Fonction appelée lorsqu'un texte est déposé.
 * @property {function} rc2d_dropbegin - Fonction appelée au début du dépôt.
 * @property {function} rc2d_dropcomplete - Fonction appelée à la fin du dépôt.
 * @property {function} rc2d_windowresized - Fonction appelée lorsque la fenêtre est redimensionnée.
 * @property {function} rc2d_windowmoved - Fonction appelée lorsque la fenêtre est déplacée.
 * @property {function} rc2d_windowsizedchanged - Fonction appelée lorsque la taille de la fenêtre est modifiée.
 * @property {function} rc2d_windowexposed - Fonction appelée lorsque la fenêtre est exposée.
 * @property {function} rc2d_windowminimized - Fonction appelée lorsque la fenêtre est réduite.
 * @property {function} rc2d_windowmaximized - Fonction appelée lorsque la fenêtre est maximisée.
 * @property {function} rc2d_windowrestored - Fonction appelée lorsque la fenêtre est restaurée.
 * @property {function} rc2d_mouseenteredwindow - Fonction appelée lorsque la souris entre dans la fenêtre.
 * @property {function} rc2d_mouseleavewindow - Fonction appelée lorsque la souris quitte la fenêtre.
 * @property {function} rc2d_keyboardfocusgained - Fonction appelée lorsque le focus clavier est obtenu.
 * @property {function} rc2d_keyboardfocuslost - Fonction appelée lorsque le focus clavier est perdu.
 * @property {function} rc2d_windowclosed - Fonction appelée lorsque la fenêtre est fermée.
 * @property {function} rc2d_windowtakefocus - Fonction appelée lorsque la fenêtre prend le focus.
 * @property {function} rc2d_windowhittest - Fonction appelée pour le test de collision de la fenêtre.
 */
typedef struct RC2D_Callbacks {
    // Game Loop Callbacks
    void (*rc2d_unload)(void);
    void (*rc2d_load)(void);
    void (*rc2d_update)(double dt);
    void (*rc2d_draw)(void);

    // Keyboard Callbacks
    void (*rc2d_keypressed)(const char* key, bool isrepeat);
    void (*rc2d_keyreleased)(const char* key);

    // Mouse Callbacks
    void (*rc2d_mousemoved)(int x, int y);
    void (*rc2d_mousepressed)(int x, int y, const char* button, int presses);
    void (*rc2d_mousereleased)(int x, int y, const char* button, int presses);
    void (*rc2d_wheelmoved)(const char* scroll);

    // Gamepad and Joystick Callbacks -> TODO: A revoir
    /*void (*rc2d_gamepadaxis)(SDL_JoystickID joystick, Uint8 axis, float value);
    void (*rc2d_gamepadpressed)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_gamepadreleased)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_joystickaxis)(SDL_JoystickID joystick, Uint8 axis, float value);
    void (*rc2d_joystickhat)(SDL_JoystickID joystick, Uint8 hat, Uint8 value);
    void (*rc2d_joystickpressed)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_joystickreleased)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_joystickadded)(Sint32 joystick);
    void (*rc2d_joystickremoved)(Sint32 joystick);*/

    // Drag and Drop Callbacks
    void (*rc2d_dropfile)(const char* pathFile);
    void (*rc2d_droptext)(const char* pathFile);
    void (*rc2d_dropbegin)(const char* pathFile);
    void (*rc2d_dropcomplete)(const char* pathFile);

    // Window Callbacks
    // TODO: Rajouter : rc2d_windowpixelsizechanged et rc2d_windowdisplayscalechanged ..etc
    void (*rc2d_windowresized)(int newWidth, int newHeight);
    void (*rc2d_windowmoved)(int x, int y);
    void (*rc2d_windowsizedchanged)(int newWidth, int newHeight);
    void (*rc2d_windowexposed)(void);
    void (*rc2d_windowminimized)(void);
    void (*rc2d_windowmaximized)(void);
    void (*rc2d_windowrestored)(void);
    void (*rc2d_mouseenteredwindow)(void);
    void (*rc2d_mouseleavewindow)(void);
    void (*rc2d_keyboardfocusgained)(void);
    void (*rc2d_keyboardfocuslost)(void);
    void (*rc2d_windowclosed)(void);
    void (*rc2d_windowtakefocus)(void);
    void (*rc2d_windowhittest)(void);


    // ------------- Touch Callbacks ------------- //
    /**
     * \brief Appelée lorsqu’un toucher est déplacé sur l’écran.
     * 
     * Cette fonction permet de suivre le mouvement d’un doigt sur l’écran tactile
     * ou sur un trackpad multitouch.
     * 
     * \param info Données relatives au mouvement du toucher.
     * 
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * 
     * \see RC2D_TouchEventInfo
     */
    void (*rc2d_touchmoved)(const RC2D_TouchEventInfo* info);

    /**
     * \brief Appelée lorsqu’un doigt touche l’écran.
     * 
     * Cette fonction est déclenchée au moment du contact initial d’un doigt
     * avec la surface tactile.
     * 
     * \param info Données relatives à l’appui.
     * 
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * 
     * \see RC2D_TouchEventInfo
     */
    void (*rc2d_touchpressed)(const RC2D_TouchEventInfo* info);

    /**
     * \brief Appelée lorsqu’un doigt est relâché de l’écran.
     * 
     * Elle permet de détecter la fin d’un contact tactile.
     * 
     * \param info Données relatives au relâchement.
     * 
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * 
     * \see RC2D_TouchEventInfo
     */
    void (*rc2d_touchreleased)(const RC2D_TouchEventInfo* info);

    /**
     * \brief Appelée lorsqu’un toucher est annulé.
     * 
     * Cela peut se produire lors d’un changement de focus, d’une fermeture de fenêtre,
     * ou d’un comportement système particulier (ex : appel système, app en arrière-plan).
     * 
     * \param info Données relatives à l’annulation du contact.
     * 
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * 
     * \see RC2D_TouchEventInfo
     */
    void (*rc2d_touchcanceled)(const RC2D_TouchEventInfo* info);


    // -------------- Display Callbacks ------------- //
    /**
     * \brief Appelée lorsqu’un changement d’orientation de l’affichage est détecté.
     * 
     * Cette fonction permet de réagir à une rotation de l’écran (portrait, paysage, etc.),
     * utile sur mobile, tablette ou appareils à écran rotatif.
     * 
     * \param orientation Nouvelle orientation de l’affichage.
     * 
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * 
     * \see RC2D_DisplayOrientation
     */
    void (*rc2d_displayorientationchanged)(RC2D_DisplayOrientation orientation);


    // ------------- Locale Callbacks ------------- //
    /**
     * \brief La préférence de la langue locale a changé.
     * 
     * Cette fonction est appelée lorsque l'utilisateur change la langue locale
     * de l'application via les paramètres système.
     * 
     * \param locales Liste actuelle des locales préférées. 
     * 
     * \note Elle est allouée dynamiquement et doit être libérée avec `rc2d_local_freeLocales()`, mais cela
     * est fait en interne par RC2D, après l'appel de cette fonction.
     * 
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * 
     * \see rc2d_local_getPreferredLocales
     * \see rc2d_local_freeLocales
     */
    void (*rc2d_localechanged)(RC2D_Locale* locales);

    
    // ------------- Clipboard Callbacks ------------- //
    /**
     * \brief Appelée lorsque le contenu du presse-papiers a changé ou la selection principale a changé.
     * 
     * \param info Informations sur le nouveau contenu du presse-papiers.
     * Peut être `NULL` si aucune information supplémentaire n’est fournie.
     * 
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * 
     * \see RC2D_ClipboardInfo
     */
    void (*rc2d_clipboardupdated)(const RC2D_ClipboardEventInfo* info);


    // ------------- Camera Callbacks ------------- //
    /**
     * \brief Callback appelée lorsqu’un nouvel appareil photo est détecté.
     * 
     * Cette fonction est appelée lorsqu’un nouveau périphérique caméra est disponible sur le système.
     * 
     * \param {RC2D_CameraEventInfo*} info - Informations sur l'appareil photo ajouté.
     */
    void (*rc2d_cameraadded)(const RC2D_CameraEventInfo* info);

    /**
     * \brief Callback appelée lorsqu’un appareil photo a été retiré du système.
     * 
     * Cette fonction permet de détecter la déconnexion d’un périphérique caméra.
     * 
     * \param {RC2D_CameraEventInfo*} info - Informations sur l'appareil photo supprimé.
     */
    void (*rc2d_cameraremoved)(const RC2D_CameraEventInfo* info);

    /**
     * \brief Callback appelée lorsque l’utilisateur autorise l’accès à une caméra.
     * 
     * Cela se produit lorsque l’utilisateur approuve une demande d’accès à la caméra,
     * généralement via une fenêtre système ou une autorisation explicite.
     * 
     * \param {RC2D_CameraEventInfo*} info - Informations sur l'appareil photo autorisé.
     */
    void (*rc2d_cameraapproved)(const RC2D_CameraEventInfo* info);

    /**
     * \brief Callback appelée lorsque l’utilisateur refuse l’accès à une caméra.
     * 
     * Cette fonction permet de réagir à un refus d’autorisation (ex: permission système).
     * 
     * \param {RC2D_CameraEventInfo*} info - Informations sur l'appareil photo refusé.
     */
    void (*rc2d_cameradenied)(const RC2D_CameraEventInfo* info);
} RC2D_EngineCallbacks;

/**
 * \brief Configuration de l'application RC2D.
 * 
 * Cette structure contient les paramètres de configuration pour l'application
 * utilisant la bibliothèque RC2D. Elle permet de personnaliser le comportement
 * et l'apparence de l'application au démarrage.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_EngineConfig {
    /**
     * Callbacks utilisateur pour gérer les événements.
     * Par défaut : NULL.
     */
    RC2D_EngineCallbacks* callbacks;

    /**
     * Largeur initiale de la fenêtre (pixels). 
     * Par défaut : 800.
     */
    int windowWidth;

    /**
     * Hauteur initiale de la fenêtre (pixels). 
     * Par défaut : 600.
     */
    int windowHeight;

    /**
     * Largeur logique (par exemple, 1920 pour CLASSIC, 640 pour PIXELART). 
     * Par défaut : 1920.
     */
    int logicalWidth;

    /**
     * Hauteur logique (par exemple, 1080 pour CLASSIC, 360 pour PIXELART). 
     * Par défaut : 1080.
     */
    int logicalHeight;

    /**
     * Mode de présentation (PIXELART ou CLASSIC).
     * Par défaut : CLASSIC.
     */
    RC2D_PresentationMode presentationMode;

    /**
     * Textures pour les bordures décoratives (letterbox/pillarbox).
     * Par défaut : NULL.
     */
    RC2D_LetterboxTextures* letterboxTextures;

    /**
     * Informations sur l'application (nom, version, identifier).
     * Par défaut :
     * - name : "RC2D Game"
     * - version : "1.0.0"
     * - identifier : "com.example.rc2dgame"
     */
    RC2D_AppInfo* appInfo;

    /**
     * Nombre d'images autorisées en vol sur le GPU (1, 2 ou 3).
     * Par défaut : RC2D_GPU_FRAMES_BALANCED (2).
     */
    RC2D_GPUFramesInFlight gpuFramesInFlight;

    /**
     * Options avancées pour la création du contexte GPU.
     * Par défaut :
     * - debugMode : true
     * - verbose : true
     * - preferLowPower : false
     * - driver : RC2D_GPU_DRIVER_DEFAULT
     */
    RC2D_GPUAdvancedOptions* gpuOptions;
} RC2D_EngineConfig;

/**
 * \brief Cette fonction DOIT être définie par l'utilisateur, si elle est absente, l'éditeur de liens renverra une erreur.
 *
 * \note Si vous souhaitez utiliser la configuration par défaut de RC2D, vous pouvez retourner NULL.
 *
 * \param {int} argc - Nombre d'arguments de la ligne de commande.
 * \param {char**} argv - Arguments de la ligne de commande.
 * \return {RC2D_Config*} Un pointeur vers la configuration personnalisée, ou NULL pour utiliser la config par défaut.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_engine_getDefaultConfig
 */
const RC2D_EngineConfig* rc2d_engine_setup(int argc, char* argv[]);

/**
 * \brief Renvoie une configuration RC2D_EngineConfig avec toutes les valeurs par défaut.
 * 
 * Cette fonction peut être utilisée par les utilisateurs qui souhaitent démarrer
 * avec une configuration standard avant de personnaliser certains champs.
 *
 * \return {RC2D_EngineConfig*} - Structure de configuration avec les valeurs par défaut.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_engine_setup
 */
RC2D_EngineConfig* rc2d_engine_getDefaultConfig(void);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_ENGINE_H