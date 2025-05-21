#ifndef RC2D_MESSAGEBOX_H
#define RC2D_MESSAGEBOX_H

#include <SDL3/SDL_messagebox.h> // Requis pour : SDL_Window, SDL_MessageBoxFlags
#include <stdbool.h>             // Requis pour : bool

/* Configuration pour les définitions de fonctions C, même en C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Type de boîte de dialogue de message dans RC2D.
 *
 * Définit les types de messages pour les boîtes de dialogue (erreur, avertissement, information).
 *
 * \since Cette énumération est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_MessageBoxType {
    /**
     * Boîte de dialogue pour une erreur.
     */
    RC2D_MESSAGEBOX_ERROR = 0,

    /**
     * Boîte de dialogue pour un avertissement.
     */
    RC2D_MESSAGEBOX_WARNING = 1,

    /**
     * Boîte de dialogue pour une information.
     */
    RC2D_MESSAGEBOX_INFORMATION = 2
} RC2D_MessageBoxType;

/**
 * \brief Structure pour un bouton dans une boîte de dialogue RC2D.
 *
 * Définit les propriétés d'un bouton personnalisé (texte, ID, comportement par défaut).
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_MessageBoxButton {
    /**
     * Texte du bouton en UTF-8.
     */
    const char *text;

    /**
     * ID utilisateur retourné si le bouton est sélectionné.
     */
    int button_id;

    /**
     * Vrai si le bouton est sélectionné par défaut avec la touche Entrée.
     */
    bool return_key_default;

    /**
     * Vrai si le bouton est sélectionné par défaut avec la touche Échap.
     */
    bool escape_key_default;
} RC2D_MessageBoxButton;

/**
 * \brief Structure pour les options de configuration d'une boîte de dialogue de message.
 *
 * Permet de personnaliser le type, le titre, le message, les boutons et les couleurs.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_MessageBoxOptions {
    /**
     * Type de la boîte de dialogue (erreur, avertissement, information).
     */
    RC2D_MessageBoxType type;

    /**
     * Fenêtre parent pour le dialogue (peut être NULL).
     */
    SDL_Window *window;

    /**
     * Titre de la boîte de dialogue en UTF-8 (peut être NULL pour le titre par défaut).
     */
    const char *title;

    /**
     * Message de la boîte de dialogue en UTF-8.
     */
    const char *message;

    /**
     * Tableau de boutons personnalisés (peut être NULL pour un bouton OK par défaut).
     */
    const RC2D_MessageBoxButton *buttons;

    /**
     * Nombre de boutons dans le tableau (ignoré si buttons est NULL).
     */
    int num_buttons;

    /**
     * Vrai pour placer les boutons de gauche à droite, faux pour droite à gauche.
     */
    bool buttons_left_to_right;

    /**
     * Schéma de couleurs personnalisé (peut être NULL pour les couleurs système).
     * Tableau de 5 couleurs RGB pour : fond, texte, bordure bouton, fond bouton, bouton sélectionné.
     */
    const Uint8 (*color_scheme)[3]; // Tableau de 5 tableaux de 3 Uint8 (r, g, b)
} RC2D_MessageBoxOptions;

/**
 * \brief Affiche une boîte de dialogue de message simple avec un bouton OK.
 *
 * \param type Type de la boîte de dialogue (erreur, avertissement, information).
 * \param title Titre de la boîte de dialogue en UTF-8 (peut être NULL).
 * \param message Message de la boîte de dialogue en UTF-8.
 * \param window Fenêtre parent (peut être NULL).
 * \return true en cas de succès, false sinon.
 *
 * \threadsafety Cette fonction doit être appelée depuis le thread qui a créé la fenêtre parente ou le thread principal.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
bool rc2d_messagebox_showSimple(RC2D_MessageBoxType type, const char *title, const char *message, SDL_Window *window);

/**
 * \brief Affiche une boîte de dialogue de message personnalisée.
 *
 * \param options Options de configuration du dialogue.
 * \param button_id Pointeur pour stocker l'ID du bouton sélectionné (peut être NULL).
 * \return true en cas de succès, false sinon.
 *
 * \threadsafety Cette fonction doit être appelée depuis le thread qui a créé la fenêtre parente ou le thread principal.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
bool rc2d_messagebox_show(const RC2D_MessageBoxOptions *options, int *button_id);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_MESSAGEBOX_H