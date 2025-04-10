#ifndef RC2D_FILESYSTEM_H
#define RC2D_FILESYSTEM_H

#include "rc2d/rc2d_data.h" // Required for : RC2D_DataType

#include <SDL.h> // Required for : Sint64
#include <stdbool.h> // Required for : bool

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumération des résultats possibles des opérations du système de fichiers.
 * Fournit des codes de retour spécifiques pour différentes conditions d'erreur ou de succès.
 * @typedef {enum} RC2D_FileSystemResult
 * @property {number} RC2D_FS_SUCCESS - Opération réussie sans erreurs.
 * @property {number} RC2D_FS_ERROR_FILE_NOT_FOUND - Le fichier spécifié n'a pas été trouvé.
 * @property {number} RC2D_FS_ERROR_CANNOT_OPEN - Le fichier spécifié ne peut pas être ouvert.
 * @property {number} RC2D_FS_ERROR_CLOSE_FAIL - Échec de la fermeture du fichier spécifié.
 * @property {number} RC2D_FS_ERROR_WRITE_FAIL - Échec de l'écriture dans le fichier spécifié.
 * @property {number} RC2D_FS_ERROR_READ_FAIL - Échec de la lecture du fichier spécifié.
 * @property {number} RC2D_FS_ERROR_PARTIAL_READ - Seule une partie du fichier a été lue, lecture incomplète.
 * @property {number} RC2D_FS_ERROR_UNKNOWN - Une erreur inconnue s'est produite lors de l'opération.
 */
typedef enum RC2D_FileSystemResult {
    RC2D_FS_SUCCESS,               
    RC2D_FS_ERROR_FILE_NOT_FOUND,  
    RC2D_FS_ERROR_CANNOT_OPEN,
    RC2D_FS_ERROR_CLOSE_FAIL,     
    RC2D_FS_ERROR_WRITE_FAIL,     
    RC2D_FS_ERROR_READ_FAIL,       
    RC2D_FS_ERROR_PARTIAL_READ,    
    RC2D_FS_ERROR_UNKNOWN          
} RC2D_FileSystemResult;

void rc2d_filesystem_quit(void); // Use internally by the engine

char* rc2d_filesystem_getWritableAppDataPath(const char* nameOrganisation, const char* nameApplication);
char* rc2d_filesystem_getPathApp(void); 

RC2D_FileSystemResult rc2d_filesystem_append(const char* pathFile, const unsigned char* data, const size_t dataSize);
RC2D_FileSystemResult rc2d_filesystem_write(const char* pathFile, const unsigned char* data, const size_t sizeData);
RC2D_FileSystemResult rc2d_filesystem_read(const char* pathFile, unsigned char** data, size_t* dataSize, const RC2D_DataType dataType);
void rc2d_filesystem_free(unsigned char* data);

RC2D_FileSystemResult rc2d_filesystem_getFileSize(const char* pathFile, Sint64* fileSize);
bool rc2d_filesystem_isFileExist(const char* pathFile);

char* rc2d_filesystem_getPathAssetsInResourceRRES(void);

#ifdef __cplusplus
}
#endif

#endif // RC2D_FILESYSTEM_H