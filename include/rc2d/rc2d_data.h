#ifndef RC2D_DATA_H
#define RC2D_DATA_H

#include <stddef.h> // Required for : size_t

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumération des types de données pris en charge.
 * @typedef {enum} RC2D_DataType
 * @property {number} RC2D_DATA_TYPE_TEXT - Type de données texte.
 * @property {number} RC2D_DATA_TYPE_RAW_DATA - Type de données brutes / binaires.
 */
typedef enum RC2D_DataType {
    RC2D_DATA_TYPE_TEXT,
    RC2D_DATA_TYPE_RAW_DATA
} RC2D_DataType;

/**
 * Enumération des formats d'encodage pris en charge.
 * @typedef {enum} RC2D_EncodeFormat
 * @property {number} RC2D_ENCODE_FORMAT_BASE64 - Format d'encodage Base64.
 * @property {number} RC2D_ENCODE_FORMAT_HEX - Format d'encodage Hexadécimal.
 */
typedef enum RC2D_EncodeFormat {
    RC2D_ENCODE_FORMAT_BASE64,
    RC2D_ENCODE_FORMAT_HEX
} RC2D_EncodeFormat;

/**
 * Structure pour stocker des données encodées ou compressées.
 * 
 * Cette structure contient un pointeur vers les données encodées, 
 * ainsi que des informations sur la taille des données originales 
 * et la taille des données après encodage.
 *
 * @typedef {struct} RC2D_EncodedData
 * @property {char*} data - Pointeur vers les données encodées ou compressées.
 * @property {size_t} originalSize - Taille des données originales.
 * @property {size_t} encodedSize - Taille des données après encodage.
 * @property {RC2D_EncodeFormat} encodeFormat - Format d'encodage utilisé.
 * @property {RC2D_DataType} dataType - Type de données, texte ou données brutes.
 */
typedef struct RC2D_EncodedData {
    char* data;
    size_t originalSize;
    size_t encodedSize;
    RC2D_EncodeFormat encodeFormat;
    RC2D_DataType dataType;
} RC2D_EncodedData;

/**
 * Enumération des formats de compression pris en charge.
 * @typedef {enum} RC2D_CompressFormat
 * @property {number} RC2D_COMPRESS_FORMAT_LZ4 - Format de compression LZ4.
 */
typedef enum RC2D_CompressFormat {
    RC2D_COMPRESS_FORMAT_LZ4
} RC2D_CompressFormat;

/**
 * Cette structure est utilisée pour stocker les résultats d'une opération de compression,
 * incluant à la fois les données compressées elles-mêmes et les informations de taille nécessaires
 * pour la décompression. Elle facilite le passage de ces données à travers les fonctions de
 * compression et de décompression.
 * 
 * @typedef {struct} RC2D_CompressedData
 * @param data Pointeur vers les données compressées doit être libéré par l'appelant.
 * @param originalSize Taille des données d'entrée avant compression. Ceci est nécessaire pour
 *                     la décompression afin de valider l'intégrité des données décompressées.
 * @param compressedSize Taille des données après compression. Ceci est utilisé lors de la
 *                       décompression pour indiquer la quantité de données à traiter.
 * @param compressFormat Format de compression utilisé pour compresser les données.
 * @param dataType Type de données d'entrée, texte ou données brutes.
 */
typedef struct RC2D_CompressedData {
    unsigned char* data;
    size_t originalSize;
    size_t compressedSize; 
    RC2D_CompressFormat compressFormat;
    RC2D_DataType dataType;
} RC2D_CompressedData;

/**
 * Enumération des formats de chiffrement pris en charge.
 * @typedef {enum} RC2D_CipherFormat
 * @property {number} RC2D_CIPHER_FORMAT_AES - Chiffrement AES.
 * @property {number} RC2D_CIPHER_FORMAT_CHACHA20 - Chiffrement ChaCha20.
 * @property {number} RC2D_CIPHER_FORMAT_CHACHA20_POLY1305 - Chiffrement ChaCha20-Poly1305.
 * @property {number} RC2D_CIPHER_FORMAT_RSA - Chiffrement RSA.
 */
typedef enum RC2D_CipherFormat {
    RC2D_CIPHER_FORMAT_AES,
    RC2D_CIPHER_FORMAT_CHACHA20,
    RC2D_CIPHER_FORMAT_CHACHA20_POLY1305,
    RC2D_CIPHER_FORMAT_RSA // TODO: Implement RSA encryption/decryption
} RC2D_CipherFormat;

/**
 * Structure pour stocker des données cryptées.
 *
 * Cette structure contient les informations nécessaires pour stocker des données après qu'elles aient été cryptées.
 * Elle inclut un pointeur vers les données cryptées elles-mêmes, la passphrase utilisée pour le cryptage,
 * ainsi que diverses métadonnées telles que la taille des données originales et cryptées, le format de cryptage utilisé,
 * et le type de données (texte ou données brutes/binaires).
 *
 * @typedef {struct} RC2D_EncryptedData
 * @property {unsigned char*} data - Pointeur vers les données cryptées.
 * @property {char*} passphrase - La passphrase utilisée pour crypter les données et c'est a l'appelant de libérer la mémoire pour des raisons de sécurité.
 * @property {unsigned char*} hmac - Le code d'authentification de message (HMAC) utilisé pour valider l'intégrité des données cryptées.
 * @property {size_t} originalSize - La taille des données originales avant le cryptage.
 * @property {size_t} encryptedSize - La taille des données après le cryptage.
 * @property {size_t} hmacSize - La taille du code d'authentification de message (HMAC).
 * @property {RC2D_CipherFormat} cipherFormat - Le format de chiffrement utilisé pour crypter les données.
 * @property {RC2D_DataType} dataType - Le type de données cryptées, indiquant s'il s'agit de texte ou de données binaires.
 */
typedef struct RC2D_EncryptedData {
    unsigned char* data;
    char* passphrase;
    unsigned char* hmac;
    size_t originalSize;
    size_t encryptedSize;
    size_t hmacSize;
    RC2D_CipherFormat cipherFormat;
    RC2D_DataType dataType;
} RC2D_EncryptedData;

/**
 * Enumération des formats de hachage pris en charge.
 * Cette énumération est utilisée pour spécifier l'algorithme de hachage à utiliser
 * lors du calcul du hachage d'une chaîne de caractères. Chaque valeur représente
 * un algorithme de hachage cryptographique différent, offrant un éventail de choix
 * en fonction des exigences de sécurité et de performance.
 * 
 * @typedef {enum} RC2D_HashFormat
 * @property {number} RC2D_HASHING_FORMAT_MD5 - Algorithme de hachage MD5, produit un hachage de 128 bits. Non recommandé pour une utilisation sécurisée en raison de vulnérabilités connues.
 * @property {number} RC2D_HASHING_FORMAT_SHA1 - Algorithme de hachage SHA-1, produit un hachage de 160 bits. Déconseillé pour une utilisation sécurisée en raison de faiblesses connues.
 * @property {number} RC2D_HASHING_FORMAT_SHA224 - Algorithme de hachage SHA-224, une variante de SHA-2, produit un hachage de 224 bits.
 * @property {number} RC2D_HASHING_FORMAT_SHA256 - Algorithme de hachage SHA-256, une variante de SHA-2, produit un hachage de 256 bits, recommandé pour une utilisation sécurisée.
 * @property {number} RC2D_HASHING_FORMAT_SHA384 - Algorithme de hachage SHA-384, une variante de SHA-2, produit un hachage de 384 bits.
 * @property {number} RC2D_HASHING_FORMAT_SHA512 - Algorithme de hachage SHA-512, une variante de SHA-2, produit un hachage de 512 bits, offrant une sécurité renforcée.
 * @property {number} RC2D_HASHING_FORMAT_SHA3_224 - Algorithme de hachage SHA3-224, produit un hachage de 224 bits, basé sur l'algorithme de hachage Keccak.
 * @property {number} RC2D_HASHING_FORMAT_SHA3_256 - Algorithme de hachage SHA3-256, produit un hachage de 256 bits, basé sur l'algorithme de hachage Keccak.
 * @property {number} RC2D_HASHING_FORMAT_SHA3_384 - Algorithme de hachage SHA3-384, produit un hachage de 384 bits, basé sur l'algorithme de hachage Keccak.
 * @property {number} RC2D_HASHING_FORMAT_SHA3_512 - Algorithme de hachage SHA3-512, produit un hachage de 512 bits, basé sur l'algorithme de hachage Keccak.
 */
typedef enum RC2D_HashFormat {
    RC2D_HASHING_FORMAT_MD5,
    RC2D_HASHING_FORMAT_SHA1,
    RC2D_HASHING_FORMAT_SHA224,
    RC2D_HASHING_FORMAT_SHA256,
    RC2D_HASHING_FORMAT_SHA384,
    RC2D_HASHING_FORMAT_SHA512,
    RC2D_HASHING_FORMAT_SHA3_224,
    RC2D_HASHING_FORMAT_SHA3_256,
    RC2D_HASHING_FORMAT_SHA3_384,
    RC2D_HASHING_FORMAT_SHA3_512
} RC2D_HashFormat;

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
RC2D_EncodedData* rc2d_data_encode(const unsigned char* data, const size_t dataSize, const RC2D_DataType dataType, const RC2D_EncodeFormat format);

/**
 * Décode des données en utilisant le format spécifié.
 *
 * @param {const RC2D_EncodedData*} encodedData - Pointeur vers l'objet RC2D_EncodedData contenant les données encodées et les métadonnées nécessaires pour le décodage.
 * @return {unsigned char*} - Pointeur vers les données décodées, ou NULL en cas d'échec.
 *
 * @note Cette fonction alloue dynamiquement de la mémoire pour les données décodées. 
 * L'appelant est responsable de libérer cette mémoire.
 */
unsigned char* rc2d_data_decode(const RC2D_EncodedData* encodedData);


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
RC2D_CompressedData* rc2d_data_compress(const unsigned char* data, const size_t dataSize, const RC2D_DataType dataType, const RC2D_CompressFormat format);

/**
 * Décompresse les données contenues dans un objet RC2D_CompressedData.
 *
 * @param {RC2D_CompressedData*} compressedData - Pointeur vers l'objet RC2D_CompressedData contenant les données compressées et les métadonnées nécessaires pour la décompression.
 * @return {unsigned char*} - Pointeur vers les données décompressées en cas de succès, ou NULL en cas de format non supporté ou d'échec de la décompression.
 *
 * @note Les données retournées sont allouées dynamiquement et doivent être libérées par l'appelant pour éviter les fuites de mémoire.
 * @warning Cette fonction suppose que l'objet RC2D_CompressedData fourni est valide et a été correctement initialisé avec des données compressées valides.
 */
unsigned char* rc2d_data_decompress(const RC2D_CompressedData* compressedData);


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
char* rc2d_data_hash(const char* data, const RC2D_HashFormat format);


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
RC2D_EncryptedData* rc2d_data_encrypt(const unsigned char* data, const size_t dataSize, const char* passphrase, const RC2D_DataType dataType, const RC2D_CipherFormat format);

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
unsigned char* rc2d_data_decrypt(const RC2D_EncryptedData* encryptedData);

/**
 * Libère une structure RC2D_EncryptedData de manière sécurisée. Cette fonction s'assure que toutes les données sensibles
 * sont zéroisées avant de libérer la mémoire allouée, pour aider à prévenir les fuites de données.
 *
 * @param {RC2D_EncryptedData*} encryptedData - Pointeur vers la structure RC2D_EncryptedData à libérer.
 */
void rc2d_data_freeSecurity(RC2D_EncryptedData* encryptedData);

#ifdef __cplusplus
}
#endif  

#endif //RC2D_DATA_H