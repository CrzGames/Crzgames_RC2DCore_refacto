#define RRES_IMPLEMENTATION

#include <RC2D/RC2D_rres.h>
#include <RC2D/RC2D_logger.h>
#include <RC2D/RC2D_internal.h>
#include <RC2D/RC2D_memory.h>

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_iostream.h>

/**
 * Remarque : Les algorithmes pour encrypter/compresser sont utilisés de base via l'outil rrespacker
 */
#include <lz4/lz4.h> // Compression algorithm: LZ4
#include <monocypher/monocypher.h> // Encryption algorithm: XChaCha20-Poly1305
#include <aes/aes.h> // Encryption algorithm: AES 

/**
 * Password pointer, managed by user libraries
 */
static const char *rc2d_rres_password = NULL;

/**
 * Buffer pour stocker le mot de passe de déchiffrement (rc2d_rres_password).
 * Limité à 15 caractères pour éviter les débordements de mémoire + 1 pour le terminateur NULL ('\0').
 */ 
static char rc2d_rres_passwordBuffer[16];

/**
 * Password par défaut utilisé par l'outil rrespacker pour empaqueter les ressources
 */
static const char *passwordDefaultInRrespacker = "password12345";

/**
 * Calcule le hachage MD5 pour une séquence de données.
 *
 * Cette fonction prend en entrée une séquence de données et sa taille, puis calcule
 * et retourne le hachage MD5 de ces données. Le hachage MD5 est un hachage de 128 bits
 * (16 octets) utilisé pour vérifier l'intégrité des données.
 *
 * La fonction utilise une série de transformations et opérations bit à bit sur les données
 * d'entrée pour produire le hachage final. Le résultat est un tableau statique de 4 entiers
 * non signés (représentant les 128 bits du hachage MD5) qui doit être utilisé immédiatement
 * ou copié ailleurs car il sera écrasé lors du prochain appel à cette fonction.
 *
 * @param data Pointeur vers les données à hacher.
 * @param size Taille des données en octets.
 * @return Un pointeur vers un tableau statique de 4 entiers non signés représentant le hachage MD5.
 */
static unsigned int *ComputeMD5(const unsigned char *data, int size)
{
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

    static unsigned int hash[4] = { 0 };

    // NOTE: All variables are unsigned 32 bit and wrap modulo 2^32 when calculating

    // r specifies the per-round shift amounts
    unsigned int r[] = {
        7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
        5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
        4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
        6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
    };

    // Use binary integer part of the sines of integers (in radians) as constants// Initialize variables:
    unsigned int k[] = {
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
        0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
        0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
        0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
        0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
        0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
        0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
        0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
        0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
    };

    hash[0] = 0x67452301;
    hash[1] = 0xefcdab89;
    hash[2] = 0x98badcfe;
    hash[3] = 0x10325476;

    // Pre-processing: adding a single 1 bit
    // Append '1' bit to message
    // NOTE: The input bytes are considered as bits strings,
    // where the first bit is the most significant bit of the byte

    // Pre-processing: padding with zeros
    // Append '0' bit until message length in bit 448 (mod 512)
    // Append length mod (2 pow 64) to message

    int newDataSize = ((((size + 8)/64) + 1)*64) - 8;

    unsigned char *msg = (unsigned char *)RC2D_calloc(newDataSize + 64, 1);   // Also appends "0" bits (we alloc also 64 extra bytes...)
    SDL_memcpy(msg, data, size);
    msg[size] = 128;                 // Write the "1" bit

    unsigned int bitsLen = 8*size;
    SDL_memcpy(msg + newDataSize, &bitsLen, 4);  // We append the len in bits at the end of the buffer

    // Process the message in successive 512-bit chunks for each 512-bit chunk of message
    for (int offset = 0; offset < newDataSize; offset += (512/8))
    {
        // Break chunk into sixteen 32-bit words w[j], 0 <= j <= 15
        unsigned int *w = (unsigned int *)(msg + offset);

        // Initialize hash value for this chunk
        unsigned int a = hash[0];
        unsigned int b = hash[1];
        unsigned int c = hash[2];
        unsigned int d = hash[3];

        for (int i = 0; i < 64; i++)
        {
            unsigned int f, g;

            if (i < 16)
            {
                f = (b & c) | ((~b) & d);
                g = i;
            }
            else if (i < 32)
            {
                f = (d & b) | ((~d) & c);
                g = (5*i + 1)%16;
            }
            else if (i < 48)
            {
                f = b ^ c ^ d;
                g = (3*i + 5)%16;
            }
            else
            {
                f = c ^ (b | (~d));
                g = (7*i)%16;
            }

            unsigned int temp = d;
            d = c;
            c = b;
            b = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
            a = temp;
        }

        // Add chunk's hash to result so far
        hash[0] += a;
        hash[1] += b;
        hash[2] += c;
        hash[3] += d;
    }

    RC2D_free(msg);

    return hash;
}

void rc2d_rres_setCipherPassword(const char *pass)
{
    // Effacez le mot de passe précédent
    rc2d_rres_cleanCipherPassword();

    size_t passwordLength = SDL_strlen(pass);
    
    // Assurez-vous que la longueur du mot de passe ne dépasse pas la limite maximale
    if (passwordLength > 15) 
    {
        RC2D_log(RC2D_LOG_WARN, "Mot de passe trop long");            
        return;
    }

    // Initialise le buffer à zéro
    SDL_memset(rc2d_rres_passwordBuffer, 0, sizeof(rc2d_rres_passwordBuffer));

    // Copie le mot de passe dans le buffer
    SDL_memcpy(rc2d_rres_passwordBuffer, pass, passwordLength);

    // Définit le mot de passe pour le décryptage
    rc2d_rres_password = rc2d_rres_passwordBuffer;
}

const char *rc2d_rres_getCipherPassword(void)
{
    if (rc2d_rres_password == NULL) 
    {   
        rc2d_rres_password = passwordDefaultInRrespacker;
    }

    return rc2d_rres_password;
}

void rc2d_rres_cleanCipherPassword(void)
{
    // Efface le mot de passe
    SDL_memset(rc2d_rres_passwordBuffer, 0, sizeof(rc2d_rres_passwordBuffer));
    rc2d_rres_password = NULL;
}

void *rc2d_rres_loadDataRawFromChunk(rresResourceChunk chunk, unsigned int *size)
{
    // RRES_DATA_RAW = Raw file data
    if (rresGetDataType(chunk.info.type) == RRES_DATA_RAW)
    {
        // Si les données ne sont pas compressées/chiffrées, elles peuvent être utilisées directement
        if ((chunk.info.compType == RRES_COMP_NONE) && (chunk.info.cipherType == RRES_CIPHER_NONE))
        {
            void *rawData = NULL;

            // Allocate memory for raw data
            rawData = RC2D_calloc(chunk.data.props[0], 1);
            if (rawData != NULL) 
            {
                SDL_memcpy(rawData, chunk.data.raw, chunk.data.props[0]);
            }

            // Set size of data chunk
            *size = chunk.data.props[0];

            return rawData;
        }
    }

    *size = 0;
    return NULL;
}

char *rc2d_rres_loadDataTextFromChunk(rresResourceChunk chunk)
{
    // RRES_DATA_TEXT = Text data
    if (rresGetDataType(chunk.info.type) == RRES_DATA_TEXT)
    {
        // Si les données ne sont pas compressées/chiffrées, elles peuvent être utilisées directement
        if ((chunk.info.compType == RRES_COMP_NONE) && (chunk.info.cipherType == RRES_CIPHER_NONE))
        {
            // Créer un SDL_IOStream à partir des données textuelles
            SDL_IOStream *rw = SDL_IOFromMem(chunk.data.raw, chunk.data.props[0]);
            if (!rw)
            {
                RC2D_log(RC2D_LOG_ERROR, "Échec de la création de SDL_IOStream: %s", SDL_GetError());
                return NULL;
            }

            // Allouer de la mémoire pour le texte + terminateur NULL pour le texte
            char *text = (char *)RC2D_malloc(chunk.data.props[0] + 1);
            if (!text)
            {
                RC2D_log(RC2D_LOG_ERROR, "Échec de l'allocation mémoire pour le texte");
                SDL_CloseIO(rw);
                return NULL;
            }

            // Lire les données textuelles
            if (SDL_ReadIO(rw, text, chunk.data.props[0]) != chunk.data.props[0])
            {
                RC2D_log(RC2D_LOG_ERROR, "Échec de la lecture des données textuelles: %s", SDL_GetError());
                RC2D_free(text);
                SDL_CloseIO(rw);
                return NULL;
            }
            text[chunk.data.props[0]] = '\0'; // Ajouter un terminateur NULL pour le texte

            // Libérer le SDL_IOStream
            if (!SDL_CloseIO(rw))
            {
                RC2D_log(RC2D_LOG_WARN, "Échec de la fermeture de SDL_IOStream: %s", SDL_GetError());
            }

            return text;
        }
    }

    return NULL;
}

void freeImage(Image *image)
{
    if (image->texture != NULL)
    {
        SDL_ReleaseGPUTexture(rc2d_gpu_getDevice(), image->texture);
        image->texture = NULL;
    }
}

Image rc2d_rres_loadImageFromChunk(rresResourceChunk chunk)
{
    Image image = { 0 };

    // Vérifier que le chunk est de type RRES_DATA_IMAGE
    if (rresGetDataType(chunk.info.type) != RRES_DATA_IMAGE)
    {
        RC2D_log(RC2D_LOG_ERROR, "Le chunk n'est pas de type RRES_DATA_IMAGE\n");
        return image;
    }

    // Si les données ne sont pas compressées/chiffrées, elles peuvent être utilisées directement
    if ((chunk.info.compType != RRES_COMP_NONE) || (chunk.info.cipherType != RRES_CIPHER_NONE))
    {
        RC2D_log(RC2D_LOG_ERROR, "Les données doivent être non compressées et non chiffrées. Appelez rc2d_rres_unpackResourceChunk.\n");
        return image;
    }

    // Extraire les dimensions de l'image
    Uint32 width = chunk.data.props[0];
    Uint32 height = chunk.data.props[1];
    int format = chunk.data.props[2];

    // Mapper rresPixelFormat à SDL_GPUTextureFormat
    SDL_GPUTextureFormat gpuFormat = SDL_GPU_TEXTUREFORMAT_INVALID;
    switch (format)
    {
        case RRES_PIXELFORMAT_UNCOMP_GRAYSCALE:
            gpuFormat = SDL_GPU_TEXTUREFORMAT_R8_UNORM; // 8 bits, niveaux de gris
            break;
        case RRES_PIXELFORMAT_UNCOMP_GRAY_ALPHA:
            gpuFormat = SDL_GPU_TEXTUREFORMAT_R8G8_UNORM; // 16 bits, gris + alpha
            break;
        case RRES_PIXELFORMAT_UNCOMP_R5G6B5:
            gpuFormat = SDL_GPU_TEXTUREFORMAT_B5G6R5_UNORM; // 5 bits rouge, 6 vert, 5 bleu
            break;
        case RRES_PIXELFORMAT_UNCOMP_R8G8B8:
            gpuFormat = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM; // RGB 8 bits par canal, alpha inutilisé
            break;
        case RRES_PIXELFORMAT_UNCOMP_R5G5B5A1:
            gpuFormat = SDL_GPU_TEXTUREFORMAT_B5G5R5A1_UNORM; // RGBA avec 1 bit alpha
            break;
        case RRES_PIXELFORMAT_UNCOMP_R4G4B4A4:
            gpuFormat = SDL_GPU_TEXTUREFORMAT_B4G4R4A4_UNORM; // RGBA 4 bits chacun
            break;
        case RRES_PIXELFORMAT_UNCOMP_R8G8B8A8:
            gpuFormat = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM; // RGBA 8 bits par canal
            break;
        case RRES_PIXELFORMAT_UNCOMP_R32:
            gpuFormat = SDL_GPU_TEXTUREFORMAT_R32_FLOAT; // 32 bits flottant, canal unique
            break;
        case RRES_PIXELFORMAT_UNCOMP_R32G32B32:
            gpuFormat = SDL_GPU_TEXTUREFORMAT_R32G32B32A32_FLOAT; // RGB flottant, alpha inutilisé
            break;
        case RRES_PIXELFORMAT_UNCOMP_R32G32B32A32:
            gpuFormat = SDL_GPU_TEXTUREFORMAT_R32G32B32A32_FLOAT; // RGBA 32 bits flottant
            break;
        case RRES_PIXELFORMAT_COMP_DXT1_RGB:
        case RRES_PIXELFORMAT_COMP_DXT1_RGBA:
            gpuFormat = SDL_GPU_TEXTUREFORMAT_BC1_RGBA_UNORM; // DXT1, alpha binaire
            break;
        case RRES_PIXELFORMAT_COMP_DXT3_RGBA:
            gpuFormat = SDL_GPU_TEXTUREFORMAT_BC2_RGBA_UNORM; // DXT3
            break;
        case RRES_PIXELFORMAT_COMP_DXT5_RGBA:
            gpuFormat = SDL_GPU_TEXTUREFORMAT_BC3_RGBA_UNORM; // DXT5
            break;
        case RRES_PIXELFORMAT_COMP_ASTC_4x4_RGBA:
            gpuFormat = SDL_GPU_TEXTUREFORMAT_ASTC_4x4_UNORM; // ASTC 4x4
            break;
        case RRES_PIXELFORMAT_COMP_ASTC_8x8_RGBA:
            gpuFormat = SDL_GPU_TEXTUREFORMAT_ASTC_8x8_UNORM; // ASTC 8x8
            break;
        case RRES_PIXELFORMAT_COMP_ETC1_RGB:
        case RRES_PIXELFORMAT_COMP_ETC2_RGB:
        case RRES_PIXELFORMAT_COMP_ETC2_EAC_RGBA:
        case RRES_PIXELFORMAT_COMP_PVRT_RGB:
        case RRES_PIXELFORMAT_COMP_PVRT_RGBA:
            RC2D_log(RC2D_LOG_ERROR, "Format compressé %d (ETC/PVRTC) non supporté par SDL3 GPU\n", format);
            return image;
        default:
            RC2D_log(RC2D_LOG_ERROR, "Format de pixel RRES inconnu %d\n", format);
            return image;
    }

    // Vérifier si le format est supporté par le matériel
    if (!SDL_GPUTextureSupportsFormat(rc2d_gpu_getDevice(), gpuFormat, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_SAMPLER))
    {
        RC2D_log(RC2D_LOG_ERROR, "Format GPU %d non supporté par le matériel\n", gpuFormat);
        return image;
    }

    // Calculer la taille des données en fonction du format
    Uint32 dataSize;
    if (format >= RRES_PIXELFORMAT_COMP_DXT1_RGB && format <= RRES_PIXELFORMAT_COMP_ASTC_8x8_RGBA)
    {
        // Formats compressés : calculer la taille en fonction des blocs
        Uint32 blockWidth = (width + 3) / 4; // Blocs de 4x4 pixels
        Uint32 blockHeight = (height + 3) / 4;
        Uint32 blockSize;
        if (format == RRES_PIXELFORMAT_COMP_DXT1_RGB || format == RRES_PIXELFORMAT_COMP_DXT1_RGBA)
            blockSize = 8; // BC1: 8 octets par bloc
        else if (format == RRES_PIXELFORMAT_COMP_DXT3_RGBA || format == RRES_PIXELFORMAT_COMP_DXT5_RGBA)
            blockSize = 16; // BC2/BC3: 16 octets par bloc
        else if (format == RRES_PIXELFORMAT_COMP_ASTC_4x4_RGBA)
            blockSize = 16; // ASTC 4x4: 16 octets par bloc
        else if (format == RRES_PIXELFORMAT_COMP_ASTC_8x8_RGBA)
            blockSize = 16; // ASTC 8x8: 16 octets par bloc (1 bit/pixel)
        dataSize = blockWidth * blockHeight * blockSize;
    }
    else
    {
        // Formats non compressés : calculer la taille en fonction des octets par pixel
        Uint32 bytesPerPixel;
        switch (gpuFormat)
        {
            case SDL_GPU_TEXTUREFORMAT_R8_UNORM: bytesPerPixel = 1; break;
            case SDL_GPU_TEXTUREFORMAT_R8G8_UNORM: bytesPerPixel = 2; break;
            case SDL_GPU_TEXTUREFORMAT_B5G6R5_UNORM: bytesPerPixel = 2; break;
            case SDL_GPU_TEXTUREFORMAT_B5G5R5A1_UNORM: bytesPerPixel = 2; break;
            case SDL_GPU_TEXTUREFORMAT_B4G4R4A4_UNORM: bytesPerPixel = 2; break;
            case SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM: bytesPerPixel = 4; break;
            case SDL_GPU_TEXTUREFORMAT_R32_FLOAT: bytesPerPixel = 4; break;
            case SDL_GPU_TEXTUREFORMAT_R32G32B32A32_FLOAT: bytesPerPixel = 16; break;
            default: bytesPerPixel = 4; break; // Par défaut, suppose 4 octets
        }
        dataSize = width * height * bytesPerPixel;
    }

    // Créer la texture GPU
    SDL_GPUTextureCreateInfo createInfo = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = gpuFormat,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER, // Utilisation pour le rendu
        .width = width,
        .height = height,
        .layer_count_or_depth = 1,
        .num_levels = 1, // Pas de mipmaps pour l'instant
        .sample_count = SDL_GPU_SAMPLECOUNT_1,
        .props = 0
    };

    SDL_GPUTexture *texture = SDL_CreateGPUTexture(rc2d_gpu_getDevice(), &createInfo);
    if (!texture)
    {
        RC2D_log(RC2D_LOG_ERROR, "Échec de la création de la texture GPU: %s\n", SDL_GetError());
        return image;
    }

    // Créer un buffer de transfert
    SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(
        rc2d_gpu_getDevice(),
        &(SDL_GPUTransferBufferCreateInfo){
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size = dataSize
        }
    );
    if (!transferBuffer)
    {
        RC2D_log(RC2D_LOG_ERROR, "Échec de la création du buffer de transfert: %s\n", SDL_GetError());
        SDL_ReleaseGPUTexture(rc2d_gpu_getDevice(), texture);
        return image;
    }

    // Mapper le buffer de transfert et copier les données
    void *mappedData = SDL_MapGPUTransferBuffer(rc2d_gpu_getDevice(), transferBuffer, false);
    if (!mappedData)
    {
        RC2D_log(RC2D_LOG_ERROR, "Échec du mappage du buffer de transfert: %s\n", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(rc2d_gpu_getDevice(), transferBuffer);
        SDL_ReleaseGPUTexture(rc2d_gpu_getDevice(), texture);
        return image;
    }

    SDL_memcpy(mappedData, chunk.data.raw, dataSize);
    SDL_UnmapGPUTransferBuffer(rc2d_gpu_getDevice(), transferBuffer);

    // Créer un copy pass pour le téléversement
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(rc2d_gpu_getDevice());
    if (!commandBuffer)
    {
        RC2D_log(RC2D_LOG_ERROR, "Échec de l'acquisition du buffer de commandes: %s\n", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(rc2d_gpu_getDevice(), transferBuffer);
        SDL_ReleaseGPUTexture(rc2d_gpu_getDevice(), texture);
        return image;
    }

    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    if (!copyPass)
    {
        RC2D_log(RC2D_LOG_ERROR, "Échec du démarrage du copy pass: %s\n", SDL_GetError());
        SDL_ReleaseGPUTransferBuffer(rc2d_gpu_getDevice(), transferBuffer);
        SDL_ReleaseGPUTexture(rc2d_gpu_getDevice(), texture);
        SDL_ReleaseGPUCommandBuffer(commandBuffer);
        return image;
    }

    // Configurer les informations de transfert
    SDL_GPUTextureTransferInfo source = {
        .transfer_buffer = transferBuffer,
        .offset = 0, // Offset aligné à 512 octets pour Direct3D 12
        .pixels_per_row = width, // Données compactes
        .rows_per_layer = height
    };

    SDL_GPUTextureRegion destination = {
        .texture = texture,
        .mip_level = 0,
        .layer = 0,
        .x = 0,
        .y = 0,
        .z = 0,
        .w = width,
        .h = height,
        .d = 1
    };

    // Téléverser les données
    SDL_UploadToGPUTexture(copyPass, &source, &destination, false);

    // Terminer le copy pass
    SDL_EndGPUCopyPass(copyPass);

    // Soumettre le buffer de commandes
    SDL_SubmitGPUCommandBuffer(commandBuffer);

    // Libérer le buffer de transfert
    SDL_ReleaseGPUTransferBuffer(rc2d_gpu_getDevice(), transferBuffer);

    // Assigner la texture à la structure Image
    image.texture = texture;

    return image;
}

void freeWave(Wave *wave)
{
    // Libérer le Mix_Chunk (commenté car SDL3_mixer n'est pas inclus)
    /*
    if (wave->sound != NULL)
    {
        Mix_FreeChunk(wave->sound);
        wave->sound = NULL;
    }
    */

    // Libérer les données audio brutes
    if (wave->data != NULL)
    {
        RC2D_free(wave->data);
        wave->data = NULL;
    }
}

Wave rc2d_rres_loadWaveFromChunk(rresResourceChunk chunk)
{
    // RRES_DATA_WAVE = Sound data
    if (rresGetDataType(chunk.info.type) == RRES_DATA_WAVE)
    {
        // Si les données ne sont pas compressées/chiffrées, elles peuvent être utilisées directement
        if ((chunk.info.compType == RRES_COMP_NONE) && (chunk.info.cipherType == RRES_CIPHER_NONE))
        {
            Wave wave = { 0 };

            wave.frameCount = chunk.data.props[0];
            wave.sampleRate = chunk.data.props[1];
            wave.sampleSize = chunk.data.props[2];
            wave.channels = chunk.data.props[3];

            // Calcul de la taille des données audio brutes attendues
            unsigned int size = wave.frameCount * wave.sampleSize * wave.channels / 8;
            wave.data = RC2D_calloc(size, 1);
            if (wave.data)
            {
                SDL_memcpy(wave.data, chunk.data.raw, size);
            }
            else
            {
                RC2D_log(RC2D_LOG_ERROR, "Échec de l'allocation mémoire pour les données audio");
            }

            // Chargement des données PCM brutes en tant que Mix_Chunk (commenté car SDL3_mixer n'est pas inclus)
            /*
            wave.sound = Mix_QuickLoad_RAW((Uint8 *)wave.data, size);
            if (wave.sound == NULL)
            {
                RC2D_log(RC2D_LOG_ERROR, "Erreur lors du chargement du son: %s\n", Mix_GetError());
            }
            */

            return wave;
        }
    }

    Wave wave = { 0 }; // Retourner une structure vide en cas d'erreur
    return wave;
}

void freeFont(Font *font)
{
    if (font->font)
    {
        TTF_CloseFont(font->font);
        font->font = NULL;
    }

    // Libérer la mémoire pour rawData
    if (font->rawData != NULL)
    {
        RC2D_free(font->rawData);
        font->rawData = NULL;
    }
}

Font rc2d_rres_loadFontFromChunk(rresResourceChunk chunk, float ptsize)
{
    // RRES_DATA_RAW = Raw file data
    if (rresGetDataType(chunk.info.type) == RRES_DATA_RAW)
    {
        // Si les données ne sont pas compressées/chiffrées, elles peuvent être utilisées directement
        if ((chunk.info.compType == RRES_COMP_NONE) && (chunk.info.cipherType == RRES_CIPHER_NONE))
        {
            Font font = { 0 };

            unsigned int dataSize = 0;
            font.rawData = (unsigned char *)rc2d_rres_loadDataRawFromChunk(chunk, &dataSize);
            if (!font.rawData)
            {
                RC2D_log(RC2D_LOG_ERROR, "Échec du chargement des données brutes de la police");
                return font;
            }

            // Créer un SDL_IOStream à partir des données brutes
            SDL_IOStream *rw = SDL_IOFromMem(font.rawData, dataSize);
            if (!rw)
            {
                RC2D_log(RC2D_LOG_ERROR, "Échec de la création de SDL_IOStream: %s", SDL_GetError());
                RC2D_free(font.rawData);
                return font;
            }

            // Charger la police avec TTF_OpenFontIO
            font.font = TTF_OpenFontIO(rw, true, ptsize);
            if (!font.font)
            {
                RC2D_log(RC2D_LOG_ERROR, "Erreur de chargement de la police: %s\n", TTF_GetError());
                RC2D_free(font.rawData);
            }

            return font;
        }
    }

    Font font = { 0 }; // Retourner une structure vide en cas d'erreur
    return font;
}

int rc2d_rres_unpackResourceChunk(rresResourceChunk *chunk)
{
    int result = 0;
    bool updateProps = false;

    // Result error codes:
    //  0 - No error, decompression/decryption successful
    //  1 - Encryption algorithm not supported
    //  2 - Invalid password on decryption
    //  3 - Compression algorithm not supported
    //  4 - Error on data decompression

    // NOTE 1: If data is compressed/encrypted the properties are not loaded by rres.h because
    // it's up to the user to process the data; *chunk must be properly updated by this function
    // NOTE 2: rres-raylib should support the same algorithms and libraries used by rrespacker tool
    void *unpackedData = NULL;    

    // STEP 1. Data decryption
    //-------------------------------------------------------------------------------------
    unsigned char *decryptedData = NULL;

    switch (chunk->info.cipherType)
    {
        case RRES_CIPHER_NONE: decryptedData = (unsigned char *)chunk->data.raw; break;
        case RRES_CIPHER_AES:
        {
            // WARNING: Implementation dependant!
            // rrespacker tool appends (salt[16] + MD5[16]) to encrypted data for convenience,
            // Actually, chunk->info.packedSize considers those additional elements

            // Get some memory for the possible message output
            decryptedData = (unsigned char *)RC2D_calloc(chunk->info.packedSize - 16 - 16, 1);
            if (decryptedData != NULL) SDL_memcpy(decryptedData, chunk->data.raw, chunk->info.packedSize - 16 - 16);

            // Required variables for key stretching
            uint8_t key[32] = { 0 };                    // Encryption key
            uint8_t salt[16] = { 0 };                   // Key stretching salt

            // Retrieve salt from chunk packed data
            // salt is stored at the end of packed data, before nonce and MAC: salt[16] + MD5[16]
            SDL_memcpy(salt, ((unsigned char *)chunk->data.raw) + (chunk->info.packedSize - 16 - 16), 16);
            
            // Key stretching configuration
            crypto_argon2_config config;
            config.algorithm = CRYPTO_ARGON2_I; // Algorithm: Argon2i
            config.nb_blocks = 16384; // Blocks: 16 MB
            config.nb_passes = 3; // Iterations
            config.nb_lanes = 1; // Single-threaded

            crypto_argon2_inputs inputs;
            inputs.pass = (const uint8_t *)rc2d_rres_getCipherPassword(); // User password
            inputs.pass_size = (uint32_t)SDL_strlen(rc2d_rres_getCipherPassword()); // Password length
            inputs.salt = salt; // Salt for the password
            inputs.salt_size = 16;

            crypto_argon2_extras extras = { 0 };  

            void *workArea = RC2D_malloc(config.nb_blocks*1024);    // Key stretching work area

            // Generate strong encryption key, generated from user password using Argon2i algorithm (256 bit)
            crypto_argon2(key, 32, workArea, config, inputs, extras);

            // Wipe key generation secrets, they are no longer needed
            crypto_wipe(salt, 16);
            RC2D_free(workArea);

            // Required variables for decryption and message authentication
            unsigned int md5[4] = { 0 };                // Message Authentication Code generated on encryption

            // Retrieve MD5 from chunk packed data
            // NOTE: MD5 is stored at the end of packed data, after salt: salt[16] + MD5[16]
            SDL_memcpy(md5, ((unsigned char *)chunk->data.raw) + (chunk->info.packedSize - 16), 4*sizeof(unsigned int));

            // Message decryption, requires key
            struct AES_ctx ctx = { 0 };
            AES_init_ctx(&ctx, key);
            AES_CTR_xcrypt_buffer(&ctx, (uint8_t *)decryptedData, chunk->info.packedSize - 16 - 16);   // AES Counter mode, stream cipher

            // Verify MD5 to check if data decryption worked
            unsigned int decryptMD5[4] = { 0 };
            unsigned int *md5Ptr = ComputeMD5(decryptedData, chunk->info.packedSize - 16 - 16);
            for (int i = 0; i < 4; i++) decryptMD5[i] = md5Ptr[i];

            // Wipe secrets if they are no longer needed
            crypto_wipe(key, 32);

            if (SDL_memcmp(decryptMD5, md5, 4*sizeof(unsigned int)) == 0)    // Decrypted successfully!
            {
                chunk->info.packedSize -= (16 + 16);    // We remove additional data size from packed size (salt[16] + MD5[16])
                RC2D_log(RC2D_LOG_DEBUG, "RRES: %c%c%c%c: Data decrypted successfully (AES)\n", chunk->info.type[0], chunk->info.type[1], chunk->info.type[2], chunk->info.type[3]);
            }
            else
            {
                result = 2;    // Data was not decrypted as expected, wrong password or message corrupted
                RC2D_log(RC2D_LOG_WARN, "RRES: %c%c%c%c: Data decryption failed, wrong password or corrupted data\n", chunk->info.type[0], chunk->info.type[1], chunk->info.type[2], chunk->info.type[3]);
            }

        } break;
        case RRES_CIPHER_XCHACHA20_POLY1305:
        {
            // WARNING: Implementation dependant!
            // rrespacker tool appends (salt[16] + nonce[24] + MAC[16]) to encrypted data for convenience,
            // Actually, chunk->info.packedSize considers those additional elements

            // Get some memory for the possible message output
            decryptedData = (unsigned char *)RC2D_calloc(chunk->info.packedSize - 16 - 24 - 16, 1);

            // Required variables for key stretching
            uint8_t key[32] = { 0 };                    // Encryption key
            uint8_t salt[16] = { 0 };                   // Key stretching salt

            // Retrieve salt from chunk packed data
            // salt is stored at the end of packed data, before nonce and MAC: salt[16] + nonce[24] + MAC[16]
            SDL_memcpy(salt, ((unsigned char *)chunk->data.raw) + (chunk->info.packedSize - 16 - 24 - 16), 16);
            
            // Key stretching configuration
            crypto_argon2_config config;
            config.algorithm = CRYPTO_ARGON2_I; // Algorithm: Argon2i
            config.nb_blocks = 16384; // Blocks: 16 MB
            config.nb_passes = 3; // Iterations
            config.nb_lanes = 1; // Single-threaded

            crypto_argon2_inputs inputs;
            inputs.pass = (const uint8_t *)rc2d_rres_getCipherPassword(); // User password
            inputs.pass_size = (uint32_t)SDL_strlen(rc2d_rres_getCipherPassword()); // Password length
            inputs.salt = salt; // Salt for the password
            inputs.salt_size = 16;

            crypto_argon2_extras extras = { 0 };  

            void *workArea = RC2D_malloc(config.nb_blocks*1024);    // Key stretching work area

            // Generate strong encryption key, generated from user password using Argon2i algorithm (256 bit)
            crypto_argon2(key, 32, workArea, config, inputs, extras);

            // Wipe key generation secrets, they are no longer needed
            crypto_wipe(salt, 16);
            RC2D_free(workArea);

            // Required variables for decryption and message authentication
            uint8_t nonce[24] = { 0 };                  // nonce used on encryption, unique to processed file
            uint8_t mac[16] = { 0 };                    // Message Authentication Code generated on encryption

            // Retrieve nonce and MAC from chunk packed data
            // nonce and MAC are stored at the end of packed data, after salt: salt[16] + nonce[24] + MAC[16]
            SDL_memcpy(nonce, ((unsigned char *)chunk->data.raw) + (chunk->info.packedSize - 16 - 24), 24);
            SDL_memcpy(mac, ((unsigned char *)chunk->data.raw) + (chunk->info.packedSize - 16), 16);

            // Message decryption requires key, nonce and MAC
            int decryptResult = crypto_aead_unlock(decryptedData, mac, key, nonce, NULL, 0, (const unsigned char *)chunk->data.raw, (chunk->info.packedSize - 16 - 24 - 16));

            // Wipe secrets if they are no longer needed
            crypto_wipe(nonce, 24);
            crypto_wipe(key, 32);

            if (decryptResult == 0)    // Decrypted successfully!
            {
                chunk->info.packedSize -= (16 + 24 + 16);    // We remove additional data size from packed size
                RC2D_log(RC2D_LOG_DEBUG, "RRES: %c%c%c%c: Data decrypted successfully (XChaCha20)\n", chunk->info.type[0], chunk->info.type[1], chunk->info.type[2], chunk->info.type[3]);
            }
            else if (decryptResult == -1)
            {
                result = 2;   // Wrong password or message corrupted
                RC2D_log(RC2D_LOG_WARN, "RRES: %c%c%c%c: Data decryption failed, wrong password or corrupted data\n", chunk->info.type[0], chunk->info.type[1], chunk->info.type[2], chunk->info.type[3]);
            }
        } break;
        default:
        {
            result = 1;    // Decryption algorithm not supported
            RC2D_log(RC2D_LOG_WARN, "RRES: %c%c%c%c: Chunk data encryption algorithm not supported\n", chunk->info.type[0], chunk->info.type[1], chunk->info.type[2], chunk->info.type[3]);
        } break;
    }

    if ((result == 0) && (chunk->info.cipherType != RRES_CIPHER_NONE))
    {
        // Data is not encrypted any more, register it
        chunk->info.cipherType = RRES_CIPHER_NONE;
        updateProps = true;
    }

    // STEP 2: Data decompression (if decryption was successful)
    //-------------------------------------------------------------------------------------
    unsigned char *uncompData = NULL;

    if (result == 0)
    {
        switch (chunk->info.compType)
        {
            case RRES_COMP_NONE: unpackedData = decryptedData; break;
            case RRES_COMP_LZ4:
            {
                int uncompDataSize = 0;
                uncompData = (unsigned char *)RC2D_calloc(chunk->info.baseSize, 1);
                uncompDataSize = LZ4_decompress_safe((const char *)decryptedData, (char *)uncompData, chunk->info.packedSize, chunk->info.baseSize);

                if ((uncompData != NULL) && (uncompDataSize > 0))     // Decompression successful
                {
                    unpackedData = uncompData;
                    chunk->info.packedSize = uncompDataSize;
                    RC2D_log(RC2D_LOG_DEBUG, "RRES: %c%c%c%c: Data decompressed successfully (LZ4)\n", chunk->info.type[0], chunk->info.type[1], chunk->info.type[2], chunk->info.type[3]);
                }
                else
                {
                    result = 4;    // Decompression process failed
                    RC2D_log(RC2D_LOG_WARN, "RRES: %c%c%c%c: Chunk data decompression failed\n", chunk->info.type[0], chunk->info.type[1], chunk->info.type[2], chunk->info.type[3]);
                }

                // WARNING: Decompression could be successful but not the original message size returned
                if (uncompDataSize != chunk->info.baseSize) RC2D_log(RC2D_LOG_WARN, "RRES: Decompressed data could be corrupted, unexpected size\n");
            } break;
            default:
            {
                result = 3;
                RC2D_log(RC2D_LOG_WARN, "RRES: %c%c%c%c: Chunk data compression algorithm not supported\n", chunk->info.type[0], chunk->info.type[1], chunk->info.type[2], chunk->info.type[3]);
            } break;
        }
    }

    if ((result == 0) && (chunk->info.compType != RRES_COMP_NONE))
    {
        // Data is not encrypted any more, register it
        chunk->info.compType = RRES_COMP_NONE;
        updateProps = true;
    }

    // Update chunk->data.propCount and chunk->data.props if required
    if (updateProps && (unpackedData != NULL))
    {
        // Data is decompressed/decrypted into chunk->data.raw but data.propCount and data.props[] are still empty,
        // they must be filled with the just updated chunk->data.raw (that contains everything)
        chunk->data.propCount = ((int *)unpackedData)[0];

        if (chunk->data.propCount > 0)
        {
            chunk->data.props = (unsigned int *)RC2D_calloc(chunk->data.propCount, sizeof(int));
            for (unsigned int i = 0; i < chunk->data.propCount; i++) chunk->data.props[i] = ((int *)unpackedData)[1 + i];
        }

        // Move chunk->data.raw pointer (chunk->data.propCount*sizeof(int)) positions
        void *raw = RC2D_calloc(chunk->info.baseSize - 20, 1);
        if (raw != NULL) SDL_memcpy(raw, ((unsigned char *)unpackedData) + 20, chunk->info.baseSize - 20);
        RC2D_free(chunk->data.raw);
        chunk->data.raw = raw;
        RC2D_free(unpackedData);
    }

    return result;
}