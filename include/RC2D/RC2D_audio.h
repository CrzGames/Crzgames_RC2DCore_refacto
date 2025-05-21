#ifndef RC2D_AUDIO_H
#define RC2D_AUDIO_H

// #include <SDL3/mixer.h>

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

// 0 and 128 it's min and max SDL2 volume.
#define RC2D_AUDIO_MIN_VOLUME 0
#define RC2D_AUDIO_MAX_VOLUME 128

/**
 * Enumération des types de fichiers audio pris en charge.
 *
 * @typedef {Enum} RC2D_FileAudioType
 * @property {Number} RC2D_FILE_TYPE_WAV - Le type de fichier audio est WAV.
 * @property {Number} RC2D_FILE_TYPE_MP3 - Le type de fichier audio est MP3.
 * @property {Number} RC2D_FILE_TYPE_UNKNOWN - Le type de fichier audio est inconnu.
 */
typedef enum RC2D_FileAudioType {
    RC2D_FILE_TYPE_WAV,
    RC2D_FILE_TYPE_MP3,
    RC2D_FILE_TYPE_UNKNOWN
} RC2D_FileAudioType;

/**
 * Enumération des types de sources audio.
 *
 * @typedef {Enum} RC2D_AudioType
 * @property {Number} RC2D_AUDIO_TYPE_MUSIC - Le type de source audio est une musique.
 * @property {Number} RC2D_AUDIO_TYPE_SOUND - Le type de source audio est un son.
 * @property {Number} RC2D_AUDIO_TYPE_UNKNOWN - Le type de source audio est inconnu.
 */
typedef enum RC2D_AudioType {
    RC2D_AUDIO_TYPE_MUSIC,
    RC2D_AUDIO_TYPE_SOUND,
    RC2D_AUDIO_TYPE_UNKNOWN
} RC2D_AudioType;

/**
 * Structure représentant une source audio pouvant être un effet sonore ou de la musique.
 * @typedef {object} RC2D_SourceAudio
 * @property {Mix_Chunk} sound - Pointeur vers un effet sonore chargé avec SDL_mixer
 * @property {Mix_Music} music - Pointeur vers une musique chargée avec SDL_mixer
 * @property {RC2D_FileAudioType} type - Type de source audio (musique ou son).
 * @property {RC2D_AudioType} fileType - Type de fichier audio (WAV ou MP3).
 */
typedef struct RC2D_SourceAudio {
    // Mix_Chunk* sound; 
    // Mix_Music* music;
    RC2D_AudioType audioType;
    RC2D_FileAudioType fileType;
} RC2D_SourceAudio;

/**
 * Crée une nouvelle source audio à partir d'un fichier spécifié.
 * Cette fonction charge un effet sonore ou de la musique en fonction du type de fichier et du type d'audio spécifiés.
 *
 * @param {const char*} filePath - Chemin vers le fichier audio à charger.
 * @param {RC2D_FileAudioType} fileType - Type du fichier audio (WAV, MP3, etc.).
 * @param {RC2D_AudioType} type - Type de l'audio (effet sonore ou musique).
 * 
 * @return {RC2D_SourceAudio} Une nouvelle source audio contenant le son ou la musique chargée. La source audio peut être vide en cas d'erreur lors du chargement.
 */
RC2D_SourceAudio rc2d_audio_newSource(const char* filePath, const RC2D_FileAudioType fileType, const RC2D_AudioType type);

/**
 * Libère les ressources associées à une source audio.
 * Cette fonction libère un effet sonore ou de la musique de la mémoire.
 *
 * @param {RC2D_SourceAudio*} sourceAudio - Pointeur vers la source audio à libérer.
 */
void rc2d_audio_freeSource(RC2D_SourceAudio* sourceAudio);

/**
 * Joue une source audio spécifiée, avec la possibilité de boucler la lecture.
 * Peut jouer un effet sonore ou de la musique en fonction de la source audio.
 *
 * @param {const RC2D_SourceAudio*} sourceAudio - La source audio à jouer.
 * @param {const int} loops - Le nombre de fois à boucler la lecture. -1 pour une boucle infinie, 0 pour une seule lecture.
 */
void rc2d_audio_play(const RC2D_SourceAudio* sourceAudio, const int loops);

/**
 * Met en pause la lecture de la source audio spécifiée.
 *
 * @param {const RC2D_SourceAudio*} sourceAudio - La source audio à mettre en pause.
 */
void rc2d_audio_pause(const RC2D_SourceAudio* sourceAudio);

/**
 * Arrête la lecture de la source audio spécifiée.
 *
 * @param {const RC2D_SourceAudio*} sourceAudio - La source audio à arrêter.
 */
void rc2d_audio_stop(const RC2D_SourceAudio* sourceAudio);

/**
 * Définit le volume sonore global pour toutes les sources audio.
 * Affecte à la fois les effets sonores et la musique.
 *
 * @param {int} volume - Le volume sonore à définir, compris entre 0 (silence) et 128 (volume maximal).
 */
void rc2d_audio_setVolume(const int volume);

#ifdef __cplusplus
}
#endif

#endif // RC2D_AUDIO_H