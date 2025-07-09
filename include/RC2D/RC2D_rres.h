#ifndef RC2D_RRES_H
#define RC2D_RRES_H

#include <rres/rres.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
// #include <SDL3_mixer/SDL_mixer.h>

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Structure représentant une image chargée à partir d'un chunk RRES.
 *
 * Contient une texture SDL3 GPU utilisée pour le rendu via l'API SDL3 GPU.
 * Cette structure est utilisée pour les sprites, les textures de fond, ou tout autre élément graphique.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct Image {
    /**
     * \brief Texture SDL3 GPU utilisée pour le rendu.
     *
     * Cette texture doit être libérée avec SDL_ReleaseGPUTexture lorsque l'image n'est plus utilisée.
     */
    SDL_GPUTexture* texture;
} Image;

/**
 * \brief Structure représentant une police chargée à partir d'un chunk RRES.
 *
 * Contient une police TTF utilisable avec SDL3_ttf et un pointeur vers les données brutes
 * pour permettre une gestion manuelle si nécessaire.
 *
 * \note Les données brutes (`rawData`) sont allouées dynamiquement et doivent être libérées
 * via  RC2D_free lorsque la police n'est plus utilisée. La police TTF doit être libérée
 * via  TTF_CloseFont.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct Font {
    /**
     * \brief Pointeur vers la police TTF chargée.
     *
     * Cette police est prête à être utilisée pour le rendu de texte avec SDL3_ttf.
     * Elle doit être libérée avec  TTF_CloseFont.
     */
    TTF_Font* font;

    /**
     * \brief Données brutes de la police (par exemple, fichier TTF ou OTF).
     *
     * Ces données sont conservées en mémoire pour permettre une gestion manuelle ou
     * un rechargement si nécessaire. Elles doivent être libérées avec  RC2D_free
     * lorsque la police n'est plus utilisée.
     */
    unsigned char* rawData;
} Font;

/**
 * \brief Structure représentant un fichier audio Wave chargé à partir d'un chunk RRES.
 *
 * Contient les métadonnées audio (nombre de frames, taux d'échantillonnage, etc.) et un pointeur
 * vers les données PCM brutes.
 *
 * \note Le champ `sound` (Mix_Chunk) est désactivé dans cette version, car SDL3_mixer n'est pas inclus
 * par défaut. Si SDL3_mixer est activé, décommentez l'inclusion correspondante et le champ `sound`.
 *
 * \warning Les données brutes (`data`) doivent être libérées avec  RC2D_free lorsque l'audio
 * n'est plus utilisé. Si `sound` est utilisé (avec SDL3_mixer), il doit être libéré avec  Mix_FreeChunk.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct Wave {
    /**
     * \brief Nombre total de frames dans l'audio.
     *
     * Une frame correspond à un échantillon par canal. Par exemple, pour un audio stéréo,
     * une frame contient deux échantillons (un pour le canal gauche, un pour le droit).
     */
    unsigned int frameCount;

    /**
     * \brief Taux d'échantillonnage de l'audio, en Hz (par exemple, 44100 Hz pour la qualité CD).
     */
    unsigned int sampleRate;

    /**
     * \brief Taille d'un échantillon en bits (par exemple, 16 bits pour une qualité standard).
     */
    unsigned int sampleSize;

    /**
     * \brief Nombre de canaux audio (1 pour mono, 2 pour stéréo, etc.).
     */
    unsigned int channels;

    /**
     * \brief Données audio PCM brutes.
     *
     * Ces données doivent être libérées avec  RC2D_free lorsque l'audio n'est plus utilisé.
     */
    void *data;

    /**
     * \brief Données audio prêtes à être jouées avec SDL3_mixer (facultatif).
     *
     * Ce champ est désactivé par défaut (commenté). Si SDL3_mixer est activé, il contient un
     * Mix_Chunk prêt à être joué via  Mix_PlayChannel. Il doit être libéré avec  Mix_FreeChunk.
     */
    // Mix_Chunk *sound;
} Wave;

/**
 * \brief Charge des données brutes à partir d'un chunk RRES de type RRES_DATA_RAW.
 *
 * Cette fonction est utile pour charger des fichiers binaires embarqués (comme des shaders, des modèles, ou des données personnalisées).
 *
 * \param chunk Le chunk RRES contenant les données brutes (doit être de type RRES_DATA_RAW).
 * \param size Pointeur vers une variable qui recevra la taille des données chargées (en octets).
 * \return Un pointeur vers les données brutes allouées, ou NULL en cas d'erreur.
 *
 * \note Les données doivent être non compressées et non chiffrées. Si elles sont compressées ou chiffrées,
 * appelez d'abord rc2d_rres_unpackResourceChunk.
 * 
 * \warning Le pointeur retourné doit être libéré par l'appelant avec `RC2D_free` lorsque les données ne sont plus nécessaires.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void *rc2d_rres_loadDataRawFromChunk(rresResourceChunk chunk, unsigned int *size);

/**
 * \brief Charge des données textuelles à partir d'un chunk RRES de type RRES_DATA_TEXT.
 *
 * Retourne une chaîne de caractères terminée par un NULL, allouée dynamiquement. Cette fonction est utile
 * pour charger des scripts, des dialogues, ou des fichiers de configuration textuels.
 *
 * \param chunk Le chunk RRES contenant les données textuelles (doit être de type RRES_DATA_TEXT).
 * \return Une chaîne de caractères allouée dynamiquement, ou NULL en cas d'erreur. La chaîne doit être libérée avec RC2D_free.
 *
 * \note Les données doivent être non compressées et non chiffrées. Si elles sont compressées ou chiffrées,
 * appelez d'abord rc2d_rres_unpackResourceChunk.
 * 
 * \warning La chaîne retournée doit être libérée par l'appelant avec `RC2D_free` lorsque le texte n'est plus nécessaire.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
char *rc2d_rres_loadDataTextFromChunk(rresResourceChunk chunk);

/**
 * \brief Charge une image à partir d'un chunk RRES de type RRES_DATA_IMAGE et crée une texture SDL3.
 *
 * Cette fonction convertit les données d'image RRES en une texture SDL3 utilisable pour le rendu. Elle prend en charge
 * différents formats de pixels définis dans rresPixelFormat et effectue les conversions nécessaires pour SDL3.
 *
 * \param chunk Le chunk RRES contenant les données d'image (doit être de type RRES_DATA_IMAGE).
 * \return Une structure Image contenant la surface SDL et les métadonnées, ou une structure vide en cas d'erreur.
 *
 * \note Les données doivent être non compressées et non chiffrées. Si elles sont compressées ou chiffrées,
 * appelez d'abord rc2d_rres_unpackResourceChunk.
 * 
 * \warning La texture `image.texture` doit être libérée par l'appelant avec `SDL_ReleaseGPUTexture` lorsque l'image n'est plus nécessaire.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread, mais le renderer doit être utilisé dans
 * un contexte thread-safe conformément aux règles de SDL3.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
Image rc2d_rres_loadImageFromChunk(rresResourceChunk chunk);

/**
 * \brief Charge des données audio Wave à partir d'un chunk RRES de type RRES_DATA_WAVE.
 *
 * Cette fonction charge les données PCM brutes et leurs métadonnées (taux d'échantillonnage, canaux, etc.). Si SDL3_mixer
 * est activé, elle peut également créer un Mix_Chunk pour la lecture audio (champ `sound`, actuellement désactivé).
 *
 * \param chunk Le chunk RRES contenant les données audio (doit être de type RRES_DATA_WAVE).
 * \return Une structure Wave contenant les données audio et les métadonnées, ou une structure vide en cas d'erreur.
 *
 * \note Les données doivent être non compressées et non chiffrées. Si elles sont compressées ou chiffrées,
 * appelez d'abord rc2d_rres_unpackResourceChunk.
 * 
 * \warning Les données brutes `wave.data` doivent être libérées par l'appelant avec `RC2D_free` lorsque l'audio n'est plus nécessaire.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
Wave rc2d_rres_loadWaveFromChunk(rresResourceChunk chunk);

/**
 * \brief Charge une police à partir d'un chunk RRES de type RRES_DATA_RAW (fichier TTF ou OTF).
 *
 * Cette fonction charge un fichier de police brut (par exemple, TTF ou OTF) et crée une police TTF utilisable avec SDL3_ttf.
 * Les données brutes sont conservées pour permettre un rechargement ou une gestion manuelle.
 *
 * \param chunk Le chunk RRES contenant les données de la police (doit être de type RRES_DATA_RAW).
 * \param ptsize La taille de la police en points.
 * \return Une structure Font contenant la police TTF et les données brutes, ou une structure vide en cas d'erreur.
 *
 * \note Les données doivent être non compressées et non chiffrées. Si elles sont compressées ou chiffrées,
 * appelez d'abord rc2d_rres_unpackResourceChunk.
 *
 * \warning La police `font.font` doit être libérée par l'appelant avec `TTF_CloseFont` lorsque la police n'est plus nécessaire 
 * et les données brutes `font.rawData` doivent être libérées par l'appelant avec `RC2D_free` lorsque la police n'est plus nécessaire.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
Font rc2d_rres_loadFontFromChunk(rresResourceChunk chunk, float ptsize);

/**
 * \brief Décompresse et/ou déchiffre les données d'un chunk RRES.
 *
 * Cette fonction prend un chunk RRES potentiellement compressé (LZ4) ou chiffré (AES, XChaCha20-Poly1305) et met à jour
 * ses données pour les rendre utilisables par les fonctions de chargement. Elle supporte les algorithmes utilisés par
 * l'outil rrespacker.
 *
 * \param chunk Pointeur vers le chunk RRES à traiter. Le chunk est modifié en place.
 * \return Un code d'erreur indiquant le résultat de l'opération :
 *         - 0 : Succès, décompression/déchiffrement terminé.
 *         - 1 : Algorithme de chiffrement non supporté.
 *         - 2 : Mot de passe incorrect lors du déchiffrement.
 *         - 3 : Algorithme de compression non supporté.
 *         - 4 : Erreur lors de la décompression des données.
 *
 * \note Le mot de passe doit être défini via rc2d_rres_setCipherPassword avant d'appeler cette fonction
 * pour les données chiffrées. Un mot de passe incorrect entraînera un code d'erreur 2.
 * 
 * \warning Les champs `chunk->data.props` et `chunk->data.raw` alloués dynamiquement doivent 
 * être libérés par l'appelant avec `RC2D_free` lorsque le chunk n'est plus nécessaire.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread, mais la gestion du mot de passe doit être
 * thread-safe si plusieurs threads y accèdent.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
int rc2d_rres_unpackResourceChunk(rresResourceChunk *chunk);

/**
 * \brief Définit le mot de passe utilisé pour le déchiffrement des données RRES.
 *
 * Cette fonction copie le mot de passe fourni dans un buffer interne sécurisé (limité à 15 caractères).
 * Le mot de passe est utilisé par rc2d_rres_unpackResourceChunk pour déchiffrer les données chiffrées.
 *
 * \param pass La chaîne de caractères contenant le mot de passe (max 15 caractères).
 *
 * \warning Le mot de passe est stocké comme un pointeur interne et doit être géré avec précaution. Appelez
 * rc2d_rres_cleanCipherPassword après utilisation pour effacer le mot de passe de la mémoire.
 *
 * \note Si le mot de passe dépasse 15 caractères, un avertissement est affiché via RC2D_log et le mot de passe
 * n'est pas défini.
 *
 * \threadsafety Cette fonction n'est pas thread-safe. Utilisez une synchronisation si plusieurs threads modifient le mot de passe.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_rres_setCipherPassword(const char *pass);

/**
 * \brief Récupère le mot de passe actuel utilisé pour le déchiffrement.
 *
 * Si aucun mot de passe n'a été défini, retourne le mot de passe par défaut ("password12345") utilisé par rrespacker.
 *
 * \return La chaîne de caractères contenant le mot de passe actuel.
 *
 * \warning Ne modifiez pas la chaîne retournée, car elle pointe vers une mémoire interne.
 *
 * \threadsafety Cette fonction n'est pas thread-safe si le mot de passe est modifié par un autre thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
const char *rc2d_rres_getCipherPassword(void);

/**
 * \brief Efface le mot de passe de déchiffrement de la mémoire.
 *
 * Cette fonction met à zéro le buffer interne contenant le mot de passe et réinitialise le pointeur interne.
 * Elle doit être appelée après le chargement des ressources chiffrées pour des raisons de sécurité.
 *
 * \threadsafety Cette fonction n'est pas thread-safe. Utilisez une synchronisation si plusieurs threads accèdent au mot de passe.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_rres_cleanCipherPassword(void);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_RRES_H