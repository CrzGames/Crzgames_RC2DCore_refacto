#ifndef RC2D_IMGUI_H
#define RC2D_IMGUI_H

#include <stdbool.h>

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Conditions pour définir quand appliquer les paramètres de fenêtre ImGui dans RC2D.
 *
 * Ces conditions contrôlent quand les paramètres comme la position ou la taille doivent être appliqués
 * à une fenêtre ImGui dans le moteur RC2D.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_ImGuiSetCondition {
    RC2D_IMGUI_SET_CONDITION_NONE = 0,         /**< Aucune condition, ne rien appliquer. */
    RC2D_IMGUI_SET_CONDITION_ALWAYS = 1 << 0,  /**< Appliquer à chaque frame. */
    RC2D_IMGUI_SET_CONDITION_ONCE = 1 << 1,    /**< Appliquer une seule fois. */
    RC2D_IMGUI_SET_CONDITION_FIRST_USE = 1 << 2, /**< Appliquer la première fois que la fenêtre est créée. */
    RC2D_IMGUI_SET_CONDITION_APPEARING = 1 << 3 /**< Appliquer lorsque la fenêtre devient visible. */
} RC2D_ImGuiSetCondition;

/**
 * \brief Flags pour configurer le comportement des fenêtres ImGui dans RC2D.
 *
 * Ces flags permettent de personnaliser l'apparence et le comportement des fenêtres ImGui
 * utilisées dans le moteur RC2D, en mappant les options de la bibliothèque ImGui.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_ImGuiWindowFlags {
    RC2D_IMGUI_WINDOW_FLAGS_NONE = 0,                   /**< Aucun flag, comportement par défaut. */
    RC2D_IMGUI_WINDOW_FLAGS_NO_TITLE = 1 << 0,         /**< Supprime la barre de titre. */
    RC2D_IMGUI_WINDOW_FLAGS_NO_RESIZE = 1 << 1,        /**< Empêche le redimensionnement. */
    RC2D_IMGUI_WINDOW_FLAGS_NO_MOVE = 1 << 2,          /**< Empêche le déplacement. */
    RC2D_IMGUI_WINDOW_FLAGS_NO_SCROLLBAR = 1 << 3,     /**< Désactive les barres de défilement. */
    RC2D_IMGUI_WINDOW_FLAGS_NO_SCROLL_WITH_MOUSE = 1 << 4, /**< Désactive le défilement avec la molette. */
    RC2D_IMGUI_WINDOW_FLAGS_NO_COLLAPSE = 1 << 5,      /**< Empêche le repli de la fenêtre. */
    RC2D_IMGUI_WINDOW_FLAGS_AUTO_SIZE = 1 << 6,        /**< Redimensionne automatiquement selon le contenu. */
    RC2D_IMGUI_WINDOW_FLAGS_NO_BACKGROUND = 1 << 7,    /**< Supprime l'arrière-plan. */
    RC2D_IMGUI_WINDOW_FLAGS_NO_SAVED_SETTINGS = 1 << 8, /**< Ne sauvegarde pas les paramètres de la fenêtre. */
    RC2D_IMGUI_WINDOW_FLAGS_NO_MOUSE_INPUTS = 1 << 9,  /**< Désactive les entrées souris. */
    RC2D_IMGUI_WINDOW_FLAGS_MENU_BAR = 1 << 10,        /**< Ajoute une barre de menu. */
    RC2D_IMGUI_WINDOW_FLAGS_HORIZONTAL_SCROLLBAR = 1 << 11, /**< Force l'affichage d'une barre de défilement horizontale. */
    RC2D_IMGUI_WINDOW_FLAGS_NO_FOCUS_ON_APPEARING = 1 << 12, /**< Ne met pas la fenêtre au premier plan à l'apparition. */
    RC2D_IMGUI_WINDOW_FLAGS_NO_BRING_TO_FRONT = 1 << 13, /**< Ne met pas la fenêtre au premier plan lors du focus. */
    RC2D_IMGUI_WINDOW_FLAGS_ALWAYS_VERTICAL_SCROLLBAR = 1 << 14, /**< Force l'affichage d'une barre de défilement verticale. */
    RC2D_IMGUI_WINDOW_FLAGS_ALWAYS_HORIZONTAL_SCROLLBAR = 1 << 15, /**< Force l'affichage d'une barre de défilement horizontale (redondant). */
    RC2D_IMGUI_WINDOW_FLAGS_NO_NAV_INPUTS = 1 << 16,   /**< Désactive les entrées de navigation (clavier/manette). */
    RC2D_IMGUI_WINDOW_FLAGS_NO_NAV_FOCUS = 1 << 17,    /**< Empêche la fenêtre de recevoir le focus de navigation. */
    RC2D_IMGUI_WINDOW_FLAGS_UNSAVED_DOCUMENT = 1 << 18, /**< Indique un document non sauvegardé (affiche un indicateur). */
    RC2D_IMGUI_WINDOW_FLAGS_NO_DOCKING = 1 << 19,      /**< Désactive le docking. */
    RC2D_IMGUI_WINDOW_FLAGS_DOCK_NODE_HOST = 1 << 23,  /**< Indique que la fenêtre héberge un nœud de docking. */
    RC2D_IMGUI_WINDOW_FLAGS_CHILD_WINDOW = 1 << 24,    /**< Fenêtre enfant (imbriquée dans une autre). */
    RC2D_IMGUI_WINDOW_FLAGS_TOOLTIP = 1 << 25,         /**< Fenêtre de type tooltip. */
    RC2D_IMGUI_WINDOW_FLAGS_POPUP = 1 << 26,           /**< Fenêtre de type popup. */
    RC2D_IMGUI_WINDOW_FLAGS_MODAL = 1 << 27,           /**< Fenêtre modale (bloque les interactions avec les autres). */
    RC2D_IMGUI_WINDOW_FLAGS_CHILD_MENU = 1 << 28       /**< Fenêtre de type menu enfant. */
} RC2D_ImGuiWindowFlags;

/**
 * \brief Flags pour configurer le comportement des boutons ImGui dans RC2D.
 *
 * Ces flags permettent de personnaliser les interactions des boutons ImGui
 * utilisés dans le moteur RC2D, en mappant les options de la bibliothèque ImGui.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_ImGuiButtonFlags {
    RC2D_IMGUI_BUTTON_FLAGS_NONE = 0,                   /**< Aucun flag, comportement par défaut. */
    RC2D_IMGUI_BUTTON_FLAGS_MOUSE_BUTTON_LEFT = 1 << 0,  /**< Active le clic avec le bouton gauche. */
    RC2D_IMGUI_BUTTON_FLAGS_MOUSE_BUTTON_RIGHT = 1 << 1, /**< Active le clic avec le bouton droit. */
    RC2D_IMGUI_BUTTON_FLAGS_MOUSE_BUTTON_MIDDLE = 1 << 2, /**< Active le clic avec le bouton du milieu. */
    RC2D_IMGUI_BUTTON_FLAGS_ENABLE_NAV = 1 << 3          /**< Permet la navigation clavier/manette. */
} RC2D_ImGuiButtonFlags;

/**
 * \brief Flags pour configurer le comportement des sliders ImGui dans RC2D.
 *
 * Ces flags permettent de personnaliser les interactions des sliders ImGui
 * utilisés dans le moteur RC2D, en mappant les options de la bibliothèque ImGui.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_ImGuiSliderFlags {
    RC2D_IMGUI_SLIDER_FLAGS_NONE = 0,                   /**< Aucun flag, comportement par défaut. */
    RC2D_IMGUI_SLIDER_FLAGS_LOGARITHMIC = 1 << 5,       /**< Utilise une échelle logarithmique. */
    RC2D_IMGUI_SLIDER_FLAGS_NO_ROUND = 1 << 6,          /**< Ne pas arrondir à la précision du format. */
    RC2D_IMGUI_SLIDER_FLAGS_NO_INPUT = 1 << 7,          /**< Désactive l'entrée texte directe. */
    RC2D_IMGUI_SLIDER_FLAGS_WRAP_AROUND = 1 << 8,       /**< Permet au slider de boucler. */
    RC2D_IMGUI_SLIDER_FLAGS_CLAMP_ON_INPUT = 1 << 9,    /**< Limite la valeur entrée aux bornes min/max. */
    RC2D_IMGUI_SLIDER_FLAGS_CLAMP_ZERO_RANGE = 1 << 10  /**< Limite la plage à zéro si min=max. */
} RC2D_ImGuiSliderFlags;

/**
 * \brief Flags pour configurer le comportement des champs de saisie de texte ImGui dans RC2D.
 *
 * Ces flags permettent de personnaliser les interactions des champs de saisie de texte ImGui
 * utilisés dans le moteur RC2D, en mappant les options de la bibliothèque ImGui.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_ImGuiInputTextFlags {
    RC2D_IMGUI_INPUT_TEXT_FLAGS_NONE = 0,                   /**< Aucun flag, comportement par défaut. */
    RC2D_IMGUI_INPUT_TEXT_FLAGS_CHARS_DECIMAL = 1 << 0,     /**< Autorise les chiffres et le point décimal. */
    RC2D_IMGUI_INPUT_TEXT_FLAGS_CHARS_HEX = 1 << 1,         /**< Autorise les caractères hexadécimaux (0-9, A-F). */
    RC2D_IMGUI_INPUT_TEXT_FLAGS_CHARS_UPPERCASE = 1 << 3,   /**< Convertit les caractères en majuscules. */
    RC2D_IMGUI_INPUT_TEXT_FLAGS_CHARS_NO_BLANK = 1 << 4,    /**< Interdit les espaces. */
    RC2D_IMGUI_INPUT_TEXT_FLAGS_AUTO_SELECT_ALL = 1 << 12,  /**< Sélectionne tout le texte à la prise de focus. */
    RC2D_IMGUI_INPUT_TEXT_FLAGS_ENTER_RETURNS_TRUE = 1 << 6, /**< Retourne true si Enter est pressé. */
    RC2D_IMGUI_INPUT_TEXT_FLAGS_READ_ONLY = 1 << 9,         /**< Champ en lecture seule. */
    RC2D_IMGUI_INPUT_TEXT_FLAGS_PASSWORD = 1 << 10,         /**< Masque les caractères (pour les mots de passe). */
    RC2D_IMGUI_INPUT_TEXT_FLAGS_NO_UNDO_REDO = 1 << 16      /**< Désactive l'annulation/rétablissement. */
} RC2D_ImGuiInputTextFlags;

/**
 * \brief Flags pour configurer le comportement des combo boxes ImGui dans RC2D.
 *
 * Ces flags permettent de personnaliser les interactions des combo boxes ImGui
 * utilisés dans le moteur RC2D, en mappant les options de la bibliothèque ImGui.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_ImGuiComboFlags {
    RC2D_IMGUI_COMBO_FLAGS_NONE = 0,                   /**< Aucun flag, comportement par défaut. */
    RC2D_IMGUI_COMBO_FLAGS_POPUP_ALIGN_LEFT = 1 << 0,  /**< Aligne le popup à gauche du champ. */
    RC2D_IMGUI_COMBO_FLAGS_HEIGHT_SMALL = 1 << 1,      /**< Hauteur réduite pour le popup. */
    RC2D_IMGUI_COMBO_FLAGS_HEIGHT_REGULAR = 1 << 2,    /**< Hauteur standard pour le popup. */
    RC2D_IMGUI_COMBO_FLAGS_HEIGHT_LARGE = 1 << 3,      /**< Hauteur agrandie pour le popup. */
    RC2D_IMGUI_COMBO_FLAGS_NO_ARROW_BUTTON = 1 << 5,   /**< Supprime le bouton flèche. */
    RC2D_IMGUI_COMBO_FLAGS_NO_PREVIEW = 1 << 6         /**< Supprime l'affichage de l'élément sélectionné. */
} RC2D_ImGuiComboFlags;

/**
 * \brief Flags pour configurer le comportement des popups ImGui dans RC2D.
 *
 * Ces flags permettent de personnaliser les interactions des popups ImGui
 * utilisés dans le moteur RC2D, en mappant les options de la bibliothèque ImGui.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_ImGuiPopupFlags {
    RC2D_IMGUI_POPUP_FLAGS_NONE = 0,                   /**< Aucun flag, comportement par défaut. */
    RC2D_IMGUI_POPUP_FLAGS_MOUSE_BUTTON_LEFT = 0,      /**< Ouvre avec un clic gauche. */
    RC2D_IMGUI_POPUP_FLAGS_MOUSE_BUTTON_RIGHT = 1,     /**< Ouvre avec un clic droit. */
    RC2D_IMGUI_POPUP_FLAGS_MOUSE_BUTTON_MIDDLE = 2,    /**< Ouvre avec un clic milieu. */
    RC2D_IMGUI_POPUP_FLAGS_NO_REOPEN = 1 << 5,         /**< Empêche la réouverture si déjà ouvert. */
    RC2D_IMGUI_POPUP_FLAGS_NO_OPEN_OVER_ITEMS = 1 << 8 /**< N'ouvre pas si la souris est sur un item. */
} RC2D_ImGuiPopupFlags;

/**
 * \brief Définit la position de la prochaine fenêtre ImGui.
 *
 * Configure la position de la prochaine fenêtre ImGui à la position spécifiée (x, y)
 * dans l'espace logique de l'écran, avec la condition donnée.
 *
 * \param x Position horizontale (en pixels logiques) du coin supérieur gauche de la fenêtre.
 * \param y Position verticale (en pixels logiques) du coin supérieur gauche de la fenêtre.
 * \param cond Condition RC2D_ImGuiSetCondition pour déterminer quand appliquer la position.
 *
 * \threadsafety Cette fonction peut être appelée depuis le thread principal de rendu.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
void rc2d_cimgui_setNextWindowPos(float x, float y, RC2D_ImGuiSetCondition cond);

/**
 * \brief Commence une fenêtre ImGui avec le nom et les flags spécifiés.
 *
 * Cette fonction crée une fenêtre ImGui avec le nom donné, à la position précédemment
 * définie (par exemple, via rc2d_cimgui_setNextWindowPos), et applique les flags configurés.
 *
 * \param name Nom de la fenêtre (doit être unique).
 * \param p_open Pointeur vers une variable booléenne contrôlant si la fenêtre est ouverte (peut être NULL).
 * \param flags Combinaison de flags RC2D_ImGuiWindowFlags pour configurer la fenêtre.
 *
 * \return bool True si la fenêtre a été créée avec succès, false sinon.
 *
 * \note Cette fonction doit être suivie d'un appel à `rc2d_cimgui_end` pour finaliser le rendu.
 *
 * \threadsafety Cette fonction peut être appelée depuis le thread principal de rendu.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
bool rc2d_cimgui_begin(const char* name, bool* p_open, RC2D_ImGuiWindowFlags flags);

/**
 * \brief Affiche un bouton ImGui avec le label donné.
 *
 * Crée un bouton interactif avec le texte spécifié et les flags configurés.
 *
 * \param label Texte affiché sur le bouton.
 * \param width Largeur du bouton (0 pour taille automatique).
 * \param height Hauteur du bouton (0 pour taille automatique).
 *
 * \return bool True si le bouton a été cliqué, false sinon.
 *
 * \threadsafety Cette fonction peut être appelée depuis le thread principal de rendu.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
bool rc2d_cimgui_button(const char* label, float width, float height);

/**
 * \brief Affiche du texte non formaté dans ImGui.
 *
 * Affiche une chaîne de texte simple sans mise en forme spéciale.
 *
 * \param text Texte à afficher.
 *
 * \threadsafety Cette fonction peut être appelée depuis le thread principal de rendu.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
void rc2d_cimgui_text(const char* text);

/**
 * \brief Affiche une case à cocher ImGui.
 *
 * Crée une case à cocher interactive avec le label donné.
 *
 * \param label Texte affiché à côté de la case.
 * \param checked Pointeur vers la variable booléenne représentant l'état (coché/non coché).
 *
 * \return bool True si l'état a changé, false sinon.
 *
 * \threadsafety Cette fonction peut être appelée depuis le thread principal de rendu.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
bool rc2d_cimgui_checkbox(const char* label, bool* checked);

/**
 * \brief Affiche un slider pour une valeur flottante ImGui.
 *
 * Crée un slider permettant de modifier une valeur flottante dans une plage donnée.
 *
 * \param label Texte affiché à côté du slider.
 * \param value Pointeur vers la valeur flottante à modifier.
 * \param min_value Valeur minimale du slider.
 * \param max_value Valeur maximale du slider.
 * \param format Format d'affichage de la valeur (par exemple, "%.3f").
 * \param flags Combinaison de flags RC2D_ImGuiSliderFlags pour configurer le slider.
 *
 * \return bool True si la valeur a changé, false sinon.
 *
 * \threadsafety Cette fonction peut être appelée depuis le thread principal de rendu.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
bool rc2d_cimgui_sliderFloat(const char* label, float* value, float min_value, float max_value, const char* format, RC2D_ImGuiSliderFlags flags);

/**
 * \brief Affiche un champ de saisie de texte ImGui.
 *
 * Crée un champ de saisie de texte interactif avec le label donné.
 *
 * \param label Texte affiché à côté du champ.
 * \param buffer Pointeur vers le buffer contenant le texte (modifié par l'utilisateur).
 * \param buffer_size Taille du buffer en octets.
 * \param flags Combinaison de flags RC2D_ImGuiInputTextFlags pour configurer le champ.
 *
 * \return bool True si le texte a été modifié, false sinon.
 *
 * \threadsafety Cette fonction peut être appelée depuis le thread principal de rendu.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
bool rc2d_cimgui_input_text(const char* label, char* buffer, size_t buffer_size, RC2D_ImGuiInputTextFlags flags);

/**
 * \brief Affiche une combo box ImGui.
 *
 * Crée une liste déroulante permettant de sélectionner un élément parmi une liste.
 *
 * \param label Texte affiché à côté de la combo box.
 * \param current_item Pointeur vers l'index de l'élément actuellement sélectionné (modifié par l'utilisateur).
 * \param items Tableau de chaînes représentant les éléments de la liste.
 * \param items_count Nombre d'éléments dans la liste.
 * \param popup_max_height_in_items Hauteur maximale du popup en nombre d'éléments (-1 pour automatique).
 *
 * \return bool True si la sélection a changé, false sinon.
 *
 * \threadsafety Cette fonction peut être appelée depuis le thread principal de rendu.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
bool rc2d_cimgui_combo(const char* label, int* current_item, const char* items[], int items_count, int popup_max_height_in_items);

/**
 * \brief Ouvre un popup ImGui.
 *
 * Demande l'ouverture d'un popup avec l'identifiant donné.
 *
 * \param id Identifiant unique du popup.
 * \param flags Combinaison de flags RC2D_ImGuiPopupFlags pour configurer l'ouverture.
 *
 * \threadsafety Cette fonction peut être appelée depuis le thread principal de rendu.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
void rc2d_cimgui_open_popup(const char* id, RC2D_ImGuiPopupFlags flags);

/**
 * \brief Commence un popup ImGui.
 *
 * Crée un popup avec l'identifiant donné, si celui-ci est ouvert.
 *
 * \param id Identifiant unique du popup.
 * \param flags Combinaison de flags RC2D_ImGuiWindowFlags pour configurer le popup.
 *
 * \return bool True si le popup est ouvert et affiché, false sinon.
 *
 * \note Cette fonction doit être suivie d'un appel à `rc2d_cimgui_end` pour finaliser le rendu.
 *
 * \threadsafety Cette fonction peut être appelée depuis le thread principal de rendu.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
bool rc2d_cimgui_begin_popup(const char* id, RC2D_ImGuiWindowFlags flags);

/**
 * \brief Affiche une list box ImGui.
 *
 * Crée une liste permettant de sélectionner un élément parmi une liste de chaînes.
 *
 * \param label Texte affiché à côté de la list box.
 * \param current_item Pointeur vers l'index de l'élément actuellement sélectionné (modifié par l'utilisateur).
 * \param items Tableau de chaînes représentant les éléments de la liste.
 * \param items_count Nombre d'éléments dans la liste.
 * \param height_in_items Hauteur de la list box en nombre d'éléments (-1 pour automatique).
 *
 * \return bool True si la sélection a changé, false sinon.
 *
 * \threadsafety Cette fonction peut être appelée depuis le thread principal de rendu.
 *
 * \since Disponible depuis RC2D 1.0.0.
 */
bool rc2d_cimgui_list_box(const char* label, int* current_item, const char* items[], int items_count, int height_in_items);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_IMGUI_H