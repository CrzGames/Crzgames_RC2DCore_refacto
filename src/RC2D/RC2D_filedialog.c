#include <RC2D/RC2D_filedialog.h>
#include <RC2D/RC2D_logger.h>
#include <RC2D/RC2D_memory.h>

#include <SDL3/SDL_dialog.h>
#include <SDL3/SDL_properties.h>

// Structure interne pour passer les données du callback
typedef struct RC2D_FileDialogCallbackData {
    RC2D_FileDialogCallback callback;
    void *userdata;
} RC2D_FileDialogCallbackData;

// Callback interne pour mapper SDL_DialogFileCallback à RC2D_FileDialogCallback
static void rc2d_filedialog_internal_callback(void *userdata, const char *const *filelist, int filter) 
{
    RC2D_FileDialogCallbackData *callback_data = (RC2D_FileDialogCallbackData *)userdata;
    if (!callback_data || !callback_data->callback) 
    {
        RC2D_log(RC2D_LOG_WARN, "données ou callback NULL");
        RC2D_free(callback_data);
        return;
    }

    // Appelle le callback utilisateur
    callback_data->callback(callback_data->userdata, filelist, filter);

    // Nettoie les données du callback
    RC2D_free(callback_data);
}

// Fonction générique pour ouvrir un dialogue de fichier
static void rc2d_filedialog_show(RC2D_FileDialogType type, RC2D_FileDialogCallback callback, void *userdata, const RC2D_FileDialogOptions *options) 
{
    if (!callback) 
    {
        RC2D_log(RC2D_LOG_ERROR, "callback NULL");
        return;
    }

    // Crée les données du callback
    RC2D_FileDialogCallbackData *callback_data = RC2D_malloc(sizeof(RC2D_FileDialogCallbackData));
    if (!callback_data)
    {
        RC2D_log(RC2D_LOG_ERROR, "echec d'allocation des données du callback");
        return;
    }
    callback_data->callback = callback;
    callback_data->userdata = userdata;

    // Crée les propriétés pour le dialogue
    SDL_PropertiesID props = SDL_CreateProperties();
    if (!props) {
        RC2D_log(RC2D_LOG_ERROR, "echec de SDL_CreateProperties : %s", SDL_GetError());
        RC2D_free(callback_data);
        return;
    }

    // Configure les options
    if (options) 
    {
        // Convertit RC2D_FileDialogFilter en SDL_DialogFileFilter
        SDL_DialogFileFilter *sdl_filters = NULL;
        if (options->filters && options->num_filters > 0 && type != RC2D_FILEDIALOG_OPEN_FOLDER) 
        {
            sdl_filters = RC2D_malloc(options->num_filters * sizeof(SDL_DialogFileFilter));
            if (!sdl_filters) 
            {
                RC2D_log(RC2D_LOG_ERROR, "rc2d_filedialog_show : échec d'allocation des filtres");
                SDL_DestroyProperties(props);
                RC2D_free(callback_data);
                return;
            }

            for (int i = 0; i < options->num_filters; i++) 
            {
                sdl_filters[i].name = options->filters[i].name;
                sdl_filters[i].pattern = options->filters[i].pattern;
            }

            SDL_SetPointerProperty(props, SDL_PROP_FILE_DIALOG_FILTERS_POINTER, sdl_filters);
            SDL_SetNumberProperty(props, SDL_PROP_FILE_DIALOG_NFILTERS_NUMBER, options->num_filters);
        }

        // Configure les autres propriétés
        if (options->window) 
        {
            SDL_SetPointerProperty(props, SDL_PROP_FILE_DIALOG_WINDOW_POINTER, options->window);
        }
        if (options->default_location) 
        {
            SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_LOCATION_STRING, options->default_location);
        }
        if (options->allow_many) 
        {
            SDL_SetBooleanProperty(props, SDL_PROP_FILE_DIALOG_MANY_BOOLEAN, options->allow_many);
        }
        if (options->title) 
        {
            SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_TITLE_STRING, options->title);
        }
        if (options->accept_label) 
        {
            SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_ACCEPT_STRING, options->accept_label);
        }
        if (options->cancel_label) 
        {
            SDL_SetStringProperty(props, SDL_PROP_FILE_DIALOG_CANCEL_STRING, options->cancel_label);
        }
    }

    // Mappe le type RC2D à SDL
    SDL_FileDialogType sdl_type;
    switch (type) 
    {
        case RC2D_FILEDIALOG_OPEN_FILE: sdl_type = SDL_FILEDIALOG_OPENFILE; break;
        case RC2D_FILEDIALOG_SAVE_FILE: sdl_type = SDL_FILEDIALOG_SAVEFILE; break;
        case RC2D_FILEDIALOG_OPEN_FOLDER: sdl_type = SDL_FILEDIALOG_OPENFOLDER; break;
        default:
            RC2D_log(RC2D_LOG_ERROR, "type de dialogue invalide");
            SDL_DestroyProperties(props);
            RC2D_free(callback_data);
            return;
    }

    // Ouvre le dialogue
    SDL_ShowFileDialogWithProperties(sdl_type, rc2d_filedialog_internal_callback, callback_data, props);

    // Les propriétés et les filtres sont nettoyés par SDL après le callback
    SDL_DestroyProperties(props);
}

void rc2d_filedialog_openFile(RC2D_FileDialogCallback callback, void *userdata, const RC2D_FileDialogOptions *options) 
{
    rc2d_filedialog_show(RC2D_FILEDIALOG_OPEN_FILE, callback, userdata, options);
}

void rc2d_filedialog_saveFile(RC2D_FileDialogCallback callback, void *userdata, const RC2D_FileDialogOptions *options) 
{
    rc2d_filedialog_show(RC2D_FILEDIALOG_SAVE_FILE, callback, userdata, options);
}

void rc2d_filedialog_openFolder(RC2D_FileDialogCallback callback, void *userdata, const RC2D_FileDialogOptions *options) 
{
    rc2d_filedialog_show(RC2D_FILEDIALOG_OPEN_FOLDER, callback, userdata, options);
}