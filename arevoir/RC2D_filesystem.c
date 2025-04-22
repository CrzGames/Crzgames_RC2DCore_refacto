#include <RC2D/RC2D_filesystem.h>
#include <RC2D/RC2D_logger.h>

#include <SDL3/SDL_stdinc.h>

static char* prefPath = NULL;
static char* basePath = NULL;

/**
 * Obtenez le chemin absolu vers un répertoire spécifique à l'utilisateur et à l'application
 * où l'écriture de fichiers est autorisée. Ce chemin est idéal pour stocker les données 
 * persistantes telles que les configurations de l'application, les sauvegardes de jeu,
 * et tout autre fichier nécessaire au fonctionnement de l'application.
 * 
 * Ce chemin est le seul endroit recommandé pour écrire des données sur le disque afin
 * d'assurer la compatibilité entre différentes plateformes et de respecter les directives
 * d'accès aux fichiers des systèmes d'exploitation.
 * 
 * Android : /data/data/com.crzgames.rc2d/files/
 * Windows : C:\Users\CrzGames\AppData\Roaming\CrzGames\SeaTyrants\
 * macOS : /Users/CrzGames/Library/Application Support/CrzGames/SeaTyrants/
 * Linux : /home/debian/.local/share/CrzGames/SeaTyrants/
 * iOS : /var/mobile/Containers/Data/Application/5B7B1F4D-5F1D-4F1C-9F1D-5F1D4F1C9F1D/Library/Application Support/CrzGames/SeaTyrants/
 * HTML5 : /libsdl/CrzGames/SeaTyrants/
 * 
 * @param nameOrganisation Le nom de l'organisation sous laquelle classer les données de l'application.
 * @param nameApplication Le nom de l'application pour laquelle obtenir le chemin.
 * @return Un pointeur vers une chaîne contenant le chemin absolu recommandé pour l'écriture de données,
 *         ou NULL en cas d'erreur ou si la plateforme n'implémente pas cette fonctionnalité.
 */
char* rc2d_filesystem_getWritableAppDataPath(const char* nameOrganisation, const char* nameApplication)
{
    if (nameOrganisation == NULL || nameApplication == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "rc2d_filesystem_getWritableAppDataPath error : nameOrganisation or nameApplication is NULL \n");
        return NULL;
    }

    prefPath = SDL_GetPrefPath(nameOrganisation, nameApplication);

    // NULL en cas de probleme (échec de la création du répertoire, etc.)
    if (prefPath == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "rc2d_filesystem_getPrefPath failed pointer SDL_GetPrefPath : %s", SDL_GetError());        
        return NULL;
    }

    return prefPath;
}

/*
Obtenez le path absolu a partir duquel l'application a ete executee.

Android : pointer NULL (toute les ressources doit être dans le dossier app/src/main/assets du dossier android-project, le path commence a partir de la directement donc le path sera relative directement a partir de cette emplacement)
Windows (.exe) : C:\Users\Corentin\Documents\SeaTyrants\bin\
macOS (.app bundle) : /Applications/SeaTyrants/SeaTyrants.app/Contents/Resources/
iOS (.app bundle) : /private/var/containers/Bundle/Application/5E3A643E-4353-44CD-85A1-690E286A944B/SeaTyrants.app/
Linux : /tmp/.mount_mygame0q6J0j/ (une AppImage est un format d empaquetage qui, lors de son execution, monte le contenu de l image dans un systeme de fichiers temporaire. Cela explique pourquoi vous obtenez un chemin dans /tmp/.)
HTML5 : / 
*/
char* rc2d_filesystem_getPathApp(void)
{
    basePath = SDL_GetBasePath();
    
    // NULL sera renvoye en cas d'erreur ou lorsque la plateforme n'implemente pas cette fonctionnalite
    if (basePath == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "rc2d_filesystem_getBasePath failed pointer SDL_GetBasePath : %s", SDL_GetError());
        return NULL;
    }

    return basePath;
}

/**
 * Libère les ressources allouées par le module de système de fichiers.
 * @returns {void}
 */
void rc2d_filesystem_quit(void)
{
    if (prefPath != NULL)
    {
        SDL_free(prefPath);
        prefPath = NULL;
    }

    if (basePath != NULL)
    {
        SDL_free(basePath);
        basePath = NULL;
    }
}

/*
    Les assets original doit être placer dans le dossier 'assets' du projet pour les plateformes Windows, Linux, macOS et iOS.
    Pour Android, les assets original doit être placer dans le dossier app/src/main/assets du dossier android-project.

    Build assets original in RRES file format (script), output example:
    - RRES: CDIR: Entry id: 0xb90bce3a | Offset: 0x00334c26 | Filename: ./assets/images/test.png (len: 28)

    On calculera le dossier 'assets' au préalable pour que ce sois relatif au dossier assets pour chaque plateforme.

    Cela ne concerne que le module rc2d_rres.
*/
char* rc2d_filesystem_getPathAssetsInResourceRRES(void)
{
    char* path = "./assets/";
    return path;
}

/**
 * Écrit des données dans un fichier spécifié.
 *
 * Cette fonction tente d'ouvrir un fichier en mode écriture binaire. Si le fichier n'existe pas,
 * il sera créé. Si le fichier existe déjà, son contenu sera écrasé par les nouvelles données.
 * La fonction retourne un code indiquant le résultat de l'opération, permettant une gestion
 * d'erreur explicite.
 *
 * Utilisation de SDL_RWops assure la compatibilité cross-plateforme de cette fonction.
 *
 * @param pathFile Le chemin absolu ou relatif du fichier dans lequel écrire les données.
 *                 Ce chemin doit inclure le nom du fichier lui-même.
 * @param data Pointeur vers les données à écrire dans le fichier.
 * @param dataSize La taille des données à écrire, en octets.
 * @return RC2D_FileSystemResult Un code indiquant le succès ou la nature de l'échec de l'opération.
 */
RC2D_FileSystemResult rc2d_filesystem_write(const char* pathFile, const unsigned char* data, const size_t dataSize) 
{
    if (pathFile == NULL || data == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'écrire dans le fichier : le chemin ou les données sont NULL dans rc2d_filesystem_write\n");
        return RC2D_FS_ERROR_UNKNOWN;
    }

    SDL_RWops *rw = SDL_RWFromFile(pathFile, "wb");
    if (rw == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ouvrir le fichier pour écriture : %s. Erreur SDL : %s dans rc2d_filesystem_write", pathFile, SDL_GetError());
        return RC2D_FS_ERROR_CANNOT_OPEN;
    }

    // Écrire les données dans le fichier en utilisant dataSize pour déterminer la quantité de données à écrire
    size_t written = SDL_RWwrite(rw, data, 1, dataSize);
    // envoie le nombre d'objets écrits, qui sera inférieur à num en cas d'erreur ; appelez SDL_GetError () pour plus d'informations.
    if (written < dataSize)
    {
        RC2D_log(RC2D_LOG_ERROR, "Échec de l'écriture dans le fichier : %s. Erreur SDL : %s dans rc2d_filesystem_write", pathFile, SDL_GetError());

        int result = SDL_RWclose(rw);
        if (result < 0)
        {
            RC2D_log(RC2D_LOG_ERROR, "Échec de la fermeture du fichier : %s. Erreur SDL : %s dans rc2d_filesystem_write", pathFile, SDL_GetError());
        }

        return RC2D_FS_ERROR_WRITE_FAIL;
    }

    int result = SDL_RWclose(rw);
    if (result < 0)
    {
        RC2D_log(RC2D_LOG_ERROR, "Échec de la fermeture du fichier : %s. Erreur SDL : %s dans rc2d_filesystem_write", pathFile, SDL_GetError());
        return RC2D_FS_ERROR_CLOSE_FAIL;
    }

    return RC2D_FS_SUCCESS;
}

/**
 * Lit le contenu intégral d'un fichier et le stocke dans une zone mémoire allouée dynamiquement.
 *
 * Ouvre un fichier en mode lecture binaire et lit son contenu intégral. Le contenu lu est retourné
 * à travers un pointeur vers une zone mémoire allouée dynamiquement, que l'appelant doit libérer
 * à l'aide de rc2d_filesystem_free une fois le contenu n'étant plus nécessaire.
 * La taille du contenu lu est retournée via un paramètre de sortie.
 * En cas d'échec, la fonction logue l'erreur et retourne un code indiquant la nature de l'échec.
 *
 * @param pathFile Le chemin vers le fichier à lire.
 * @param data Un pointeur vers un pointeur de unsigned char qui sera modifié pour pointer
 *             vers le contenu lu du fichier. La mémoire est allouée par la fonction.
 * @param dataSize Un pointeur vers une variable size_t où la taille du contenu lu sera stockée.
 * @param dataType Le type de données attendu, indiquant si un caractère nul doit être ajouté
 *                 à la fin des données pour les chaînes de caractères.
 * @return RC2D_FileSystemResult Un code indiquant le succès ou la nature de l'échec de l'opération.
 */
RC2D_FileSystemResult rc2d_filesystem_read(const char* pathFile, unsigned char** data, size_t* dataSize, const RC2D_DataType dataType) 
{
    if (pathFile == NULL || data == NULL || dataSize == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de lire le fichier : le chemin ou les données sont NULL.\n");
        return RC2D_FS_ERROR_UNKNOWN;
    }

    SDL_RWops* rw = SDL_RWFromFile(pathFile, "rb");
    if (rw == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ouvrir le fichier pour la lecture : %s, Erreur SDL : %s dans rc2d_filesystem_read\n", pathFile, SDL_GetError());
        return RC2D_FS_ERROR_CANNOT_OPEN;
    }

    Sint64 fileSize = SDL_RWsize(rw);
    if (fileSize < 0) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de récupérer la taille du fichier : %s, Erreur SDL : %s dans rc2d_filesystem_read\n", pathFile, SDL_GetError());
        
        int result = SDL_RWclose(rw);
        if (result < 0)
        {
            RC2D_log(RC2D_LOG_ERROR, "Échec de la fermeture du fichier : %s. Erreur SDL : %s dans rc2d_filesystem_read", pathFile, SDL_GetError());
        }
        
        return RC2D_FS_ERROR_READ_FAIL;
    }

    // Allouer la mémoire nécessaire pour contenir le contenu du fichier.
    // Si dataType est texte, allouer un octet supplémentaire pour le caractère nul de fin.
    *data = (unsigned char*)SDL_malloc(fileSize + (dataType == RC2D_DATA_TYPE_TEXT ? 1 : 0));
    if (*data == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'allouer de la mémoire pour le contenu du fichier : %s\n", pathFile);
        
        int result = SDL_RWclose(rw);
        if (result < 0)
        {
            RC2D_log(RC2D_LOG_ERROR, "Échec de la fermeture du fichier : %s. Erreur SDL : %s dans rc2d_filesystem_read", pathFile, SDL_GetError());
        }

        return RC2D_FS_ERROR_WRITE_FAIL;
    }

    size_t nbItemsRead = SDL_RWread(rw, *data, 1, fileSize);
    if (nbItemsRead != fileSize) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de lire le contenu complet du fichier : %s, Erreur SDL : %s dans rc2d_filesystem_read\n", pathFile, SDL_GetError());
        SDL_free(*data);
        
        int result = SDL_RWclose(rw);
        if (result < 0)
        {
            RC2D_log(RC2D_LOG_ERROR, "Échec de la fermeture du fichier : %s. Erreur SDL : %s dans rc2d_filesystem_read", pathFile, SDL_GetError());
        }

        return RC2D_FS_ERROR_PARTIAL_READ;
    }

    // Pour les données textuelles, ajouter un caractère nul de fin pour assurer la compatibilité avec les chaînes C.
    if (dataType == RC2D_DATA_TYPE_TEXT) 
    {
        (*data)[fileSize] = '\0';
    }

    *dataSize = nbItemsRead; // Retourner la taille réelle des données lues.

    int result = SDL_RWclose(rw);
    if (result < 0)
    {
        RC2D_log(RC2D_LOG_ERROR, "Échec de la fermeture du fichier : %s. Erreur SDL : %s dans rc2d_filesystem_read", pathFile, SDL_GetError());
        return RC2D_FS_ERROR_CLOSE_FAIL;
    }

    return RC2D_FS_SUCCESS;
}

/**
 * Libère la mémoire allouée pour le contenu d'un fichier lu par rc2d_filesystem_read.
 *
 * Cette fonction doit être appelée par l'appelant pour libérer la mémoire allouée
 * par rc2d_filesystem_read une fois que le contenu n'est plus nécessaire.
 *
 * @param content Pointeur vers la mémoire allouée contenant le contenu du fichier.
 */
void rc2d_filesystem_free(unsigned char* data) 
{
    if (data != NULL) 
    {
        SDL_free(data);
        data = NULL;
    }
}

/**
 * Ajoute des données à la fin d'un fichier spécifié.
 *
 * Cette fonction ouvre un fichier en mode append binaire. Si le fichier n'existe pas, il sera créé.
 * Les données fournies sont ajoutées à la fin du fichier existant. La fonction retourne un code
 * indiquant le résultat de l'opération, permettant une gestion d'erreur explicite.
 *
 * Utilisation de SDL_RWops assure la compatibilité cross-plateforme de cette fonction.
 *
 * @param pathFile Le chemin et le nom du fichier où les données seront ajoutées.
 * @param data Pointeur vers les données à ajouter au fichier.
 * @param dataSize La taille des données à ajouter, en octets.
 * @return RC2D_FileSystemResult Le résultat de l'opération, indiquant le succès ou le type d'erreur rencontré.
 */
RC2D_FileSystemResult rc2d_filesystem_append(const char* pathFile, const unsigned char* data, const size_t dataSize) 
{
    if (pathFile == NULL || data == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ajouter des données au fichier : le chemin ou les données sont NULL.\n");
        return RC2D_FS_ERROR_UNKNOWN;
    }

    SDL_RWops* rw = SDL_RWFromFile(pathFile, "ab"); // Ouvre le fichier en mode append binaire
    if (rw == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Unable to open file for appending: %s", SDL_GetError());
        return RC2D_FS_ERROR_CANNOT_OPEN;
    }

    size_t written = SDL_RWwrite(rw, data, 1, dataSize); // Utilise dataSize pour déterminer la quantité de données à ajouter
    if (written < dataSize) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ajouter toutes les données au fichier : %s. Erreur SDL : %s\n", pathFile, SDL_GetError());
        
        int result = SDL_RWclose(rw);
        if (result < 0)
        {
            RC2D_log(RC2D_LOG_ERROR, "Échec de la fermeture du fichier : %s. Erreur SDL : %s dans rc2d_filesystem_append", pathFile, SDL_GetError());
        }

        return RC2D_FS_ERROR_WRITE_FAIL;
    }

    int result = SDL_RWclose(rw);
    if (result < 0)
    {
        RC2D_log(RC2D_LOG_ERROR, "Échec de la fermeture du fichier : %s. Erreur SDL : %s dans rc2d_filesystem_append", pathFile, SDL_GetError());
        return RC2D_FS_ERROR_CLOSE_FAIL;
    }

    return RC2D_FS_SUCCESS;
}

/**
 * Vérifie si un fichier existe.
 *
 * Cette fonction tente d'ouvrir le fichier spécifié en mode lecture.
 * Si l'ouverture réussit, cela signifie que le fichier existe.
 * La fonction ferme ensuite le fichier (si ouvert) et retourne un booléen
 * indiquant le résultat de cette vérification.
 *
 * Note : Cette fonction est optimisée pour vérifier l'existence de fichiers.
 * Elle peut ne pas être appropriée pour vérifier l'existence de répertoires
 * sur toutes les plateformes, car SDL_RWFromFile est principalement conçu
 * pour travailler avec des fichiers.
 *
 * @param pathFile Le chemin vers le fichier à vérifier.
 * @return bool Retourne true si le fichier existe, false sinon.
 */
bool rc2d_filesystem_isFileExist(const char* pathFile) 
{
    if (pathFile == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de vérifier l'existence du fichier : le chemin est NULL.\n");
        return false;
    }

    // Tente d'ouvrir le fichier en mode lecture ("r").
    SDL_RWops *file = SDL_RWFromFile(pathFile, "r");
    if (file != NULL) 
    {
        RC2D_log(RC2D_LOG_INFO, "Le fichier n'existe pas : %s, Erreur SDL : %s dans rc2d_filesystem_isFileExist\n", pathFile, SDL_GetError());

        int result = SDL_RWclose(file);
        if (result < 0)
        {
            RC2D_log(RC2D_LOG_ERROR, "Échec de la fermeture du fichier : %s. Erreur SDL : %s dans rc2d_filesystem_isFileExist", pathFile, SDL_GetError());
        }

        return true; // Confirme que le fichier existe.
    }

    // Le fichier spécifié n'existe pas.
    return false;
}

/**
 * Obtient la taille d'un fichier.
 *
 * @param filename Le chemin et le nom du fichier.
 * @param fileSize Pointeur vers une variable de type Sint64 où la taille du fichier sera stockée en cas de succès.
 * @return RC2D_FileSystemResult Le résultat de l'opération, indiquant le succès ou le type d'erreur rencontré.
 */
RC2D_FileSystemResult rc2d_filesystem_getFileSize(const char* pathFile, Sint64* fileSize) 
{
    if (pathFile == NULL || fileSize == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible de récupérer la taille du fichier : le chemin ou la taille sont NULL.\n");
        return RC2D_FS_ERROR_UNKNOWN;
    }

    SDL_RWops *rw = SDL_RWFromFile(pathFile, "rb");
    if (rw == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Impossible d'ouvrir le fichier : %s, Erreur SDL : %s dans rc2d_filesystem_getFileSize\n", pathFile, SDL_GetError());
        return RC2D_FS_ERROR_CANNOT_OPEN;
    }

    Sint64 size = SDL_RWsize(rw);
    if (size < 0) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Erreur lors de la récupération de la taille du fichier : %s, Erreur SDL : %s dans rc2d_filesystem_getFileSize\n", pathFile, SDL_GetError());
        return RC2D_FS_ERROR_READ_FAIL;
    }

    int result = SDL_RWclose(rw);
    if (result < 0)
    {
        RC2D_log(RC2D_LOG_ERROR, "Échec de la fermeture du fichier : %s. Erreur SDL : %s dans rc2d_filesystem_getFileSize", pathFile, SDL_GetError());
    }

    *fileSize = size;

    return RC2D_FS_SUCCESS;
}