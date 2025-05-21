#ifndef RC2D_FILEDIALOG_H
#define RC2D_FILEDIALOG_H

#include <SDL3/SDL_dialog.h> // Requis pour : SDL_Window, SDL_DialogFileFilter
#include <stdbool.h>         // Requis pour : bool

/* Configuration pour les définitions de fonctions C, même en C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Type de dialogue de fichier dans RC2D.
 *
 * Définit les types de boîtes de dialogue disponibles pour sélectionner des fichiers ou dossiers.
 *
 * \since Cette énumération est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_FileDialogType {
    /**
     * Dialogue pour ouvrir un fichier existant.
     */
    RC2D_FILEDIALOG_OPEN_FILE = 0,

    /**
     * Dialogue pour enregistrer un fichier (nouveau ou existant).
     */
    RC2D_FILEDIALOG_SAVE_FILE = 1,

    /**
     * Dialogue pour sélectionner un dossier.
     */
    RC2D_FILEDIALOG_OPEN_FOLDER = 2
} RC2D_FileDialogType;

/**
 * \brief Structure pour un filtre de fichier dans RC2D.
 *
 * Définit un filtre pour restreindre les types de fichiers affichés dans le dialogue.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_FileDialogFilter {
    /**
     * Nom lisible du filtre (ex. : "Images PNG").
     */
    const char *name;

    /**
     * Liste d'extensions séparées par des points-virgules (ex. : "png;jpg") ou "*" pour tous les fichiers.
     */
    const char *pattern;
} RC2D_FileDialogFilter;

/**
 * \brief Structure pour les options de configuration d'un dialogue de fichier.
 *
 * Permet de personnaliser le comportement du dialogue, comme les filtres, le chemin par défaut, ou la multi-sélection.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_FileDialogOptions {
    /**
     * Fenêtre parent pour le dialogue (peut être NULL).
     */
    SDL_Window *window;

    /**
     * Tableau de filtres pour les fichiers (peut être NULL).
     */
    const RC2D_FileDialogFilter *filters;

    /**
     * Nombre de filtres dans le tableau (ignoré si filters est NULL).
     */
    int num_filters;

    /**
     * Chemin par défaut pour ouvrir le dialogue (peut être NULL).
     */
    const char *default_location;

    /**
     * Permet la sélection de plusieurs fichiers ou dossiers (non supporté sur toutes les plateformes).
     */
    bool allow_many;

    /**
     * Titre du dialogue (peut être NULL pour le titre par défaut).
     */
    const char *title;

    /**
     * Texte du bouton d'acceptation (peut être NULL pour le texte par défaut).
     */
    const char *accept_label;

    /**
     * Texte du bouton d'annulation (peut être NULL pour le texte par défaut).
     */
    const char *cancel_label;
} RC2D_FileDialogOptions;

/**
 * \brief Callback pour les résultats des dialogues de fichier.
 *
 * Appelé lorsque l'utilisateur sélectionne un fichier/dossier, annule, ou si une erreur survient.
 *
 * \param userdata Données utilisateur passées lors de l'ouverture du dialogue.
 * \param filelist Liste des chemins sélectionnés (terminée par NULL), vide si annulé, ou NULL si erreur.
 * \param filter_index Index du filtre sélectionné, ou -1 si non applicable.
 *
 * \since Ce type est disponible depuis RC2D 1.0.0.
 */
typedef void (*RC2D_FileDialogCallback)(void *userdata, const char *const *filelist, int filter_index);

/**
 * \brief Ouvre un dialogue pour sélectionner un fichier existant.
 *
 * \param callback Fonction appelée avec les résultats du dialogue.
 * \param userdata Données utilisateur passées au callback.
 * \param options Options de configuration du dialogue (peut être NULL pour les valeurs par défaut).
 *
 * \threadsafety Cette fonction doit être appelée depuis le thread principal.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_filedialog_openFile(RC2D_FileDialogCallback callback, void *userdata, const RC2D_FileDialogOptions *options);

/**
 * \brief Ouvre un dialogue pour enregistrer un fichier (nouveau ou existant).
 *
 * \param callback Fonction appelée avec les résultats du dialogue.
 * \param userdata Données utilisateur passées au callback.
 * \param options Options de configuration du dialogue (peut être NULL pour les valeurs par défaut).
 *
 * \threadsafety Cette fonction doit être appelée depuis le thread principal.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_filedialog_saveFile(RC2D_FileDialogCallback callback, void *userdata, const RC2D_FileDialogOptions *options);

/**
 * \brief Ouvre un dialogue pour sélectionner un dossier.
 *
 * \param callback Fonction appelée avec les résultats du dialogue.
 * \param userdata Données utilisateur passées au callback.
 * \param options Options de configuration du dialogue (peut être NULL pour les valeurs par défaut).
 *
 * \threadsafety Cette fonction doit être appelée depuis le thread principal.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_filedialog_openFolder(RC2D_FileDialogCallback callback, void *userdata, const RC2D_FileDialogOptions *options);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_FILEDIALOG_H