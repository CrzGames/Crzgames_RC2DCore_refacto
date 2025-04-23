#include <RC2D/RC2D_internal.h>
#include <RC2D/RC2D_logger.h>
#include <RC2D/RC2D_assert.h>
#include <RC2D/RC2D_window.h>
#include <RC2D/RC2D_math.h>
#include <RC2D/RC2D_filesystem.h>
#include <RC2D/RC2D_touch.h>

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3_ttf/SDL_ttf.h>
//#include <SDL3_mixer/SDL_mixer.h>

RC2D_EngineState rc2d_engine_state = {0};

RC2D_EngineConfig* rc2d_engine_getDefaultConfig(void)
{
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
        .presentationMode = RC2D_PRESENTATION_CLASSIC,
        .letterboxTextures = NULL,
        .appInfo = &default_app_info,
        .gpuFramesInFlight = RC2D_GPU_FRAMES_BALANCED,
        .gpuOptions = &default_gpu_options
    };

    return &default_config;
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

    // État d'exécution de la boucle de jeu
    rc2d_engine_state.fps = 60;
    rc2d_engine_state.delta_time = 0.0;
    rc2d_engine_state.game_is_running = true;
    rc2d_engine_state.last_frame_time = 0;

    // Paramètres de rendu
    rc2d_engine_state.render_scale = 1.0f;
    rc2d_engine_state.letterbox_textures.mode = RC2D_LETTERBOX_NONE;
    // rc2d_engine_state.rc2d_letterbox_areas : est déjà zéro-initialisé
    rc2d_engine_state.letterbox_count = 0;
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
        RC2D_assert_release(false, RC2D_LOG_CRITICAL, "Échec de l'initialisation d'OpenSSL : %s", ERR_error_string(ERR_get_error(), NULL));
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
		RC2D_log(RC2D_LOG_CRITICAL, "Could not init SDL3_ttf : %s \n", SDL_GetError());
		return false;
    }
    else
    {
        RC2D_log(RC2D_LOG_INFO, "SDL3_ttf initialized successfully.\n");
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
    RC2D_log(RC2D_LOG_INFO, "SDL3_ttf cleaned up successfully.\n");
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
    RC2D_log(RC2D_LOG_INFO, "SDL3_mixer cleaned up successfully.\n");
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
            RC2D_log(RC2D_LOG_CRITICAL, "Failed to init SDL subsystem %s: %s\n", names[i], SDL_GetError());
        } 
        else {
            RC2D_log(RC2D_LOG_INFO, "Initialized SDL subsystem %s successfully.\n", names[i]);
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
    RC2D_log(RC2D_LOG_INFO, "SDL3 cleaned up successfully.\n");
}

/**
 * \brief Crée la fenêtre principale de l'application RC2D.
 * 
 * Cette fonction configure et crée la fenêtre SDL3 avec les propriétés spécifiées dans la configuration du moteur.
 * La fenêtre est initialement cachée pour éviter des artefacts visuels jusqu'à ce que le rendu GPU soit prêt.
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
        RC2D_log(RC2D_LOG_CRITICAL, "Error creating window: %s", SDL_GetError());
        return false;
    }
    else
    {
        RC2D_log(RC2D_LOG_INFO, "Window created successfully, is hidden temporarily.\n");
    }

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
    /**
     * Créer le GPU device pour le rendu graphique
     * Vérifie la compatibilité du GPU avec les propriétés spécifiées.
     */
    SDL_PropertiesID gpu_props = SDL_CreateProperties();
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
        case RC2D_GPU_DRIVER_DEFAULT:
        default:
            // Ne pas setter la propriété pour laisser SDL choisir automatiquement
            break;
    }

    // Vérification de la compatibilité du GPU avec les propriétés spécifiées.
    if (!SDL_GPUSupportsProperties(gpu_props)) {
        RC2D_log(RC2D_LOG_CRITICAL, "GPU not supported with current properties (1).");
        SDL_DestroyProperties(gpu_props);
        return false;
    }

    /**
     * Créer le GPU device temporaire pour vérifier plus loin les formats de shaders supportés
     */
    SDL_GPUDevice* gpu_device_tmp = SDL_CreateGPUDeviceWithProperties(gpu_props);
    if (!gpu_device_tmp) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Error create GPU device (1) : %s", SDL_GetError());
        SDL_DestroyProperties(gpu_props);
        return false;
    }

    /**
     * Détecter les formats supportés par le GPU
     * Activation de tous les formats de shaders compatibles (SPIR-V, DXIL, MSL, etc.)
     */
    SDL_GPUShaderFormat supported_formats = SDL_GetGPUShaderFormats(gpu_device_tmp);
    RC2D_log(RC2D_LOG_INFO, "Supported shader formats : ");
    if (supported_formats & SDL_GPU_SHADERFORMAT_PRIVATE)
    {
        SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_PRIVATE_BOOLEAN, true);
        RC2D_log(RC2D_LOG_INFO, "- PRIVATE(NDA)");
    }
    if (supported_formats & SDL_GPU_SHADERFORMAT_SPIRV)
    {
        SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_SPIRV_BOOLEAN, true);
        RC2D_log(RC2D_LOG_INFO, "- SPIR-V");
    }
    if (supported_formats & SDL_GPU_SHADERFORMAT_DXBC)
    {
        SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXBC_BOOLEAN, true);
        RC2D_log(RC2D_LOG_INFO, "- DXBC");
    }
    if (supported_formats & SDL_GPU_SHADERFORMAT_DXIL)
    {
        SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_DXIL_BOOLEAN, true);
        RC2D_log(RC2D_LOG_INFO, "- DXIL");
    }
    if (supported_formats & SDL_GPU_SHADERFORMAT_MSL)
    {
        SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_MSL_BOOLEAN, true);
        RC2D_log(RC2D_LOG_INFO, "- MSL");
    }
    if (supported_formats & SDL_GPU_SHADERFORMAT_METALLIB)
    {
        SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_SHADERS_METALLIB_BOOLEAN, true);
        RC2D_log(RC2D_LOG_INFO, "- METALLIB");
    }

    // Détruire le GPU device temporaire
    if (gpu_device_tmp != NULL) 
    {
        SDL_DestroyGPUDevice(gpu_device_tmp);
    }

    // Vérification de la compatibilité du GPU avec les nouvelles propriétés spécifiées.
    if (!SDL_GPUSupportsProperties(gpu_props)) {
        RC2D_log(RC2D_LOG_CRITICAL, "GPU not supported with current properties (2).");
        SDL_DestroyProperties(gpu_props);
        return false;
    }

    // Créer le GPU device avec les propriétés spécifiées
    rc2d_engine_state.gpu_device = SDL_CreateGPUDeviceWithProperties(gpu_props);
    SDL_DestroyProperties(gpu_props);
    if (!rc2d_engine_state.gpu_device) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Error create GPU device (2) : %s", SDL_GetError());
        return false;
    }
    else
    {
        RC2D_log(RC2D_LOG_INFO, "GPU device created successfully.\n");
    }

    // Associe la fenêtre au GPU
    SDL_ClaimWindowForGPUDevice(rc2d_engine_state.gpu_device, rc2d_engine_state.window);

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
    /**
     * On utilisera SDL_GPU_PRESENTMODE_VSYNC par défaut, puisque c'est le mode le plus sûr et le plus compatible.
     * Mais si le GPU supporte le mode SDL_GPU_PRESENTMODE_MAILBOX, on l'utilisera.
     */
    for (int i = 0; i < SDL_arraysize(present_modes); i++) 
    {
        if (SDL_WindowSupportsGPUPresentMode(rc2d_engine_state.gpu_device, rc2d_engine_state.window, present_modes[i])) 
        {
            rc2d_engine_state.gpu_present_mode = present_modes[i];
            break;
        }
    }

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
     * On utilisera SDL_GPU_SWAPCHAINCOMPOSITION_SDR par défaut, 
     * car il est toujours disponible sur tous les systèmes.
     * 
     * Mais si le GPU supporte le mode SDL_GPU_SWAPCHAINCOMPOSITION_HDR10_ST2084, 
     * ou meilleur choix qu'on peut faire, on l'utilisera.
     */
    for (int i = 0; i < SDL_arraysize(compositions); i++) 
    {
        if (SDL_WindowSupportsGPUSwapchainComposition(rc2d_engine_state.gpu_device, rc2d_engine_state.window, compositions[i])) 
        {
            rc2d_engine_state.gpu_swapchain_composition = compositions[i];
            break;
        }
    }

    // Set GPU Swapchain parameters
    if (!SDL_SetGPUSwapchainParameters(rc2d_engine_state.gpu_device, rc2d_engine_state.window, rc2d_engine_state.gpu_present_mode, rc2d_engine_state.gpu_swapchain_composition))
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Could not set swapchain parameters: %s", SDL_GetError());
        return false;
    }

    // Set GPU frames in flight
    if (!SDL_SetGPUAllowedFramesInFlight(rc2d_engine_state.gpu_device, (Uint32)rc2d_engine_state.config->gpuFramesInFlight)) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Failed to set GPU frames in flight: %s", SDL_GetError());
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
    if (rc2d_engine_state.config->presentationMode == RC2D_PRESENTATION_PIXELART) {
        // Calcul de mise à l’échelle entière
        int int_scale = SDL_min(effective_width / rc2d_engine_state.config->logicalWidth, effective_height / rc2d_engine_state.config->logicalHeight);

        // Si l’échelle est trop petite, on la fixe à 1 pour éviter les problèmes d’affichage
        if (int_scale < 1) int_scale = 1;
        scale = (float)int_scale;

        // Calcul de la taille du viewport
        viewport_width = rc2d_engine_state.config->logicalWidth * scale;
        viewport_height = rc2d_engine_state.config->logicalHeight * scale;
    }
    // --- Mode Classique ---
    else 
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
    SDL_GPUViewport rc2d_gpu_viewport = {0};
    rc2d_gpu_viewport.x = viewport_x;
    rc2d_gpu_viewport.y = viewport_y;
    rc2d_gpu_viewport.w = viewport_width;
    rc2d_gpu_viewport.h = viewport_height;
    rc2d_gpu_viewport.min_depth = 0.0f;
    rc2d_gpu_viewport.max_depth = 1.0f;
    // TODO: add SDL_SetGPUViewport()

    // Applique l’échelle de rendu interne
    rc2d_engine_state.render_scale = scale * display_scale;

    // Calcule les zones de letterbox si le viewport ne remplit pas la zone sûre
    if (viewport_width < safe_area.width || viewport_height < safe_area.height) 
    {
        // Barres verticales noir (gauche/droite)
        if (viewport_x > safe_area.x) 
        {
            // Barre gauche
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].x = safe_area.x;
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].y = safe_area.y;
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].width = viewport_x - safe_area.x;
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].height = safe_area.height;
            rc2d_engine_state.letterbox_count++;

            // Barre droite
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].x = viewport_x + viewport_width;
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].y = safe_area.y;
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].width = safe_area.x + safe_area.width - (viewport_x + viewport_width);
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].height = safe_area.height;
            rc2d_engine_state.letterbox_count++;
        }

        // Barres horizontales noir (haut/bas)
        if (viewport_y > safe_area.y) 
        {
            // Barre haute
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].x = safe_area.x;
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].y = safe_area.y;
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].width = safe_area.width;
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].height = viewport_y - safe_area.y;
            rc2d_engine_state.letterbox_count++;

            // Barre basse
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].x = safe_area.x;
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].y = viewport_y + viewport_height;
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].width = safe_area.width;
            rc2d_engine_state.letterbox_areas[rc2d_engine_state.letterbox_count].height = safe_area.y + safe_area.height - (viewport_y + viewport_height);
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
    const SDL_DisplayMode* currentDisplayMode = NULL;
    if (SDL_GetCurrentDisplayMode(displayID) == NULL) 
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
}

void rc2d_engine_deltatimeframerates_start(void)
{
    // Capture le temps au debut de la frame actuelle
    Uint64 now = SDL_GetPerformanceCounter();

    // Calcule le delta time depuis la derniere frame
    rc2d_engine_state.delta_time = (double)(now - rc2d_engine_state.last_frame_time) / (double)SDL_GetPerformanceFrequency();
    
    // Met a jour 'lastFrameTime' pour la prochaine utilisation
    rc2d_engine_state.last_frame_time = now;
}

void rc2d_engine_deltatimeframerates_end(void)
{
    if (rc2d_engine_state.gpu_present_mode == SDL_GPU_PRESENTMODE_IMMEDIATE)
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
	/**
     * Détruire les ressources internes des modules de la lib RC2D.
     */
	//rc2d_filesystem_quit();
    //rc2d_touch_freeTouchState();

    // Lib OpenSSL Deinitialize
    rc2d_engine_cleanup_openssl();

    // Lib SDL3_ttf Deinitialize
    rc2d_engine_cleanup_sdlttf();

    // Lib SDL3_mixer Deinitialize
    rc2d_engine_cleanup_sdlmixer();
    
    // Destroy GPU device
    if (rc2d_engine_state.gpu_device != NULL)
    {
        SDL_DestroyGPUDevice(rc2d_engine_state.gpu_device);
        rc2d_engine_state.gpu_device = NULL;
    }

    // Destroy window
    if (rc2d_engine_state.window != NULL)
    {
        SDL_DestroyWindow(rc2d_engine_state.window);
        rc2d_engine_state.window = NULL;
    }

    // TODO: Destroy RC2D Engine state (tout ce qui est dans rc2d_engine_state)

    // Cleanup SDL3
	rc2d_engine_cleanup_sdl();
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
    if (config->presentationMode == RC2D_PRESENTATION_PIXELART ||
        config->presentationMode == RC2D_PRESENTATION_CLASSIC)
    {
        rc2d_engine_state.config->presentationMode = config->presentationMode;
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "Invalid presentation mode provided. Using default values.\n");
        rc2d_engine_state.config->presentationMode = RC2D_PRESENTATION_CLASSIC;
    }

    /**
     * Vérifie si la propriété concernant le mode de rendu des textures pour les letterbox est valide.
     * 
     * Si le mode de rendu est valide, on l'utilise, sinon on utilise les valeurs par défaut.
     */
    if (config->letterboxTextures != NULL)
    {
        rc2d_engine_state.letterbox_textures = *(config->letterboxTextures);
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "No letterbox textures provided. Default black bars will be used.\n");
    }
}