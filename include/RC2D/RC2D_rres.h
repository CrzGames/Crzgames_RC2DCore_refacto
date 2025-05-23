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
 * Contient une texture SDL3 GPU et des métadonnées sur l'image (format, dimensions).
 * Cette structure est utilisée pour les sprites, les textures de fond, ou tout autre élément graphique
 * rendu via SDL3.
 *
 * \note La texture est créée à partir d'une surface SDL3 temporaire, qui est libérée automatiquement après
 * la création de la texture. L'utilisateur doit libérer la texture via SDL_DestroyTexture lorsqu'elle
 * n'est plus nécessaire.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct Image {
    /**
     * \brief Format de pixel de l'image, correspondant à un SDL_PIXELFORMAT_*.
     *
     * Les formats possibles incluent SDL_PIXELFORMAT_RGBA8888, SDL_PIXELFORMAT_RGB565, etc.,
     * en fonction du format RRES (par exemple, RRES_PIXELFORMAT_UNCOMP_R8G8B8A8).
     *
     * \note Certains formats RRES (comme RRES_PIXELFORMAT_UNCOMP_R32) n'ont pas d'équivalent direct
     * dans SDL3 et peuvent nécessiter une conversion manuelle.
     */
    int format;

    /**
     * \brief Largeur de l'image en pixels.
     */
    int width;

    /**
     * \brief Hauteur de l'image en pixels.
     */
    int height;

    /**
     * \brief Texture SDL3 GPU contenant les données de l'image.
     *
     * Cette texture est prête à être rendue via un renderer SDL3. Elle doit être libérée
     * avec  SDL_DestroyTexture lorsque l'image n'est plus utilisée.
     */
    SDL_Texture* texture;
} Image;

/**
 * \brief Structure représentant une police chargée à partir d'un chunk RRES.
 *
 * Contient une police TTF utilisable avec SDL3_ttf et un pointeur vers les données brutes
 * pour permettre une gestion manuelle si nécessaire.
 *
 * \note Les données brutes (`rawData`) sont allouées dynamiquement et doivent être libérées
 * via  SDL_free lorsque la police n'est plus utilisée. La police TTF doit être libérée
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
     * un rechargement si nécessaire. Elles doivent être libérées avec  SDL_free
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
 * \warning Les données brutes (`data`) doivent être libérées avec  SDL_free lorsque l'audio
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
     * Ces données doivent être libérées avec  SDL_free lorsque l'audio n'est plus utilisé.
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
 * Les données sont allouées dynamiquement et doivent être libérées avec SDL_free.
 *
 * \param chunk Le chunk RRES contenant les données brutes (doit être de type RRES_DATA_RAW).
 * \param size Pointeur vers une variable qui recevra la taille des données chargées (en octets).
 * \return Un pointeur vers les données brutes allouées, ou NULL en cas d'erreur.
 *
 * \warning Les données doivent être non compressées et non chiffrées. Si elles sont compressées ou chiffrées,
 * appelez d'abord rc2d_rres_unpackResourceChunk.
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
 * \return Une chaîne de caractères allouée dynamiquement, ou NULL en cas d'erreur. La chaîne doit être libérée avec SDL_free.
 *
 * \warning Les données doivent être non compressées et non chiffrées. Si elles sont compressées ou chiffrées,
 * appelez d'abord rc2d_rres_unpackResourceChunk.
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
 * \param renderer Le renderer SDL3 utilisé pour créer la texture.
 * \return Une structure Image contenant la texture et les métadonnées, ou une structure vide en cas d'erreur.
 *
 * \warning Les données doivent être non compressées et non chiffrées. Si elles sont compressées ou chiffrées,
 * appelez d'abord rc2d_rres_unpackResourceChunk.
 *
 * \note Certains formats RRES (comme RRES_PIXELFORMAT_UNCOMP_R32) n'ont pas d'équivalent direct dans SDL3 et peuvent
 * entraîner un échec de chargement. Dans ce cas, une conversion manuelle peut être nécessaire.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread, mais le renderer doit être utilisé dans
 * un contexte thread-safe conformément aux règles de SDL3.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
Image rc2d_rres_loadImageFromChunk(rresResourceChunk chunk, SDL_Renderer* renderer);

/**
 * \brief Charge des données audio Wave à partir d'un chunk RRES de type RRES_DATA_WAVE.
 *
 * Cette fonction charge les données PCM brutes et leurs métadonnées (taux d'échantillonnage, canaux, etc.). Si SDL3_mixer
 * est activé, elle peut également créer un Mix_Chunk pour la lecture audio (champ `sound`, actuellement désactivé).
 *
 * \param chunk Le chunk RRES contenant les données audio (doit être de type RRES_DATA_WAVE).
 * \return Une structure Wave contenant les données audio et les métadonnées, ou une structure vide en cas d'erreur.
 *
 * \warning Les données doivent être non compressées et non chiffrées. Si elles sont compressées ou chiffrées,
 * appelez d'abord rc2d_rres_unpackResourceChunk.
 *
 * \note La taille des données audio est calculée comme `frameCount * sampleSize * channels / 8`. Une erreur dans les
 * métadonnées peut entraîner une allocation incorrecte (voir problème GitHub #13 de rres).
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
 * \return Une structure Font contenant la police TTF et les données brutes, ou une structure vide en cas d'erreur.
 *
 * \warning Les données doivent être non compressées et non chiffrées. Si elles sont compressées ou chiffrées,
 * appelez d'abord rc2d_rres_unpackResourceChunk.
 *
 * \note La police est chargée avec une taille par défaut de 30 points. Pour une taille différente, rechargez la police
 * manuellement avec TTF_OpenFontRW.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
Font rc2d_rres_loadFontFromChunk(rresResourceChunk chunk);

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
 * \warning Le mot de passe doit être défini via rc2d_rres_setCipherPassword avant d'appeler cette fonction
 * pour les données chiffrées. Un mot de passe incorrect entraînera un code d'erreur 2.
 *
 * \note Les données chiffrées avec rrespacker incluent des métadonnées supplémentaires (comme le sel, le nonce, ou le MAC),
 * qui sont retirées du chunk après déchiffrement.
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