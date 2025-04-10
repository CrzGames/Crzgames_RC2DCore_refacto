#ifndef __EMSCRIPTEN__

#include "rc2d/rc2d_data.h"
#include "rc2d/rc2d_logger.h"

/*
Librairies pour la compression de données
*/
#include <lz4/lz4.h>      // Required for : Algorithm LZ4

/*
Librairies OpenSSL pour les algorithmes de chiffrement symétrique / asymétrique et de hachage
*/
#include <openssl/rsa.h>  // Required for : Algorithm RSA
#include <openssl/pem.h>  // Required for : Algorithm RSA
#include <openssl/evp.h>  // Required for : Algorithm AES, Algorithm SHA, XCHACHA..etc
#include <openssl/hmac.h> // Required for : HMAC (Keyed-Hashing for Message Authentication)
#include <openssl/rand.h> // Required for :

#include <openssl/err.h>  // Required for : Gestion des erreurs OpenSSL

/*
Libraries Standard C
*/
#include <stdlib.h>       // Required for : malloc, free
#include <memory.h>       // Required for : memcpy, memset
#include <string.h>       // Required for : strlen

/*
Encrypt and Decrypt
*/
#define SALT_SIZE 16      // Taille du sel pour le chiffrement par mot de passe (PBKDF2)
#define ITERATIONS 100000 // Nombre d'itérations pour le chiffrement par mot de passe (PBKDF2)

/**
 * Convertit un caractère hexadécimal en sa valeur décimale.
 * 
 * @param {char} c - Le caractère hexadécimal à convertir.
 * @returns {char} La valeur décimale du caractère hexadécimal.
 * @algorithm Conversion d'un caractère hexadécimal en décimal.
 */
static char hex_char_to_val(char c) 
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return 10 + c - 'A';
    if (c >= 'a' && c <= 'f') return 10 + c - 'a';

    return 0; // Gestion simplifiée des erreurs
}


static RC2D_EncodedData* hex_encode(const unsigned char* data, size_t sizeData, RC2D_DataType dataType) 
{
    if (data == NULL || sizeData == 0) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour l'encodage hexadécimal dans hex_encode().\n");
        return NULL; // Invalid input
    }

    static const char* hex_map = "0123456789ABCDEF";
    size_t extraChar = (dataType == RC2D_DATA_TYPE_TEXT) ? 1 : 0; // +1 pour le caractère nul si c'est du texte

    // Allouer de l'espace pour la chaîne hexadécimale avec ou sans caractère nul supplémentaire
    char* hex = malloc(sizeData * 2 + extraChar);
    if (hex == NULL) 
    {
        return NULL; // Échec de l'allocation mémoire
    }

    for (size_t i = 0; i < sizeData; ++i) 
    {
        hex[i * 2] = hex_map[(data[i] >> 4) & 0xF];
        hex[i * 2 + 1] = hex_map[data[i] & 0xF];
    }

    // Ajouter un caractère nul à la fin uniquement pour les données textuelles
    if (extraChar) 
    {
        hex[sizeData * 2] = '\0'; // Assure la compatibilité avec les chaînes C pour les données textuelles
    }

    RC2D_EncodedData* encodedData = malloc(sizeof(RC2D_EncodedData));
    if (encodedData == NULL) 
    {
        free(hex); // Assurez-vous de libérer hex si l'allocation de encodedData échoue
        return NULL;
    }

    // Set the properties of the encoded data structure
    encodedData->data = hex;
    encodedData->originalSize = sizeData;
    encodedData->encodedSize = sizeData * 2;
    encodedData->encodeFormat = RC2D_ENCODE_FORMAT_HEX;
    encodedData->dataType = dataType;

    return encodedData;
}


static unsigned char* hex_decode(const char* hex, RC2D_DataType dataType) 
{
    if (hex == NULL) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour le décodage hexadécimal dans hex_decode().\n");
        return NULL; // Invalid input
    }

    size_t len = strlen(hex) / 2;

    // Allouer de l'espace supplémentaire pour le caractère nul uniquement si c'est du texte
    size_t extraChar = (dataType == RC2D_DATA_TYPE_TEXT) ? 1 : 0;

    unsigned char* plain = malloc(len + extraChar);
    if (plain == NULL) 
    {
        return NULL; // Gestion de l'échec de l'allocation mémoire
    }

    for (size_t i = 0; i < len; ++i) 
    {
        plain[i] = (hex_char_to_val(hex[i * 2]) << 4) + hex_char_to_val(hex[i * 2 + 1]);
    }

    // Ajouter un caractère nul à la fin uniquement pour les données de type texte
    if (extraChar) 
    {
        plain[len] = '\0';
    }

    return plain;
}

// Base64 encoding and decoding
static const char base64_map[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

/**
 * Encode des données en utilisant l'encodage Base64.
 *
 * @param {const unsigned char*} data - Pointeur vers les données à encoder.
 * @param {size_t} sizeData - Taille des données à encoder en octets.
 * @param {RC2D_DataType} dataType - Type des données à encoder.
 * @return {RC2D_EncodedData*} - Pointeur vers un objet RC2D_EncodedData contenant les données encodées et les métadonnées, ou NULL en cas d'échec.
 *
 * @note Cette fonction alloue dynamiquement de la mémoire pour les données encodées et pour la structure RC2D_EncodedData retournée. 
 * L'appelant est responsable de libérer cette mémoire.
 */
static RC2D_EncodedData* base64_encode(const unsigned char* data, size_t sizeData, RC2D_DataType dataType) 
{
    if (data == NULL || sizeData == 0) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour l'encodage Base64 dans base64_encode().\n");
        return NULL; // Invalid input
    }

    size_t output_len = 4 * ((sizeData + 2) / 3);

    // Adjust allocation for null-termination based on data type
    char* cipher = malloc(output_len + (dataType == RC2D_DATA_TYPE_TEXT ? 1 : 0)); 
    if (cipher == NULL) 
    {
        return NULL; // Memory allocation failed
    }

    size_t i, c = 0;
    for (i = 0; i < sizeData;) 
    {
        uint32_t octet_a = i < sizeData ? data[i++] : 0;
        uint32_t octet_b = i < sizeData ? data[i++] : 0;
        uint32_t octet_c = i < sizeData ? data[i++] : 0;

        uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

        cipher[c++] = base64_map[(triple >> 18) & 63];
        cipher[c++] = base64_map[(triple >> 12) & 63];
        cipher[c++] = base64_map[(triple >> 6) & 63];
        cipher[c++] = base64_map[triple & 63];
    }

    // Handle padding and null-termination
    if (sizeData % 3 > 0) 
    { // Add padding '=' characters if needed
        for (size_t pad = sizeData % 3; pad < 3; ++pad) 
        {
            cipher[c - (3 - pad)] = '=';
        }
    }

    if (dataType == RC2D_DATA_TYPE_TEXT) 
    { // Add null-terminator if data is text
        cipher[output_len] = '\0';
    }

    // Allocate memory for the encoded data structure
    RC2D_EncodedData* encodedData = malloc(sizeof(RC2D_EncodedData));
    if (!encodedData) 
    {
        free(cipher);
        return NULL;
    }

    // Set the properties of the encoded data structure
    encodedData->data = cipher;
    encodedData->originalSize = sizeData;
    encodedData->encodedSize = output_len;
    encodedData->encodeFormat = RC2D_ENCODE_FORMAT_BASE64;
    encodedData->dataType = dataType;

    return encodedData;
}

/**
 * Décode des données encodées en Base64.
 *
 * @param {const char*} cipher - Chaîne de caractères encodée en Base64 à décoder.
 * @param {RC2D_DataType} dataType - Type des données attendues après décodage.
 * @return {unsigned char*} - Pointeur vers les données décodées, ou NULL en cas d'échec.
 *
 * @note Cette fonction alloue dynamiquement de la mémoire pour les données décodées. 
 * L'appelant est responsable de libérer cette mémoire.
 */
static unsigned char* base64_decode(const char* cipher, RC2D_DataType dataType) 
{
    if (cipher == NULL) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour le décodage Base64 dans base64_decode().\n");
        return NULL;
    }

    // Calculer la taille des données décodées
    size_t input_len = strlen(cipher);
    size_t output_len = (input_len / 4) * 3;
    if (cipher[input_len - 1] == '=') output_len--;
    if (cipher[input_len - 2] == '=') output_len--;

    // Allouer avec ou sans espace supplémentaire pour le caractère nul selon le dataType
    unsigned char* plain = malloc(output_len + (dataType == RC2D_DATA_TYPE_TEXT ? 1 : 0));
    if (plain == NULL) 
    {
        return NULL;
    }

    int i, p = 0;
    for (i = 0; i < input_len; i += 4) 
    {
        // Trouver les positions des caractères dans base64_map
        int b[4];
        for (int j = 0; j < 4; j++) {
            b[j] = (cipher[i+j] == '=') ? 0 : (strchr(base64_map, cipher[i+j]) - base64_map);
        }

        // Convertir les 4 octets de base64 en 3 octets normaux
        plain[p++] = ((b[0] << 2) | (b[1] >> 4));
        if (cipher[i + 2] != '=') plain[p++] = ((b[1] & 0xF) << 4) | (b[2] >> 2);
        if (cipher[i + 3] != '=') plain[p++] = ((b[2] & 0x3) << 6) | b[3];
    }

    // Ajouter un caractère nul uniquement si les données sont de type texte
    if (dataType == RC2D_DATA_TYPE_TEXT) 
    {
        plain[output_len] = '\0';
    }

    return plain;
}

/**
 * Encode des données en utilisant le format spécifié.
 *
 * @param {const unsigned char*} data - Pointeur vers les données à encoder.
 * @param {size_t} dataSize - Taille des données à encoder en octets.
 * @param {RC2D_DataType} dataType - Type des données à encoder.
 * @param {RC2D_EncodeFormat} format - Format d'encodage à utiliser.
 * @return {RC2D_EncodedData*} - Pointeur vers un objet RC2D_EncodedData contenant les données encodées et les métadonnées, ou NULL en cas d'échec.
 *
 * @note Cette fonction alloue dynamiquement de la mémoire pour la structure RC2D_EncodedData retournée. 
 * L'appelant est responsable de libérer cette mémoire.
 */
RC2D_EncodedData* rc2d_data_encode(const unsigned char* data, size_t dataSize, RC2D_DataType dataType, RC2D_EncodeFormat format) {
    if (data == NULL || dataSize == 0) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour l'encodage dans rc2d_data_encode().\n");
        return NULL; // Invalid input
    }
    
    // Allocation de mémoire pour la structure RC2D_EncodedData
    RC2D_EncodedData* encodedData = (RC2D_EncodedData*)malloc(sizeof(RC2D_EncodedData));
    if (!encodedData) 
    {
        return NULL; // Memory allocation failed
    }

    // Encodage des données en utilisant le format spécifié
    switch (format) 
    {
        case RC2D_ENCODE_FORMAT_BASE64:
            encodedData = base64_encode(data, dataSize, dataType);
            break;
        case RC2D_ENCODE_FORMAT_HEX:
            encodedData = hex_encode(data, dataSize, dataType);
            break;
        default:
            free(encodedData);
            encodedData = NULL;
            return NULL; // Unsupported format
    }

    // Vérifier si l'encodage a échoué
    if (!encodedData || !encodedData->data) 
    {
        if (encodedData) 
        {
            free(encodedData);
            encodedData = NULL;
        }
        
        return NULL; // Encoding failed
    }

    // Additional properties are already set by the specific encoding functions
    return encodedData;
}

/**
 * Décode des données en utilisant le format spécifié.
 *
 * @param {const RC2D_EncodedData*} encodedData - Pointeur vers l'objet RC2D_EncodedData contenant les données encodées et les métadonnées nécessaires pour le décodage.
 * @return {unsigned char*} - Pointeur vers les données décodées, ou NULL en cas d'échec.
 *
 * @note Cette fonction alloue dynamiquement de la mémoire pour les données décodées. 
 * L'appelant est responsable de libérer cette mémoire.
 */
unsigned char* rc2d_data_decode(const RC2D_EncodedData* encodedData) 
{
    if (!encodedData || !encodedData->data) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour le décodage dans rc2d_data_decode().\n");
        return NULL; // Invalid data
    }

    unsigned char* decodedData = NULL;
    switch (encodedData->encodeFormat) 
    {
        case RC2D_ENCODE_FORMAT_BASE64:
            decodedData = base64_decode(encodedData->data, encodedData->dataType);
            break;
        case RC2D_ENCODE_FORMAT_HEX:
            decodedData = hex_decode(encodedData->data, encodedData->dataType);
            break;
        default:
            return NULL; // Unsupported format
    }

    // The caller is responsible for freeing the memory allocated for the decoded data
    return decodedData;
}

/**
 * Compresse des données en utilisant l'algorithme de compression LZ4.
 *
 * @param {const unsigned char*} data - Pointeur vers les données en clair à compresser.
 * @param {size_t} dataSize - Taille des données en clair en octets.
 * @param {RC2D_CompressFormat} format - Format de compression utilisé, défini par l'énumération RC2D_CompressFormat.
 * @param {RC2D_DataType} dataType - Type de données fournies, influençant potentiellement l'optimisation de la compression.
 * @return {RC2D_CompressedData*} - Pointeur vers un nouvel objet RC2D_CompressedData contenant les données compressées et les métadonnées, ou NULL en cas d'échec de la compression ou de problèmes d'allocation mémoire.
 *
 * Cette fonction calcule la taille maximale des données compressées, alloue dynamiquement de la mémoire pour le résultat de la compression, puis effectue la compression. En cas de succès, elle retourne un objet RC2D_CompressedData contenant les données compressées ainsi que des informations supplémentaires telles que la taille originale et compressée des données, le format de compression et le type de données.
 *
 * @note L'objet RC2D_CompressedData retourné doit être libéré par l'appelant pour éviter les fuites de mémoire.
 * @warning La fonction vérifie la capacité d'allouer suffisamment de mémoire pour les données compressées et la structure RC2D_CompressedData. Elle retourne NULL en cas d'allocation mémoire échouée.
 */
static RC2D_CompressedData* compress_lz4(const unsigned char* data, size_t dataSize, RC2D_CompressFormat format, RC2D_DataType dataType) 
{
    if (!data || dataSize == 0) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour la compression dans compress_lz4().\n");
        return NULL; // Invalid input
    }

    // Calcul de la taille maximale des données compressées
    int maxCompressedSize = LZ4_compressBound(dataSize);

    // Allocation de mémoire pour les données compressées
    unsigned char* compressedBuffer = (unsigned char*)malloc(maxCompressedSize);
    if (!compressedBuffer) 
    {
        // Échec de l'allocation mémoire
        return NULL;
    }

    // Compression des données
    int compressedDataSize = LZ4_compress_default((const char*)data, (char*)compressedBuffer, dataSize, maxCompressedSize);
    if (compressedDataSize <= 0) 
    {
        // Échec de la compression
        free(compressedBuffer);
        return NULL;
    }

    // Allocation de la structure pour stocker les résultats de la compression
    RC2D_CompressedData* compressedData = (RC2D_CompressedData*)malloc(sizeof(RC2D_CompressedData));
    if (!compressedData) 
    {
        free(compressedBuffer);
        return NULL;
    }

    // Stockage des résultats de la compression dans la structure
    compressedData->data = compressedBuffer;
    compressedData->originalSize = dataSize;
    compressedData->compressedSize = compressedDataSize;
    compressedData->compressFormat = format;
    compressedData->dataType = dataType;

    return compressedData;
}

/**
 * Décompresse les données compressées avec LZ4 contenues dans un objet RC2D_CompressedData.
 *
 * @param {const RC2D_CompressedData*} compressedData - Pointeur vers l'objet RC2D_CompressedData contenant les données compressées, la taille originale des données, le format de compression et le type de données.
 * @return {unsigned char*} - Pointeur vers les données décompressées en cas de succès, ou NULL en cas d'échec de la décompression ou de problèmes de validation des données d'entrée.
 *
 * Cette fonction alloue dynamiquement de la mémoire pour les données décompressées, y compris un espace supplémentaire pour un caractère nul si les données sont de type texte, pour assurer leur compatibilité avec les chaînes C. L'appelant est responsable de la libération de cette mémoire pour éviter les fuites.
 *
 * @note L'allocation mémoire tient compte de la nécessité d'ajouter un caractère nul à la fin des données de type texte.
 * @warning La fonction vérifie la validité de l'objet RC2D_CompressedData fourni ainsi que la présence des données compressées. Elle retourne NULL en cas de données d'entrée invalides ou d'échec de l'allocation mémoire.
 */
static unsigned char* decompress_lz4(const RC2D_CompressedData* compressedData) 
{
    if (!compressedData || !compressedData->data) 
    {
        // Validation des données d'entrée
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour la décompression dans decompress_lz4().\n");
        return NULL;
    }

    // Détermine si un espace supplémentaire est nécessaire pour le caractère nul dans le cas des données textuelles
    size_t extraSpace = compressedData->dataType == RC2D_DATA_TYPE_TEXT ? 1 : 0;

    // Allocation de mémoire pour les données décompressées avec éventuellement un espace supplémentaire pour le caractère nul
    unsigned char* decompressedData = (unsigned char*)malloc(compressedData->originalSize + extraSpace);
    if (!decompressedData) 
    {
        // Échec de l'allocation mémoire
        free(decompressedData); // Assurez-vous de libérer la mémoire si l'allocation échoue
        return NULL;
    }

    // Décompression des données
    int decompressedSize = LZ4_decompress_safe((const char*)compressedData->data, (char*)decompressedData, compressedData->compressedSize, compressedData->originalSize);
    if (decompressedSize < 0) 
    {
        // Échec de la décompression
        free(decompressedData);
        return NULL;
    }

    // Ajoute un caractère nul à la fin si les données sont de type texte pour assurer la compatibilité avec les chaînes C
    if (extraSpace) 
    {
        decompressedData[compressedData->originalSize] = '\0';
    }

    return decompressedData;
}

/**
 * Compresse des données en utilisant le format de compression spécifié.
 *
 * @param {const unsigned char*} data - Pointeur vers les données en clair à compresser.
 * @param {size_t} dataSize - Taille des données en clair en octets.
 * @param {RC2D_DataType} dataType - Type de données fournies, influençant potentiellement l'optimisation de la compression.
 * @param {RC2D_CompressFormat} format - Format de compression à utiliser, défini par l'énumération RC2D_CompressFormat.
 * @return {RC2D_CompressedData*} - Pointeur vers un nouvel objet RC2D_CompressedData contenant les données compressées et les métadonnées, ou NULL en cas de format non supporté ou d'échec de la compression.
 *
 * @note L'objet RC2D_CompressedData retourné encapsule les données compressées ainsi que des informations supplémentaires comme la taille originale des données et le format de compression utilisé.
 * @warning Il est de la responsabilité de l'appelant de libérer la mémoire allouée pour l'objet RC2D_CompressedData retourné afin d'éviter les fuites de mémoire.
 */
RC2D_CompressedData* rc2d_data_compress(const unsigned char* data, const size_t dataSize, const RC2D_DataType dataType, const RC2D_CompressFormat format)
{
    if (data == NULL || dataSize == 0) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour la compression dans rc2d_data_compress().\n");
        return NULL; // Données invalides
    }

    switch (format) 
    {
        case RC2D_COMPRESS_FORMAT_LZ4:
            return compress_lz4(data, dataSize, format, dataType);
        default:
            return NULL; // Format non supporté
    }
}

/**
 * Décompresse les données contenues dans un objet RC2D_CompressedData.
 *
 * @param {RC2D_CompressedData*} compressedData - Pointeur vers l'objet RC2D_CompressedData contenant les données compressées et les métadonnées nécessaires pour la décompression.
 * @return {unsigned char*} - Pointeur vers les données décompressées en cas de succès, ou NULL en cas de format non supporté ou d'échec de la décompression.
 *
 * @note Les données retournées sont allouées dynamiquement et doivent être libérées par l'appelant pour éviter les fuites de mémoire.
 * @warning Cette fonction suppose que l'objet RC2D_CompressedData fourni est valide et a été correctement initialisé avec des données compressées valides.
 */
unsigned char* rc2d_data_decompress(const RC2D_CompressedData* compressedData)
{
    if (compressedData == NULL || compressedData->data == NULL) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour la décompression dans rc2d_data_decompress().\n");
        return NULL; // Données invalides
    }

    switch (compressedData->compressFormat) 
    {
        case RC2D_COMPRESS_FORMAT_LZ4:
            return decompress_lz4(compressedData);
        default:
            return NULL; // Format non supporté
    }
}

/**
 * Convertit un tableau d'octets en une chaîne hexadécimale.
 * 
 * @param {unsigned char*} hash - Le tableau d'octets à convertir.
 * @param {size_t} size - La taille du tableau d'octets.
 * @returns {char*} La chaîne hexadécimale résultante.
 * @algorithm Conversion en hexadécimal pour l'affichage.
 */
static char* to_hex(unsigned char* hash, size_t size) 
{
    if (hash == NULL || size == 0) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour la conversion en hexadécimal dans to_hex().\n");
        return NULL; // Invalid input
    }

    char* output = malloc((size * 2) + 1);
    for (size_t i = 0; i < size; i++) 
    {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }

    output[size * 2] = '\0';

    return output;
}

/**
 * Calcule le hash d'une chaîne de caractères en utilisant le format de hashage spécifié.
 * Cette fonction prend une chaîne de caractères en entrée et retourne son hash calculé
 * selon l'algorithme spécifié par `format`. Les formats supportés incluent MD5, SHA1,
 * SHA224, SHA256, SHA384, SHA512, ainsi que les variantes SHA3. La sortie est convertie
 * en une chaîne hexadécimale représentant le hash. OpenSSL est utilisé pour le calcul du hash.
 * 
 * @param data La chaîne de caractères à hasher. Doit être une chaîne C terminée par un null ('\0').
 * @param format L'identifiant du format de hashage, spécifié par l'énumération `RC2D_HashFormat`.
 *        Les valeurs possibles sont :
 *        - RC2D_HASHING_FORMAT_MD5 (0)
 *        - RC2D_HASHING_FORMAT_SHA1 (1)
 *        - RC2D_HASHING_FORMAT_SHA224 (2)
 *        - RC2D_HASHING_FORMAT_SHA256 (3)
 *        - RC2D_HASHING_FORMAT_SHA384 (4)
 *        - RC2D_HASHING_FORMAT_SHA512 (5)
 *        - RC2D_HASHING_FORMAT_SHA3_224 (6)
 *        - RC2D_HASHING_FORMAT_SHA3_256 (7)
 *        - RC2D_HASHING_FORMAT_SHA3_384 (8)
 *        - RC2D_HASHING_FORMAT_SHA3_512 (9)
 *        Les valeurs non supportées entraînent un retour `NULL`.
 * 
 * @return Retourne une chaîne de caractères hexadécimale représentant le hash calculé de `data`.
 *         En cas d'échec (par exemple, allocation mémoire impossible, format de hashage non supporté,
 *         ou erreur dans le processus de hashage), la fonction retourne `NULL`.
 *         Le résultat doit être libéré par l'appelant pour éviter une fuite de mémoire.
 * 
 * @note Cette fonction utilise la bibliothèque OpenSSL pour effectuer le hashage. Assurez-vous
 *       que votre projet est configuré pour lier OpenSSL correctement.
 */
char* rc2d_data_hash(const char* data, const RC2D_HashFormat format) 
{
    if (data == NULL)
    {
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour le hachage dans rc2d_data_hash().\n");
        return NULL; // Invalid input
    }

    EVP_MD_CTX* mdctx;
    const EVP_MD* md_type;
    unsigned char hash_result[EVP_MAX_MD_SIZE]; // Buffer pour le résultat du hash
    unsigned int hash_len; // La longueur réelle du hash

    switch (format) 
    {
        case RC2D_HASHING_FORMAT_MD5:
            md_type = EVP_md5();
            break;
        case RC2D_HASHING_FORMAT_SHA1:
            md_type = EVP_sha1();
            break;
        case RC2D_HASHING_FORMAT_SHA224:
            md_type = EVP_sha224();
            break;
        case RC2D_HASHING_FORMAT_SHA256:
            md_type = EVP_sha256();
            break;
        case RC2D_HASHING_FORMAT_SHA384:
            md_type = EVP_sha384();
            break;
        case RC2D_HASHING_FORMAT_SHA512:
            md_type = EVP_sha512();
            break;
        case RC2D_HASHING_FORMAT_SHA3_224:
            md_type = EVP_sha3_224();
            break;
        case RC2D_HASHING_FORMAT_SHA3_256:
            md_type = EVP_sha3_256();
            break;
        case RC2D_HASHING_FORMAT_SHA3_384:
            md_type = EVP_sha3_384();
            break;
        case RC2D_HASHING_FORMAT_SHA3_512:
            md_type = EVP_sha3_512();
            break;
        default:
            return NULL; // Format de hachage non supporté
    }

    // Création du contexte de hachage
    mdctx = EVP_MD_CTX_new();
    if (!mdctx) 
        return NULL;

    // Initialisation du contexte de hachage
    // Calcul du hash
    // Finalisation du hash
    if (1 != EVP_DigestInit_ex(mdctx, md_type, NULL) || 
        1 != EVP_DigestUpdate(mdctx, data, strlen(data)) || 
        1 != EVP_DigestFinal_ex(mdctx, hash_result, &hash_len)) 
    {
        EVP_MD_CTX_free(mdctx);
        return NULL;
    }

    // Libération du contexte de hachage
    EVP_MD_CTX_free(mdctx);

    return to_hex(hash_result, hash_len);
}

/**
 * Remplit une zone de mémoire avec des zéros de manière sécurisée pour empêcher l'optimisation par le compilateur.
 * Cette fonction est utile pour effacer des données sensibles de la mémoire.
 *
 * @param {void*} v - Pointeur vers la zone de mémoire à zéroiser.
 * @param {size_t} n - Nombre d'octets à zéroiser.
 */
static void secure_zeroize(void* v, size_t n) 
{
    volatile unsigned char* p = (volatile unsigned char*)v;
    while (n--) *p++ = 0;
}

/**
 * Libère une structure RC2D_EncryptedData de manière sécurisée. Cette fonction s'assure que toutes les données sensibles
 * sont zéroisées avant de libérer la mémoire allouée, pour aider à prévenir les fuites de données.
 *
 * @param {RC2D_EncryptedData*} encryptedData - Pointeur vers la structure RC2D_EncryptedData à libérer.
 */
void rc2d_data_freeSecurity(RC2D_EncryptedData* encryptedData) 
{
    if (encryptedData != NULL) 
    {
        // Zéroise et libère les données chiffrées
        if (encryptedData->data != NULL) 
        {
            secure_zeroize(encryptedData->data, encryptedData->encryptedSize);
            free(encryptedData->data);
            encryptedData->data = NULL;
        }

        // Zéroise et libère le champ HMAC
        if (encryptedData->hmac != NULL) 
        {
            secure_zeroize(encryptedData->hmac, encryptedData->hmacSize);
            free(encryptedData->hmac);
            encryptedData->hmac = NULL;
        }
        
        // Libère la structure elle-même
        free(encryptedData);
        encryptedData = NULL;
    }
}

/**
 * Génère un HMAC (Hash-based Message Authentication Code) pour les données fournies en utilisant la clé spécifiée.
 * Cette fonction est utilisée pour assurer l'intégrité et l'authenticité des données.
 *
 * @param {const void*} key - Pointeur vers la clé utilisée pour générer le HMAC.
 * @param {int} keylen - Longueur de la clé en octets.
 * @param {const unsigned char*} data - Pointeur vers les données sur lesquelles générer le HMAC.
 * @param {int} datalen - Longueur des données en octets.
 * @param {unsigned char*} result - Pointeur vers le buffer de sortie où le HMAC sera stocké.
 * @param {unsigned int*} resultlen - Pointeur vers une variable où la longueur du HMAC généré sera stockée.
 * @return {unsigned char*} - Pointeur vers le buffer de sortie contenant le HMAC généré, ou NULL en cas d'échec.
 */
static unsigned char *generate_hmac(const void *key, int keylen, const unsigned char *data, int datalen, unsigned char *result, unsigned int *resultlen) 
{
    const EVP_MD *md = EVP_sha3_512();
    return HMAC(md, key, keylen, data, datalen, result, resultlen);
}

/**
 * Vérifie si le HMAC fourni correspond au HMAC des données données, en utilisant la clé spécifiée. Cette fonction
 * est utilisée pour vérifier l'intégrité et l'authenticité des données.
 *
 * @param {const void*} key - Pointeur vers la clé utilisée pour vérifier le HMAC.
 * @param {int} keylen - Longueur de la clé en octets.
 * @param {const unsigned char*} data - Pointeur vers les données dont l'intégrité est à vérifier.
 * @param {int} datalen - Longueur des données en octets.
 * @param {const unsigned char*} expected_hmac - Pointeur vers le HMAC attendu pour les données.
 * @param {unsigned int} expected_hmac_len - Longueur du HMAC attendu en octets.
 * @return {int} - Renvoie 0 si le HMAC ne correspond pas (échec de la vérification), et 1 si le HMAC correspond.
 *                 En cas de longueur différente entre le HMAC généré et le HMAC attendu, pourrait renvoyer -1 pour indiquer l'incompatibilité.
 */
static int verify_hmac(const void *key, int keylen, const unsigned char *data, int datalen, const unsigned char *expected_hmac, unsigned int expected_hmac_len) 
{
    unsigned char result[EVP_MAX_MD_SIZE];
    unsigned int resultlen;

    generate_hmac(key, keylen, data, datalen, result, &resultlen);

    if (expected_hmac_len != resultlen) 
    {
        return 0; // Longueur différente, échec de la vérification (remplacer par -1 je penses par convention)
    }

    return CRYPTO_memcmp(expected_hmac, result, resultlen) == 0;
}

/**
 * Chiffre des données en utilisant le cipher spécifié, une clé et un vecteur d'initialisation (IV).
 *
 * @param plaintext Pointeur vers les données en clair à chiffrer.
 * @param plaintext_len Longueur des données en clair en octets.
 * @param key Clé de chiffrement utilisée.
 * @param iv Vecteur d'initialisation pour le chiffrement.
 * @param ciphertext Buffer destiné à recevoir les données chiffrées. Doit être alloué par l'appelant.
 * @param format Format de chiffrement spécifié par l'énumération RC2D_CipherFormat.
 * @return La longueur des données chiffrées en cas de succès, ou -1 en cas d'échec.
 *
 * @note La taille du buffer `ciphertext` doit être au moins égale à `plaintext_len` + la taille du bloc de cipher.
 * @warning Les données chiffrées peuvent être plus longues que les données en clair en raison du padding.
 */
static int encrypt(const unsigned char *plaintext, int plaintext_len, const unsigned char *key, const unsigned char *iv, unsigned char *ciphertext, const RC2D_CipherFormat format) 
{
    if (plaintext == NULL || key == NULL || iv == NULL || ciphertext == NULL) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour le chiffrement dans encrypt().\n");
        return -1; // Invalid input
    }

    EVP_CIPHER_CTX *ctx;
    int len, ciphertext_len;
    const EVP_CIPHER *cipher;

    switch (format) 
    {
        case RC2D_CIPHER_FORMAT_AES:
            cipher = EVP_aes_256_cbc();
            break;
        case RC2D_CIPHER_FORMAT_CHACHA20:
            cipher = EVP_chacha20();
            break;
        case RC2D_CIPHER_FORMAT_CHACHA20_POLY1305:
            cipher = EVP_chacha20_poly1305();
            break;
        default:
            return -1;
    }

    // Création du contexte de chiffrement
    if(!(ctx = EVP_CIPHER_CTX_new())) 
        return -1;

    // Initialisation du contexte de chiffrement
    if(1 != EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv)) 
        return -1;

    // Chiffrement des données
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) 
        return -1;

    // Finalisation du chiffrement
    ciphertext_len = len;
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) 
        return -1;
    ciphertext_len += len;

    // Libération du contexte de chiffrement
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

/**
 * Déchiffre des données en utilisant le cipher spécifié, une clé et un vecteur d'initialisation (IV).
 *
 * @param ciphertext Pointeur vers les données chiffrées à déchiffrer.
 * @param ciphertext_len Longueur des données chiffrées en octets.
 * @param key Clé de déchiffrement utilisée.
 * @param iv Vecteur d'initialisation utilisé lors du chiffrement.
 * @param plaintext Buffer destiné à recevoir les données déchiffrées. Doit être alloué par l'appelant.
 * @param format Format de déchiffrement spécifié par l'énumération RC2D_CipherFormat.
 * @return La longueur des données déchiffrées en cas de succès, ou -1 en cas d'échec.
 *
 * @note La taille du buffer `plaintext` doit être au moins égale à `ciphertext_len`.
 * @warning Il est crucial que `key` et `iv` correspondent exactement à ceux utilisés lors du chiffrement.
 */
static int decrypt(const unsigned char *ciphertext, int ciphertext_len, const unsigned char *key, const unsigned char *iv, unsigned char *plaintext, const RC2D_CipherFormat format) 
{
    if (ciphertext == NULL || key == NULL || iv == NULL || plaintext == NULL) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour le déchiffrement dans decrypt().\n");
        return -1; // Invalid input
    }

    EVP_CIPHER_CTX *ctx;
    int len, plaintext_len;
    const EVP_CIPHER *cipher;

    switch (format) 
    {
        case RC2D_CIPHER_FORMAT_AES:
            cipher = EVP_aes_256_cbc();
            break;
        case RC2D_CIPHER_FORMAT_CHACHA20:
            cipher = EVP_chacha20();
            break;
        case RC2D_CIPHER_FORMAT_CHACHA20_POLY1305:
            cipher = EVP_chacha20_poly1305();
            break;
        default:
            return -1;
    }

    // Création du contexte de chiffrement
    if(!(ctx = EVP_CIPHER_CTX_new())) 
        return -1;

    // Initialisation du contexte de chiffrement
    if(1 != EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv)) 
        return -1;

    // Déchiffrement des données
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) 
        return -1;

    // Finalisation du déchiffrement
    plaintext_len = len;
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) 
        return -1;
    plaintext_len += len;

    // Libération du contexte de chiffrement
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

/**
 * Chiffre des données à l'aide d'une passphrase, en utilisant un format de chiffrement spécifié. Cette fonction génère une clé de chiffrement dérivée de la passphrase fournie au moyen d'une fonction de dérivation robuste, en combinant la passphrase avec un sel généré de manière aléatoire pour renforcer la sécurité. Elle utilise ensuite cette clé pour chiffrer les données et génère un HMAC des données chiffrées pour permettre un futur test d'intégrité. Les données chiffrées, le sel, l'IV, et le HMAC sont ensuite combinés en un objet RC2D_EncryptedData qui contient également des métadonnées telles que la taille originale des données et le format de chiffrement utilisé.
 *
 * @param {Uint8Array} data - Un pointeur vers les données non chiffrées à chiffrer.
 * @param {number} dataSize - La taille des données en octets.
 * @param {string} passphrase - Une passphrase utilisée pour générer la clé de chiffrement via une fonction de dérivation.
 * @param {RC2D_DataType} dataType - Le type de données fournies, influençant le traitement des données.
 * @param {RC2D_CipherFormat} format - Le format de chiffrement à utiliser, défini par l'énumération RC2D_CipherFormat.
 * @return {RC2D_EncryptedData*} Un pointeur vers un nouvel objet RC2D_EncryptedData contenant les données chiffrées, le HMAC, le sel, l'IV, et des métadonnées. Retourne NULL en cas d'échec.
 *
 * Processus détaillé :
 * 1. Validation des paramètres d'entrée.
 * 2. Sélection de l'algorithme de chiffrement basé sur le format spécifié.
 * 3. Génération aléatoire d'un sel unique pour renforcer la sécurité de la clé dérivée.
 * 4. Dérivation de la clé de chiffrement et de l'IV à partir de la passphrase et du sel généré, utilisant un algorithme de hachage sécurisé.
 * 5. Chiffrement des données avec la clé et l'IV, et préparation de l'espace pour le bloc de padding.
 * 6. Combinaison du sel, de l'IV, et des données chiffrées en une structure unique.
 * 7. Génération d'un HMAC pour les données chiffrées combinées, servant à vérifier l'intégrité des données lors du déchiffrement.
 * 8. Construction de l'objet RC2D_EncryptedData incluant toutes les informations nécessaires pour le déchiffrement et la vérification de l'intégrité.
 */
RC2D_EncryptedData* rc2d_data_encrypt(const unsigned char* data, size_t dataSize, const char* passphrase, const RC2D_DataType dataType, const RC2D_CipherFormat format) 
{
    if (data == NULL || dataSize == 0 || passphrase == NULL) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour le chiffrement dans rc2d_data_encrypt().\n");
        return NULL; // Données invalides
    }

    if (format == RC2D_CIPHER_FORMAT_AES || format == RC2D_CIPHER_FORMAT_CHACHA20 || 
        format == RC2D_CIPHER_FORMAT_CHACHA20_POLY1305) 
    {
        // Sélection du cipher en fonction du format
        const EVP_CIPHER *cipher;
        switch (format) 
        {
            case RC2D_CIPHER_FORMAT_AES:
                cipher = EVP_aes_256_cbc();
                break;
            case RC2D_CIPHER_FORMAT_CHACHA20:
                cipher = EVP_chacha20();
                break;
            case RC2D_CIPHER_FORMAT_CHACHA20_POLY1305:
                cipher = EVP_chacha20_poly1305();
                break;
            default:
                return NULL; // Gestion des formats non pris en charge
        }

        // Génération d'un sel unique pour cette opération de chiffrement
        unsigned char salt[SALT_SIZE]; // Taille standard pour un sel
        if (!RAND_bytes(salt, sizeof(salt))) 
        {
            rc2d_log(RC2D_LOG_ERROR, "Échec de la génération du sel dans rc2d_data_encrypt().\n");
            return NULL; // Échec de la génération du sel
        }

        // Préparation de la clé et de l'IV
        unsigned char key[EVP_MAX_KEY_LENGTH];
        unsigned char iv[EVP_MAX_IV_LENGTH];
        if (!PKCS5_PBKDF2_HMAC(passphrase, -1, salt, SALT_SIZE, ITERATIONS, EVP_sha3_512(), EVP_MAX_KEY_LENGTH, key)) 
        {
            rc2d_log(RC2D_LOG_ERROR, "Échec de la dérivation de la clé dans rc2d_data_encrypt().\n");
            return NULL; // Échec de la dérivation de la clé
        }

        // Génération aléatoire de l'IV
        if (!RAND_bytes(iv, sizeof(iv))) 
        {
            rc2d_log(RC2D_LOG_ERROR, "Échec de la génération de l'IV dans rc2d_data_encrypt().\n");
            return NULL; // Échec de la génération de l'IV
        }

        // Allocation et chiffrement
        unsigned char* ciphertext = malloc(dataSize + EVP_CIPHER_block_size(cipher)); // Prévoir espace pour le bloc de padding
        if (!ciphertext) 
        {
            rc2d_log(RC2D_LOG_ERROR, "Échec de l'allocation mémoire pour le ciphertext dans rc2d_data_encrypt().\n");
            return NULL; // Échec de l'allocation
        }

        // Chiffrement
        int ciphertext_len = encrypt(data, dataSize, key, iv, ciphertext, format);
        if (ciphertext_len < 0) 
        {
            rc2d_log(RC2D_LOG_ERROR, "Échec du chiffrement dans rc2d_data_encrypt().\n");
            free(ciphertext);
            return NULL; // Échec du chiffrement
        }

        // Combinaison du sel, de l'IV et du ciphertext
        size_t totalSize = sizeof(salt) + sizeof(iv) + ciphertext_len;
        unsigned char* encryptedData = malloc(totalSize);
        if (!encryptedData) 
        {
            rc2d_log(RC2D_LOG_ERROR, "Échec de l'allocation mémoire pour les données chiffrées dans rc2d_data_encrypt().\n");
            free(ciphertext);
            return NULL;
        }

        // Construction des données chiffrées: sel + IV + ciphertext
        memcpy(encryptedData, salt, sizeof(salt));
        memcpy(encryptedData + sizeof(salt), iv, sizeof(iv));
        memcpy(encryptedData + sizeof(salt) + sizeof(iv), ciphertext, ciphertext_len);
        
        // Libération du ciphertext intermédiaire
        free(ciphertext);

        // Création de la structure RC2D_EncryptedData
        RC2D_EncryptedData* encryptedDataStruct = malloc(sizeof(RC2D_EncryptedData));
        if (!encryptedDataStruct) 
        {
            rc2d_log(RC2D_LOG_ERROR, "Échec de l'allocation mémoire pour la structure RC2D_EncryptedData dans rc2d_data_encrypt().\n");
            free(encryptedData);
            return NULL;
        }

        // Générez le HMAC pour les données chiffrées
        unsigned char hmac[EVP_MAX_MD_SIZE];
        unsigned int hmac_len;
        if (!generate_hmac(key, EVP_MAX_KEY_LENGTH, encryptedData, totalSize, hmac, &hmac_len)) 
        {
            rc2d_log(RC2D_LOG_ERROR, "Échec de la génération de HMAC dans rc2d_data_encrypt().\n");
            // Gérer l'échec de la génération de HMAC
            free(encryptedDataStruct);
            free(encryptedData);
            return NULL;
        }

        // Stockez le HMAC dans la structure
        encryptedDataStruct->hmac = malloc(hmac_len);
        if (encryptedDataStruct->hmac == NULL) 
        {
            rc2d_log(RC2D_LOG_ERROR, "Échec de l'allocation mémoire pour le HMAC dans rc2d_data_encrypt().\n");
            // Gérer l'échec de l'allocation mémoire pour HMAC
            free(encryptedDataStruct);
            free(encryptedData);
            return NULL;
        }
        memcpy(encryptedDataStruct->hmac, hmac, hmac_len);
        encryptedDataStruct->hmacSize = hmac_len;

        // Stockez les données dans la structure :
        encryptedDataStruct->data = encryptedData;      // Données chiffrées (sel + IV + ciphertext)
        encryptedDataStruct->passphrase = passphrase;   // La gestion de la passphrase devrait être effectuée de manière sécurisée
        encryptedDataStruct->originalSize = dataSize;   // Taille des données avant chiffrement
        encryptedDataStruct->encryptedSize = totalSize; // Inclut sel, IV et ciphertext
        encryptedDataStruct->cipherFormat = format;     // Format de chiffrement utilisé
        encryptedDataStruct->dataType = dataType;       // Type de données fournies
        
        return encryptedDataStruct;
    }
    else if (format == RC2D_CIPHER_FORMAT_RSA) 
    {
        
    }
    else 
    {
        // Gestion des formats non pris en charge.
        return NULL;
    }
}

/**
 * Déchiffre les données contenues dans un objet RC2D_EncryptedData en utilisant la passphrase et le format de chiffrement spécifiés lors du chiffrement. Avant le déchiffrement, la fonction vérifie l'intégrité des données chiffrées en utilisant le HMAC fourni. Si le test d'intégrité réussit, la fonction procède au déchiffrement des données à l'aide de la clé dérivée de la passphrase et du sel inclus dans l'objet RC2D_EncryptedData. Cette approche garantit que seules les données non altérées seront déchiffrées, renforçant la sécurité des données échangées.
 *
 * @param {RC2D_EncryptedData*} encryptedData - Un pointeur vers l'objet RC2D_EncryptedData contenant les données chiffrées, l'IV, le sel, le HMAC, et des métadonnées.
 * @return {Uint8Array} Un pointeur vers les données déchiffrées en cas de succès, ou NULL en cas d'échec.
 *
 * Processus détaillé :
 * 1. Validation des paramètres d'entrée et vérification que l'objet RC2D_EncryptedData contient toutes les informations nécessaires.
 * 2. Extraction du sel et de l'IV à partir de l'objet RC2D_EncryptedData.
 * 3. Dérivation de la clé de chiffrement à partir de la passphrase et du sel inclus dans les données chiffrées.
 * 4. Vérification de l'intégrité des données chiffrées en utilisant le HMAC. Si les données ont été altérées, la fonction retourne NULL.
 * 5. Déchiffrement des données en utilisant la clé dérivée et l'IV.
 * 6. Si les données sont de type texte, ajout d'un caractère nul à la fin pour correctement terminer la chaîne de caractères.
 * 7. Retour des données déchiffrées en cas de succès.
 *
 * Cette fonction assure que seules les données validées par le HMAC seront déchiffrées, offrant ainsi une couche supplémentaire de sécurité.
 */
unsigned char* rc2d_data_decrypt(const RC2D_EncryptedData* encryptedData) 
{
    if (encryptedData == NULL || encryptedData->data == NULL || encryptedData->passphrase == NULL) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Données invalides pour le déchiffrement dans rc2d_data_decrypt().\n");
        return NULL; // Données d'entrée invalides
    }

    // Vérification du format de chiffrement pour déterminer l'action appropriée
    if (encryptedData->cipherFormat == RC2D_CIPHER_FORMAT_AES || encryptedData->cipherFormat == RC2D_CIPHER_FORMAT_CHACHA20 || 
        encryptedData->cipherFormat == RC2D_CIPHER_FORMAT_CHACHA20_POLY1305) 
    {
        // Extraction du sel et de l'IV
        unsigned char salt[SALT_SIZE];
        memcpy(salt, encryptedData->data, SALT_SIZE);

        unsigned char iv[EVP_MAX_IV_LENGTH];
        memcpy(iv, encryptedData->data + SALT_SIZE, EVP_MAX_IV_LENGTH);

        // Préparation de la clé
        unsigned char key[EVP_MAX_KEY_LENGTH];
        if (!PKCS5_PBKDF2_HMAC(encryptedData->passphrase, -1, salt, SALT_SIZE, ITERATIONS, EVP_sha3_512(), EVP_MAX_KEY_LENGTH, key)) 
        {
            rc2d_log(RC2D_LOG_ERROR, "Échec de la dérivation de la clé pour le déchiffrement dans rc2d_data_decrypt().\n");
            return NULL; // Échec de la dérivation de la clé
        }

        // Le début du ciphertext dans les données chiffrées
        unsigned char *ciphertext = encryptedData->data + SALT_SIZE + EVP_MAX_IV_LENGTH;
        size_t ciphertext_len = encryptedData->encryptedSize - (SALT_SIZE + EVP_MAX_IV_LENGTH);

        // Vérification de l'intégrité des données
        if (!verify_hmac(key, EVP_MAX_KEY_LENGTH, encryptedData->data, encryptedData->encryptedSize, encryptedData->hmac, encryptedData->hmacSize)) 
        {
            // L'intégrité des données a échoué, les données ont été altérées
            rc2d_log(RC2D_LOG_ERROR, "Intégrité des données altérée pour le déchiffrement dans rc2d_data_decrypt().\n");
            return NULL;
        }

        // Allocation pour le plaintext
        unsigned char* plaintext = malloc(ciphertext_len); // L'espace nécessaire pourrait être légèrement supérieur à `ciphertext_len`
        if (!plaintext) 
        {
            rc2d_log(RC2D_LOG_ERROR, "Échec de l'allocation mémoire pour le plaintext dans rc2d_data_decrypt().\n");
            return NULL; // Échec de l'allocation
        }

        // Déchiffrement
        int plaintext_len = decrypt(ciphertext, ciphertext_len, key, iv, plaintext, encryptedData->cipherFormat);
        if (plaintext_len < 0) 
        {
            rc2d_log(RC2D_LOG_ERROR, "Échec du déchiffrement dans rc2d_data_decrypt().\n");
            free(plaintext);
            return NULL; // Échec du déchiffrement
        }

        // Ajout d'un caractère nul à la fin si les données sont de type texte
        if (encryptedData->dataType == RC2D_DATA_TYPE_TEXT) 
        {
            plaintext = realloc(plaintext, plaintext_len + 1); // Ajustement pour le caractère nul
            if (!plaintext) 
            {
                rc2d_log(RC2D_LOG_ERROR, "Échec de l'allocation mémoire pour le caractère nul dans rc2d_data_decrypt().\n");
                return NULL; // Échec de l'allocation
            }

            plaintext[plaintext_len] = '\0';
        }

        return plaintext;
    }
    else if (encryptedData->cipherFormat == RC2D_CIPHER_FORMAT_RSA) 
    {

    }
    else 
    {
        // Gestion des formats non pris en charge.
        return NULL;
    }
}

#endif // __EMSCRIPTEN__