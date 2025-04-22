#ifndef RC2D_ENGINE_H
#define RC2D_ENGINE_H

#include <RC2D/RC2D_gpu.h>

/* Set up for C function definitions, even when using C++ */
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

    // Touch Callbacks
    void (*rc2d_touchmoved)(SDL_TouchID touchID, SDL_FingerID fingerID, float x, float y, float dx, float dy);
    void (*rc2d_touchpressed)(SDL_TouchID touchID, SDL_FingerID fingerID, float x, float y, float pressure);
    void (*rc2d_touchreleased)(SDL_TouchID touchID, SDL_FingerID fingerID, float x, float y, float pressure);

    // Gamepad and Joystick Callbacks
    void (*rc2d_gamepadaxis)(SDL_JoystickID joystick, Uint8 axis, float value);
    void (*rc2d_gamepadpressed)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_gamepadreleased)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_joystickaxis)(SDL_JoystickID joystick, Uint8 axis, float value);
    void (*rc2d_joystickhat)(SDL_JoystickID joystick, Uint8 hat, Uint8 value);
    void (*rc2d_joystickpressed)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_joystickreleased)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_joystickadded)(Sint32 joystick);
    void (*rc2d_joystickremoved)(Sint32 joystick);

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
} RC2D_Callbacks;

/**
 * \brief Configuration de l'application RC2D.
 * 
 * Cette structure contient les paramètres de configuration pour l'application
 * utilisant la bibliothèque RC2D. Elle permet de personnaliser le comportement
 * et l'apparence de l'application au démarrage.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_Config {
    /**
     * Callbacks utilisateur pour gérer les événements.
     * Par défaut : NULL.
     */
    RC2D_Callbacks* callbacks;

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
} RC2D_Config;

/**
 * \brief Cette fonction DOIT être définie par l'utilisateur, si elle est absente, l'éditeur de liens renverra une erreur.
 *
 * \note Si vous souhaitez utiliser la configuration par défaut de RC2D, vous pouvez retourner NULL.
 *
 * \return {RC2D_Config*} Un pointeur vers la configuration personnalisée, ou NULL pour utiliser la config par défaut.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
const RC2D_Config* rc2d_setup(void);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_ENGINE_H