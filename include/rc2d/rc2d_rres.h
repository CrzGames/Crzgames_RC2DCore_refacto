#ifndef RC2D_RRES_H
#define RC2D_RRES_H

#include <rres/rres.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Image {
    int format; // SDL_PIXELFORMAT_*
    int width;
    int height;
    SDL_Texture* texture;
} Image;

typedef struct Font {
    TTF_Font* font;
    unsigned char* rawData; // Libéré la mémoire SEULEMENT quand nous utilisons plus la police
} Font;

typedef struct Wave {
    unsigned int frameCount; // Nombre total de frames dans l'audio
    unsigned int sampleRate; // Taux d'échantillonnage (par exemple, 44100 Hz)
    unsigned int sampleSize; // Taille d'un échantillon en bits (par exemple, 16 bits)
    unsigned int channels;   // Nombre de canaux (1 pour mono, 2 pour stéréo, etc.)
    void *data;              // Données audio brutes PCM + Libéré la mémoire SEULEMENT quand nous utilisons plus l'audio
    Mix_Chunk *sound;        // Données audio prêtes à être jouées
} Wave;


// REMARQUE : les données fragmentées (chunk) doivent être fournies non compressées ET non chiffrées
void *rc2d_rres_loadDataRawFromChunk(rresResourceChunk chunk, unsigned int *size);   // Load Data from rres resource chunk : RRES_DATA_RAW
char *rc2d_rres_loadDataTextFromChunk(rresResourceChunk chunk);                      // Load Text data from rres resource chunk : RRES_DATA_TEXT
Image rc2d_rres_loadImageFromChunk(rresResourceChunk chunk, SDL_Renderer* renderer); // Load Image data from rres resource chunk : RRES_DATA_IMAGE
Wave rc2d_rres_loadWaveFromChunk(rresResourceChunk chunk);                           // Load Wave data from rres resource chunk : RRES_DATA_WAVE
Font rc2d_rres_loadFontFromChunk(rresResourceChunk chunk);                           // Load Font data from rres resource chunk : RRES_DATA_RAW


/* 
rresUnpackResourceChunk (decompress or/and decrypt data)

REMARQUE, Function return :
0 - No error, decompression/decryption successful
1 - Encryption algorithm not supported
2 - Invalid password on decryption
3 - Compression algorithm not supported
4 - Error on data decompression
*/
int rc2d_rres_unpackResourceChunk(rresResourceChunk *chunk);


/*
Gérer le mot de passe pour le décryptage/déchiffrement des données

REMARQUE : 
- le mot de passe chiffré est conservé comme un pointeur interne vers la chaîne fournie.
- Il appartient à l'utilisateur de gérer correctement ces données sensibles.
- Le mot de passe doit être alloué et défini avant de charger une ressource chiffrée et il doit être nettoyé/effacé une fois la ressource chiffrée chargée
*/
void rc2d_rres_setCipherPassword(const char *pass); // Set password to be used on data decryption
const char *rc2d_rres_getCipherPassword(void);      // Get password to be used on data decryption
void rc2d_rres_cleanCipherPassword(void);           // Clean password to be used on data decryption


// Compute MD5 hash code, returns 4 integers array
unsigned int *ComputeMD5(unsigned char *data, int size); // AES encryption algorithm

#ifdef __cplusplus
}
#endif

#endif // RC2D_RRES_H