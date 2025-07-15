#ifndef RC2D_ENGINE_H
#define RC2D_ENGINE_H

#include <RC2D/RC2D_gpu.h>
#include <RC2D/RC2D_window.h>
#include <RC2D/RC2D_local.h>
#include <RC2D/RC2D_touch.h>
#include <RC2D/RC2D_camera.h>
#include <RC2D/RC2D_mouse.h>

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_sensor.h>
#include <SDL3/SDL_keyboard.h>

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

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
 * \brief Informations relatives à un événement de texte en cours d'édition (IME).
 *
 * Cette structure est transmise au callback `rc2d_textediting`.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_TextEditingEventInfo {
    const char *text;   /**< Le texte en cours d'édition (UTF-8). */
    Sint32 start;       /**< Position du curseur dans le texte (en caractères UTF-8). */
    Sint32 length;      /**< Longueur du texte sélectionné à remplacer (en caractères UTF-8). */
    SDL_WindowID windowID; /**< Identifiant de la fenêtre avec le focus clavier. */
} RC2D_TextEditingEventInfo;

/**
 * \brief Informations relatives à un événement de candidats d'édition de texte (IME).
 *
 * Cette structure est transmise au callback `rc2d_texteditingcandidates`.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_TextEditingCandidatesEventInfo {
    const char * const *candidates; /**< Liste des candidats (tableau de chaînes UTF-8). */
    Sint32 num_candidates;          /**< Nombre de candidats dans la liste. */
    Sint32 selected_candidate;      /**< Index du candidat sélectionné, ou -1 si aucun. */
    bool horizontal;                /**< true si la liste est horizontale, false si verticale. */
    SDL_WindowID windowID;          /**< Identifiant de la fenêtre avec le focus clavier. */
} RC2D_TextEditingCandidatesEventInfo;

/**
 * \brief Informations relatives à un événement d'entrée de texte.
 *
 * Cette structure est transmise au callback `rc2d_textinput`.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_TextInputEventInfo {
    const char *text;   /**< Le texte entré (UTF-8). */
    SDL_WindowID windowID; /**< Identifiant de la fenêtre avec le focus clavier. */
} RC2D_TextInputEventInfo;

/**
 * \brief Informations relatives à un événement de périphérique clavier.
 *
 * Cette structure est transmise aux callbacks `rc2d_keyboardadded` et `rc2d_keyboardremoved`.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_KeyboardDeviceEventInfo {
    SDL_KeyboardID keyboardID; /**< Identifiant unique du clavier. */
    const char *name;          /**< Nom du clavier (peut être vide si non disponible). */
} RC2D_KeyboardDeviceEventInfo;

/**
 * \brief Informations relatives à un événement de mise à jour de capteur (par exemple, accéléromètre, gyroscope).
 *
 * Cette structure est transmise au callback `rc2d_sensorupdate`.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_SensorEventInfo {
    SDL_SensorID sensorID;      /**< Identifiant unique du capteur. */
    SDL_SensorType type;        /**< Type de capteur (ex: SDL_SENSOR_ACCEL, SDL_SENSOR_GYRO). */
    const char *name;           /**< Nom du capteur (peut être vide si non disponible). */
    float data[6];              /**< Données du capteur (jusqu'à 6 valeurs, selon le type). */
    Uint64 timestamp;           /**< Horodatage de la lecture du capteur (en nanosecondes). */
} RC2D_SensorEventInfo;

/**
 * \brief Informations relatives à un événement de drag-and-drop.
 *
 * Cette structure est transmise aux callbacks `rc2d_dropbegin`, `rc2d_dropfile`,
 * `rc2d_droptext`, `rc2d_dropcomplete`, et `rc2d_dropposition`. Les champs contiennent
 * des données spécifiques selon le type d'événement, comme décrit ci-dessous.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_DropEventInfo {
    /**
     * Identifiant de la fenêtre cible sur laquelle l'événement de drag-and-drop a lieu.
     * - Valide pour tous les callbacks (`rc2d_dropbegin`, `rc2d_dropfile`, `rc2d_droptext`,
     *   `rc2d_dropcomplete`, `rc2d_dropposition`).
     * - Représente l'identifiant unique de la fenêtre SDL3 (SDL_WindowID).
     */
    SDL_WindowID windowID;

    /**
     * Coordonnée X du curseur relative à la fenêtre, en pixels, au moment du drop.
     * - Pour `rc2d_dropfile` et `rc2d_droptext`: Position où le fichier ou texte a été déposé.
     * - Pour `rc2d_dropposition`: Position actuelle du curseur pendant le survol.
     * - Pour `rc2d_dropbegin` et `rc2d_dropcomplete`: Toujours 0.0f, car non applicable.
     */
    float x;

    /**
     * Coordonnée Y du curseur relative à la fenêtre, en pixels, au moment du drop.
     * - Pour `rc2d_dropfile` et `rc2d_droptext`: Position où le fichier ou texte a été déposé.
     * - Pour `rc2d_dropposition`: Position actuelle du curseur pendant le survol.
     * - Pour `rc2d_dropbegin` et `rc2d_dropcomplete`: Toujours 0.0f, car non applicable.
     */
    float y;

    /**
     * Nom de l'application source ayant initié l'opération de drag-and-drop.
     * - Valide pour tous les callbacks, mais peut être NULL si l'information n'est pas disponible.
     * - Exemple: Nom de l'application (e.g., "Finder" sur macOS, "Explorer" sur Windows).
     */
    const char *source;

    /**
     * Données associées à l'événement de drag-and-drop.
     * - Pour `rc2d_dropfile`: Chemin absolu du fichier déposé (chaîne UTF-8).
     * - Pour `rc2d_droptext`: Texte déposé (chaîne UTF-8, e.g., contenu text/plain).
     * - Pour `rc2d_dropbegin`, `rc2d_dropcomplete`, et `rc2d_dropposition`: Toujours NULL.
     */
    const char *data;

    /**
     * Horodatage de l'événement, en nanosecondes, depuis le démarrage de l'application.
     * - Valide pour tous les callbacks.
     * - Obtenu via SDL_GetTicksNS(), permettant de suivre la chronologie des événements.
     */
    Uint64 timestamp;
} RC2D_DropEventInfo;

/**
 * \brief Structure contenant tous les callbacks de l'application RC2D.
 * 
 * Cette structure regroupe les fonctions de rappel (callbacks) utilisées par le moteur RC2D
 * pour gérer les événements du jeu, les entrées clavier, souris, et autres interactions.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_Callbacks {
    // -------------- Core Game Loop Callbacks ------------- //
    /**
     * \brief Appelée juste avant la fermeture de l'application.
     *
     * Cette fonction est invoquée en interne par le moteur RC2D immédiatement avant
     * que l'application ne se termine, permettant de libérer les ressources allouées
     * (ex. : mémoire, textures, fichiers ouverts). Elle est appelée une seule fois
     * lors de l'arrêt du programme.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_unload)(void);

    /**
     * \brief Appelée au démarrage de l'application, avant le début de la boucle de jeu.
     *
     * Cette fonction est invoquée une seule fois au tout début, avant que la boucle
     * principale du jeu ne commence. Elle permet d'initialiser les ressources nécessaires
     * (ex. : chargement des assets, configuration initiale de l'état du jeu).
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_load)(void);

    /**
     * \brief Appelée à intervalles réguliers pour mettre à jour la logique du jeu.
     *
     * Cette fonction est invoquée à chaque itération de la boucle de jeu, à une fréquence
     * synchronisée avec le taux de rafraîchissement du moniteur, mais limitée à un framerate
     * maximum défini par le moteur. Le paramètre `dt` représente le temps écoulé (en secondes)
     * depuis la dernière mise à jour, permettant des calculs indépendants du framerate.
     *
     * \param dt Temps écoulé depuis la dernière mise à jour (en secondes).
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_update)(double dt);

    /**
     * \brief Appelée à intervalles réguliers pour rendre la frame de jeu.
     *
     * Cette fonction est invoquée immédiatement après `rc2d_update` à chaque itération
     * de la boucle de jeu, à la même fréquence limitée par le framerate défini. Elle
     * est responsable du rendu graphique de l'état actuel du jeu (ex. : dessin des
     * sprites, mise à jour de l'écran).
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_draw)(void);


    // ------------- Keyboard Callbacks ------------- //
    /**
     * \brief Appelée lorsqu'une touche est enfoncée.
     *
     * \param key Nom de la touche (UTF-8, obtenu via SDL_GetKeyName).
     * \param scancode Code physique de la touche (SDL_Scancode).
     * \param keycode Code virtuel de la touche (SDL_Keycode).
     * \param mod État des modificateurs (SDL_Keymod, ex: Shift, Ctrl).
     * \param isrepeat True si l'événement est une répétition de touche.
     * \param keyboardID Identifiant du clavier (SDL_KeyboardID).
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_keypressed)(const char *key, SDL_Scancode scancode, SDL_Keycode keycode, SDL_Keymod mod, bool isrepeat, SDL_KeyboardID keyboardID);

    /**
     * \brief Appelée lorsqu'une touche est relâchée.
     *
     * \param key Nom de la touche (UTF-8, obtenu via SDL_GetKeyName).
     * \param scancode Code physique de la touche (SDL_Scancode).
     * \param keycode Code virtuel de la touche (SDL_Keycode).
     * \param mod État des modificateurs (SDL_Keymod, ex: Shift, Ctrl).
     * \param keyboardID Identifiant du clavier (SDL_KeyboardID).
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_keyreleased)(const char *key, SDL_Scancode scancode, SDL_Keycode keycode, SDL_Keymod mod, SDL_KeyboardID keyboardID);

    /**
     * \brief Appelée lorsqu'un texte est en cours d'édition via un IME.
     *
     * \param info Informations sur l'événement d'édition.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * 
     * \see RC2D_TextEditingEventInfo
     */
    void (*rc2d_textediting)(const RC2D_TextEditingEventInfo *info);

    /**
     * \brief Appelée lorsque des candidats d'édition de texte (IME) sont disponibles.
     *
     * \param info Informations sur les candidats.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * 
     * \see RC2D_TextEditingCandidatesEventInfo
     */
    void (*rc2d_texteditingcandidates)(const RC2D_TextEditingCandidatesEventInfo *info);

    /**
     * \brief Appelée lorsqu'un texte est entré (après validation de l'IME ou saisie directe).
     *
     * \param info Informations sur le texte entré.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * 
     * \see RC2D_TextInputEventInfo
     */
    void (*rc2d_textinput)(const RC2D_TextInputEventInfo *info);

    /**
     * \brief Appelée lorsque la disposition du clavier ou la langue change.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_keymapchanged)(void);

    /**
     * \brief Appelée lorsqu'un nouveau clavier est connecté.
     *
     * \param info Informations sur le clavier ajouté.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * 
     * \see RC2D_KeyboardDeviceEventInfo
     */
    void (*rc2d_keyboardadded)(const RC2D_KeyboardDeviceEventInfo *info);

    /**
     * \brief Appelée lorsqu'un clavier est déconnecté.
     *
     * \param info Informations sur le clavier supprimé.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * 
     * \see RC2D_KeyboardDeviceEventInfo
     */
    void (*rc2d_keyboardremoved)(const RC2D_KeyboardDeviceEventInfo *info);


    // ------------- Mouse Callbacks ------------- //
    /**
     * \brief Appelée lorsque la souris est déplacée.
     *
     * \param x Position X de la souris, relative à la fenêtre (en pixels, type float).
     * \param y Position Y de la souris, relative à la fenêtre (en pixels, type float).
     * \param xrel Mouvement relatif en X depuis le dernier événement (en pixels, type float).
     * \param yrel Mouvement relatif en Y depuis le dernier événement (en pixels, type float).
     * \param mouseID Identifiant de la souris (SDL_MouseID, ou SDL_TOUCH_MOUSEID pour les événements tactiles).
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_mousemoved)(float x, float y, float xrel, float yrel, SDL_MouseID mouseID);

    /**
     * \brief Appelée lorsqu’un bouton de la souris est enfoncé.
     *
     * \param x Position X de la souris, relative à la fenêtre (en pixels, type float).
     * \param y Position Y de la souris, relative à la fenêtre (en pixels, type float).
     * \param button Bouton de la souris enfoncé (RC2D_MouseButton).
     * \param clicks Nombre de clics (1 pour simple clic, 2 pour double clic, etc.).
     * \param mouseID Identifiant de la souris (SDL_MouseID, ou SDL_TOUCH_MOUSEID pour les événements tactiles).
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_mousepressed)(float x, float y, RC2D_MouseButton button, int clicks, SDL_MouseID mouseID);

    /**
     * \brief Appelée lorsqu’un bouton de la souris est relâché.
     *
     * \param x Position X de la souris, relative à la fenêtre (en pixels, type float).
     * \param y Position Y de la souris, relative à la fenêtre (en pixels, type float).
     * \param button Bouton de la souris relâché (RC2D_MouseButton).
     * \param clicks Nombre de clics (1 pour simple clic, 2 pour double clic, etc.).
     * \param mouseID Identifiant de la souris (SDL_MouseID, ou SDL_TOUCH_MOUSEID pour les événements tactiles).
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_mousereleased)(float x, float y, RC2D_MouseButton button, int clicks, SDL_MouseID mouseID);

    /**
     * \brief Appelée lorsqu’un mouvement de la molette de la souris est détecté.
     *
     * \param direction Direction du défilement (RC2D_MouseWheelDirection).
     * \param x Quantité de défilement horizontal (positif vers la droite, négatif vers la gauche, type float).
     * \param y Quantité de défilement vertical (positif vers le haut, négatif vers le bas, type float).
     * \param integer_x Quantité parcourue horizontalement, cumulée sur l'ensemble des « ticks » de défilement.
     * \param integer_y Quantité parcourue verticalement, cumulée sur l'ensemble des « ticks » de défilement.
     * \param mouse_x Position X de la souris au moment du défilement (en pixels, type float).
     * \param mouse_y Position Y de la souris au moment du défilement (en pixels, type float).
     * \param mouseID Identifiant de la souris (SDL_MouseID, ou SDL_TOUCH_MOUSEID pour les événements tactiles).
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_mousewheelmoved)(RC2D_MouseWheelDirection direction, float x, float y, Sint32 integer_x, Sint32 integer_y, float mouse_x, float mouse_y, SDL_MouseID mouseID);

    /**
     * \brief Appelée lorsqu’une nouvelle souris est connectée au système.
     *
     * \param mouseID Identifiant de la souris ajoutée (SDL_MouseID).
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_mouseadded)(SDL_MouseID mouseID);

    /**
     * \brief Appelée lorsqu’une souris est déconnectée du système.
     *
     * \param mouseID Identifiant de la souris supprimée (SDL_MouseID).
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_mouseremoved)(SDL_MouseID mouseID);


    // TODO: A refaire les callbacks pour les gamepads et joysticks
    // -------------- Gamepad and Joystick Callbacks ------------- //
    /*void (*rc2d_gamepadaxis)(SDL_JoystickID joystick, Uint8 axis, float value);
    void (*rc2d_gamepadpressed)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_gamepadreleased)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_joystickaxis)(SDL_JoystickID joystick, Uint8 axis, float value);
    void (*rc2d_joystickhat)(SDL_JoystickID joystick, Uint8 hat, Uint8 value);
    void (*rc2d_joystickpressed)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_joystickreleased)(SDL_JoystickID joystick, Uint8 button);
    void (*rc2d_joystickadded)(Sint32 joystick);
    void (*rc2d_joystickremoved)(Sint32 joystick);*/


    // -------------- Drag and Drop Callbacks ------------- //
    /**
     * \brief Appelée lorsqu'une opération de drag-and-drop commence.
     *
     * Reçoit un `RC2D_DropEventInfo` avec `data` et `x`, `y` à NULL/0, indiquant
     * le début d'une séquence de drag-and-drop.
     *
     * \param info Informations sur l'événement de début de drop.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * \see RC2D_DropEventInfo
     */
    void (*rc2d_dropbegin)(const RC2D_DropEventInfo *info);

    /**
     * \brief Appelée lorsqu'un fichier est déposé sur la fenêtre.
     *
     * Reçoit un `RC2D_DropEventInfo` avec `data` contenant le chemin du fichier,
     * et `x`, `y` indiquant la position du drop.
     *
     * \param info Informations sur l'événement, incluant le chemin du fichier.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * \see RC2D_DropEventInfo
     */
    void (*rc2d_dropfile)(const RC2D_DropEventInfo *info);

    /**
     * \brief Appelée lorsqu'un texte est déposé sur la fenêtre.
     *
     * Reçoit un `RC2D_DropEventInfo` avec `data` contenant le texte déposé,
     * et `x`, `y` indiquant la position du drop.
     *
     * \param info Informations sur l'événement, incluant le texte déposé.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * \see RC2D_DropEventInfo
     */
    void (*rc2d_droptext)(const RC2D_DropEventInfo *info);

    /**
     * \brief Appelée lorsque l'opération de drag-and-drop est terminée.
     *
     * Reçoit un `RC2D_DropEventInfo` avec `data` et `x`, `y` à NULL/0, indiquant
     * la fin de la séquence de drag-and-drop.
     *
     * \param info Informations sur l'événement de fin de drop.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * \see RC2D_DropEventInfo
     */
    void (*rc2d_dropcomplete)(const RC2D_DropEventInfo *info);

    /**
     * \brief Appelée lorsque la position du curseur change pendant un drag-over.
     *
     * Reçoit un `RC2D_DropEventInfo` avec `x`, `y` indiquant la position actuelle
     * du curseur, et `data` à NULL.
     *
     * \param info Informations sur l'événement, incluant les coordonnées.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * \see RC2D_DropEventInfo
     */
    void (*rc2d_dropposition)(const RC2D_DropEventInfo *info);

    
    // -------------- Window Callbacks ------------- //
    /**
     * \brief Appelée lorsque la fenêtre devient visible.
     *
     * L'événement indique que la fenêtre, précédemment masquée, est maintenant affichée
     * (par exemple, après un appel à SDL_ShowWindow ou une restauration).
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowshown)(void);

    /**
     * \brief Appelée lorsque la fenêtre est occluse (couverte ou non visible).
     *
     * L'événement indique que la fenêtre est entièrement ou partiellement masquée
     * par une autre fenêtre, le bureau, ou un autre élément de l'interface.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowoccluded)(void);

    /**
     * \brief Appelée lorsque la fenêtre a changé de moniteur.
     * 
     * Cela peut arriver lorsqu’un utilisateur glisse la fenêtre vers un autre écran.
     * 
     * \param displayIndex L'index du nouveau moniteur (commence à 0).
     * 
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowdisplaychanged)(int displayIndex);

    /**
     * \brief Appelée lorsque la fenêtre est redimensionnée.
     * 
     * Cette fonction est déclenchée lorsque l'utilisateur redimensionne la fenêtre de l'application.
     * 
     * \param width Nouvelle largeur de la fenêtre.
     * \param height Nouvelle hauteur de la fenêtre.
     * 
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowresized)(int width, int height);

    /**
     * \brief Appelée lorsque la fenêtre est exposée à nouveau à l’écran.
     * 
     * Cela signifie que la fenêtre a été dévoilée (ex: après avoir été recouverte ou restaurée).
     * Le moteur doit alors redessiner immédiatement son contenu.
     * 
     * \note Cela ne signifie pas que la taille a changé, seulement que la fenêtre est à nouveau visible.
     * 
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowexposed)(void);

    /**
     * \brief Appelée lorsque la fenêtre a été déplacée sur l'écran.
     *
     * Cette fonction est déclenchée lorsque l'utilisateur déplace la fenêtre de l'application,
     * par exemple en la faisant glisser à l'aide de la souris ou via un raccourci clavier.
     *
     * Elle fournit les nouvelles coordonnées de la fenêtre dans l'espace écran.
     * Cela peut être utile pour ajuster certains comportements liés à la position de la fenêtre,
     * comme repositionner des fenêtres secondaires, recalculer des offsets, etc.
     *
     * \param x Nouvelle position horizontale (coordonnée X) de la fenêtre.
     * \param y Nouvelle position verticale (coordonnée Y) de la fenêtre.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowmoved)(int x, int y);
        
    /**
     * \brief Appelée lorsque la fenêtre est minimisée.
     *
     * Cela signifie que la fenêtre a été réduite (icône dans la barre des tâches ou cachée).
     * L'application peut choisir de mettre en pause certains traitements ou de réduire l'utilisation CPU/GPU.
     *
     * \note Aucun redimensionnement n’a lieu. La fenêtre est juste rendue invisible ou inactive.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowminimized)(void);

    /**
     * \brief Appelée lorsque la fenêtre est maximisée.
     *
     * Cela signifie que la fenêtre a été agrandie pour occuper tout l’espace disponible sur l’écran.
     * Cela peut être utile pour recalculer le layout, adapter les ressources ou déclencher un rendu.
     *
     * \note Contrairement au redimensionnement manuel, cela résulte d'une action de l'utilisateur ou du système.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowmaximized)(void);

    /**
     * \brief Appelée lorsque la fenêtre est restaurée.
     *
     * Cette fonction est déclenchée lorsque la fenêtre revient à son état normal
     * après avoir été minimisée ou maximisée.
     *
     * Cela peut être utile pour relancer des animations, reprendre un rendu actif,
     * ou adapter les comportements de pause/reprise.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowrestored)(void);

    /**
     * \brief Appelée lorsque la fenêtre entre en mode plein écran.
     * 
     * Cette fonction est déclenchée lorsque l'utilisateur active le mode plein écran
     * pour la fenêtre de l'application.
     * 
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowenterfullscreen)(void);

    /**
     * \brief Appelée lorsque la fenêtre quitte le mode plein écran.
     * 
     * Cette fonction est déclenchée lorsque l'utilisateur désactive le mode plein écran
     * pour la fenêtre de l'application.
     * 
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowleavefullscreen)(void);

    /**
     * \brief Appelée lorsque la souris entre dans la fenêtre.
     *
     * Cette fonction est déclenchée lorsqu'un curseur de souris entre dans la surface de la fenêtre.
     * Cela peut être utile pour déclencher des effets visuels (highlight, surbrillance, etc.)
     * ou pour gérer un comportement spécifique lorsque l'utilisateur pointe la fenêtre avec la souris.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowmouseenter)(void);

    /**
     * \brief Appelée lorsque la souris quitte la fenêtre.
     *
     * Cette fonction est déclenchée lorsqu'un curseur de souris sort de la surface de la fenêtre.
     * Elle peut être utilisée pour annuler des effets visuels, masquer des survols ou réinitialiser des états UI.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowmouseleave)(void);

    /**
     * \brief Appelée lorsque la fenêtre obtient le focus clavier.
     *
     * Cette fonction est déclenchée lorsque l'utilisateur sélectionne la fenêtre avec le clavier, 
     * par exemple en cliquant dessus ou en utilisant un raccourci ALT+TAB.
     * Cela permet à l'application de reprendre des interactions clavier ou de réactiver certains états.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowkeyboardfocus)(void);

    /**
     * \brief Appelée lorsque la fenêtre perd le focus clavier.
     *
     * Cette fonction est déclenchée lorsque la fenêtre n'est plus la cible active des entrées clavier.
     * Cela peut être utilisé pour suspendre les raccourcis clavier, désactiver la saisie, ou mettre le jeu en pause.
     * 
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowkeyboardlost)(void);

    /**
     * \brief Appelée lorsque la fenêtre reçoit une requête de fermeture.
     * 
     * Cette fonction est déclenchée lorsque le gestionnaire de fenêtre (window manager)
     * demande la fermeture de la fenêtre de l'application, généralement suite à un clic sur la croix de fermeture
     * ou via ALT+F4 sur certains systèmes.
     * 
     * \note L'application peut choisir d'ignorer ou de gérer cette requête, par exemple pour afficher une boîte de confirmation.
     * 
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_windowclosed)(void);


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


    // -------------- Display/Monitor Callbacks ------------- //
    /**
     * \brief Appelée lorsqu’un changement d’orientation de l’affichage est détecté.
     * 
     * Cette fonction permet de réagir à une rotation de l’écran (portrait, paysage, etc.),
     * utile sur mobile, tablette ou appareils à écran rotatif.
     * 
     * \param monitorID Identifiant du moniteur concerné par le changement d’orientation.
     * \param orientation Nouvelle orientation de l’affichage.
     * 
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * 
     * \see RC2D_DisplayOrientation
     */
    void (*rc2d_monitororientationchanged)(SDL_DisplayID monitorID, RC2D_DisplayOrientation orientation);

    /**
     * \brief Appelée lorsqu'un nouveau moniteur est ajouté au système (ex. : connexion d'un écran externe).
     *
     * Utile pour mettre à jour la liste des moniteurs ou reconfigurer une disposition multi-écrans.
     *
     * \param info Identifiant du moniteur ajouté.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_monitoradded)(SDL_DisplayID monitorID);

    /**
     * \brief Appelée lorsqu'un moniteur est supprimé du système (ex. : déconnexion d'un écran).
     *
     * Utile pour ajuster le rendu ou déplacer les fenêtres vers les moniteurs restants.
     *
     * \param monitorID Identifiant du moniteur supprimé.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_monitorremoved)(SDL_DisplayID monitorID);

    /**
     * \brief Appelée lorsqu'un moniteur change de position (ex. : réorganisation dans une configuration multi-écrans).
     *
     * Utile pour suivre la disposition des moniteurs.
     *
     * \param monitorID Identifiant du moniteur dont la position a changé.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_monitormoved)(SDL_DisplayID monitorID);

    /**
     * \brief Appelée lorsque le mode de bureau d'un moniteur change (ex. : résolution, taux de rafraîchissement).
     *
     * \param monitorID Identifiant du moniteur dont le mode a changé.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_monitordesktopmodechanged)(SDL_DisplayID monitorID);

    /**
     * \brief Appelée lorsque le mode actuel d'un moniteur change (ex. : passage à un nouveau mode d'affichage).
     *
     * \param monitorID Identifiant du moniteur dont le mode a changé.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_monitorcurrentmodechanged)(SDL_DisplayID monitorID);


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


    // ------------- Sensor Callbacks ------------- //
    /**
     * \brief Appelée lorsqu'un capteur (ex: accéléromètre, gyroscope) fournit de nouvelles données.
     *
     * \param info Informations sur la mise à jour du capteur.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     * \see RC2D_SensorEventInfo
     */
    void (*rc2d_sensorupdate)(const RC2D_SensorEventInfo *info);


    // ------------- System Callbacks ------------- //
    /**
     * \brief Appelée lorsque le thème système de l'OS change (ex. : passage en mode clair ou sombre).
     *
     * \param theme Le thème système actuel :
     *   - `SDL_SYSTEMTHEME_LIGHT` : Thème clair.
     *   - `SDL_SYSTEMTHEME_DARK` : Thème sombre.
     *   - `SDL_SYSTEMTHEME_UNKNOWN` : Thème indéterminé.
     *
     * \since Cette fonction est disponible depuis RC2D 1.0.0.
     */
    void (*rc2d_systemthemechanged)(SDL_SystemTheme theme);
} RC2D_EngineCallbacks;

/**
 * \brief Modes d'affichage pour le contenu du letterbox/pillarbox.
 * 
 * Définit comment les zones de letterbox/pillarbox (marges visibles autour du rendu principal)
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
    RC2D_LETTERBOX_BACKGROUND_FULL,

    /**
     * Un shader est appliqué aux zones de letterbox/pillarbox.
     * 
     * Cela permet de dessiner un effet visuel personnalisé (via un shader) dans les zones
     * de letterbox/pillarbox, au lieu d'utiliser des textures.
     */
    RC2D_LETTERBOX_SHADER
} RC2D_LetterboxMode;

/**
 * \brief Textures de remplissage pour les marges du letterbox/pillarbox.
 * 
 * Cette structure permet de définir le comportement visuel des marges 
 * (haut, bas, gauche, droite) en cas de mise à l’échelle logique avec le mode 
 * `RC2D_LOGICAL_PRESENTATION_LETTERBOX` ou `RC2D_LOGICAL_PRESENTATION_INTEGER_SCALE`.
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
    const char* uniform_filename;

    /**
     * Texture spécifique pour chaque côtés (mode `RC2D_LETTERBOX_PER_SIDE`).
     */
    const char* top_filename;
    const char* bottom_filename;
    const char* left_filename;
    const char* right_filename;

    /**
     * Texture géante utilisée en arrière-plan total (mode `RC2D_LETTERBOX_BACKGROUND_FULL`).
     */
    const char* background_filename;

    /**
     * Noms des fichiers de shader pour le mode `RC2D_LETTERBOX_SHADER`.
     */
    const char* shader_vertex_filename;
    const char* shader_fragment_filename;
} RC2D_LetterboxTextures;

/**
 * \brief Définit comment le contenu du jeu est affiché à l'écran.
 *
 * Ce mode influence la manière dont la résolution logique est convertie en pixels physiques.
 * Chaque mode a des implications sur l’apparence visuelle (mise à l’échelle, ratio, bandes noires, etc.).
 * 
 * \since Cette enum est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_LogicalPresentationMode {
    /**
     * Mise à l’échelle entière (INTEGER_SCALE) avec ajout de bandes noires (letterbox/pillarbox) si nécessaire.
     * 
     * Chaque pixel logique est agrandi en un bloc de pixels physiques complet (ex. 2x, 3x, 4x).
     * Aucun filtrage n’est appliqué, ce qui évite toute distorsion ou flou.
     * 
     * \note Idéal pour les jeux en pixel art, afin de préserver la netteté des sprites.
     */
    RC2D_LOGICAL_PRESENTATION_INTEGER_SCALE,

    /**
     * Mise à l’échelle proportionnelle pour occuper au maximum l’espace disponible,
     * en conservant le ratio d’aspect d’origine.
     * 
     * Si la résolution physique ne correspond pas exactement au ratio logique, des bandes noires
     * (letterbox/pillarbox) sont ajoutées pour compenser.
     * 
     * \note Idéal pour les jeux modernes ou vectoriels, s’adaptant à tous les écrans sans distorsion.
     */
    RC2D_LOGICAL_PRESENTATION_LETTERBOX,
} RC2D_LogicalPresentationMode;

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
     * 
     * Par défaut : NULL.
     */
    RC2D_EngineCallbacks* callbacks;

    /**
     * Largeur initiale de la fenêtre (pixels).
     * 
     * Par défaut : 800.
     */
    int windowWidth;

    /**
     * Hauteur initiale de la fenêtre (pixels). 
     * 
     * Par défaut : 600.
     */
    int windowHeight;

    /**
     * Largeur logique (par exemple, 1920 pour CLASSIC, 640 pour PIXELART).
     * 
     * Par défaut : 1920.
     */
    int logicalWidth;

    /**
     * Hauteur logique (par exemple, 1080 pour CLASSIC, 360 pour PIXELART). 
     * 
     * Par défaut : 1080.
     */
    int logicalHeight;

    /**
     * \brief Si le rendu doit être effectué en mode pixel art (true) ou en mode classique (false).
     * 
     * Si c'est true, il y a des choses qui sont différentes :
     * - MSAA est désactivé (anti-aliasing).
     * - Le filtrage des textures est désactivé (les textures sont affichées en mode nearest neighbor).
     * 
     * Par défaut : false (mode classique).
     */
    bool pixelartMode;

    /**
     * Mode de présentation de RC2D_LogicalPresentationMode.
     * 
     * Par défaut : RC2D_LOGICAL_PRESENTATION_LETTERBOX.
     */
    RC2D_LogicalPresentationMode logicalPresentationMode;

    /**
     * Textures pour les bordures décoratives (letterbox/pillarbox), pour les modes suivants :
     * - RC2D_LOGICAL_PRESENTATION_LETTERBOX
     * - RC2D_LOGICAL_PRESENTATION_INTEGER_SCALE
     * 
     * Par défaut : NULL.
     */
    RC2D_LetterboxTextures* letterboxTextures;

    /**
     * Informations sur l'application (nom, version, identifier).
     * 
     * Par défaut :
     * - name : "RC2D Game"
     * - version : "1.0.0"
     * - identifier : "com.example.rc2dgame"
     */
    RC2D_AppInfo* appInfo;

    /**
     * Nombre d'images autorisées en vol sur le GPU (1, 2 ou 3).
     * 
     * Par défaut : RC2D_GPU_FRAMES_BALANCED (2).
     */
    RC2D_GPUFramesInFlight gpuFramesInFlight;

    /**
     * Options avancées pour la création du contexte GPU.
     * 
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
 * Si vous souhaitez utiliser la configuration par défaut de RC2D, vous pouvez retourner NULL, mais
 * vous n'aurais aucune callback d'événement, donc juste un écran noir avec RC2D qui tourne en boucle.
 * 
 * \note Utiliser plutôt `rc2d_engine_getDefaultConfig()` pour obtenir une configuration par défaut, puis
 * personnaliser les champs nécessaires.
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