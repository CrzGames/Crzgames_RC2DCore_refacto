#ifndef RC2D_MESSAGEBOX_H
#define RC2D_MESSAGEBOX_H

#include <stdbool.h> // Required for : bool

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumération représentant les types de boîtes de message.
 * 
 * \since Cette enum est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_MessageBoxType {
    /**
     * Boîte de message d'erreur.
     */
    RC2D_MESSAGEBOX_ERROR,

    /**
     * Boîte de message d'avertissement.
     */
    RC2D_MESSAGEBOX_WARNING,
    
    /**
     * Boîte de message d'information.
     */
    RC2D_MESSAGEBOX_INFORMATION
} RC2D_MessageBoxType;

/**
 * Affiche une boîte de message personnalisée avec une liste de boutons.
 * 
 * @param title Le titre de la boîte de message.
 * @param message Le texte à l'intérieur de la boîte de message.
 * @param buttonlist Un tableau contenant une liste de noms de boutons à afficher.
 * @param numButtons Le nombre de boutons dans le tableau buttonlist.
 * @param pressedButton Un pointeur vers une variable où l'index du bouton pressé sera stocké.
 * @param type Le type de la boîte de message (erreur, avertissement, information).
 * @param attachToWindow Booléen indiquant si la boîte de message doit être attachée à la fenêtre ou non.
 * @return L'index du bouton pressé par l'utilisateur. Peut être 0 si la boîte de dialogue de message a été fermée sans appuyer sur un bouton.
 */
bool rc2d_window_showMessageBox(const char *title, const char *message, const char *buttonlist[], uint32_t numButtons, uint32_t *pressedButton, RC2D_MessageBoxType type, bool attachToWindow);

/**
 * Affiche une boîte de message simple avec un bouton 'OK'.
 * 
 * @param title Le titre de la boîte de message.
 * @param message Le texte à l'intérieur de la boîte de message.
 * @param type Le type de la boîte de message (erreur, avertissement, information).
 * @param attachToWindow Booléen indiquant si la boîte de message doit être attachée à la fenêtre ou non.
 * @return true si la boîte de message a été affichée avec succès, sinon false.
 */
bool rc2d_window_showSimpleMessageBox(const char *title, const char *message, RC2D_MessageBoxType type, bool attachToWindow);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_MESSAGEBOX_H