#include <RC2D/RC2D_internal.h>
#include <RC2D/RC2D_logger.h>
#include <RC2D/RC2D_assert.h>
#include <RC2D/RC2D_math.h>
#include <RC2D/RC2D_filesystem.h>
#include <RC2D/RC2D_platform_defines.h>
#include <RC2D/RC2D_memory.h>
#include <RC2D/RC2D_config.h>

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_gpu.h>

#include <SDL3_ttf/SDL_ttf.h>

//#include <SDL3_mixer/SDL_mixer.h>

#if RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
#include <SDL3_shadercross/SDL_shadercross.h>
#endif

RC2D_EngineState rc2d_engine_state = {0};

RC2D_EngineConfig* rc2d_engine_getDefaultConfig(void)
{
    static RC2D_LetterboxTextures default_letterbox_textures = {0};
    
    static RC2D_AppInfo default_app_info = {
        .name = "RC2D Game",
        .version = "1.0.0",
        .identifier = "com.example.rc2dgame"
    };

    static RC2D_GPUAdvancedOptions default_gpu_options = {
        .debugMode = true,
        .verbose = true,
        .preferLowPower = false,
        .driver = RC2D_GPU_DRIVER_DEFAULT
    };

    static RC2D_EngineCallbacks default_callbacks = {0};

    static RC2D_EngineConfig default_config = {
        .callbacks = &default_callbacks,
        .windowWidth = 800,
        .windowHeight = 600,
        .logicalWidth = 1920,
        .logicalHeight = 1080,
        .logicalPresentationMode = RC2D_LOGICAL_PRESENTATION_LETTERBOX,
        .letterboxTextures = &default_letterbox_textures,
        .appInfo = &default_app_info,
        .gpuFramesInFlight = RC2D_GPU_FRAMES_BALANCED,
        .gpuOptions = &default_gpu_options
    };

    return &default_config;
}

/**
 * \brief Affiche la liste des pilotes GPU supportés par SDL3.
 *
 * Cette fonction vérifie si au moins un backend GPU est supporté par SDL3.
 * Elle affiche également la liste des pilotes GPU disponibles.
 * 
 * \return {bool} - true si au moins un backend GPU est supporté, false sinon.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static bool rc2d_engine_supported_gpu_backends(void)
{
    int count = SDL_GetNumGPUDrivers();
    if (count <= 0) {
        RC2D_log(RC2D_LOG_CRITICAL, "Aucun backend GPU compatible pour SDL3 détecté.");
        return false;
    }

    RC2D_log(RC2D_LOG_INFO, "Pilotes GPU disponibles pour SDL3 (%d détecté%s) :", count, count > 1 ? "s" : "");
    for (int i = 0; i < count; ++i) 
    {
        const char* name = SDL_GetGPUDriver(i);
        if (name != NULL)
        {
            RC2D_log(RC2D_LOG_INFO, "  - %d : %s", i, name);
        }
    }

    return true;
}

/**
 * \brief Convertit le mode de présentation SDL_GPU en chaîne de caractères.
 *
 * Cette fonction convertit un mode de présentation SDL_GPU en une chaîne de caractères lisible.
 * 
 * \param {SDL_GPUPresentMode} mode - Le mode de présentation à convertir.
 * \return {const char*} - La chaîne de caractères représentant le mode de présentation.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
const char* rc2d_present_mode_to_string(SDL_GPUPresentMode mode) {
    switch (mode) {
        case SDL_GPU_PRESENTMODE_MAILBOX:   return "RC2D_GPU_PRESENTMODE_MAILBOX";
        case SDL_GPU_PRESENTMODE_VSYNC:     return "RC2D_GPU_PRESENTMODE_VSYNC";
        case SDL_GPU_PRESENTMODE_IMMEDIATE: return "RC2D_GPU_PRESENTMODE_IMMEDIATE";
        default: return "RC2D_GPU_PRESENTMODE_UNKNOWN";
    }
}

/**
 * \brief Convertit la composition de swapchain SDL_GPU en chaîne de caractères.
 *
 * Cette fonction convertit une composition de swapchain SDL_GPU en une chaîne de caractères lisible.
 * 
 * \param {SDL_GPUSwapchainComposition} comp - La composition de swapchain à convertir.
 * \return {const char*} - La chaîne de caractères représentant la composition de swapchain.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
const char* rc2d_composition_to_string(SDL_GPUSwapchainComposition comp) {
    switch (comp) {
        case SDL_GPU_SWAPCHAINCOMPOSITION_HDR10_ST2084:      return "RC2D_GPU_SWAPCHAINCOMPOSITION_HDR10_ST2084";
        case SDL_GPU_SWAPCHAINCOMPOSITION_HDR_EXTENDED_LINEAR:return "RC2D_GPU_SWAPCHAINCOMPOSITION_HDR_EXTENDED_LINEAR";
        case SDL_GPU_SWAPCHAINCOMPOSITION_SDR_LINEAR:        return "RC2D_GPU_SWAPCHAINCOMPOSITION_SDR_LINEAR";
        case SDL_GPU_SWAPCHAINCOMPOSITION_SDR:               return "RC2D_GPU_SWAPCHAINCOMPOSITION_SDR";
        default: return "RC2D_GPU_SWAPCHAINCOMPOSITION_UNKNOWN";
    }
}

/**
 * \brief Configure le swapchain GPU avec la meilleure combinaison de mode de présentation et de composition.
 *
 * Cette fonction tente de trouver et d'appliquer la meilleure combinaison supportée de mode de présentation
 * et de composition de swapchain pour le dispositif GPU et la fenêtre donnés.
 *
 * \return true si une configuration valide a été appliquée, false sinon.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static bool rc2d_engine_configure_swapchain(void)
{
    // SDL3 : Configurer le mode de présentation du GPU
    /**
     * SDL_GPU_PRESENTMODE_MAILBOX est utiliser par défaut,
     * car il est généralement le meilleur choix pour la plupart des applications.
     * Il offre un bon équilibre entre la latence et la fluidité de l'affichage, 
     * mais il n'est pas toujours disponible sur tous les systèmes.
     * 
     * SDL_GPU_PRESENTMODE_VSYNC est un bon choix si vous voulez éviter le tearing,
     * mais il peut introduire une latence supplémentaire, mais il est toujours disponible et sûr.
     * 
     * SDL_GPU_PRESENTMODE_IMMEDIATE est le moins recommandé, car il peut entraîner du tearing,
     * mais il peut être utilisé si vous avez besoin de la latence la plus basse possible.
     */
    SDL_GPUPresentMode present_modes[] = {
        SDL_GPU_PRESENTMODE_MAILBOX,
        SDL_GPU_PRESENTMODE_VSYNC,
        SDL_GPU_PRESENTMODE_IMMEDIATE
    };

    // Configurer le swapchain pour le GPU
    /**
     * SDL_GPU_SWAPCHAINCOMPOSITION_HDR10_ST2084 est le meilleur choix pour les écrans HDR,
     * mais il n'est pas toujours disponible sur tous les systèmes.
     * 
     * SDL_GPU_SWAPCHAINCOMPOSITION_HDR_EXTENDED_LINEAR est un bon choix pour les écrans HDR,
     * mais il n'est pas toujours disponible sur tous les systèmes.
     * 
     * SDL_GPU_SWAPCHAINCOMPOSITION_SDR_LINEAR est un bon choix pour les écrans SDR,
     * mais il n'est pas toujours disponible sur tous les systèmes.
     * 
     * SDL_GPU_SWAPCHAINCOMPOSITION_SDR est toujours disponible sur tous les systèmes.
     */
    SDL_GPUSwapchainComposition compositions[] = {
        SDL_GPU_SWAPCHAINCOMPOSITION_HDR10_ST2084,
        SDL_GPU_SWAPCHAINCOMPOSITION_HDR_EXTENDED_LINEAR,
        SDL_GPU_SWAPCHAINCOMPOSITION_SDR_LINEAR,
        SDL_GPU_SWAPCHAINCOMPOSITION_SDR
    };
    
    /**
     * Appliquer le swapchain pour le GPU
     * Rechercher la meilleure combinaison supportée entre le mode de présentation et la composition.
     */
    bool swapchain_combo_found = false;
    for (int i = 0; i < SDL_arraysize(present_modes); ++i)
    {
        for (int j = 0; j < SDL_arraysize(compositions); ++j) 
        {
            SDL_GPUPresentMode pm = present_modes[i];
            SDL_GPUSwapchainComposition sc = compositions[j];

            // Vérifie si la combinaison est supportée individuellement
            if (SDL_WindowSupportsGPUPresentMode(rc2d_engine_state.gpu_device, rc2d_engine_state.window, pm) &&
                SDL_WindowSupportsGPUSwapchainComposition(rc2d_engine_state.gpu_device, rc2d_engine_state.window, sc)) 
            {
                // Essaye la combinaison
                if (SDL_SetGPUSwapchainParameters(rc2d_engine_state.gpu_device, rc2d_engine_state.window, pm, sc)) 
                {
                    // Si la combinaison est supportée, on l'applique
                    rc2d_engine_state.gpu_present_mode = pm;
                    rc2d_engine_state.gpu_swapchain_composition = sc;
                    RC2D_log(RC2D_LOG_INFO, "GPU swapchain configuré avec succès : present_mode = %s, composition = %s", rc2d_present_mode_to_string(pm), rc2d_composition_to_string(sc));
                    swapchain_combo_found = true;
                    break;
                }
                else
                {
                    /**
                     * Si la combinaison de mode de présentation et de composition est supportée individuellement
                     * mais qu'elle échoue lors de l'application avec SDL_SetGPUSwapchainParameters, on loggue un avertissement
                     * en précisant les noms lisibles de la combinaison qui a échoué.
                     */
                    RC2D_log(RC2D_LOG_WARN, "La combinaison de mode de présentation et de composition a échoué : present_mode = %s, composition = %s", rc2d_present_mode_to_string(pm), rc2d_composition_to_string(sc));
                }
            }
        }

        // Si une combinaison valide a été trouvée, on sort de la boucle externe
        if (swapchain_combo_found) break;
    }

    if (!swapchain_combo_found) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Could not find any valid swapchain configuration.");
        return false;
    }

    return true;
}

/**
 * \brief Initialise les valeurs par défaut de l'état global du moteur RC2D.
 *
 * Cette fonction configure les valeurs par défaut pour toutes les variables de la structure RC2D_EngineState.
 * Elle est appelée avant toute autre opération pour garantir que l'état du moteur est correctement initialisé.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static void rc2d_engine_stateInit(void) {
    // Configuration de l'application (mettre toutes les valeurs par défaut)
    rc2d_engine_state.config = rc2d_engine_getDefaultConfig();

    // SDL : Fenêtre et événements
    rc2d_engine_state.window = NULL;
    // rc2d_engine_state.rc2d_event est déjà zéro-initialisé

    // SDL GPU
    rc2d_engine_state.gpu_device = NULL;
    rc2d_engine_state.gpu_present_mode = SDL_GPU_PRESENTMODE_VSYNC;
    rc2d_engine_state.gpu_swapchain_composition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
    rc2d_engine_state.gpu_current_command_buffer = NULL;
    rc2d_engine_state.gpu_current_render_pass = NULL;
    rc2d_engine_state.gpu_current_swapchain_texture = NULL;
    static SDL_GPUViewport default_viewport = {0, 0, 0, 0};
    rc2d_engine_state.gpu_current_viewport = &default_viewport;

    // Initialiser le cache des shaders graphiques
    rc2d_engine_state.gpu_graphics_shader_count = 0;
    rc2d_engine_state.gpu_graphics_shaders_cache = NULL;
    rc2d_engine_state.gpu_graphics_shader_mutex = SDL_CreateMutex();
    if (!rc2d_engine_state.gpu_graphics_shader_mutex) {
        RC2D_assert_release(false, RC2D_LOG_CRITICAL, "Erreur lors de la création du mutex pour les shaders : %s", SDL_GetError());
        return;
    }

    // Initialiser le cache des pipelines graphiques pour les shaders graphiques
    rc2d_engine_state.gpu_graphics_pipeline_count = 0;
    rc2d_engine_state.gpu_graphics_pipelines_cache = NULL;
    rc2d_engine_state.gpu_graphics_pipeline_mutex = SDL_CreateMutex();
    if (!rc2d_engine_state.gpu_graphics_pipeline_mutex) {
        RC2D_assert_release(false, RC2D_LOG_CRITICAL, "Erreur lors de la création du mutex pour les pipelines : %s", SDL_GetError());
        return;
    }
    
    // Initialiser le cache des shaders de calcul
    rc2d_engine_state.gpu_compute_shader_count = 0;
    rc2d_engine_state.gpu_compute_shaders_cache = NULL;
    rc2d_engine_state.gpu_compute_shader_mutex = SDL_CreateMutex();
    if (!rc2d_engine_state.gpu_compute_shader_mutex) {
        RC2D_assert_release(false, RC2D_LOG_CRITICAL, "Erreur lors de la création du mutex pour les shaders de calcul : %s", SDL_GetError());
        return;
    }

    // État d'exécution de la boucle de jeu
    rc2d_engine_state.fps = 60;
    rc2d_engine_state.delta_time = 0.0;
    rc2d_engine_state.game_is_running = true;
    rc2d_engine_state.last_frame_time = 0;

    // Paramètres de rendu
    rc2d_engine_state.render_scale = 1.0f;

    // Letterbox / Pillarbox
    rc2d_engine_state.letterbox_textures.mode = RC2D_LETTERBOX_NONE;
    rc2d_engine_state.letterbox_count = 0;

    rc2d_engine_state.letterbox_uniform_texture = RC2D_calloc(1, sizeof(RC2D_GPUTexture));
    rc2d_engine_state.letterbox_top_texture = RC2D_calloc(1, sizeof(RC2D_GPUTexture));
    rc2d_engine_state.letterbox_bottom_texture = RC2D_calloc(1, sizeof(RC2D_GPUTexture));
    rc2d_engine_state.letterbox_left_texture = RC2D_calloc(1, sizeof(RC2D_GPUTexture));
    rc2d_engine_state.letterbox_right_texture = RC2D_calloc(1, sizeof(RC2D_GPUTexture));
    rc2d_engine_state.letterbox_background_texture = RC2D_calloc(1, sizeof(RC2D_GPUTexture));

    if (!rc2d_engine_state.letterbox_uniform_texture || !rc2d_engine_state.letterbox_top_texture ||
        !rc2d_engine_state.letterbox_bottom_texture || !rc2d_engine_state.letterbox_left_texture ||
        !rc2d_engine_state.letterbox_right_texture || !rc2d_engine_state.letterbox_background_texture) {
        RC2D_assert_release(false, RC2D_LOG_CRITICAL, "Cannot continue with invalid letterbox texture allocations");
    }
}

/**
 * \brief Initialise la bibliothèque SDL3_shadercross.
 * 
 * Cette fonction initialise la bibliothèque SDL3_shadercross pour le rechargement à chaud des shaders.
 * Elle doit être appelée avant d'utiliser les fonctionnalités de rechargement à chaud des shaders.
 * 
 * \return true si l'initialisation a réussi, false sinon.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static bool rc2d_engine_init_sdlshadercross(void)
{
#if RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
    if (!SDL_ShaderCross_Init()) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Erreur lors de l'initialisation de SDL_shadercross.");
        return false;
    }
    else 
    {
        RC2D_log(RC2D_LOG_INFO, "SDL_shadercross initialisé avec succès.");
        return true;
    }
#endif

    // Si le rechargement à chaud des shaders n'est pas activé, on retourne true par défaut
    return true;
}

/**
 * \brief Libère les ressources SDL3_shadercross.
 * 
 * Cette fonction libère les ressources allouées par SDL3_shadercross.
 * Elle doit être appelée avant de quitter l'application pour éviter les fuites de mémoire.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static void rc2d_engine_cleanup_sdlshadercross(void)
{
#if RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
    SDL_ShaderCross_Quit();
    RC2D_log(RC2D_LOG_INFO, "SDL_shadercross nettoyé avec succès.");
#endif
}

/**
 * \brief Initialise la bibliothèque OpenSSL avec options de log.
 * 
 * Cette fonction appelle OPENSSL_init_ssl() avec les options standards de chargement
 * des chaînes d’erreur et d’algorithmes. Elle loggue et quitte le programme si 
 * l’initialisation échoue.
 * 
 * \return true si l'initialisation a réussi, false sinon.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static bool rc2d_engine_init_openssl(void) 
{
    if (OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS | OPENSSL_INIT_LOAD_CRYPTO_STRINGS, NULL) == 0)
    {
        RC2D_assert_release(false, RC2D_LOG_CRITICAL, "Erreur lors de l'initialisation d'OpenSSL : %s", ERR_error_string(ERR_get_error(), NULL));
        return false;
    }
    else 
    {
        RC2D_log(RC2D_LOG_INFO, "OpenSSL initialisé avec succès.");
        return true;
    }
}

/**
 * \brief Libère les ressources OpenSSL.
 *
 * Cette fonction libère les ressources allouées par OpenSSL et nettoie les chaînes d'erreur.
 * Elle doit être appelée avant de quitter l'application pour éviter les fuites de mémoire.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static void rc2d_engine_cleanup_openssl(void)
{
    ERR_free_strings();
    EVP_cleanup();
    RC2D_log(RC2D_LOG_INFO, "OpenSSL nettoyé avec succès.");
}

/**
 * \brief Initialise la bibliothèque SDL3_ttf.
 *
 * Cette fonction initialise la bibliothèque SDL3_ttf pour le rendu de polices.
 * Elle doit être appelée avant d'utiliser les fonctions de rendu de texte.
 *
 * \return true si l'initialisation a réussi, false sinon.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static bool rc2d_engine_init_sdlttf(void) 
{
    if (!TTF_Init()) 
    {
		RC2D_log(RC2D_LOG_CRITICAL, "Erreur lors de l'initialisation de SDL3_ttf : %s\n", SDL_GetError());
		return false;
    }
    else
    {
        RC2D_log(RC2D_LOG_INFO, "SDL3_ttf initialisé avec succès.\n");
        return true;
    }
}

/**
 * \brief Libère les ressources SDL3_ttf.
 *
 * Cette fonction libère les ressources allouées par SDL3_ttf.
 * Elle doit être appelée avant de quitter l'application pour éviter les fuites de mémoire.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static void rc2d_engine_cleanup_sdlttf(void)
{
    TTF_Quit();
    RC2D_log(RC2D_LOG_INFO, "SDL3_ttf nettoyé avec succès.\n");
}

/**
 * \brief Initialise la bibliothèque SDL3_mixer.
 *
 * Cette fonction initialise la bibliothèque SDL3_mixer pour le rendu audio.
 * Elle doit être appelée avant d'utiliser les fonctions de rendu audio.
 *
 * \return true si l'initialisation a réussi, false sinon.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static bool rc2d_engine_init_sdlmixer(void) 
{
    /*int audioFlags = MIX_INIT_OGG | MIX_INIT_MP3; // MIX_INIT_OGG for Nintendo Switch and other platforms
    if (Mix_Init(audioFlags) == -1) {
        RC2D_log(RC2D_LOG_CRITICAL, "Could not init SDL3_mixer : %s\n", Mix_GetError());
		return -1;
    }
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) == -1) {
        RC2D_log(RC2D_LOG_CRITICAL, "Could not init Mix_OpenAudio : %s\n", SDL_GetError());
        return -1;
    }*/

    // FIXEME : En attente de la mise en œuvre de SDL3_mixer
    return true;
}

/**
 * \brief Libère les ressources SDL3_mixer.
 *
 * Cette fonction libère les ressources allouées par SDL3_mixer.
 * Elle doit être appelée avant de quitter l'application pour éviter les fuites de mémoire.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static void rc2d_engine_cleanup_sdlmixer(void)
{
    // FIXEME : En attente de la mise en œuvre de SDL3_mixer
    /*Mix_CloseAudio();
    Mix_Quit();*/
    RC2D_log(RC2D_LOG_INFO, "SDL3_mixer nettoyé avec succès.\n");
}

/**
 * \brief Initialise la bibliothèque SDL3.
 *
 * Cette fonction initialise les sous-systèmes SDL3 nécessaires au moteur RC2D.
 *
 * \return true si l'initialisation a réussi, false sinon.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static bool rc2d_engine_init_sdl(void)
{
    /**
     * IMPORTANT:
     * Obligation pour le Steam Deck, sans le getenv("WAYLAND_DISPLAY") ou getenv("DISPLAY"),
     * l'application ne démarre pas.
     * Doit être fait avant l'initialisation de SDL3.
     */
    getenv("DISPLAY");
    getenv("WAYLAND_DISPLAY");

    /**
     * Liste des sous-systèmes SDL3 à initialiser.
     */
    int subsystems[] = {
        SDL_INIT_AUDIO,
        SDL_INIT_VIDEO,
        SDL_INIT_JOYSTICK,
        SDL_INIT_HAPTIC,
        SDL_INIT_GAMEPAD,
        SDL_INIT_EVENTS,
        SDL_INIT_SENSOR,
        SDL_INIT_CAMERA
    };

    /**
     * Liste des noms des sous-systèmes SDL3 pour le logging.
     * Doit être dans le même ordre que la liste des flags ci-dessus.
     */
    const char* names[] = {
        "AUDIO", 
        "VIDEO", 
        "JOYSTICK", 
        "HAPTIC", 
        "GAMEPAD", 
        "EVENTS", 
        "SENSOR", 
        "CAMERA"
    };

    /**
     * Initialisation de SDL3 avec tous les sous-systèmes nécessaires.
     * On vérifie si chaque sous-système s'initialise correctement.
     * Si un sous-système échoue, on loggue l'erreur et on continue.
     */
    for (int i = 0; i < sizeof(subsystems) / sizeof(subsystems[0]); ++i) 
    {
        if (!SDL_InitSubSystem(subsystems[i])) 
        {
            RC2D_log(RC2D_LOG_CRITICAL, "Erreur lors de l'initialisation du sous-système SDL3 %s : %s\n", names[i], SDL_GetError());
        } 
        else {
            RC2D_log(RC2D_LOG_INFO, "Initialise le sous-système SDL3 %s avec succès.\n", names[i]);
        }
    }

    return true;
}

/**
 * \brief Libère les ressources SDL3.
 *
 * Cette fonction libère les ressources allouées par SDL3.
 * Elle doit être appelée avant de quitter l'application pour éviter les fuites de mémoire.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static void rc2d_engine_cleanup_sdl(void)
{
    SDL_Quit();
    RC2D_log(RC2D_LOG_INFO, "SDL3 nettoyé avec succès.\n");
}

/**
 * \brief Crée la fenêtre principale de l'application RC2D.
 * 
 * Cette fonction configure et crée la fenêtre SDL3 avec les propriétés spécifiées dans la configuration du moteur.
 * La fenêtre est initialement cachée pour éviter des artefacts visuels jusqu'à ce que le rendu GPU soit prêt.
 * 
 * \note La fenetre sera visible juste avant la première frame de la boucle de jeu.
 * 
 * \return true si la fenêtre a été créée avec succès, false sinon.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static bool rc2d_engine_create_window(void)
{
    SDL_PropertiesID window_props = SDL_CreateProperties();
    SDL_SetStringProperty(window_props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, rc2d_engine_state.config->appInfo->name);
    SDL_SetNumberProperty(window_props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, rc2d_engine_state.config->windowWidth);
    SDL_SetNumberProperty(window_props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, rc2d_engine_state.config->windowHeight);
    SDL_SetBooleanProperty(window_props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
    SDL_SetBooleanProperty(window_props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);
    SDL_SetNumberProperty(window_props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
    SDL_SetNumberProperty(window_props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);

    /**
     * IMPORTANT :
     * Cache la fenêtre tant que le rendu GPU n'est pas prêt pour éviter des artefacts visuels.
     * On l'affichera plus tard juste avant la première frame de la boucle de jeu.
     */
    SDL_SetBooleanProperty(window_props, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, true);

    rc2d_engine_state.window = SDL_CreateWindowWithProperties(window_props);
    SDL_DestroyProperties(window_props);
    if (!rc2d_engine_state.window) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Erreur lors de la création de la fenêtre : %s", SDL_GetError());
        return false;
    }
    else
    {
        RC2D_log(RC2D_LOG_INFO, "La fenêtre est créée avec succès, mais temporairement masquée le temps que tout soit prêt.");
    }

    return true;
}

/**
 * \brief Convertit une valeur SDL_GPUSampleCount en une chaîne lisible.
 *
 * \param sample_count La valeur SDL_GPUSampleCount à convertir.
 * \return Une chaîne statique décrivant le niveau de MSAA, ou "Unknown" si la valeur est invalide.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static const char* rc2d_engine_sampleCountToString(SDL_GPUSampleCount sample_count)
{
    switch (sample_count)
    {
        case SDL_GPU_SAMPLECOUNT_1:
            return "No MSAA (1x)";
        case SDL_GPU_SAMPLECOUNT_2:
            return "MSAA 2x";
        case SDL_GPU_SAMPLECOUNT_4:
            return "MSAA 4x";
        case SDL_GPU_SAMPLECOUNT_8:
            return "MSAA 8x";
        default:
            return "Unknown";
    }
}

/**
 * \brief Configure le niveau de MSAA (Multi-Sample Anti-Aliasing) pour le rendu graphique.
 * 
 * Cette fonction vérifie les niveaux de MSAA supportés par le GPU et configure le moteur RC2D
 * pour utiliser le niveau de MSAA le plus élevé disponible. Si aucun niveau de MSAA n'est supporté,
 * elle utilise SAMPLECOUNT_1 (pas de MSAA).
 * 
 * \return true si la configuration du MSAA a réussi, false sinon.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static bool rc2d_engine_configureMSAA(void)
{
    // Si c'est un jeu en pixel art, on n'utilise pas de MSAA
    if (rc2d_engine_state.config->logicalPresentationMode == RC2D_LOGICAL_PRESENTATION_INTEGER_SCALE)
    {
        RC2D_log(RC2D_LOG_INFO, "MSAA désactivé pour les jeux en pixel art.");
        rc2d_engine_state.gpu_current_sample_count_supported = SDL_GPU_SAMPLECOUNT_1;
        return true;
    }

    // Récupérer le format de swapchain pour vérifier la compatibilité avec le MSAA
    SDL_GPUTextureFormat swapchain_format = SDL_GetGPUSwapchainTextureFormat(rc2d_gpu_getDevice(), rc2d_window_getWindow());
    if (swapchain_format == SDL_GPU_TEXTUREFORMAT_INVALID)
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Echec de la recuperation du format de swapchain : %s", SDL_GetError());
        return false;
    }

    // Ordre décroissant de qualité pour le MSAA
    SDL_GPUSampleCount sample_counts[] = {
        SDL_GPU_SAMPLECOUNT_8,  // MSAA 8x : meilleure qualité
        SDL_GPU_SAMPLECOUNT_4,  // MSAA 4x : bon compromis
        SDL_GPU_SAMPLECOUNT_2,  // MSAA 2x : qualité modérée
        SDL_GPU_SAMPLECOUNT_1   // Pas de MSAA : toujours supporté
    };

    bool msaa_supported = false;
    SDL_GPUSampleCount selected_sample_count = SDL_GPU_SAMPLECOUNT_1; // Fallback par défaut

    // On parcourt les niveaux de MSAA supportés par le GPU
    for (int i = 0; i < SDL_arraysize(sample_counts); ++i)
    {
        SDL_GPUSampleCount sample_count = sample_counts[i];
        if (SDL_GPUTextureSupportsSampleCount(rc2d_engine_state.gpu_device, swapchain_format, sample_count))
        {
            selected_sample_count = sample_count;
            msaa_supported = true;
            RC2D_log(RC2D_LOG_INFO, "%s supporter par rapport au format de la swapchain", rc2d_engine_sampleCountToString(sample_count));
            break; // Arrêter dès qu'un niveau supporté est trouvé
        }
        else
        {
            RC2D_log(RC2D_LOG_DEBUG, "%s non supporter par rapport au format de la swapchain", rc2d_engine_sampleCountToString(sample_count));
        }
    }

    // Si aucun niveau de MSAA n'est supporté, on utilise SAMPLECOUNT_1 donc pas de MSAA
    if (!msaa_supported)
    {
        RC2D_log(RC2D_LOG_INFO, "Aucun niveau de MSAA supporter, utilisation de SAMPLECOUNT_1 donc pas de MSAA.");
    }

    // On set le niveau de MSAA sélectionné dans l'état du moteur
    rc2d_engine_state.gpu_current_sample_count_supported = selected_sample_count;

    return true;
}

/**
 * \brief Initialise le dispositif GPU pour le rendu graphique dans RC2D.
 * 
 * Cette fonction configure et crée le dispositif GPU SDL3, vérifie les formats de shaders supportés,
 * configure les modes de présentation et les paramètres de swapchain, et associe la fenêtre au GPU.
 * 
 * \return true si le GPU a été initialisé avec succès, false sinon.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static bool rc2d_engine_create_gpu(void)
{
    SDL_PropertiesID gpu_props = SDL_CreateProperties();
    
    /**
     * Propriété concernant le GPU :
     * - SDL_PROP_GPU_DEVICE_CREATE_DEBUGMODE_BOOLEAN : active le mode debug GPU.
     * - SDL_PROP_GPU_DEVICE_CREATE_VERBOSE_BOOLEAN : active les logs détaillés.
     * - SDL_PROP_GPU_DEVICE_CREATE_PREFERLOWPOWER_BOOLEAN : privilégie un GPU à faible consommation ou non.
     * - SDL_PROP_GPU_DEVICE_CREATE_NAME_STRING : nom du GPU (Vulkan, Metal, Direct3D12 ou un backend privé).
     */
    SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_DEBUGMODE_BOOLEAN, rc2d_engine_state.config->gpuOptions->debugMode);
    //SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_VERBOSE_BOOLEAN, rc2d_engine_state.config->gpuOptions->verbose);
    SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_PREFERLOWPOWER_BOOLEAN, rc2d_engine_state.config->gpuOptions->preferLowPower);
    
    // Pilote GPU forcé si nécessaire
    switch (rc2d_engine_state.config->gpuOptions->driver)
    {
        case RC2D_GPU_DRIVER_VULKAN:
            SDL_SetStringProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_NAME_STRING, "vulkan");
            break;
        case RC2D_GPU_DRIVER_METAL:
            SDL_SetStringProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_NAME_STRING, "metal");
            break;
        case RC2D_GPU_DRIVER_DIRECT3D12:
            SDL_SetStringProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_NAME_STRING, "direct3d12");
            break;
        case RC2D_GPU_DRIVER_PRIVATE:
            SDL_SetStringProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_NAME_STRING, "a voir plus tard avec SDL3");
            break;
        case RC2D_GPU_DRIVER_DEFAULT:
        default:
            // Ne pas setter la propriété pour laisser SDL choisir automatiquement
            break;
    }

    /**
     * Désactiver certaines fonctionnalités GPU qui ne sont pas nécessaires et permet surtout pour la plateforme Android
     * d'être d'avantage compatible.
     * 
     * - SDL_PROP_GPU_DEVICE_CREATE_VULKAN_SHADERCLIPDISTANCE_BOOLEAN : désactive le support de clip distance pour Vulkan.
     * 
     * - SDL_PROP_GPU_DEVICE_CREATE_VULKAN_DEPTHCLAMP_BOOLEAN : Si elle est désactivée, la propriété
     *   enable_depth_clip dans SDL_GPURasterizerState doit toujours être définie sur « true ».
     * 
     * - SDL_PROP_GPU_DEVICE_CREATE_VULKAN_DRAWINDIRECTFIRST_BOOLEAN : Si elle est désactivée, l'argument
     *   first_instance de SDL_GPUIndirectDrawCommand doit être défini sur « 0 ».
     * 
     * - SDL_PROP_GPU_DEVICE_CREATE_VULKAN_SAMPLERANISOTROPY_BOOLEAN : Si elle est désactivée, la propriété 
     *   enable_anisotropy de SDL_GPUSamplerCreateInfo doit être définie sur « false ». 
     */
    /*SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_VULKAN_SHADERCLIPDISTANCE_BOOLEAN, false);
    SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_VULKAN_DEPTHCLAMP_BOOLEAN, false);
    SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_VULKAN_DRAWINDIRECTFIRST_BOOLEAN, false);
    SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_VULKAN_SAMPLERANISOTROPY_BOOLEAN, false);*/

    /**
     * Propriétés concernant le GPU :
     * - SDL_PROP_GPU_DEVICE_CREATE_SHADERS_PRIVATE_BOOLEAN : active le format de shader privé (NDA).
     * - SDL_PROP_GPU_DEVICE_CREATE_SHADERS_SPIRV_BOOLEAN : active le format de shader SPIR-V.
     * - SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXBC_BOOLEAN : active le format de shader DXBC.
     * - SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXIL_BOOLEAN : active le format de shader DXIL.
     * - SDL_PROP_GPU_DEVICE_CREATE_SHADERS_MSL_BOOLEAN : active le format de shader MSL.
     * - SDL_PROP_GPU_DEVICE_CREATE_SHADERS_METALLIB_BOOLEAN : active le format de shader METALLIB.
     */
    SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_PRIVATE_BOOLEAN, true);
    SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_SPIRV_BOOLEAN, true);
    SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXBC_BOOLEAN, true);
    SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXIL_BOOLEAN, true);
    SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_MSL_BOOLEAN, true);
    SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_METALLIB_BOOLEAN, true);

    // Vérification de la compatibilité du GPU avec les propriétés spécifiées.
    if (!SDL_GPUSupportsProperties(gpu_props)) {
        RC2D_log(RC2D_LOG_CRITICAL, "Le GPU ne supporte pas les propriétés spécifiées.");
        SDL_DestroyProperties(gpu_props);
        return false;
    }

    // Créer le GPU device avec les propriétés spécifiées
    rc2d_engine_state.gpu_device = SDL_CreateGPUDeviceWithProperties(gpu_props);
    SDL_DestroyProperties(gpu_props);
    if (!rc2d_engine_state.gpu_device) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Erreur lors de la création du GPU device : %s", SDL_GetError());
        return false;
    }
    else
    {
        RC2D_log(RC2D_LOG_INFO, "GPU device créé avec succès.");
    }

    /**
     * Détecter les formats supportés par le GPU
     */
    SDL_GPUShaderFormat supported_formats = SDL_GetGPUShaderFormats(rc2d_engine_state.gpu_device);
    RC2D_log(RC2D_LOG_INFO, "Supported shader formats : ");
    if (supported_formats & SDL_GPU_SHADERFORMAT_PRIVATE)
    {
        RC2D_log(RC2D_LOG_INFO, "- PRIVATE(NDA)");
    }
    if (supported_formats & SDL_GPU_SHADERFORMAT_SPIRV)
    {
        RC2D_log(RC2D_LOG_INFO, "- SPIR-V");
    }
    if (supported_formats & SDL_GPU_SHADERFORMAT_DXBC)
    {
        RC2D_log(RC2D_LOG_INFO, "- DXBC");
    }
    if (supported_formats & SDL_GPU_SHADERFORMAT_DXIL)
    {
        RC2D_log(RC2D_LOG_INFO, "- DXIL");
    }
    if (supported_formats & SDL_GPU_SHADERFORMAT_MSL)
    {
        RC2D_log(RC2D_LOG_INFO, "- MSL");
    }
    if (supported_formats & SDL_GPU_SHADERFORMAT_METALLIB)
    {
        RC2D_log(RC2D_LOG_INFO, "- METALLIB");
    }

    /**
     * Associe la fenêtre au GPU device
     */
    if (!SDL_ClaimWindowForGPUDevice(rc2d_engine_state.gpu_device, rc2d_engine_state.window))
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Erreur lors de l'association de la fenêtre au GPU : %s", SDL_GetError());
        return false;
    }
    else
    {
        RC2D_log(RC2D_LOG_INFO, "Fenêtre associée au GPU avec succès.");
    }

    // Configurer le swapchain en utilisant la fonction dédiée
    if (!rc2d_engine_configure_swapchain())
    {
        return false;
    }

    /**
     * Configurer le nombre de frames en vol pour le GPU
     * On utilise le nombre de frames en vol spécifié dans la configuration du moteur.
     */
    if (!SDL_SetGPUAllowedFramesInFlight(rc2d_engine_state.gpu_device, (Uint32)rc2d_engine_state.config->gpuFramesInFlight)) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Echec de la configuration des frames en vol pour le GPU : %s", SDL_GetError());
        return false;
    }
    else
    {
        RC2D_log(RC2D_LOG_INFO, "Frames en vol configurees avec succes : %d", rc2d_engine_state.config->gpuFramesInFlight);
    }

    // Configurer le MSAA (Multi-Sample Anti-Aliasing)
    if (!rc2d_engine_configureMSAA()) 
    {
        return false;
    }

    return true;
}

/**
 * \brief Calcule l'échelle de rendu et le viewport GPU.
 *
 * Cette fonction calcule l'échelle de rendu interne et le viewport GPU en fonction de la taille de la fenêtre,
 * de la zone sûre, du DPI et du mode de présentation.
 * Elle doit être appelée après la création de la fenêtre et avant le rendu.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static void rc2d_engine_calculate_renderscale_and_gpuviewport(void) 
{
    // Récupère la taille réelle de la fenêtre (pixels visibles, indépendamment du DPI)
    int window_width, window_height;
    rc2d_window_getSize(&window_width, &window_height);

    // Récupère la zone sûre : certaines plateformes (TV, téléphones à encoche..etc) ont des zones à éviter
    RC2D_Rect safe_area;
    rc2d_window_getSafeArea(&safe_area);

    // Gère le high DPI : pixel_density > 1.0 = écran Retina, etc.
    float pixel_density = rc2d_window_getPixelDensity();

    // Récupère l'échelle d'affichage de la fenêtre (ex: 1.0 pour 100%, 2.0 pour 200%)
    float display_scale = rc2d_window_getDisplayScale();

    // Calcule la taille réelle (en pixels) de la zone sûre
    int effective_width = (int)(safe_area.width * pixel_density);
    int effective_height = (int)(safe_area.height * pixel_density);

    // Initialisation des variables de viewport
    float viewport_x, viewport_y, viewport_width, viewport_height;

    // Initialisation de l'échelle de rendu
    float scale;

    // --- Mode Pixel Art ---
    if (rc2d_engine_state.config->logicalPresentationMode == RC2D_LOGICAL_PRESENTATION_INTEGER_SCALE) 
    {
        // Calcul de mise à l’échelle entière
        int int_scale = SDL_min(effective_width / rc2d_engine_state.config->logicalWidth, effective_height / rc2d_engine_state.config->logicalHeight);

        // Si l’échelle est trop petite, on la fixe à 1 pour éviter les problèmes d’affichage
        if (int_scale < 1) int_scale = 1;
        scale = (float)int_scale;

        // Calcul de la taille du viewport
        viewport_width = rc2d_engine_state.config->logicalWidth * scale;
        viewport_height = rc2d_engine_state.config->logicalHeight * scale;
    }
    // --- Mode Letterbox ---
    else if (rc2d_engine_state.config->logicalPresentationMode == RC2D_LOGICAL_PRESENTATION_LETTERBOX)
    {
        float logical_aspect = (float)rc2d_engine_state.config->logicalWidth / rc2d_engine_state.config->logicalHeight;
        float window_aspect = (float)effective_width / effective_height;

        // On adapte la largeur ou la hauteur selon le ratio d’aspect (16:9, 4:3, etc.)
        if (window_aspect > logical_aspect) 
        {
            scale = (float)effective_height / rc2d_engine_state.config->logicalHeight;
            viewport_width = rc2d_engine_state.config->logicalWidth * scale;
            viewport_height = effective_height;
        } 
        else 
        {
            scale = (float)effective_width / rc2d_engine_state.config->logicalWidth;
            viewport_width = effective_width;
            viewport_height = rc2d_engine_state.config->logicalHeight * scale;
        }
    }

    // Convertit la taille du viewport de pixels physiques vers pixels logiques
    viewport_width /= pixel_density;
    viewport_height /= pixel_density;

    // Centre le viewport dans la zone sûre
    viewport_x = safe_area.x + (safe_area.width - viewport_width) / 2.0f;
    viewport_y = safe_area.y + (safe_area.height - viewport_height) / 2.0f;

    // Applique le viewport au GPU
    rc2d_engine_state.gpu_current_viewport->x = viewport_x;
    rc2d_engine_state.gpu_current_viewport->y = viewport_y;
    rc2d_engine_state.gpu_current_viewport->w = viewport_width;
    rc2d_engine_state.gpu_current_viewport->h = viewport_height;
    rc2d_engine_state.gpu_current_viewport->min_depth = 0.0f;
    rc2d_engine_state.gpu_current_viewport->max_depth = 1.0f;

    // Applique l’échelle de rendu interne
    rc2d_engine_state.render_scale = scale * display_scale;

    // Calcule les zones de letterbox/pillarbox si nécessaire
    rc2d_engine_state.letterbox_count = 0;
    SDL_memset(rc2d_engine_state.letterbox_areas, 0, sizeof(RC2D_Rect) * 4);

    if (viewport_width < safe_area.width || viewport_height < safe_area.height) 
    {
        // Barres verticales (gauche/droite) - Letterbox
        if (viewport_x > safe_area.x) 
        {
            // Barre gauche
            rc2d_engine_state.letterbox_areas[0].x = safe_area.x;
            rc2d_engine_state.letterbox_areas[0].y = safe_area.y;
            rc2d_engine_state.letterbox_areas[0].width = viewport_x - safe_area.x;
            rc2d_engine_state.letterbox_areas[0].height = safe_area.height;
            rc2d_engine_state.letterbox_count++;

            // Barre droite
            rc2d_engine_state.letterbox_areas[1].x = viewport_x + viewport_width;
            rc2d_engine_state.letterbox_areas[1].y = safe_area.y;
            rc2d_engine_state.letterbox_areas[1].width = safe_area.x + safe_area.width - (viewport_x + viewport_width);
            rc2d_engine_state.letterbox_areas[1].height = safe_area.height;
            rc2d_engine_state.letterbox_count++;
        }

        // Barres horizontales (haut/bas) - Pillarbox
        if (viewport_y > safe_area.y) 
        {
            // Barre haute
            rc2d_engine_state.letterbox_areas[2].x = safe_area.x;
            rc2d_engine_state.letterbox_areas[2].y = safe_area.y;
            rc2d_engine_state.letterbox_areas[2].width = safe_area.width;
            rc2d_engine_state.letterbox_areas[2].height = viewport_y - safe_area.y;
            rc2d_engine_state.letterbox_count++;

            // Barre basse
            rc2d_engine_state.letterbox_areas[3].x = safe_area.x;
            rc2d_engine_state.letterbox_areas[3].y = viewport_y + viewport_height;
            rc2d_engine_state.letterbox_areas[3].width = safe_area.width;
            rc2d_engine_state.letterbox_areas[3].height = safe_area.y + safe_area.height - (viewport_y + viewport_height);
            rc2d_engine_state.letterbox_count++;
        }
    }
}

/**
 * \brief Met à jour le FPS en fonction du moniteur.
 *
 * Cette fonction met à jour le FPS en fonction du taux de rafraîchissement du moniteur associé à la fenêtre.
 * Elle doit être appelée après la création de la fenêtre et avant le rendu.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static void rc2d_engine_update_fps_based_on_monitor(void) 
{
    // Récupére le moniteur associé a la fenetre.
    SDL_DisplayID displayID = SDL_GetDisplayForWindow(rc2d_engine_state.window);
    if (displayID == 0) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Could not get display index for window: %s", SDL_GetError());
        return;
    }

    // Obtient le mode d'affichage actuel du moniteur.
    const SDL_DisplayMode* currentDisplayMode = SDL_GetCurrentDisplayMode(displayID);
    if (currentDisplayMode == NULL) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Could not get current display mode for display #%d: %s", displayID, SDL_GetError());
        return;
    }

    // Met à jour les FPS selon le taux de rafraîchissement du moniteur.
    if (currentDisplayMode->refresh_rate_numerator > 0 && currentDisplayMode->refresh_rate_denominator > 0) 
    {
        rc2d_engine_state.fps = (double)currentDisplayMode->refresh_rate_numerator / currentDisplayMode->refresh_rate_denominator;
    } 
    else if (currentDisplayMode->refresh_rate > 0.0f) 
    {
        rc2d_engine_state.fps = (double)currentDisplayMode->refresh_rate;
    } 
    else 
    {
        rc2d_engine_state.fps = 60.0; // fallback
    }

    /**
     * Permet définir le tickrate de la callback SDL_AppIterate qui est appelé par SDL3,
     * par rapport au taux de rafraîchissement du moniteur.
     */
    char fps_str[16];
    SDL_snprintf(fps_str, sizeof(fps_str), "%d", (int)rc2d_engine_state.fps);

    // FIXME: En attendant que SDL3 puisse : Utilise une précision à virgule flottante pour par exemple 59.94 Hz
    //SDL_snprintf(fps_str, sizeof(fps_str), "%.2f", rc2d_engine_state.fps);

    if (!SDL_SetHintWithPriority(SDL_HINT_MAIN_CALLBACK_RATE, fps_str, SDL_HINT_OVERRIDE)) 
    {
        RC2D_log(RC2D_LOG_WARN, "Failed to set SDL_HINT_MAIN_CALLBACK_RATE to %s Hz with OVERRIDE priority: %s", fps_str, SDL_GetError());
    }
}

void rc2d_engine_deltatime_start(void)
{
    // Capture le temps au debut de la frame actuelle
    Uint64 now = SDL_GetPerformanceCounter();

    // Calcule le delta time depuis la derniere frame
    rc2d_engine_state.delta_time = (double)(now - rc2d_engine_state.last_frame_time) / (double)SDL_GetPerformanceFrequency();
    
    // Met a jour 'lastFrameTime' pour la prochaine frame
    rc2d_engine_state.last_frame_time = now;
}

void rc2d_engine_deltatime_end(void)
{
    /**
     * Vérifie si la hint SDL_HINT_MAIN_CALLBACK_RATE est active
     * Fallback : utilise SDL_DelayPrecise si la hint n'est pas définie ou définie à 0
     * puis que c'est possible que SDL_HINT_MAIN_CALLBACK_RATE ne sois pas pris en compte sur certaines plateformes.
     */
    const char* callback_rate = SDL_GetHint(SDL_HINT_MAIN_CALLBACK_RATE);
    if (callback_rate == NULL || SDL_strcmp(callback_rate, "0") == 0)
    {
        // Capture le temps a la fin de la frame actuelle
        Uint64 frameEnd = SDL_GetPerformanceCounter();

        // Calcule le temps de la frame actuelle en millisecondes
        double frameTimeMs = (double)(frameEnd - rc2d_engine_state.last_frame_time) * 1000.0 / (double)SDL_GetPerformanceFrequency();

        // Attendre le temps necessaire pour atteindre le FPS cible
        double targetFrameMs = 1000.0 / rc2d_engine_state.fps;
        if (frameTimeMs < targetFrameMs) 
        {
            Uint64 delayNs = (Uint64)((targetFrameMs - frameTimeMs) * 1e6);
            SDL_DelayPrecise(delayNs);
        } 
    }
}

SDL_AppResult rc2d_engine_processevent(SDL_Event *event) 
{
    // Quit program
    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS;
    }

    // Le presse-papiers ou la sélection principale a changé
    else if (event->type == SDL_EVENT_CLIPBOARD_UPDATE)
    {
        if (rc2d_engine_state.config != NULL &&
            rc2d_engine_state.config->callbacks != NULL &&
            rc2d_engine_state.config->callbacks->rc2d_clipboardupdated != NULL)
        {
            const SDL_ClipboardEvent* e = &event->clipboard;

            RC2D_ClipboardEventInfo info = {
                .is_owner = e->owner,
                .num_mime_types = e->num_mime_types,
                .mime_types = e->mime_types
            };

            rc2d_engine_state.config->callbacks->rc2d_clipboardupdated(&info);
        }
    }

    else if (event->type == SDL_EVENT_CAMERA_DEVICE_ADDED ||
            event->type == SDL_EVENT_CAMERA_DEVICE_REMOVED ||
            event->type == SDL_EVENT_CAMERA_DEVICE_APPROVED ||
            event->type == SDL_EVENT_CAMERA_DEVICE_DENIED)
    {
        const SDL_CameraDeviceEvent* e = &event->cdevice;

        RC2D_CameraEventInfo info = {
            .deviceID = e->which
        };

        if (rc2d_engine_state.config && rc2d_engine_state.config->callbacks) 
        {
            switch (event->type) 
            {
                case SDL_EVENT_CAMERA_DEVICE_ADDED:
                    if (rc2d_engine_state.config->callbacks->rc2d_cameraadded)
                        rc2d_engine_state.config->callbacks->rc2d_cameraadded(&info);
                    break;
                case SDL_EVENT_CAMERA_DEVICE_REMOVED:
                    if (rc2d_engine_state.config->callbacks->rc2d_cameraremoved)
                        rc2d_engine_state.config->callbacks->rc2d_cameraremoved(&info);
                    break;
                case SDL_EVENT_CAMERA_DEVICE_APPROVED:
                    if (rc2d_engine_state.config->callbacks->rc2d_cameraapproved)
                        rc2d_engine_state.config->callbacks->rc2d_cameraapproved(&info);
                    break;
                case SDL_EVENT_CAMERA_DEVICE_DENIED:
                    if (rc2d_engine_state.config->callbacks->rc2d_cameradenied)
                        rc2d_engine_state.config->callbacks->rc2d_cameradenied(&info);
                    break;
            }
        }
    }

    else if (event->type == SDL_EVENT_WILL_ENTER_FOREGROUND) 
    {
/**
 * Appelez SDL_GDKResumeGPU pour reprendre le fonctionnement du GPU sur Xbox 
 * lorsqu'on recoit l'événement : SDL_EVENT_WILL_ENTER_FOREGROUND.
 * 
 * IMPORTANT: Lors de la reprise, cette fonction (SDL_GDKResumeGPU) DOIT être appelée 
 * avant d'appeler toute autre fonction SDL_GPU .
 */
#if defined(RC2D_PLATFORM_XBOXSERIES) || defined(RC2D_PLATFORM_XBOXONE)
    SDL_GDKResumeGPU(rc2d_gpu_getDevice());
#endif 
    }

    else if (event->type == SDL_EVENT_DID_ENTER_BACKGROUND) 
    {
/**
 * Appelez SDL_GDKSuspendGPU pour suspendre le fonctionnement du GPU sur Xbox 
 * lorsqu'on recoit l'événement : SDL_EVENT_DID_ENTER_BACKGROUND.
 * 
 * IMPORTANT: N'appelez aucune fonction SDL_GPU après avoir appelé cette fonction (SDL_GDKSuspendGPU) ! 
 * Celle-ci doit également être appelée avant SDL_GDKSuspendComplete .
 */
#if defined(RC2D_PLATFORM_XBOXSERIES) || defined(RC2D_PLATFORM_XBOXONE)
    SDL_GDKSuspendGPU(rc2d_gpu_getDevice());
#endif
    }

    // La préférence de la langue locale a changé
    else if (event->type == SDL_EVENT_LOCALE_CHANGED)
    {
        if (rc2d_engine_state.config != NULL &&
            rc2d_engine_state.config->callbacks != NULL &&
            rc2d_engine_state.config->callbacks->rc2d_localechanged != NULL) 
        {
            RC2D_Locale* locales = rc2d_local_getPreferredLocales();
            rc2d_engine_state.config->callbacks->rc2d_localechanged(locales);
            rc2d_local_freeLocales(locales);
        }
    }

    // Quand l'orientation de l'affichage change
    else if (event->type == SDL_EVENT_DISPLAY_ORIENTATION) 
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_monitororientationchanged != NULL) 
        {
            /**
             * Recalculer le viewport GPU et le render scale, puisque l'orientation de l'affichage a changé.
             * Cela est nécessaire pour s'assurer que le rendu s'adapte correctement à la nouvelle orientation.
             */
            rc2d_engine_calculate_renderscale_and_gpuviewport();
            rc2d_engine_update_fps_based_on_monitor();

            RC2D_DisplayOrientation newOrientation = rc2d_window_getDisplayOrientation();
            rc2d_engine_state.config->callbacks->rc2d_monitororientationchanged(event->display.displayID, newOrientation);
        }
    }

        // Monitor Added
    else if (event->type == SDL_EVENT_DISPLAY_ADDED)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_monitoradded != NULL) 
        {
            rc2d_engine_calculate_renderscale_and_gpuviewport();
            rc2d_engine_update_fps_based_on_monitor();

            rc2d_engine_state.config->callbacks->rc2d_monitoradded(event->display.displayID);
        }
    }

    // Monitor Removed
    else if (event->type == SDL_EVENT_DISPLAY_REMOVED)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_monitorremoved != NULL) 
        {
            rc2d_engine_calculate_renderscale_and_gpuviewport();
            rc2d_engine_update_fps_based_on_monitor();

            rc2d_engine_state.config->callbacks->rc2d_monitorremoved(event->display.displayID);
        }
    }

    // Monitor Moved
    else if (event->type == SDL_EVENT_DISPLAY_MOVED)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_monitormoved != NULL) 
        {
            rc2d_engine_calculate_renderscale_and_gpuviewport();
            rc2d_engine_update_fps_based_on_monitor();

            rc2d_engine_state.config->callbacks->rc2d_monitormoved(event->display.displayID);
        }
    }

    // Monitor Desktop Mode Changed
    else if (event->type == SDL_EVENT_DISPLAY_DESKTOP_MODE_CHANGED)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_monitordesktopmodechanged != NULL) 
        {
            rc2d_engine_calculate_renderscale_and_gpuviewport();
            rc2d_engine_update_fps_based_on_monitor();

            rc2d_engine_state.config->callbacks->rc2d_monitordesktopmodechanged(event->display.displayID);
        }
    }

    // Monitor Current Mode Changed
    else if (event->type == SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_monitorcurrentmodechanged != NULL) 
        {
            rc2d_engine_calculate_renderscale_and_gpuviewport();
            rc2d_engine_update_fps_based_on_monitor();

            rc2d_engine_state.config->callbacks->rc2d_monitorcurrentmodechanged(event->display.displayID);
        }
    }

    // Monitor Content Scale Changed
    else if (event->type == SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED)
    {
        rc2d_engine_calculate_renderscale_and_gpuviewport();
        rc2d_engine_update_fps_based_on_monitor();
    }

    // Window HDR State changed
    else if (event->type == SDL_EVENT_WINDOW_HDR_STATE_CHANGED ||
            event->type == SDL_EVENT_WINDOW_ICCPROF_CHANGED)
    {
        // Re-set le meilleur swapchain disponible
        if (!rc2d_engine_configure_swapchain())
        {
            RC2D_log(RC2D_LOG_ERROR, "Failed to update swapchain on HDR state change: %s", SDL_GetError());
        }
    }

    else if (event->type == SDL_EVENT_FINGER_DOWN ||
            event->type == SDL_EVENT_FINGER_UP ||
            event->type == SDL_EVENT_FINGER_MOTION ||
            event->type == SDL_EVENT_FINGER_CANCELED)
    {
        const SDL_TouchFingerEvent* e = &event->tfinger;

        RC2D_TouchEventInfo info = {
            .touchID = e->touchID,
            .fingerID = e->fingerID,
            .x = e->x,
            .y = e->y,
            .dx = e->dx,
            .dy = e->dy,
            .pressure = e->pressure
        };

        if (rc2d_engine_state.config && rc2d_engine_state.config->callbacks)
        {
            switch (event->type) 
            {
                case SDL_EVENT_FINGER_DOWN:
                    if (rc2d_engine_state.config->callbacks->rc2d_touchpressed)
                        rc2d_engine_state.config->callbacks->rc2d_touchpressed(&info);
                    break;
                case SDL_EVENT_FINGER_UP:
                    if (rc2d_engine_state.config->callbacks->rc2d_touchreleased)
                        rc2d_engine_state.config->callbacks->rc2d_touchreleased(&info);
                    break;
                case SDL_EVENT_FINGER_MOTION:
                    if (rc2d_engine_state.config->callbacks->rc2d_touchmoved)
                        rc2d_engine_state.config->callbacks->rc2d_touchmoved(&info);
                    break;
                case SDL_EVENT_FINGER_CANCELED:
                    if (rc2d_engine_state.config->callbacks->rc2d_touchcanceled)
                        rc2d_engine_state.config->callbacks->rc2d_touchcanceled(&info);
                    break;
            }
        }

        // Mise à jour de l’état du toucher :
        rc2d_touch_updateState(info.touchID, info.fingerID, event->type, info.pressure, info.x, info.y);
    }

    // Window safe area changed
    else if (event->type == SDL_EVENT_WINDOW_SAFE_AREA_CHANGED) 
    {
        /**
         * Quand la zone de sécurité de la fenêtre change,
         * on indique que le viewport du gpu et render scale doit être recalculé.
         */
        rc2d_engine_calculate_renderscale_and_gpuviewport();
        rc2d_engine_update_fps_based_on_monitor();
    }

    // Window enter fullscreen
    else if (event->type == SDL_EVENT_WINDOW_ENTER_FULLSCREEN) 
    {
        /**
         * Quand la fenêtre entre en mode plein écran, 
         * on met à jour la largeur et la hauteur de la fenêtre
         * on met à jour les FPS en fonction du moniteur actuel
         * et on indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_engine_update_fps_based_on_monitor();
        rc2d_engine_calculate_renderscale_and_gpuviewport();

        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowenterfullscreen != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_windowenterfullscreen();
        }
    }

    // Window Shown
    else if (event->type == SDL_EVENT_WINDOW_SHOWN)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowshown != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_windowshown();
        }
    }

    // Window leave fullscreen
    else if (event->type == SDL_EVENT_WINDOW_LEAVE_FULLSCREEN) 
    {
        /**
         * Quand la fenêtre quitte le mode plein écran, 
         * on met à jour la largeur et la hauteur de la fenêtre
         * on met à jour les FPS en fonction du moniteur actuel
         * et on indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_engine_update_fps_based_on_monitor();
        rc2d_engine_calculate_renderscale_and_gpuviewport();

        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowleavefullscreen != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_windowleavefullscreen();
        }
    }

    // Window pixel size changed
    else if (event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) 
    {
        /**
         * En cas de changement de taille de pixels de la fenêtre (ex: changement de DPI), 
         * On indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_engine_calculate_renderscale_and_gpuviewport();
        rc2d_engine_update_fps_based_on_monitor();
    }

    // Window display scale changed
    else if (event->type == SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED) 
    {
        /** 
         * En cas de changement d'échelle d'affichage de la fenêtre, 
         * On indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_engine_calculate_renderscale_and_gpuviewport();
        rc2d_engine_update_fps_based_on_monitor();
    }

    // Window Occluded
    else if (event->type == SDL_EVENT_WINDOW_OCCLUDED)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowoccluded != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_windowoccluded();
        }
    }

    // Window resized
    else if (event->type == SDL_EVENT_WINDOW_RESIZED) 
    {
        /** 
         * En cas de changement de la taille de la fenêtre,
         * on met à jour la largeur et la hauteur de la fenêtre 
         * et on indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_engine_calculate_renderscale_and_gpuviewport();
        rc2d_engine_update_fps_based_on_monitor();

        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowresized != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_windowresized(event->window.data1, event->window.data2);
        }
    }

    // Window moved
    else if (event->type == SDL_EVENT_WINDOW_MOVED) 
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowmoved != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_windowmoved(event->window.data1, event->window.data2);
        }
    }

    // Window display changed
    else if (event->type == SDL_EVENT_WINDOW_DISPLAY_CHANGED) 
    {
        /**
         * Quand la fenêtre change de moniteur,
         * on met à jour la largeur et la hauteur de la fenêtre
         * on met à jour les FPS en fonction du moniteur actuel
         * et on indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_engine_update_fps_based_on_monitor();
        rc2d_engine_calculate_renderscale_and_gpuviewport();

        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowdisplaychanged != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_windowdisplaychanged(event->window.data1);
        }
    }

    // Window exposed
    else if (event->type == SDL_EVENT_WINDOW_EXPOSED) 
    {
        /**
         * Quand la fenêtre est exposée (par exemple, après avoir été masquée ou minimisée),
         * on indique que le viewport du gpu et le render scale interne doit être recalculé.
         * 
         * Egalement si jamais entre temps on a changé de moniteur,
         * on met à jour les FPS en fonction du moniteur actuel.
         */
        rc2d_engine_update_fps_based_on_monitor();
        rc2d_engine_calculate_renderscale_and_gpuviewport();

        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowexposed != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_windowexposed();
        }
    }

    // Window minimized
    else if (event->type == SDL_EVENT_WINDOW_MINIMIZED) 
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowminimized != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_windowminimized();
        }
    }

    // Window maximized
    else if (event->type == SDL_EVENT_WINDOW_MAXIMIZED) 
    {
        /** 
         * En cas de changement de la taille de la fenêtre,
         * on met à jour la largeur et la hauteur de la fenêtre 
         * et on indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_engine_calculate_renderscale_and_gpuviewport();

        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowmaximized != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_windowmaximized();
        }
    }

    // Window restored
    else if (event->type == SDL_EVENT_WINDOW_RESTORED) 
    {
        /** 
         * La fenêtre a été restaurée après avoir été minimisée ou maximisée à son état normal.
         * on met à jour la largeur et la hauteur de la fenêtre 
         * et on indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_engine_calculate_renderscale_and_gpuviewport();

        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowrestored != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_windowrestored();
        }
    }

    // Mouse entered window
    else if (event->type == SDL_EVENT_WINDOW_MOUSE_ENTER) 
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowmouseenter != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_windowmouseenter();
        }
    }

    // Mouse leave window
    else if (event->type == SDL_EVENT_WINDOW_MOUSE_LEAVE) 
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowmouseleave != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_windowmouseleave();
        }
    }

    // Keyboard focus gained
    else if (event->type == SDL_EVENT_WINDOW_FOCUS_GAINED) 
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowkeyboardfocus != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_windowkeyboardfocus();
        }
    }

    // Keyboard focus lost
    else if (event->type == SDL_EVENT_WINDOW_FOCUS_LOST) 
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowkeyboardlost != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_windowkeyboardlost();
        }
    }

    // Window closed
    else if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) 
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_windowclosed != NULL)
        {
            rc2d_engine_state.config->callbacks->rc2d_windowclosed();
        }
        
        return SDL_APP_SUCCESS;
    }
    
    // Mouse Moved
    else if (event->type == SDL_EVENT_MOUSE_MOTION) 
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_mousemoved != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_mousemoved(
                event->motion.x,
                event->motion.y,
                event->motion.xrel,
                event->motion.yrel,
                event->motion.which
            );
        }
    }

    // Mouse Wheel
    else if (event->type == SDL_EVENT_MOUSE_WHEEL) 
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_mousewheelmoved != NULL) 
        {
            RC2D_MouseWheelDirection direction = RC2D_SCROLL_NONE;
            float x = event->wheel.x;
            float y = event->wheel.y;

            // Ajuster les valeurs en fonction de la direction (normal ou flipped)
            if (event->wheel.direction == SDL_MOUSEWHEEL_FLIPPED) 
            {
                x *= -1.0f;
                y *= -1.0f;
            }

            // Déterminer la direction principale
            if (y > 0.0f) 
            {
                direction = RC2D_SCROLL_UP;
            } 
            else if (y < 0.0f) 
            {
                direction = RC2D_SCROLL_DOWN;
            } 
            else if (x > 0.0f) 
            {
                direction = RC2D_SCROLL_RIGHT;
            } 
            else if (x < 0.0f) 
            {
                direction = RC2D_SCROLL_LEFT;
            }

            rc2d_engine_state.config->callbacks->rc2d_mousewheelmoved(
                direction,
                x,
                y,
                event->wheel.integer_x,
                event->wheel.integer_y,
                event->wheel.mouse_x,
                event->wheel.mouse_y,
                event->wheel.which
            );
        }
    }

    // Mouse Pressed
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) 
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_mousepressed != NULL) 
        {
            RC2D_MouseButton button = RC2D_MOUSE_UNKNOWN;

            switch (event->button.button) 
            {
                case SDL_BUTTON_LEFT:   button = RC2D_MOUSE_LEFT; break;
                case SDL_BUTTON_MIDDLE: button = RC2D_MOUSE_MIDDLE; break;
                case SDL_BUTTON_RIGHT:  button = RC2D_MOUSE_RIGHT; break;
                case SDL_BUTTON_X1:     button = RC2D_MOUSE_X1; break;
                case SDL_BUTTON_X2:     button = RC2D_MOUSE_X2; break;
                default:                button = RC2D_MOUSE_UNKNOWN; break;
            }

            rc2d_engine_state.config->callbacks->rc2d_mousepressed(
                event->button.x,
                event->button.y,
                button,
                event->button.clicks,
                event->button.which
            );
        }
    }

    // Mouse Released
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) 
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_mousereleased != NULL) 
        {
            RC2D_MouseButton button = RC2D_MOUSE_UNKNOWN;

            switch (event->button.button) 
            {
                case SDL_BUTTON_LEFT:   button = RC2D_MOUSE_LEFT; break;
                case SDL_BUTTON_MIDDLE: button = RC2D_MOUSE_MIDDLE; break;
                case SDL_BUTTON_RIGHT:  button = RC2D_MOUSE_RIGHT; break;
                case SDL_BUTTON_X1:     button = RC2D_MOUSE_X1; break;
                case SDL_BUTTON_X2:     button = RC2D_MOUSE_X2; break;
                default:                button = RC2D_MOUSE_UNKNOWN; break;
            }

            rc2d_engine_state.config->callbacks->rc2d_mousereleased(
                event->button.x,
                event->button.y,
                button,
                event->button.clicks,
                event->button.which
            );
        }
    }

    // Mouse Added
    else if (event->type == SDL_EVENT_MOUSE_ADDED) 
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_mouseadded != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_mouseadded(event->mdevice.which);
        }
    }

    // Mouse Removed
    else if (event->type == SDL_EVENT_MOUSE_REMOVED) 
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_mouseremoved != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_mouseremoved(event->mdevice.which);
        }
    }

    // Keyboard Pressed
    else if (event->type == SDL_EVENT_KEY_DOWN)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_keypressed != NULL) 
        {
            const char* key_name = SDL_GetKeyName(event->key.key);
            rc2d_engine_state.config->callbacks->rc2d_keypressed(
                key_name,
                event->key.scancode,
                event->key.key,
                event->key.mod,
                event->key.repeat,
                event->key.which
            );
        }
    }

    // Keyboard Released
    else if (event->type == SDL_EVENT_KEY_UP)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_keyreleased != NULL) 
        {
            const char* key_name = SDL_GetKeyName(event->key.key);
            rc2d_engine_state.config->callbacks->rc2d_keyreleased(
                key_name,
                event->key.scancode,
                event->key.key,
                event->key.mod,
                event->key.which
            );
        }
    }

    // Text Editing (IME)
    else if (event->type == SDL_EVENT_TEXT_EDITING)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_textediting != NULL) 
        {
            RC2D_TextEditingEventInfo info = {
                .text = event->edit.text,
                .start = event->edit.start,
                .length = event->edit.length,
                .windowID = event->edit.windowID
            };
            rc2d_engine_state.config->callbacks->rc2d_textediting(&info);
        }
    }

    // Text Editing Candidates (IME)
    else if (event->type == SDL_EVENT_TEXT_EDITING_CANDIDATES)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_texteditingcandidates != NULL) 
        {
            RC2D_TextEditingCandidatesEventInfo info = {
                .candidates = event->edit_candidates.candidates,
                .num_candidates = event->edit_candidates.num_candidates,
                .selected_candidate = event->edit_candidates.selected_candidate,
                .horizontal = event->edit_candidates.horizontal,
                .windowID = event->edit_candidates.windowID
            };
            rc2d_engine_state.config->callbacks->rc2d_texteditingcandidates(&info);
        }
    }

    // Text Input
    else if (event->type == SDL_EVENT_TEXT_INPUT)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_textinput != NULL) 
        {
            RC2D_TextInputEventInfo info = {
                .text = event->text.text,
                .windowID = event->text.windowID
            };
            rc2d_engine_state.config->callbacks->rc2d_textinput(&info);
        }
    }

    // Keymap Changed
    else if (event->type == SDL_EVENT_KEYMAP_CHANGED)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_keymapchanged != NULL) 
        {
            rc2d_engine_state.config->callbacks->rc2d_keymapchanged();
        }
    }

    // Keyboard Added
    else if (event->type == SDL_EVENT_KEYBOARD_ADDED)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_keyboardadded != NULL) 
        {
            RC2D_KeyboardDeviceEventInfo info = {
                .keyboardID = event->kdevice.which,
                .name = SDL_GetKeyboardNameForID(event->kdevice.which)
            };
            rc2d_engine_state.config->callbacks->rc2d_keyboardadded(&info);
        }
    }

    // Keyboard Removed
    else if (event->type == SDL_EVENT_KEYBOARD_REMOVED)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_keyboardremoved != NULL) 
        {
            RC2D_KeyboardDeviceEventInfo info = {
                .keyboardID = event->kdevice.which,
                .name = SDL_GetKeyboardNameForID(event->kdevice.which)
            };
            rc2d_engine_state.config->callbacks->rc2d_keyboardremoved(&info);
        }
    }

    // Sensor Update
    else if (event->type == SDL_EVENT_SENSOR_UPDATE)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_sensorupdate != NULL) 
        {
            RC2D_SensorEventInfo info = {
                .sensorID = event->sensor.which,
                .type = SDL_GetSensorType(event->sensor.which),
                .name = SDL_GetSensorNameForID(event->sensor.which),
                .timestamp = event->sensor.sensor_timestamp
            };
            // Copy sensor data (up to 6 values)
            for (int i = 0; i < 6; i++) 
            {
                info.data[i] = event->sensor.data[i];
            }
            rc2d_engine_state.config->callbacks->rc2d_sensorupdate(&info);
        }
    }

    // Drag-and-Drop Events
    else if (event->type == SDL_EVENT_DROP_BEGIN)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_dropbegin != NULL) 
        {
            RC2D_DropEventInfo info = {
                .windowID = event->drop.windowID,
                .x = 0.0f,
                .y = 0.0f,
                .source = event->drop.source,
                .data = NULL,
                .timestamp = event->drop.timestamp
            };
            rc2d_engine_state.config->callbacks->rc2d_dropbegin(&info);
        }
    }
    else if (event->type == SDL_EVENT_DROP_FILE)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_dropfile != NULL) 
        {
            RC2D_DropEventInfo info = {
                .windowID = event->drop.windowID,
                .x = event->drop.x,
                .y = event->drop.y,
                .source = event->drop.source,
                .data = event->drop.data,
                .timestamp = event->drop.timestamp
            };
            rc2d_engine_state.config->callbacks->rc2d_dropfile(&info);
        }
    }
    else if (event->type == SDL_EVENT_DROP_TEXT)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_droptext != NULL) 
        {
            RC2D_DropEventInfo info = {
                .windowID = event->drop.windowID,
                .x = event->drop.x,
                .y = event->drop.y,
                .source = event->drop.source,
                .data = event->drop.data,
                .timestamp = event->drop.timestamp
            };
            rc2d_engine_state.config->callbacks->rc2d_droptext(&info);
        }
    }
    else if (event->type == SDL_EVENT_DROP_COMPLETE)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_dropcomplete != NULL) 
        {
            RC2D_DropEventInfo info = {
                .windowID = event->drop.windowID,
                .x = 0.0f,
                .y = 0.0f,
                .source = event->drop.source,
                .data = NULL,
                .timestamp = event->drop.timestamp
            };
            rc2d_engine_state.config->callbacks->rc2d_dropcomplete(&info);
        }
    }
    else if (event->type == SDL_EVENT_DROP_POSITION)
    {
        if (rc2d_engine_state.config != NULL && 
            rc2d_engine_state.config->callbacks != NULL && 
            rc2d_engine_state.config->callbacks->rc2d_dropposition != NULL) 
        {
            RC2D_DropEventInfo info = {
                .windowID = event->drop.windowID,
                .x = event->drop.x,
                .y = event->drop.y,
                .source = event->drop.source,
                .data = NULL,
                .timestamp = event->drop.timestamp
            };
            rc2d_engine_state.config->callbacks->rc2d_dropposition(&info);
        }

        // System Theme Changed
        else if (event->type == SDL_EVENT_SYSTEM_THEME_CHANGED)
        {
            if (rc2d_engine_state.config != NULL && 
                rc2d_engine_state.config->callbacks != NULL && 
                rc2d_engine_state.config->callbacks->rc2d_systemthemechanged != NULL) 
            {
                SDL_SystemTheme theme = SDL_GetSystemTheme();
                rc2d_engine_state.config->callbacks->rc2d_systemthemechanged(theme);
            }
        }
    }

    /**
     * SDL_APP_CONTINUE : Cela indique que l'application 
     * doit continuer à traiter les événements.
     */
    return SDL_APP_CONTINUE;
}

/**
 * \brief Initialise le moteur RC2D.
 * 
 * Cette fonction initialise les bibliothèques nécessaires, crée la fenêtre et le GPU, 
 * et configure les paramètres de l'application.
 * 
 * \return true si l'engine a été initialisé avec succès, false sinon.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static bool rc2d_engine(void)
{
    /**
     * IMPORTANT:
     * 
     * Appel de SDL_GetPath() explicitement, la raison : 
     * 
     * SDL met en cache le résultat de cet appel en interne, mais le premier appel à cette fonction 
     * n'est pas nécessairement rapide, alors planifiez en conséquence.
     */
    SDL_GetBasePath();

    /**
     * Doit être appelé avant tout code pour initialiser les asserts 
     * et les utiliser dès le début de l'application.
     */
    rc2d_assert_init();
    
    /**
     * Set les informations de l'application.
     * Dois toujours etre fait avant d'initialiser SDL3
     */
    SDL_SetAppMetadata(rc2d_engine_state.config->appInfo->name, rc2d_engine_state.config->appInfo->version, rc2d_engine_state.config->appInfo->identifier);

    /**
     * Initialiser la librairie OpenSSL
     */
    if (!rc2d_engine_init_openssl())
    {
        return false;
    }

    /**
     * Initialiser la librairie SDL3_ttf
     */
    if (!rc2d_engine_init_sdlttf())
    {
        return false;
    }

    /**
     * Initialiser la librairie SDL3_mixer
     */
    if (!rc2d_engine_init_sdlmixer())
    {
        return false;
    }

	/**
     * Initialiser la librairie SDL3
     */
    if (!rc2d_engine_init_sdl())
    {
        return false;
    }

    /**
     * Initialiser la librairie SDL3_shadercross
     */
    if (!rc2d_engine_init_sdlshadercross())
    {
        return false;
    }

    /**
     * Vérifier si le GPU de l'utilisateur est supporté par l'API SDL3_GPU.
     * 
     * Cela permet de s'assurer que le GPU est compatible avec au 
     * moins un des backends supportés par SDL3_GPU.
     * 
     * Si le GPU n'est pas supporté, on ne peut pas continuer.
     */
    if (!rc2d_engine_supported_gpu_backends())
    {
        return false;
    }

    /**
     * Créer la fenêtre principale
     */
    if (!rc2d_engine_create_window())
    {
        return false;
    }

    /**
     * Initialiser et créer le dispositif GPU
     */
    if (!rc2d_engine_create_gpu())
    {
        return false;
    }

    /**
     * Calcul initial du viewport GPU et de l'échelle de rendu pour l'ensemble de l'application.
     * Cela permet de s'assurer que le rendu est effectué à la bonne échelle et dans la bonne zone de la fenêtre.
     */
    rc2d_engine_calculate_renderscale_and_gpuviewport();

    /**
     * Recupere les donnees du moniteur qui contient la fenetre window pour regarder 
     * le nombre de HZ du moniteur et lui set les FPS.
     * 
	 * Si les hz n'ont pas etait trouve, FPS par default : 60.
     */
    rc2d_engine_update_fps_based_on_monitor();

    /**
     * Une variable contrôlant les orientations autorisées sur iOS/Android.
     */
#if defined(RC2D_PLATFORM_IOS) || defined(RC2D_PLATFORM_ANDROID)
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
#endif

    /**
     * Initialiser certains modules internes de RC2D
     */
	//rc2d_keyboard_init();
    rc2d_timer_init();

    if (!rc2d_onnx_init())
    {
        return false;
    }

    if (!rc2d_gpu_initRectangle())
    {
        return false;
    }

    // vérifie le nombre de letterbox count
    RC2D_log(RC2D_LOG_DEBUG, "Letterbox count: %d\n", rc2d_engine_state.letterbox_count);

    // Log pour indiquer que tout le moteur a été initialisé avec succès
    RC2D_log(RC2D_LOG_INFO, "RC2D Engine initialized successfully.\n");

    // Retourne true pour indiquer que l'initialisation a réussi
	return true;
}

bool rc2d_engine_init(void)
{
	// Init GameEngine house
	if (!rc2d_engine())
    {
		return false;
    }

	return true;
}

void rc2d_engine_quit(void)
{
    // Attendre que le GPU soit inactif avant de libérer les ressources
    SDL_WaitForGPUIdle(rc2d_gpu_getDevice());

    /**
     * Détruire les ressources internes des modules de la lib RC2D.
     */
	//rc2d_filesystem_quit();
    //rc2d_touch_freeTouchState();
    rc2d_onnx_cleanup();

    // Lib OpenSSL Deinitialize
    rc2d_engine_cleanup_openssl();

    // Lib SDL3_ttf Deinitialize
    rc2d_engine_cleanup_sdlttf();

    // Lib SDL3_mixer Deinitialize
    rc2d_engine_cleanup_sdlmixer();

    // Lib SDL3_shadercross Deinitialize
    rc2d_engine_cleanup_sdlshadercross();
    
    /* Libérer les shaders graphiques (vertex/fragment) */
    if (rc2d_engine_state.gpu_graphics_shader_mutex) 
    {
        SDL_LockMutex(rc2d_engine_state.gpu_graphics_shader_mutex);
        for (int i = 0; i < rc2d_engine_state.gpu_graphics_shader_count; i++)
        {
            if (rc2d_engine_state.gpu_graphics_shaders_cache[i].filename) 
            {
                RC2D_safe_free(rc2d_engine_state.gpu_graphics_shaders_cache[i].filename);
                rc2d_engine_state.gpu_graphics_shaders_cache[i].filename = NULL;
            }
        }
        RC2D_safe_free(rc2d_engine_state.gpu_graphics_shaders_cache);
        rc2d_engine_state.gpu_graphics_shaders_cache = NULL;
        rc2d_engine_state.gpu_graphics_shader_count = 0;
        SDL_UnlockMutex(rc2d_engine_state.gpu_graphics_shader_mutex);
        SDL_DestroyMutex(rc2d_engine_state.gpu_graphics_shader_mutex);
        rc2d_engine_state.gpu_graphics_shader_mutex = NULL;
    }

    /* Libérer les shaders de calcul */
    if (rc2d_engine_state.gpu_compute_shader_mutex) 
    {
        SDL_LockMutex(rc2d_engine_state.gpu_compute_shader_mutex);
        for (int i = 0; i < rc2d_engine_state.gpu_compute_shader_count; i++) 
        {
            if (rc2d_engine_state.gpu_compute_shaders_cache[i].filename) 
            {
                RC2D_safe_free(rc2d_engine_state.gpu_compute_shaders_cache[i].filename);
                rc2d_engine_state.gpu_compute_shaders_cache[i].filename = NULL;
            }
        }
        RC2D_safe_free(rc2d_engine_state.gpu_compute_shaders_cache);
        rc2d_engine_state.gpu_compute_shaders_cache = NULL;
        rc2d_engine_state.gpu_compute_shader_count = 0;
        SDL_UnlockMutex(rc2d_engine_state.gpu_compute_shader_mutex);
        SDL_DestroyMutex(rc2d_engine_state.gpu_compute_shader_mutex);
        rc2d_engine_state.gpu_compute_shader_mutex = NULL;
    }

    /* Libérer les pipelines graphiques */
    if (rc2d_engine_state.gpu_graphics_pipeline_mutex) 
    {
        SDL_LockMutex(rc2d_engine_state.gpu_graphics_pipeline_mutex);
        for (int i = 0; i < rc2d_engine_state.gpu_graphics_pipeline_count; i++) 
        {
            if (rc2d_engine_state.gpu_graphics_pipelines_cache[i].vertex_shader_filename) 
            {
                RC2D_safe_free(rc2d_engine_state.gpu_graphics_pipelines_cache[i].vertex_shader_filename);
                rc2d_engine_state.gpu_graphics_pipelines_cache[i].vertex_shader_filename = NULL;
            }
            if (rc2d_engine_state.gpu_graphics_pipelines_cache[i].fragment_shader_filename) 
            {
                RC2D_safe_free(rc2d_engine_state.gpu_graphics_pipelines_cache[i].fragment_shader_filename);
                rc2d_engine_state.gpu_graphics_pipelines_cache[i].fragment_shader_filename = NULL;
            }
        }
        RC2D_safe_free(rc2d_engine_state.gpu_graphics_pipelines_cache);
        rc2d_engine_state.gpu_graphics_pipelines_cache = NULL;
        rc2d_engine_state.gpu_graphics_pipeline_count = 0;
        SDL_UnlockMutex(rc2d_engine_state.gpu_graphics_pipeline_mutex);
        SDL_DestroyMutex(rc2d_engine_state.gpu_graphics_pipeline_mutex);
        rc2d_engine_state.gpu_graphics_pipeline_mutex = NULL;
    }

    // Libérer les ressources GPU des rectangles de RC2D
    rc2d_gpu_releaseRectangle();

    // Nettoyer les textures de letterbox
    RC2D_safe_free(rc2d_engine_state.letterbox_uniform_texture);
    RC2D_safe_free(rc2d_engine_state.letterbox_top_texture);
    RC2D_safe_free(rc2d_engine_state.letterbox_bottom_texture);
    RC2D_safe_free(rc2d_engine_state.letterbox_left_texture);
    RC2D_safe_free(rc2d_engine_state.letterbox_right_texture);
    RC2D_safe_free(rc2d_engine_state.letterbox_background_texture);

    /* Annuler la revendication de la fenêtre */
    if (rc2d_engine_state.gpu_device && rc2d_engine_state.window) 
    {
        SDL_ReleaseWindowFromGPUDevice(rc2d_engine_state.gpu_device, rc2d_engine_state.window);
    }

    /* Détruire la fenêtre */
    if (rc2d_engine_state.window) 
    {
        SDL_DestroyWindow(rc2d_engine_state.window);
        rc2d_engine_state.window = NULL;
    }

    /* Détruire le périphérique GPU */
    if (rc2d_engine_state.gpu_device) 
    {
        SDL_DestroyGPUDevice(rc2d_engine_state.gpu_device);
        rc2d_engine_state.gpu_device = NULL;
    }

    // Cleanup SDL3
	rc2d_engine_cleanup_sdl();

    /**
     * Affiche un rapport des fuites mémoire détectées.
     * Cela est utile pour identifier les fuites de mémoire dans l'application.
     * 
     * Note : 
     * - Ce rapport est affiché uniquement si RC2D_MEMORY_DEBUG_ENABLED est défini à 1.
     * - Il est recommandé de l'utiliser uniquement en mode développement pour éviter les ralentissements en production.
     */
    rc2d_memory_report();
}

void rc2d_engine_configure(const RC2D_EngineConfig* config)
{
    /**
     * Cela permet de s'assurer que l'état du moteur est dans un 
     * état valide avant de le configurer.
     */
    rc2d_engine_stateInit();

    /**
     * Vérifie si l'état du moteur RC2D est valide.
     * Si l'état du moteur est NULL, alors on ne peut pas le configurer.
     */
    if (rc2d_engine_state.config == NULL)
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Engine state config is NULL. Cannot configure.\n");
        return;
    }

    /**
     * Vérifie si le pointeur de configuration du framework RC2D est valide.
     * 
     * Si le pointeur est NULL, alors on utilisera toutes les valeurs 
     * par défaut pour la configuration.
     */
    if (config == NULL)
    {
        RC2D_log(RC2D_LOG_WARN, "No RC2D_Config provided. Using default values.\n");
        return;
    }

    /**
     * Vérifie si la structure concernant les informations de l'application est valide.
     * 
     * Si les informations de l'application sont NULL, on peut continuer,
     * puisque les valeurs par défaut seront utilisées.
     */
    if (config->appInfo != NULL)
    {
        rc2d_engine_state.config->appInfo = config->appInfo;
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "No RC2D_AppInfo provided. Using default values.\n");
    }

    /**
     * Vérifie si la structure concernant les callbacks est valide.
     * 
     * On peut aussi choisir de ne pas les utiliser, mais dans ce cas,
     * on ne pourrait pas utiliser les callbacks de la librairie RC2D, 
     * comme dessiner, charger, etc.
     * 
     * Donc cela serait simplement une fenetre SDL3 noir sans rien d'autre.
     */
    if (config->callbacks != NULL)
    {
        rc2d_engine_state.config->callbacks = config->callbacks;
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "No RC2D_Callbacks provided. Some events may not be handled.\n");
    }

    /**
     * Vérifie si la propriété concernant l'enumération du nombre d'images en vol pour le GPU est valide.
     * 
     * Sinon on utilise la valeur par défaut de 2 images en vol (RC2D_GPU_FRAMES_BALANCED).
     */
    if (config->gpuFramesInFlight == RC2D_GPU_FRAMES_LOW_LATENCY ||
        config->gpuFramesInFlight == RC2D_GPU_FRAMES_BALANCED ||
        config->gpuFramesInFlight == RC2D_GPU_FRAMES_HIGH_THROUGHPUT)
    {
        rc2d_engine_state.config->gpuFramesInFlight = config->gpuFramesInFlight;
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "Invalid RC2D_GPUFramesInFlight value. Using default.\n");
        rc2d_engine_state.config->gpuFramesInFlight = RC2D_GPU_FRAMES_BALANCED;
    }

    /**
     * Vérifie si la propriété concernant la configuration avancée du GPU est valide.
     * 
     * Si la configuration avancée est valide, on l'utilise, sinon on utilise les valeurs par défaut.
     */
    if (config->gpuOptions != NULL)
    {
        rc2d_engine_state.config->gpuOptions = config->gpuOptions;
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "No RC2D_GPUAdvancedOptions provided. Using default GPU settings.\n");
    }

    /**
     * Vérifie si la propriété concernant la taille de la fenêtre en largeur de l'application est valide.
     * 
     * Si la taille de la fenêtre est valide (> 0), on l'utilise, sinon on utilise les valeurs par défaut.
     */
    if (config->windowWidth > 0)
    {
        rc2d_engine_state.config->windowWidth = config->windowWidth;
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "Invalid window size width provided. Using default values.\n");
    }

    /**
     * Vérifie si la propriété concernant la taille de la fenêtre en hauteur de l'application est valide.
     * 
     * Si la taille de la fenêtre est valide (> 0), on l'utilise, sinon on utilise les valeurs par défaut.
     */
    if (config->windowHeight > 0)
    {
        rc2d_engine_state.config->windowHeight = config->windowHeight;
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "Invalid window size height provided. Using default values.\n");
    }

    /**
     * Vérifie si la propriété concernant la taille logique en largeur de l'application est valide.
     * 
     * Si la taille logique est valide (> 0), on l'utilise, sinon on utilise les valeurs par défaut.
     */
    if (config->logicalWidth > 0)
    {
        rc2d_engine_state.config->logicalWidth = config->logicalWidth;
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "Invalid logical size width provided. Using default values.\n");
    }

    /**
     * Vérifie si la propriété concernant la taille logique en hauteur de l'application est valide.
     * 
     * Si la taille logique est valide (> 0), on l'utilise, sinon on utilise les valeurs par défaut.
     */
    if (config->logicalHeight > 0)
    {
        rc2d_engine_state.config->logicalHeight = config->logicalHeight;
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "Invalid logical size height provided. Using default values.\n");
    }
    
    /**
     * Vérifie si la propriété concernant le mode de présentation est valide.
     * 
     * Si le mode de présentation est valide, on l'utilise, sinon on utilise les valeurs par défaut.
     */
    if (config->logicalPresentationMode == RC2D_LOGICAL_PRESENTATION_INTEGER_SCALE ||
        config->logicalPresentationMode == RC2D_LOGICAL_PRESENTATION_LETTERBOX)
    {
        rc2d_engine_state.config->logicalPresentationMode = config->logicalPresentationMode;
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "Invalid presentation mode provided. Using default values.\n");
        rc2d_engine_state.config->logicalPresentationMode = RC2D_LOGICAL_PRESENTATION_LETTERBOX;
    }

    /**
     * Vérifie si la propriété concernant le mode de rendu des textures pour les letterbox est valide.
     * 
     * Si le mode de rendu est valide, on l'utilise, sinon on utilise les valeurs par défaut.
     */
    if (config->letterboxTextures != NULL) 
    {
        rc2d_engine_state.letterbox_textures = *config->letterboxTextures;
    } 
    else 
    {
        RC2D_log(RC2D_LOG_WARN, "No letterbox textures provided. Default black bars will be used.\n");
        rc2d_engine_state.letterbox_textures.mode = RC2D_LETTERBOX_NONE;
    }
}