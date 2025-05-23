/**********************************************************************************************
*
*   rres v1.0 - A simple and easy-to-use file-format to package resources
*
*   CONFIGURATION:
*
*   #define RRES_IMPLEMENTATION
*       Generates the implementation of the library into the included file.
*       If not defined, the library is in header only mode and can be included in other headers
*       or source files without problems. But only ONE file should hold the implementation.
*
*   DEPENDENCIES:
*
*   rres library dependencies have been kept to the minimum. It depends only on some libc and SDL3 functionality:
*
*     - stdlib.h: Required for memory allocation: malloc(), calloc(), free()
*                 NOTE: Allocators can be redefined with macros RRES_MALLOC, RRES_CALLOC, RRES_FREE
*     - SDL3/SDL_iostream.h: Required for file access functionality: SDL_IOStream, SDL_IOFromFile(), SDL_SeekIO(), SDL_ReadIO(), SDL_TellIO(), SDL_CloseIO()
*     - string.h: Required for memory data management: memcpy(), memcmp()
*
*   VERSION HISTORY:
*
*     - 1.0 (12-May-2022): Implementation review for better alignment with rres specs
*     - 0.9 (28-Apr-2022): Initial implementation of rres specs
*
*   LICENSE: MIT
*
*   Copyright (c) 2016-2022 Ramon Santamaria (@raysan5)
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*
**********************************************************************************************/

#ifndef RRES_H
#define RRES_H

// Function specifiers in case library is build/used as a shared library (Windows)
#if defined(_WIN32)
    #if defined(BUILD_LIBTYPE_SHARED)
        #define RRESAPI __declspec(dllexport)
    #elif defined(USE_LIBTYPE_SHARED)
        #define RRESAPI __declspec(dllimport)
    #endif
#endif

#ifndef RRESAPI
    #define RRESAPI
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#ifndef RRES_MALLOC
    #define RRES_MALLOC(sz)         malloc(sz)
#endif
#ifndef RRES_CALLOC
    #define RRES_CALLOC(ptr,sz)     calloc(ptr,sz)
#endif
#ifndef RRES_REALLOC
    #define RRES_REALLOC(ptr,sz)    realloc(ptr,sz)
#endif
#ifndef RRES_FREE
    #define RRES_FREE(ptr)          free(ptr)
#endif

#define RRES_SUPPORT_LOG_INFO
#if defined(RRES_SUPPORT_LOG_INFO)
    #define RRES_LOG(...) printf(__VA_ARGS__)
#else
    #define RRES_LOG(...)
#endif

#define RRES_MAX_FILENAME_SIZE      1024

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct rresFileHeader {
    unsigned char id[4];
    unsigned short version;
    unsigned short chunkCount;
    unsigned int cdOffset;
    unsigned int reserved;
} rresFileHeader;

typedef struct rresResourceChunkInfo {
    unsigned char type[4];
    unsigned int id;
    unsigned char compType;
    unsigned char cipherType;
    unsigned short flags;
    unsigned int packedSize;
    unsigned int baseSize;
    unsigned int nextOffset;
    unsigned int reserved;
    unsigned int crc32;
} rresResourceChunkInfo;

typedef struct rresResourceChunkData {
    unsigned int propCount;
    unsigned int *props;
    void *raw;
} rresResourceChunkData;

typedef struct rresResourceChunk {
    rresResourceChunkInfo info;
    rresResourceChunkData data;
} rresResourceChunk;

typedef struct rresResourceMulti {
    unsigned int count;
    rresResourceChunk *chunks;
} rresResourceMulti;

typedef struct rresDirEntry {
    unsigned int id;
    unsigned int offset;
    unsigned int reserved;
    unsigned int fileNameSize;
    char fileName[RRES_MAX_FILENAME_SIZE];
} rresDirEntry;

typedef struct rresCentralDir {
    unsigned int count;
    rresDirEntry *entries;
} rresCentralDir;

typedef struct rresFontGlyphInfo {
    int x, y, width, height;
    int value;
    int offsetX, offsetY;
    int advanceX;
} rresFontGlyphInfo;

//----------------------------------------------------------------------------------
// Enums Definition
//----------------------------------------------------------------------------------
typedef enum rresResourceDataType {
    RRES_DATA_NULL = 0,
    RRES_DATA_RAW = 1,
    RRES_DATA_TEXT = 2,
    RRES_DATA_IMAGE = 3,
    RRES_DATA_WAVE = 4,
    RRES_DATA_VERTEX = 5,
    RRES_DATA_FONT_GLYPHS = 6,
    RRES_DATA_LINK = 99,
    RRES_DATA_DIRECTORY = 100,
} rresResourceDataType;

typedef enum rresCompressionType {
    RRES_COMP_NONE = 0,
    RRES_COMP_RLE = 1,
    RRES_COMP_DEFLATE = 10,
    RRES_COMP_LZ4 = 20,
    RRES_COMP_LZMA2 = 30,
    RRES_COMP_QOI = 40,
} rresCompressionType;

typedef enum rresEncryptionType {
    RRES_CIPHER_NONE = 0,
    RRES_CIPHER_XOR = 1,
    RRES_CIPHER_DES = 10,
    RRES_CIPHER_TDES = 11,
    RRES_CIPHER_IDEA = 20,
    RRES_CIPHER_AES = 30,
    RRES_CIPHER_AES_GCM = 31,
    RRES_CIPHER_XTEA = 40,
    RRES_CIPHER_BLOWFISH = 50,
    RRES_CIPHER_RSA = 60,
    RRES_CIPHER_SALSA20 = 70,
    RRES_CIPHER_CHACHA20 = 71,
    RRES_CIPHER_XCHACHA20 = 72,
    RRES_CIPHER_XCHACHA20_POLY1305 = 73,
} rresEncryptionType;

typedef enum rresErrorType {
    RRES_SUCCESS = 0,
    RRES_ERROR_FILE_NOT_FOUND,
    RRES_ERROR_FILE_FORMAT,
    RRES_ERROR_MEMORY_ALLOC,
} rresErrorType;

typedef enum rresTextEncoding {
    RRES_TEXT_ENCODING_UNDEFINED = 0,
    RRES_TEXT_ENCODING_UTF8 = 1,
    RRES_TEXT_ENCODING_UTF8_BOM = 2,
    RRES_TEXT_ENCODING_UTF16_LE = 10,
    RRES_TEXT_ENCODING_UTF16_BE = 11,
} rresTextEncoding;

typedef enum rresCodeLang {
    RRES_CODE_LANG_UNDEFINED = 0,
    RRES_CODE_LANG_C,
    RRES_CODE_LANG_CPP,
    RRES_CODE_LANG_CS,
    RRES_CODE_LANG_LUA,
    RRES_CODE_LANG_JS,
    RRES_CODE_LANG_PYTHON,
    RRES_CODE_LANG_RUST,
    RRES_CODE_LANG_ZIG,
    RRES_CODE_LANG_ODIN,
    RRES_CODE_LANG_JAI,
    RRES_CODE_LANG_GDSCRIPT,
    RRES_CODE_LANG_GLSL,
} rresCodeLang;

typedef enum rresPixelFormat {
    RRES_PIXELFORMAT_UNDEFINED = 0,
    RRES_PIXELFORMAT_UNCOMP_GRAYSCALE = 1,
    RRES_PIXELFORMAT_UNCOMP_GRAY_ALPHA,
    RRES_PIXELFORMAT_UNCOMP_R5G6B5,
    RRES_PIXELFORMAT_UNCOMP_R8G8B8,
    RRES_PIXELFORMAT_UNCOMP_R5G5B5A1,
    RRES_PIXELFORMAT_UNCOMP_R4G4B4A4,
    RRES_PIXELFORMAT_UNCOMP_R8G8B8A8,
    RRES_PIXELFORMAT_UNCOMP_R32,
    RRES_PIXELFORMAT_UNCOMP_R32G32B32,
    RRES_PIXELFORMAT_UNCOMP_R32G32B32A32,
    RRES_PIXELFORMAT_COMP_DXT1_RGB,
    RRES_PIXELFORMAT_COMP_DXT1_RGBA,
    RRES_PIXELFORMAT_COMP_DXT3_RGBA,
    RRES_PIXELFORMAT_COMP_DXT5_RGBA,
    RRES_PIXELFORMAT_COMP_ETC1_RGB,
    RRES_PIXELFORMAT_COMP_ETC2_RGB,
    RRES_PIXELFORMAT_COMP_ETC2_EAC_RGBA,
    RRES_PIXELFORMAT_COMP_PVRT_RGB,
    RRES_PIXELFORMAT_COMP_PVRT_RGBA,
    RRES_PIXELFORMAT_COMP_ASTC_4x4_RGBA,
    RRES_PIXELFORMAT_COMP_ASTC_8x8_RGBA
} rresPixelFormat;

typedef enum rresVertexAttribute {
    RRES_VERTEX_ATTRIBUTE_POSITION = 0,
    RRES_VERTEX_ATTRIBUTE_TEXCOORD1 = 10,
    RRES_VERTEX_ATTRIBUTE_TEXCOORD2 = 11,
    RRES_VERTEX_ATTRIBUTE_TEXCOORD3 = 12,
    RRES_VERTEX_ATTRIBUTE_TEXCOORD4 = 13,
    RRES_VERTEX_ATTRIBUTE_NORMAL = 20,
    RRES_VERTEX_ATTRIBUTE_TANGENT = 30,
    RRES_VERTEX_ATTRIBUTE_COLOR = 40,
    RRES_VERTEX_ATTRIBUTE_INDEX = 100,
} rresVertexAttribute;

typedef enum rresVertexFormat {
    RRES_VERTEX_FORMAT_UBYTE = 0,
    RRES_VERTEX_FORMAT_BYTE,
    RRES_VERTEX_FORMAT_USHORT,
    RRES_VERTEX_FORMAT_SHORT,
    RRES_VERTEX_FORMAT_UINT,
    RRES_VERTEX_FORMAT_INT,
    RRES_VERTEX_FORMAT_HFLOAT,
    RRES_VERTEX_FORMAT_FLOAT,
} rresVertexFormat;

typedef enum rresFontStyle {
    RRES_FONT_STYLE_UNDEFINED = 0,
    RRES_FONT_STYLE_REGULAR,
    RRES_FONT_STYLE_BOLD,
    RRES_FONT_STYLE_ITALIC,
} rresFontStyle;

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

RRESAPI rresResourceChunk rresLoadResourceChunk(const char *fileName, int rresId);
RRESAPI void rresUnloadResourceChunk(rresResourceChunk chunk);
RRESAPI rresResourceMulti rresLoadResourceMulti(const char *fileName, int rresId);
RRESAPI void rresUnloadResourceMulti(rresResourceMulti multi);
RRESAPI rresResourceChunkInfo rresLoadResourceChunkInfo(const char *fileName, int rresId);
RRESAPI rresResourceChunkInfo *rresLoadResourceChunkInfoAll(const char *fileName, unsigned int *chunkCount);
RRESAPI rresCentralDir rresLoadCentralDirectory(const char *fileName);
RRESAPI void rresUnloadCentralDirectory(rresCentralDir dir);
RRESAPI unsigned int rresGetDataType(const unsigned char *fourCC);
RRESAPI int rresGetResourceId(rresCentralDir dir, const char *fileName);
RRESAPI unsigned int rresComputeCRC32(unsigned char *data, int len);
RRESAPI void rresSetCipherPassword(const char *pass);
RRESAPI const char *rresGetCipherPassword(void);

#ifdef __cplusplus
}
#endif

#endif // RRES_H

/***********************************************************************************
*
*   RRES IMPLEMENTATION
*
************************************************************************************/

#if defined(RRES_IMPLEMENTATION)

#if (defined(__STDC__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
    #include <stdbool.h>
#elif !defined(__cplusplus) && !defined(bool)
    typedef enum bool { false = 0, true = !false } bool;
    #define RL_BOOL_TYPE
#endif

#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL_iostream.h>

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static const char *password = NULL;

//----------------------------------------------------------------------------------
// Module Internal Functions Declaration
//----------------------------------------------------------------------------------
static rresResourceChunkData rresLoadResourceChunkData(rresResourceChunkInfo info, void *packedData);

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
rresResourceChunk rresLoadResourceChunk(const char *fileName, int rresId)
{
    rresResourceChunk chunk = { 0 };
    SDL_IOStream *rresFile = SDL_IOFromFile(fileName, "rb");

    if (rresFile == NULL)
    {
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: [%s] rres file could not be opened: %s\n", fileName, SDL_GetError());
        return chunk;
    }

    RC2D_log(RC2D_LOG_INFO, "RRES: INFO: Loading resource from file: %s\n", fileName);

    rresFileHeader header = { 0 };
    if (SDL_ReadIO(rresFile, &header, sizeof(rresFileHeader)) != sizeof(rresFileHeader))
    {
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read file header: %s\n", SDL_GetError());
        SDL_CloseIO(rresFile);
        return chunk;
    }

    if (header.id[0] == 'r' && header.id[1] == 'r' && header.id[2] == 'e' && header.id[3] == 's' && header.version == 100)
    {
        bool found = false;

        for (int i = 0; i < header.chunkCount; i++)
        {
            rresResourceChunkInfo info = { 0 };
            if (SDL_ReadIO(rresFile, &info, sizeof(rresResourceChunkInfo)) != sizeof(rresResourceChunkInfo))
            {
                RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read chunk info: %s\n", SDL_GetError());
                break;
            }

            if (info.id == rresId)
            {
                found = true;
                RC2D_log(RC2D_LOG_INFO, "RRES: INFO: Found requested resource id: 0x%08x\n", info.id);
                RC2D_log(RC2D_LOG_INFO, "RRES: %c%c%c%c: Id: 0x%08x | Base size: %i | Packed size: %i\n",
                         info.type[0], info.type[1], info.type[2], info.type[3], info.id, info.baseSize, info.packedSize);

                if (info.nextOffset != 0)
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Multiple linked resource chunks available for the provided id");

                void *data = RRES_MALLOC(info.packedSize);
                if (data == NULL)
                {
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to allocate memory for chunk data\n");
                    break;
                }

                if (SDL_ReadIO(rresFile, data, info.packedSize) != info.packedSize)
                {
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read chunk data: %s\n", SDL_GetError());
                    RRES_FREE(data);
                    break;
                }

                chunk.data = rresLoadResourceChunkData(info, data);
                chunk.info = info;
                RRES_FREE(data);
                break;
            }
            else
            {
                if (SDL_SeekIO(rresFile, info.packedSize, SDL_IO_SEEK_CUR) == -1)
                {
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to seek to next chunk: %s\n", SDL_GetError());
                    break;
                }
            }
        }

        if (!found) RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Requested resource not found: 0x%08x\n", rresId);
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: The provided file is not a valid rres file, file signature or version not valid\n");
    }

    if (!SDL_CloseIO(rresFile))
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to close file: %s\n", SDL_GetError());

    return chunk;
}

void rresUnloadResourceChunk(rresResourceChunk chunk)
{
    RRES_FREE(chunk.data.props);
    RRES_FREE(chunk.data.raw);
}

rresResourceMulti rresLoadResourceMulti(const char *fileName, int rresId)
{
    rresResourceMulti rres = { 0 };
    SDL_IOStream *rresFile = SDL_IOFromFile(fileName, "rb");

    if (rresFile == NULL)
    {
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: [%s] rres file could not be opened: %s\n", fileName, SDL_GetError());
        return rres;
    }

    rresFileHeader header = { 0 };
    if (SDL_ReadIO(rresFile, &header, sizeof(rresFileHeader)) != sizeof(rresFileHeader))
    {
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read file header: %s\n", SDL_GetError());
        SDL_CloseIO(rresFile);
        return rres;
    }

    if (header.id[0] == 'r' && header.id[1] == 'r' && header.id[2] == 'e' && header.id[3] == 's' && header.version == 100)
    {
        bool found = false;

        for (int i = 0; i < header.chunkCount; i++)
        {
            rresResourceChunkInfo info = { 0 };
            if (SDL_ReadIO(rresFile, &info, sizeof(rresResourceChunkInfo)) != sizeof(rresResourceChunkInfo))
            {
                RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read chunk info: %s\n", SDL_GetError());
                break;
            }

            if (info.id == rresId)
            {
                found = true;
                RC2D_log(RC2D_LOG_INFO, "RRES: INFO: Found requested resource id: 0x%08x\n", info.id);
                RC2D_log(RC2D_LOG_INFO, "RRES: %c%c%c%c: Id: 0x%08x | Base size: %i | Packed size: %i\n",
                         info.type[0], info.type[1], info.type[2], info.type[3], info.id, info.baseSize, info.packedSize);

                rres.count = 1;
                Sint64 currentFileOffset = SDL_TellIO(rresFile);
                if (currentFileOffset == -1)
                {
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to get current file offset: %s\n", SDL_GetError());
                    break;
                }

                rresResourceChunkInfo temp = info;
                while (temp.nextOffset != 0)
                {
                    if (SDL_SeekIO(rresFile, temp.nextOffset, SDL_IO_SEEK_SET) == -1)
                    {
                        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to seek to next chunk: %s\n", SDL_GetError());
                        break;
                    }
                    if (SDL_ReadIO(rresFile, &temp, sizeof(rresResourceChunkInfo)) != sizeof(rresResourceChunkInfo))
                    {
                        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read chunk info: %s\n", SDL_GetError());
                        break;
                    }
                    rres.count++;
                }

                rres.chunks = (rresResourceChunk *)RRES_CALLOC(rres.count, sizeof(rresResourceChunk));
                if (rres.chunks == NULL)
                {
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to allocate memory for chunks\n");
                    break;
                }

                if (SDL_SeekIO(rresFile, currentFileOffset, SDL_IO_SEEK_SET) == -1)
                {
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to seek back to first chunk: %s\n", SDL_GetError());
                    RRES_FREE(rres.chunks);
                    break;
                }

                void *data = RRES_MALLOC(info.packedSize);
                if (data == NULL)
                {
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to allocate memory for chunk data\n");
                    RRES_FREE(rres.chunks);
                    break;
                }

                if (SDL_ReadIO(rresFile, data, info.packedSize) != info.packedSize)
                {
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read chunk data: %s\n", SDL_GetError());
                    RRES_FREE(data);
                    RRES_FREE(rres.chunks);
                    break;
                }

                rres.chunks[0].data = rresLoadResourceChunkData(info, data);
                rres.chunks[0].info = info;
                RRES_FREE(data);

                int j = 1;
                while (info.nextOffset != 0)
                {
                    if (SDL_SeekIO(rresFile, info.nextOffset, SDL_IO_SEEK_SET) == -1)
                    {
                        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to seek to next chunk: %s\n", SDL_GetError());
                        break;
                    }
                    if (SDL_ReadIO(rresFile, &info, sizeof(rresResourceChunkInfo)) != sizeof(rresResourceChunkInfo))
                    {
                        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read chunk info: %s\n", SDL_GetError());
                        break;
                    }

                    RC2D_log(RC2D_LOG_INFO, "RRES: %c%c%c%c: Id: 0x%08x | Base size: %i | Packed size: %i\n",
                             info.type[0], info.type[1], info.type[2], info.type[3], info.id, info.baseSize, info.packedSize);

                    data = RRES_MALLOC(info.packedSize);
                    if (data == NULL)
                    {
                        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to allocate memory for chunk data\n");
                        break;
                    }

                    if (SDL_ReadIO(rresFile, data, info.packedSize) != info.packedSize)
                    {
                        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read chunk data: %s\n", SDL_GetError());
                        RRES_FREE(data);
                        break;
                    }

                    rres.chunks[j].data = rresLoadResourceChunkData(info, data);
                    rres.chunks[j].info = info;
                    RRES_FREE(data);
                    j++;
                }

                break;
            }
            else
            {
                if (SDL_SeekIO(rresFile, info.packedSize, SDL_IO_SEEK_CUR) == -1)
                {
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to seek to next chunk: %s\n", SDL_GetError());
                    break;
                }
            }
        }

        if (!found) RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Requested resource not found: 0x%08x\n", rresId);
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: The provided file is not a valid rres file, file signature or version not valid\n");
    }

    if (!SDL_CloseIO(rresFile))
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to close file: %s\n", SDL_GetError());

    return rres;
}

void rresUnloadResourceMulti(rresResourceMulti multi)
{
    for (unsigned int i = 0; i < multi.count; i++)
        rresUnloadResourceChunk(multi.chunks[i]);
    RRES_FREE(multi.chunks);
}

RRESAPI rresResourceChunkInfo rresLoadResourceChunkInfo(const char *fileName, int rresId)
{
    rresResourceChunkInfo info = { 0 };
    SDL_IOStream *rresFile = SDL_IOFromFile(fileName, "rb");

    if (rresFile == NULL)
    {
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: [%s] rres file could not be opened: %s\n", fileName, SDL_GetError());
        return info;
    }

    rresFileHeader header = { 0 };
    if (SDL_ReadIO(rresFile, &header, sizeof(rresFileHeader)) != sizeof(rresFileHeader))
    {
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read file header: %s\n", SDL_GetError());
        SDL_CloseIO(rresFile);
        return info;
    }

    if (header.id[0] == 'r' && header.id[1] == 'r' && header.id[2] == 'e' && header.id[3] == 's' && header.version == 100)
    {
        for (int i = 0; i < header.chunkCount; i++)
        {
            if (SDL_ReadIO(rresFile, &info, sizeof(rresResourceChunkInfo)) != sizeof(rresResourceChunkInfo))
            {
                RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read chunk info: %s\n", SDL_GetError());
                break;
            }

            if (info.id == rresId)
            {
                break;
            }
            else
            {
                if (SDL_SeekIO(rresFile, info.packedSize, SDL_IO_SEEK_CUR) == -1)
                {
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to seek to next chunk: %s\n", SDL_GetError());
                    break;
                }
            }
        }
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: The provided file is not a valid rres file, file signature or version not valid\n");
    }

    if (!SDL_CloseIO(rresFile))
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to close file: %s\n", SDL_GetError());

    return info;
}

RRESAPI rresResourceChunkInfo *rresLoadResourceChunkInfoAll(const char *fileName, unsigned int *chunkCount)
{
    rresResourceChunkInfo *infos = NULL;
    unsigned int count = 0;

    SDL_IOStream *rresFile = SDL_IOFromFile(fileName, "rb");

    if (rresFile == NULL)
    {
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: [%s] rres file could not be opened: %s\n", fileName, SDL_GetError());
        *chunkCount = 0;
        return NULL;
    }

    rresFileHeader header = { 0 };
    if (SDL_ReadIO(rresFile, &header, sizeof(rresFileHeader)) != sizeof(rresFileHeader))
    {
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read file header: %s\n", SDL_GetError());
        SDL_CloseIO(rresFile);
        *chunkCount = 0;
        return NULL;
    }

    if (header.id[0] == 'r' && header.id[1] == 'r' && header.id[2] == 'e' && header.id[3] == 's' && header.version == 100)
    {
        infos = (rresResourceChunkInfo *)RRES_CALLOC(header.chunkCount, sizeof(rresResourceChunkInfo));
        if (infos == NULL)
        {
            RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to allocate memory for chunk infos\n");
            SDL_CloseIO(rresFile);
            *chunkCount = 0;
            return NULL;
        }

        count = header.chunkCount;

        for (unsigned int i = 0; i < count; i++)
        {
            if (SDL_ReadIO(rresFile, &infos[i], sizeof(rresResourceChunkInfo)) != sizeof(rresResourceChunkInfo))
            {
                RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read chunk info: %s\n", SDL_GetError());
                RRES_FREE(infos);
                count = 0;
                break;
            }

            if (infos[i].nextOffset > 0)
            {
                if (SDL_SeekIO(rresFile, infos[i].nextOffset, SDL_IO_SEEK_SET) == -1)
                {
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to seek to next chunk: %s\n", SDL_GetError());
                    RRES_FREE(infos);
                    count = 0;
                    break;
                }
            }
            else
            {
                if (SDL_SeekIO(rresFile, infos[i].packedSize, SDL_IO_SEEK_CUR) == -1)
                {
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to seek to next chunk: %s\n", SDL_GetError());
                    RRES_FREE(infos);
                    count = 0;
                    break;
                }
            }
        }
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: The provided file is not a valid rres file, file signature or version not valid\n");
    }

    if (!SDL_CloseIO(rresFile))
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to close file: %s\n", SDL_GetError());

    *chunkCount = count;
    return infos;
}

rresCentralDir rresLoadCentralDirectory(const char *fileName)
{
    rresCentralDir dir = { 0 };
    SDL_IOStream *rresFile = SDL_IOFromFile(fileName, "rb");

    if (rresFile == NULL)
    {
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: [%s] rres file could not be opened: %s\n", fileName, SDL_GetError());
        return dir;
    }

    rresFileHeader header = { 0 };
    if (SDL_ReadIO(rresFile, &header, sizeof(rresFileHeader)) != sizeof(rresFileHeader))
    {
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read file header: %s\n", SDL_GetError());
        SDL_CloseIO(rresFile);
        return dir;
    }

    if (header.id[0] == 'r' && header.id[1] == 'r' && header.id[2] == 'e' && header.id[3] == 's' && header.version == 100)
    {
        if (header.cdOffset == 0)
        {
            RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: CDIR: No central directory found\n");
        }
        else
        {
            rresResourceChunkInfo info = { 0 };
            if (SDL_SeekIO(rresFile, header.cdOffset, SDL_IO_SEEK_SET) == -1)
            {
                RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to seek to central directory: %s\n", SDL_GetError());
                SDL_CloseIO(rresFile);
                return dir;
            }

            if (SDL_ReadIO(rresFile, &info, sizeof(rresResourceChunkInfo)) != sizeof(rresResourceChunkInfo))
            {
                RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read chunk info: %s\n", SDL_GetError());
                SDL_CloseIO(rresFile);
                return dir;
            }

            if (info.type[0] == 'C' && info.type[1] == 'D' && info.type[2] == 'I' && info.type[3] == 'R')
            {
                RC2D_log(RC2D_LOG_INFO, "RRES: CDIR: Central Directory found at offset: 0x%08x\n", header.cdOffset);

                void *data = RRES_MALLOC(info.packedSize);
                if (data == NULL)
                {
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to allocate memory for central directory data\n");
                    SDL_CloseIO(rresFile);
                    return dir;
                }

                if (SDL_ReadIO(rresFile, data, info.packedSize) != info.packedSize)
                {
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to read central directory data: %s\n", SDL_GetError());
                    RRES_FREE(data);
                    SDL_CloseIO(rresFile);
                    return dir;
                }

                rresResourceChunkData chunkData = rresLoadResourceChunkData(info, data);
                RRES_FREE(data);

                dir.count = chunkData.props[0];
                RC2D_log(RC2D_LOG_INFO, "RRES: CDIR: Central Directory file entries count: %i\n", dir.count);

                unsigned char *ptr = (unsigned char *)chunkData.raw;
                dir.entries = (rresDirEntry *)RRES_CALLOC(dir.count, sizeof(rresDirEntry));
                if (dir.entries == NULL)
                {
                    RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to allocate memory for directory entries\n");
                    RRES_FREE(chunkData.props);
                    RRES_FREE(chunkData.raw);
                    SDL_CloseIO(rresFile);
                    return dir;
                }

                for (unsigned int i = 0; i < dir.count; i++)
                {
                    dir.entries[i].id = ((int *)ptr)[0];
                    dir.entries[i].offset = ((int *)ptr)[1];
                    dir.entries[i].fileNameSize = ((int *)ptr)[3];
                    memcpy(dir.entries[i].fileName, ptr + 16, dir.entries[i].fileNameSize);
                    ptr += (16 + dir.entries[i].fileNameSize);
                }

                RRES_FREE(chunkData.props);
                RRES_FREE(chunkData.raw);
            }
        }
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: The provided file is not a valid rres file, file signature or version not valid\n");
    }

    if (!SDL_CloseIO(rresFile))
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: Failed to close file: %s\n", SDL_GetError());

    return dir;
}

void rresUnloadCentralDirectory(rresCentralDir dir)
{
    RRES_FREE(dir.entries);
}

unsigned int rresGetDataType(const unsigned char *fourCC)
{
    unsigned int type = 0;

    if (fourCC != NULL)
    {
        if (memcmp(fourCC, "NULL", 4) == 0) type = RRES_DATA_NULL;
        else if (memcmp(fourCC, "RAWD", 4) == 0) type = RRES_DATA_RAW;
        else if (memcmp(fourCC, "TEXT", 4) == 0) type = RRES_DATA_TEXT;
        else if (memcmp(fourCC, "IMGE", 4) == 0) type = RRES_DATA_IMAGE;
        else if (memcmp(fourCC, "WAVE", 4) == 0) type = RRES_DATA_WAVE;
        else if (memcmp(fourCC, "VRTX", 4) == 0) type = RRES_DATA_VERTEX;
        else if (memcmp(fourCC, "FNTG", 4) == 0) type = RRES_DATA_FONT_GLYPHS;
        else if (memcmp(fourCC, "LINK", 4) == 0) type = RRES_DATA_LINK;
        else if (memcmp(fourCC, "CDIR", 4) == 0) type = RRES_DATA_DIRECTORY;
    }

    return type;
}

int rresGetResourceId(rresCentralDir dir, const char *fileName)
{
    int id = 0;

    for (unsigned int i = 0; i < dir.count; i++)
    {
        unsigned int len = (unsigned int)strlen(fileName);
        if (strncmp((const char *)dir.entries[i].fileName, fileName, len) == 0)
        {
            id = dir.entries[i].id;
            break;
        }
    }

    return id;
}

unsigned int rresComputeCRC32(unsigned char *data, int len)
{
    static unsigned int crcTable[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
        0x0eDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
        0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
        0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
        0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
        0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
        0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
        0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
        0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
        0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
        0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
        0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
        0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
        0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
        0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
        0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
        0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
        0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
        0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
        0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
        0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
        0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
        0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
        0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
        0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
        0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
        0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
        0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
        0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
        0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
        0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
        0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
    };

    unsigned int crc = ~0u;
    for (int i = 0; i < len; i++) crc = (crc >> 8) ^ crcTable[data[i] ^ (crc & 0xff)];
    return ~crc;
}

void rresSetCipherPassword(const char *pass)
{
    password = pass;
}

const char *rresGetCipherPassword(void)
{
    if (password == NULL) password = "password12345";
    return password;
}

static rresResourceChunkData rresLoadResourceChunkData(rresResourceChunkInfo info, void *data)
{
    rresResourceChunkData chunkData = { 0 };

    unsigned int crc32 = rresComputeCRC32((unsigned char *)data, info.packedSize);

    if (rresGetDataType(info.type) != RRES_DATA_NULL && crc32 == info.crc32)
    {
        if (info.compType == RRES_COMP_NONE && info.cipherType == RRES_CIPHER_NONE)
        {
            chunkData.propCount = ((unsigned int *)data)[0];

            if (chunkData.propCount > 0)
            {
                chunkData.props = (unsigned int *)RRES_CALLOC(chunkData.propCount, sizeof(unsigned int));
                for (unsigned int i = 0; i < chunkData.propCount; i++)
                    chunkData.props[i] = ((unsigned int *)data)[i + 1];
            }

            int rawSize = info.baseSize - sizeof(int) - (chunkData.propCount * sizeof(int));
            chunkData.raw = RRES_MALLOC(rawSize);
            if (chunkData.raw != NULL)
                memcpy(chunkData.raw, ((unsigned char *)data) + sizeof(int) + (chunkData.propCount * sizeof(int)), rawSize);
        }
        else
        {
            chunkData.raw = RRES_MALLOC(info.packedSize);
            if (chunkData.raw != NULL)
                memcpy(chunkData.raw, (unsigned char *)data, info.packedSize);
        }
    }

    if (crc32 != info.crc32)
        RC2D_log(RC2D_LOG_WARN, "RRES: WARNING: [ID %i] CRC32 does not match, data can be corrupted\n", info.id);

    return chunkData;
}

#endif // RRES_IMPLEMENTATION