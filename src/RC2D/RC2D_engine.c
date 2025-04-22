#include <RC2D/RC2D_internal.h>
#include <RC2D/RC2D_logger.h>
#include <RC2D/RC2D_assert.h>

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <SDL3_ttf/SDL_ttf.h>
//#include <SDL3_mixer/SDL_mixer.h>

SDL_GPUDevice* rc2d_gpu_device = NULL; 
SDL_GPUPresentMode rc2d_gpu_present_mode = SDL_GPU_PRESENTMODE_VSYNC;
SDL_GPUSwapchainComposition rc2d_gpu_swapchain_composition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;

RC2D_AppInfo rc2d_app_info = {0};
rc2d_app_info.name = "RC2D Engine";
rc2d_app_info.version = "1.0.0";
rc2d_app_info.identifier = "com.rc2d.app";

bool rc2d_game_is_running = true;
double rc2d_delta_time = 0.0;
double rc2d_fps = 60.0;
Uint64 rc2d_last_frame_time = 0;

SDL_Event rc2d_event;

float rc2d_render_scale = 1.0f;

RC2D_LetterboxTextures rc2d_letterbox_textures = {0};
rc2d_letterbox_textures.mode = RC2D_LETTERBOX_NONE;
SDL_Rect rc2d_letterbox_areas[4] = {0};
int rc2d_letterbox_count = 0;

SDL_Window* rc2d_window = NULL;

RC2D_Callbacks rc2d_callbacks_engine = {0};

/**
 * Utiliser en interne simplement lors de l'initialisation du framework RC2D.
 * 
 * Si l'utilisateur lors de rc2d_setup() ne spécifie pas de taille de fenêtre, 
 * on utilise la taille par défaut de 800x600.
 */
static int rc2d_window_width = 800;
static int rc2d_window_height = 600;

/**
 * Utiliser en interne simplement lors de l'initialisation du framework RC2D.
 * 
 * - Si l'utilisateur lors de rc2d_setup() ne spécifie pas de taille logique,
 * on utilise la taille par défaut de 1920x1080.
 * 
 * - Egalement utilisé pour calculer le ratio d'aspect de la fenêtre, pour la
 * fonction : rc2d_calculate_renderscale_and_gpuviewport()
 */
static int rc2d_logical_width = 1920;
static int rc2d_logical_height = 1080;

// Mode de présentation du rendu par défaut.
static RC2D_PresentationMode rc2d_presentation_mode = RC2D_PRESENTATION_CLASSIC;

/**
 * 
 */
static RC2D_GPUAdvancedOptions rc2d_gpu_advanced_options = {0};
rc2d_gpu_advanced_options.debugMode = true;
rc2d_gpu_advanced_options.verbose = true;
rc2d_gpu_advanced_options.preferLowPower = false;
rc2d_gpu_advanced_options.driver = RC2D_GPU_DRIVER_DEFAULT;
static RC2D_GPUFramesInFlight rc2d_gpu_frames_in_flight = RC2D_GPU_FRAMES_BALANCED;

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
static bool rc2d_init_openssl(void) 
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
static void rc2d_cleanup_openssl(void)
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
static bool rc2d_init_sdlttf(void) 
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
static void rc2d_cleanup_sdlttf(void)
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
static bool rc2d_init_sdlmixer(void) 
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
static void rc2d_cleanup_sdlmixer(void)
{
    // FIXEME : En attente de la mise en œuvre de SDL3_mixer
    /*Mix_CloseAudio();
    Mix_Quit();*/
    RC2D_log(RC2D_LOG_INFO, "SDL3_mixer cleaned up successfully.\n");
}

/**
 * \brief Initialise la bibliothèque SDL3.
 *
 * Cette fonction initialise la bibliothèque SDL3 pour le rendu graphique, audio et les événements.
 * Elle doit être appelée avant d'utiliser les fonctions de rendu ou d'événements.
 *
 * \return true si l'initialisation a réussi, false sinon.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static bool rc2d_init_sdl(void) 
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS | SDL_INIT_SENSOR | SDL_INIT_CAMERA) != 0)
	{
		RC2D_log(RC2D_LOG_CRITICAL, "Could not init SDL3 Error : %s \n", SDL_GetError());
		return false;
	}
    else
    {
        RC2D_log(RC2D_LOG_INFO, "SDL3 initialized successfully.\n");
        return true;
    }
}

/**
 * \brief Libère les ressources SDL3.
 *
 * Cette fonction libère les ressources allouées par SDL3.
 * Elle doit être appelée avant de quitter l'application pour éviter les fuites de mémoire.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
static void rc2d_cleanup_sdl(void)
{
    SDL_Quit();
    RC2D_log(RC2D_LOG_INFO, "SDL3 cleaned up successfully.\n");
}

/**
 * @brief Calcule le viewport GPU et les zones de letterbox/pillerbox selon le mode de présentation (PIXELART ou CLASSIC).
 *
 * Cette fonction s'assure que le jeu s'affiche correctement selon le ratio logique défini par le développeur,
 * que ce soit en mode PIXELART (mise à l’échelle entière) ou CLASSIC (mise à l’échelle fluide).
 * Elle applique aussi une lettrebox si besoin et renvoie les zones à remplir (bords noirs ou texture).
 */
static void rc2d_calculate_renderscale_and_gpuviewport(void) 
{
    // Récupère la taille réelle de la fenêtre (pixels visibles, indépendamment du DPI)
    int window_width, window_height;
    if (!SDL_GetWindowSize(rc2d_window, &window_width, &window_height)) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to get window size: %s", SDL_GetError());
        return;
    }

    // Récupère la zone sûre : certaines plateformes (TV, téléphones à encoche..etc) ont des zones à éviter
    SDL_Rect safe_area;
    if (!SDL_GetWindowSafeArea(rc2d_window, &safe_area)) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to get window safe area: %s", SDL_GetError());
        return;
    }

    // Gère le high DPI : pixel_density > 1.0 = écran Retina, etc.
    float pixel_density = SDL_GetWindowPixelDensity(rc2d_window);
    float display_scale = SDL_GetWindowDisplayScale(rc2d_window);

    // Calcule la taille réelle (en pixels) de la zone sûre
    int effective_width = (int)(safe_area.w * pixel_density);
    int effective_height = (int)(safe_area.h * pixel_density);

    // Initialisation des variables de viewport
    float viewport_x, viewport_y, viewport_width, viewport_height;
    float scale;

    // --- Mode Pixel Art ---
    if (rc2d_presentation_mode == RC2D_PRESENTATION_PIXELART) {
        // Calcul de mise à l’échelle entière
        int int_scale = SDL_min(effective_width / rc2d_logical_width, effective_height / rc2d_logical_height);

        // Si l’échelle est trop petite, on la fixe à 1 pour éviter les problèmes d’affichage
        if (int_scale < 1) int_scale = 1;

        // Calcul de la taille du viewport
        scale = (float)int_scale;
        viewport_width = rc2d_logical_width * scale;
        viewport_height = rc2d_logical_height * scale;

    // --- Mode Classique ---
    } 
    else 
    {
        float logical_aspect = (float)rc2d_logical_width / rc2d_logical_height;
        float window_aspect = (float)effective_width / effective_height;

        // On adapte la largeur ou la hauteur selon le ratio d’aspect (16:9, 4:3, etc.)
        if (window_aspect > logical_aspect) 
        {
            scale = (float)effective_height / rc2d_logical_height;
            viewport_width = rc2d_logical_width * scale;
            viewport_height = effective_height;
        } 
        else 
        {
            scale = (float)effective_width / rc2d_logical_width;
            viewport_width = effective_width;
            viewport_height = rc2d_logical_height * scale;
        }
    }

    // Convertit la taille du viewport de pixels physiques vers pixels logiques
    viewport_width /= pixel_density;
    viewport_height /= pixel_density;

    // Centre le viewport dans la zone sûre
    viewport_x = safe_area.x + (safe_area.w - viewport_width) / 2.0f;
    viewport_y = safe_area.y + (safe_area.h - viewport_height) / 2.0f;

    // Applique le viewport au GPU
    SDL_GPUViewport* rc2d_gpu_viewport = {0};
    rc2d_gpu_viewport->x = viewport_x;
    rc2d_gpu_viewport->y = viewport_y;
    rc2d_gpu_viewport->w = viewport_width;
    rc2d_gpu_viewport->h = viewport_height;
    rc2d_gpu_viewport->min_depth = 0.0f;
    rc2d_gpu_viewport->max_depth = 1.0f;
    // TODO: SDL_SetGPUViewport()

    // Applique l’échelle de rendu interne
    rc2d_render_scale = scale * display_scale;

    // Calcule les zones de letterbox si le viewport ne remplit pas la zone sûre
    if (viewport_width < safe_area.w || viewport_height < safe_area.h) 
    {
        // Barres verticales noir (gauche/droite)
        if (viewport_x > safe_area.x) 
        {
            // Barre gauche
            rc2d_letterbox_areas[rc2d_letterbox_count].x = safe_area.x;
            rc2d_letterbox_areas[rc2d_letterbox_count].y = safe_area.y;
            rc2d_letterbox_areas[rc2d_letterbox_count].w = viewport_x - safe_area.x;
            rc2d_letterbox_areas[rc2d_letterbox_count].h = safe_area.h;
            rc2d_letterbox_count++;

            // Barre droite
            rc2d_letterbox_areas[rc2d_letterbox_count].x = viewport_x + viewport_width;
            rc2d_letterbox_areas[rc2d_letterbox_count].y = safe_area.y;
            rc2d_letterbox_areas[rc2d_letterbox_count].w = safe_area.x + safe_area.w - (viewport_x + viewport_width);
            rc2d_letterbox_areas[rc2d_letterbox_count].h = safe_area.h;
            rc2d_letterbox_count++;
        }

        // Barres horizontales noir (haut/bas)
        if (viewport_y > safe_area.y) 
        {
            // Barre haute
            rc2d_letterbox_areas[rc2d_letterbox_count].x = safe_area.x;
            rc2d_letterbox_areas[rc2d_letterbox_count].y = safe_area.y;
            rc2d_letterbox_areas[rc2d_letterbox_count].w = safe_area.w;
            rc2d_letterbox_areas[rc2d_letterbox_count].h = viewport_y - safe_area.y;
            rc2d_letterbox_count++;

            // Barre basse
            rc2d_letterbox_areas[rc2d_letterbox_count].x = safe_area.x;
            rc2d_letterbox_areas[rc2d_letterbox_count].y = viewport_y + viewport_height;
            rc2d_letterbox_areas[rc2d_letterbox_count].w = safe_area.w;
            rc2d_letterbox_areas[rc2d_letterbox_count].h = safe_area.y + safe_area.h - (viewport_y + viewport_height);
            rc2d_letterbox_count++;
        }
    }
}

/**
 * @brief Met à jour le FPS en fonction du moniteur actuel.
 * 
 * Récupère le moniteur actuel associé à la fenêtre window et met à jour la variable rc2d_fps
 */
static void rc2d_update_fps_based_on_monitor(void) 
{
    // Récupére le moniteur associé a la fenetre.
    SDL_DisplayID displayID = SDL_GetDisplayForWindow(rc2d_window);
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
        rc2d_fps = (double)currentDisplayMode->refresh_rate_numerator / currentDisplayMode->refresh_rate_denominator;
    } 
    else if (currentDisplayMode->refresh_rate > 0.0f) 
    {
        rc2d_fps = (double)currentDisplayMode->refresh_rate;
    } 
    else 
    {
        rc2d_fps = 60.0; // fallback
    }
}

/**
 * @brief Démarre le calcul du delta time et des frame rates.
 * 
 * Capture le temps au début de la frame actuelle.
 */
void rc2d_deltatimeframerates_start(void)
{
    // Capture le temps au debut de la frame actuelle
    Uint64 now = SDL_GetPerformanceCounter();

    // Calcule le delta time depuis la derniere frame
    rc2d_delta_time = (double)(now - rc2d_last_frame_time) / (double)SDL_GetPerformanceFrequency();
    
    // Met a jour 'lastFrameTime' pour la prochaine utilisation
    rc2d_last_frame_time = now;
}

/**
 * @brief Termine le calcul du delta time et des frame rates.
 * 
 * Capture le temps à la fin de la frame actuelle et ajuste le délai pour atteindre le FPS cible.
 * Cette fonction est utilisée uniquement si le mode de présentation SDL_GPU_PRESENTMODE_IMMEDIATE de la swapchain GPU est utiliser.
 * Puisque SDL_GPU_PRESENTMODE_VSYNC et SDL_GPU_PRESENTMODE_MAILBOX gèrent déjà ce délai automatiquement.
 */
void rc2d_deltatimeframerates_end(void)
{
    if (rc2d_gpu_present_mode == SDL_GPU_PRESENTMODE_IMMEDIATE)
    {
        // Capture le temps a la fin de la frame actuelle
        Uint64 frameEnd = SDL_GetPerformanceCounter();

        // Calcule le temps de la frame actuelle en millisecondes
        double frameTimeMs = (double)(frameEnd - rc2d_last_frame_time) * 1000.0 / (double)SDL_GetPerformanceFrequency();

        // Attendre le temps necessaire pour atteindre le FPS cible
        double targetFrameMs = 1000.0 / rc2d_fps;
        if (frameTimeMs < targetFrameMs) 
        {
            Uint64 delayNs = (Uint64)((targetFrameMs - frameTimeMs) * 1e6);
            SDL_DelayPrecise(delayNs);
        }   
    }
}

/**
 * @brief Traite les événements de la boucle de jeu.
 * 
 * Gère les événements SDL, y compris les entrées utilisateur, les mouvements de fenêtre..etc
 */
SDL_AppResult rc2d_processevent(void) 
{
    // Quit program
    if (rc2d_event.type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS;
    }

    // La préférence de la langue local à changé
    else if (rc2d_event.type == SDL_EVENT_LOCALE_CHANGED)
    {
        if (rc2d_callbacks_engine.rc2d_localechanged != NULL) 
        {
            rc2d_callbacks_engine.rc2d_localechanged();
        }
    }

    else if (rc2d_event.type == SDL_EVENT_DISPLAY_ORIENTATION) 
    {
        if (rc2d_callbacks_engine.rc2d_displayorientation != NULL) 
        {
            rc2d_callbacks_engine.rc2d_displayorientation(rc2d_event.display.orientation);
        }
    }

    else if (rc2d_event.type == SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED) 
    {
        // Met à jour le viewport GPU et le render scale
        rc2d_calculate_renderscale_and_gpuviewport();
    }

    // Window HDR State changed
    else if (rc2d_event.type == SDL_EVENT_WINDOW_HDR_STATE_CHANGED)
    {
        // Re-vérifie le meilleur swapchain disponible
        SDL_GPUSwapchainComposition compositions[] = {
            SDL_GPU_SWAPCHAINCOMPOSITION_HDR10_ST2084,
            SDL_GPU_SWAPCHAINCOMPOSITION_HDR_EXTENDED_LINEAR,
            SDL_GPU_SWAPCHAINCOMPOSITION_SDR_LINEAR,
            SDL_GPU_SWAPCHAINCOMPOSITION_SDR
        };
        for (int i = 0; i < SDL_arraysize(compositions); i++) 
        {
            if (SDL_WindowSupportsGPUSwapchainComposition(rc2d_gpu_device, rc2d_window, compositions[i])) 
            {
                rc2d_gpu_swapchain_composition = compositions[i];
                break;
            }
        }

        // Re-set la composition (en gardant le mode de présentation actuel)
        if (!SDL_SetGPUSwapchainParameters(rc2d_gpu_device, rc2d_window, rc2d_gpu_swapchain_composition, rc2d_gpu_present_mode)) 
        {
            RC2D_log(RC2D_LOG_ERROR, "Failed to update swapchain on HDR state change: %s", SDL_GetError());
        }
    }

    // Touch moved
    else if (rc2d_event.type == SDL_EVENT_FINGER_MOTION) 
    {
        if (rc2d_callbacks_engine.rc2d_touchmoved != NULL) 
        {
            rc2d_callbacks_engine.rc2d_touchmoved(rc2d_event.tfinger.touchId, rc2d_event.tfinger.fingerId,
                                            rc2d_event.tfinger.x, rc2d_event.tfinger.y,
                                            rc2d_event.tfinger.dx, rc2d_event.tfinger.dy);
        }

        // Mettre à jour l'état des pressions tactiles
        rc2d_touch_updateState(rc2d_event.tfinger.fingerId, SDL_EVENT_FINGER_MOTION, rc2d_event.tfinger.pressure, rc2d_event.tfinger.x, rc2d_event.tfinger.y);
    }

    // Touch pressed
    else if (rc2d_event.type == SDL_EVENT_FINGER_DOWN) 
    {
        if (rc2d_callbacks_engine.rc2d_touchpressed != NULL) 
        {
            rc2d_callbacks_engine.rc2d_touchpressed(rc2d_event.tfinger.touchId, rc2d_event.tfinger.fingerId,
                                            rc2d_event.tfinger.x, rc2d_event.tfinger.y,
                                            rc2d_event.tfinger.pressure);
        }

        // Mettre à jour l'état des pressions tactiles
        rc2d_touch_updateState(rc2d_event.tfinger.fingerId, SDL_EVENT_FINGER_DOWN, rc2d_event.tfinger.pressure, rc2d_event.tfinger.x, rc2d_event.tfinger.y);
    }

    // Touch released
    else if (rc2d_event.type == SDL_EVENT_FINGER_UP) 
    {
        if (rc2d_callbacks_engine.rc2d_touchreleased != NULL) 
        {
            rc2d_callbacks_engine.rc2d_touchreleased(rc2d_event.tfinger.touchId, rc2d_event.tfinger.fingerId,
                                                rc2d_event.tfinger.x, rc2d_event.tfinger.y,
                                                rc2d_event.tfinger.pressure);
        }

        // Mettre à jour l'état des pressions tactiles
        rc2d_touch_updateState(rc2d_event.tfinger.fingerId, SDL_EVENT_FINGER_UP, 0.0f, 0.0f, 0.0f);
    }

    // Window safe area changed
    else if (rc2d_event.type == SDL_EVENT_WINDOW_SAFE_AREA_CHANGED) 
    {
        /**
         * Quand la zone de sécurité de la fenêtre change,
         * on indique que le viewport du gpu et render scale doit être recalculé.
         */
        rc2d_calculate_renderscale_and_gpuviewport();
    }

    // Window enter fullscreen
    else if (rc2d_event.type == SDL_EVENT_WINDOW_ENTER_FULLSCREEN) 
    {
        /**
         * Quand la fenêtre entre en mode plein écran, 
         * on met à jour la largeur et la hauteur de la fenêtre
         * on met à jour les FPS en fonction du moniteur actuel
         * et on indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_update_fps_based_on_monitor();
        rc2d_calculate_renderscale_and_gpuviewport();

        if (rc2d_callbacks_engine.rc2d_windowenterfullscreen != NULL) 
        {
            rc2d_callbacks_engine.rc2d_windowenterfullscreen();
        }
    }

    // Window leave fullscreen
    else if (rc2d_event.type == SDL_EVENT_WINDOW_LEAVE_FULLSCREEN) 
    {
        /**
         * Quand la fenêtre quitter le mode plein écran, 
         * on met à jour la largeur et la hauteur de la fenêtre
         * on met à jour les FPS en fonction du moniteur actuel
         * et on indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_update_fps_based_on_monitor();
        rc2d_calculate_renderscale_and_gpuviewport();

        if (rc2d_callbacks_engine.rc2d_windowleavefullscreen != NULL) 
        {
            rc2d_callbacks_engine.rc2d_windowleavefullscreen();
        }
    }

    // Window pixel size changed
    else if (rc2d_event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) 
    {
        /**
         * En cas de changement de tailel de pixels de la fenêtre (ex: changement de DPI), 
         * On indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_calculate_renderscale_and_gpuviewport();
    }

    // Window display scale changed
    else if (rc2d_event.type == SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED) 
    {
        /** 
         * En cas de changement d'échelle d'affichage de la fenêtre, 
         * On indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_calculate_renderscale_and_gpuviewport();
    }

    // Window resized
    else if (rc2d_event.window.event == SDL_EVENT_WINDOW_RESIZED) 
    {
        /** 
         * En cas de changement de la taille de la fenêtre,
         * on met à jour la largeur et la hauteur de la fenêtre 
         * et on indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_calculate_renderscale_and_gpuviewport();

        if (rc2d_callbacks_engine.rc2d_windowresized != NULL) 
        {
            rc2d_callbacks_engine.rc2d_windowresized(rc2d_event.window.data1, rc2d_event.window.data2);
        }
    }

    // Window moved
    else if (rc2d_event.window.event == SDL_EVENT_WINDOW_MOVED) 
    {
        if (rc2d_callbacks_engine.rc2d_windowmoved != NULL) 
        {
            rc2d_callbacks_engine.rc2d_windowmoved(rc2d_event.window.data1, rc2d_event.window.data2);

        }
    }

    // Window display changed
    else if (rc2d_event.window.event == SDL_EVENT_WINDOW_DISPLAY_CHANGED) 
    {
        /**
         * Quand la fenêtre change de moniteur,
         * on met à jour la largeur et la hauteur de la fenêtre
         * on met à jour les FPS en fonction du moniteur actuel
         * et on indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_update_fps_based_on_monitor();
        rc2d_calculate_renderscale_and_gpuviewport();

        if (rc2d_callbacks_engine.rc2d_windowdisplaychanged != NULL) 
        {
            rc2d_callbacks_engine.rc2d_windowdisplaychanged(rc2d_event.window.data1, rc2d_event.window.data2);
        }
    }

    // Window exposed
    else if (rc2d_event.window.event == SDL_EVENT_WINDOW_EXPOSED) 
    {
        if (rc2d_callbacks_engine.rc2d_windowexposed != NULL) 
        {
            rc2d_callbacks_engine.rc2d_windowexposed();
        }
    }

    // Window minimized
    else if (rc2d_event.window.event == SDL_EVENT_WINDOW_MINIMIZED) 
    {
        if (rc2d_callbacks_engine.rc2d_windowminimized != NULL) 
        {
            rc2d_callbacks_engine.rc2d_windowminimized();
        }
    }

    // Window maximized
    else if (rc2d_event.window.event == SDL_EVENT_WINDOW_MAXIMIZED) 
    {
        /** 
         * En cas de changement de la taille de la fenêtre,
         * on met à jour la largeur et la hauteur de la fenêtre 
         * et on indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_calculate_renderscale_and_gpuviewport();

        if (rc2d_callbacks_engine.rc2d_windowmaximized != NULL) 
        {
            rc2d_callbacks_engine.rc2d_windowmaximized();
        }
    }

    // Window restored
    else if (rc2d_event.window.event == SDL_EVENT_WINDOW_RESTORED) 
    {
        /** 
         * La fenêtre a été restaurée après avoir été minimisée ou maximisée à son état normal.
         * on met à jour la largeur et la hauteur de la fenêtre 
         * et on indique que le viewport du gpu et le render scale interne doit être recalculé.
         */
        rc2d_calculate_renderscale_and_gpuviewport();

        if (rc2d_callbacks_engine.rc2d_windowrestored != NULL) 
        {
            rc2d_callbacks_engine.rc2d_windowrestored();
        }
    }

    // Mouse entered window
    else if (rc2d_event.window.event == SDL_EVENT_WINDOW_MOUSE_ENTER) 
    {
        if (rc2d_callbacks_engine.rc2d_mouseenteredwindow != NULL) 
        {
            rc2d_callbacks_engine.rc2d_mouseenteredwindow();
        }
    }

    // Mouse leave window
    else if (rc2d_event.window.event == SDL_EVENT_WINDOW_MOUSE_LEAVE) 
    {
        if (rc2d_callbacks_engine.rc2d_mouseleavewindow != NULL) 
        {
            rc2d_callbacks_engine.rc2d_mouseleavewindow();
        }
    }

    // Keyboard focus gained
    else if (rc2d_event.window.event == SDL_EVENT_WINDOW_FOCUS_GAINED) 
    {
        if (rc2d_callbacks_engine.rc2d_keyboardfocusgained != NULL) 
        {
            rc2d_callbacks_engine.rc2d_keyboardfocusgained();
        }
    }

    // Keyboard focus lost
    else if (rc2d_event.window.event == SDL_EVENT_WINDOW_FOCUS_LOST) 
    {
        if (rc2d_callbacks_engine.rc2d_keyboardfocuslost != NULL) 
        {
            rc2d_callbacks_engine.rc2d_keyboardfocuslost();
        }
    }

    // Window closed
    else if (rc2d_event.window.event == SDL_EVENT_WINDOW_CLOSE_REQUESTED) 
    {
        if (rc2d_callbacks_engine.rc2d_windowclosed != NULL)
        {
            rc2d_callbacks_engine.rc2d_windowclosed();
        }
        
        return SDL_APP_SUCCESS;
    }
    
    // Mouse Moved
    else if (rc2d_event.type == SDL_EVENT_MOUSE_MOTION) 
    {
        if (rc2d_callbacks_engine.rc2d_mousemoved != NULL) 
        {
            rc2d_callbacks_engine.rc2d_mousemoved(rc2d_event.motion.x, rc2d_event.motion.y);
        }
    }

    // Mouse Wheel
    else if (rc2d_event.type == SDL_EVENT_MOUSE_WHEEL) 
    {
        if (rc2d_callbacks_engine.rc2d_wheelmoved != NULL) 
        {
            const char* stateScroll = "";

            if (rc2d_event.wheel.y > 0) stateScroll = "up"; // scroll up
            else if (rc2d_event.wheel.y < 0) stateScroll = "down"; // scroll down
            else if (rc2d_event.wheel.x > 0) stateScroll = "right"; // scroll right
            else if (rc2d_event.wheel.x < 0) stateScroll = "left"; // scroll left

            rc2d_callbacks_engine.rc2d_wheelmoved(stateScroll);
        }
    }

    // Mouse pressed
    else if (rc2d_event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) 
    {
        if (rc2d_callbacks_engine.rc2d_mousepressed != NULL) 
        {
            const char* button = "";

            if (rc2d_event.button.button == SDL_BUTTON_LEFT) button = "left"; // Click gauche
            else if (rc2d_event.button.button == SDL_BUTTON_MIDDLE) button = "middle"; // Click roulette
            else if (rc2d_event.button.button == SDL_BUTTON_RIGHT) button = "right"; // Click droit

            rc2d_callbacks_engine.rc2d_mousepressed(rc2d_event.button.x, rc2d_event.button.y, button, rc2d_event.button.clicks);
        }
    }

    // Mouse released
    else if (rc2d_event.type == SDL_EVENT_MOUSE_BUTTON_UP) 
    {
        if (rc2d_callbacks_engine.rc2d_mousereleased != NULL) 
        {
            const char* button = "";

            if (rc2d_event.button.button == SDL_BUTTON_LEFT) button = "left"; // Click gauche
            else if (rc2d_event.button.button == SDL_BUTTON_MIDDLE) button = "middle"; // Click roulette
            else if (rc2d_event.button.button == SDL_BUTTON_RIGHT) button = "right"; // Click droit

            rc2d_callbacks_engine.rc2d_mousereleased(rc2d_event.button.x, rc2d_event.button.y, button, rc2d_event.button.clicks);
        }
    }

    // Keyboard pressed
    else if (rc2d_event.type == SDL_EVENT_KEY_DOWN)
    {
        if (rc2d_callbacks_engine.rc2d_keypressed != NULL) 
        {
            const char* keyCurrent = SDL_GetKeyName(rc2d_event.key.keysym.sym);

            bool keyRepeat = rc2d_event.key.repeat != 0;

            rc2d_callbacks_engine.rc2d_keypressed(keyCurrent, keyRepeat);
        }
    }

    // Keyboard released
    else if (rc2d_event.type == SDL_EVENT_KEY_UP)
    {
        if (rc2d_callbacks_engine.rc2d_keyreleased != NULL) 
        {
            const char* keyCurrent = SDL_GetKeyName(rc2d_event.key.keysym.sym);
            rc2d_callbacks_engine.rc2d_keyreleased(keyCurrent);
        }
    }

    // Gamepad axis moved
    else if (rc2d_event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION) 
    {
        if (rc2d_callbacks_engine.rc2d_gamepadaxis != NULL) 
        {
            /*
            En normalisant les valeurs d'axe de cette manière, vous assurez que les callbacks reçoivent une valeur flottante entre -1.0 et 1.0, 
            ce qui est généralement plus utile pour la logique de jeu, car cela indique la direction et l'intensité de l'entrée de manière cohérente, 
            indépendamment de la plateforme ou du contrôleur spécifique.
            */
            float valueNormalized = rc2d_event.caxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
            rc2d_callbacks_engine.rc2d_gamepadaxis(rc2d_event.caxis.which, rc2d_event.caxis.axis, valueNormalized);
        }
    }

    // Gamepad Pressed
    else if (rc2d_event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) 
    {
        if (rc2d_callbacks_engine.rc2d_gamepadpressed != NULL) 
        {
            rc2d_callbacks_engine.rc2d_gamepadpressed(rc2d_event.cbutton.which, rc2d_event.cbutton.button);
        }
    }

    // Gamepad Released
    else if (rc2d_event.type == SDL_EVENT_GAMEPAD_BUTTON_UP) 
    {
        if (rc2d_callbacks_engine.rc2d_gamepadreleased != NULL) 
        {
            rc2d_callbacks_engine.rc2d_gamepadreleased(rc2d_event.cbutton.which, rc2d_event.cbutton.button);
        }
    }

    // Joystick axis moved
    else if (rc2d_event.type == SDL_EVENT_JOYSTICK_AXIS_MOTION) 
    {
        if (rc2d_callbacks_engine.rc2d_joystickaxis != NULL) 
        {
            /*
            En normalisant les valeurs d'axe de cette manière, vous assurez que les callbacks reçoivent une valeur flottante entre -1.0 et 1.0, 
            ce qui est généralement plus utile pour la logique de jeu, car cela indique la direction et l'intensité de l'entrée de manière cohérente, 
            indépendamment de la plateforme ou du contrôleur spécifique.
            */
            float valueNormalized = rc2d_event.jaxis.value / (float)SDL_JOYSTICK_AXIS_MAX;
            rc2d_callbacks_engine.rc2d_joystickaxis(rc2d_event.jaxis.which, rc2d_event.jaxis.axis, valueNormalized);
        }
    }

    // Joystick Pressed
    else if (rc2d_event.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN) 
    {
        if (rc2d_callbacks_engine.rc2d_joystickpressed != NULL)
        {
            rc2d_callbacks_engine.rc2d_joystickpressed(rc2d_event.jbutton.which, rc2d_event.jbutton.button);
        }
    }

    // Joystick Released
    else if (rc2d_event.type == SDL_EVENT_JOYSTICK_BUTTON_UP)
        {
        if (rc2d_callbacks_engine.rc2d_joystickreleased != NULL)
        {
            rc2d_callbacks_engine.rc2d_joystickreleased(rc2d_event.jbutton.which, rc2d_event.jbutton.button);
        }
    }

    // Joystick added
    else if (rc2d_event.type == SDL_EVENT_JOYSTICK_ADDED) 
    {
        if (rc2d_callbacks_engine.rc2d_joystickadded != NULL)
        {
            rc2d_callbacks_engine.rc2d_joystickadded(rc2d_event.jdevice.which);
        }
    }

    // Joystick remove
    else if (rc2d_event.type == SDL_EVENT_JOYSTICK_REMOVED) 
    {
        if (rc2d_callbacks_engine.rc2d_joystickremoved != NULL) 
        {
            rc2d_callbacks_engine.rc2d_joystickremoved(rc2d_event.jdevice.which);
        }
    }

    // Joystick hat moved
    else if (rc2d_event.type == SDL_EVENT_JOYSTICK_HAT_MOTION) 
    {
        if (rc2d_callbacks_engine.rc2d_joystickhat != NULL) 
        {
            rc2d_callbacks_engine.rc2d_joystickhat(rc2d_event.jhat.which, rc2d_event.jhat.hat, rc2d_event.jhat.value);
        }
    }

    // DROP FILE
    else if (rc2d_event.type == SDL_EVENT_DROP_FILE) 
    {
        if (rc2d_callbacks_engine.rc2d_dropfile != NULL)
        {
            char* filedir = rc2d_event.drop.file;
            rc2d_callbacks_engine.rc2d_dropfile(filedir);
            SDL_free(filedir);
        }
    }

    // SDL_DROPTEXT
    else if (rc2d_event.type == SDL_EVENT_DROP_TEXT) 
    {
        if (rc2d_callbacks_engine.rc2d_droptext != NULL) 
        {
            char* filedir = rc2d_event.drop.file;
            rc2d_callbacks_engine.rc2d_droptext(filedir);
            SDL_free(filedir);
        }
    }

    // SDL_DROPBEGIN
    else if (rc2d_event.type == SDL_EVENT_DROP_BEGIN) 
    {
        if (rc2d_callbacks_engine.rc2d_dropbegin != NULL) 
        {
            char* filedir = rc2d_event.drop.file;
            rc2d_callbacks_engine.rc2d_dropbegin(filedir);
            SDL_free(filedir);
        }
    }

    // SDL_DROPCOMPLETE
    else if (rc2d_event.type == SDL_EVENT_DROP_COMPLETE) 
    {
        if (rc2d_callbacks_engine.rc2d_dropcomplete != NULL) 
        {
            char* filedir = rc2d_event.drop.file;
            rc2d_callbacks_engine.rc2d_dropcomplete(filedir);
            SDL_free(filedir);
        }
    }

    /**
     * SDL_APP_CONTINUE : Cela indique que l'application 
     * doit continuer à traiter les événements.
     */
    return SDL_APP_CONTINUE;
}

/**
 * @brief Définit les callbacks de la librairie RC2D.
 * 
 * @param callbacks Pointeur vers la structure RC2D_Callbacks contenant les callbacks à définir.
 */
static void rc2d_set_callbacks(RC2D_Callbacks* callbacks) 
{
    // Game loop callbacks
    if (callbacks->rc2d_unload != NULL) rc2d_callbacks_engine.rc2d_unload = callbacks->rc2d_unload;
    if (callbacks->rc2d_load != NULL) rc2d_callbacks_engine.rc2d_load = callbacks->rc2d_load;
    if (callbacks->rc2d_draw != NULL) rc2d_callbacks_engine.rc2d_draw = callbacks->rc2d_draw;
    if (callbacks->rc2d_update != NULL) rc2d_callbacks_engine.rc2d_update = callbacks->rc2d_update;

    // Keyboard callbacks
    if (callbacks->rc2d_keypressed != NULL) rc2d_callbacks_engine.rc2d_keypressed = callbacks->rc2d_keypressed;
    if (callbacks->rc2d_keyreleased != NULL) rc2d_callbacks_engine.rc2d_keyreleased = callbacks->rc2d_keyreleased;

    // Mouse callbacks
    if (callbacks->rc2d_mousemoved != NULL) rc2d_callbacks_engine.rc2d_mousemoved = callbacks->rc2d_mousemoved;
    if (callbacks->rc2d_mousepressed != NULL) rc2d_callbacks_engine.rc2d_mousepressed = callbacks->rc2d_mousepressed;
    if (callbacks->rc2d_mousereleased != NULL) rc2d_callbacks_engine.rc2d_mousereleased = callbacks->rc2d_mousereleased;
    if (callbacks->rc2d_wheelmoved != NULL) rc2d_callbacks_engine.rc2d_wheelmoved = callbacks->rc2d_wheelmoved;

    // Touch callbacks
    if (callbacks->rc2d_touchmoved != NULL) rc2d_callbacks_engine.rc2d_touchmoved = callbacks->rc2d_touchmoved;
    if (callbacks->rc2d_touchpressed != NULL) rc2d_callbacks_engine.rc2d_touchpressed = callbacks->rc2d_touchpressed;
    if (callbacks->rc2d_touchreleased != NULL) rc2d_callbacks_engine.rc2d_touchreleased = callbacks->rc2d_touchreleased;

    // Joystick/Gamepad callbacks
    if (callbacks->rc2d_gamepadpressed != NULL) rc2d_callbacks_engine.rc2d_gamepadpressed = callbacks->rc2d_gamepadpressed;
    if (callbacks->rc2d_gamepadreleased != NULL) rc2d_callbacks_engine.rc2d_gamepadreleased = callbacks->rc2d_gamepadreleased;
    if (callbacks->rc2d_joystickpressed != NULL) rc2d_callbacks_engine.rc2d_joystickpressed = callbacks->rc2d_joystickpressed;
    if (callbacks->rc2d_joystickreleased != NULL) rc2d_callbacks_engine.rc2d_joystickreleased = callbacks->rc2d_joystickreleased;
    if (callbacks->rc2d_joystickadded != NULL) rc2d_callbacks_engine.rc2d_joystickadded = callbacks->rc2d_joystickadded;
    if (callbacks->rc2d_joystickremoved != NULL) rc2d_callbacks_engine.rc2d_joystickremoved = callbacks->rc2d_joystickremoved;
    if (callbacks->rc2d_joystickaxis != NULL) rc2d_callbacks_engine.rc2d_joystickaxis = callbacks->rc2d_joystickaxis;
    if (callbacks->rc2d_joystickhat != NULL) rc2d_callbacks_engine.rc2d_joystickhat = callbacks->rc2d_joystickhat;
    if (callbacks->rc2d_gamepadaxis != NULL) rc2d_callbacks_engine.rc2d_gamepadaxis = callbacks->rc2d_gamepadaxis;

    // File drop callbacks
    if (callbacks->rc2d_dropfile != NULL) rc2d_callbacks_engine.rc2d_dropfile = callbacks->rc2d_dropfile;
    if (callbacks->rc2d_droptext != NULL) rc2d_callbacks_engine.rc2d_droptext = callbacks->rc2d_droptext;
    if (callbacks->rc2d_dropbegin != NULL) rc2d_callbacks_engine.rc2d_dropbegin = callbacks->rc2d_dropbegin;
    if (callbacks->rc2d_dropcomplete != NULL) rc2d_callbacks_engine.rc2d_dropcomplete = callbacks->rc2d_dropcomplete;

    // Window callbacks
    if (callbacks->rc2d_windowexposed != NULL) rc2d_callbacks_engine.rc2d_windowexposed = callbacks->rc2d_windowexposed;
    if (callbacks->rc2d_windowmoved != NULL) rc2d_callbacks_engine.rc2d_windowmoved = callbacks->rc2d_windowmoved;
    if (callbacks->rc2d_windowsizedchanged != NULL) rc2d_callbacks_engine.rc2d_windowsizedchanged = callbacks->rc2d_windowsizedchanged;
    if (callbacks->rc2d_windowminimized != NULL) rc2d_callbacks_engine.rc2d_windowminimized = callbacks->rc2d_windowminimized;
    if (callbacks->rc2d_windowmaximized != NULL) rc2d_callbacks_engine.rc2d_windowmaximized = callbacks->rc2d_windowmaximized;
    if (callbacks->rc2d_windowrestored != NULL) rc2d_callbacks_engine.rc2d_windowrestored = callbacks->rc2d_windowrestored;
    if (callbacks->rc2d_mouseenteredwindow != NULL) rc2d_callbacks_engine.rc2d_mouseenteredwindow = callbacks->rc2d_mouseenteredwindow;
    if (callbacks->rc2d_mouseleavewindow != NULL) rc2d_callbacks_engine.rc2d_mouseleavewindow = callbacks->rc2d_mouseleavewindow;
    if (callbacks->rc2d_keyboardfocusgained != NULL) rc2d_callbacks_engine.rc2d_keyboardfocusgained = callbacks->rc2d_keyboardfocusgained;
    if (callbacks->rc2d_keyboardfocuslost != NULL) rc2d_callbacks_engine.rc2d_keyboardfocuslost = callbacks->rc2d_keyboardfocuslost;
    if (callbacks->rc2d_windowclosed != NULL) rc2d_callbacks_engine.rc2d_windowclosed = callbacks->rc2d_windowclosed;
    if (callbacks->rc2d_windowtakefocus != NULL) rc2d_callbacks_engine.rc2d_windowtakefocus = callbacks->rc2d_windowtakefocus;
    if (callbacks->rc2d_windowhittest != NULL) rc2d_callbacks_engine.rc2d_windowhittest = callbacks->rc2d_windowhittest;
    if (callbacks->rc2d_windowresized != NULL) rc2d_callbacks_engine.rc2d_windowresized = callbacks->rc2d_windowresized;
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
    RC2D_InitAssert();

    /**
     * Set les informations de l'application
     * Dois toujours etre fait avant d'initialiser SDL3
     */
    SDL_SetAppMetadata(rc2d_app_info.name, rc2d_app_info.version, rc2d_app_info.identifier);

    /**
     * Initialiser la librairie OpenSSL
     */
    if (!rc2d_init_openssl())
    {
        return false;
    }

    /**
     * Initialiser la librairie SDL3_ttf
     */
    if (!rc2d_init_sdlttf())
    {
        return false;
    }

    /**
     * Initialiser la librairie SDL3_mixer
     */
    if (!rc2d_init_sdlmixer())
    {
        return false;
    }

	/**
     * Initialiser la librairie SDL3
     */
    if (!rc2d_init_sdl())
    {
        return false;
    }

	/**
     * SDL3
     * Créer la fenêtre principale de l'application
     */
	SDL_PropertiesID window_props = SDL_CreateProperties();
    SDL_SetStringProperty(window_props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, rc2d_app_info.name);
    SDL_SetNumberProperty(window_props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, rc2d_window_width);
    SDL_SetNumberProperty(window_props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, rc2d_window_height);
    SDL_SetBooleanProperty(window_props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
    SDL_SetBooleanProperty(window_props, SDL_PROP_WINDOW_CREATE_HIGH_PIXEL_DENSITY_BOOLEAN, true);
    SDL_SetNumberProperty(window_props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
    SDL_SetNumberProperty(window_props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);

    /**
     * IMPORTANT :
     * Cache la fenêtre tant que le rendu GPU n'est pas prêt..etc pour éviter des artefacts visuels.
     * On l'affichera plus tard juste avant la première frame de la boucle de jeu.
     */
    SDL_SetBooleanProperty(window_props, SDL_PROP_WINDOW_CREATE_HIDDEN_BOOLEAN, true);

    rc2d_window = SDL_CreateWindowWithProperties(window_props);
    SDL_DestroyProperties(window_props);
    if (!rc2d_window) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Error create window : %s", SDL_GetError());
        return false;
    }
    else
    {
        RC2D_log(RC2D_LOG_INFO, "Window created successfully, is hidden temporarily.\n");
    }

    /**
     * SDL3
     * Créer le GPU device pour le rendu graphique
     * Vérifie la compatibilité du GPU avec les propriétés spécifiées.
     */
    SDL_PropertiesID gpu_props = SDL_CreateProperties();
    SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_DEBUGMODE_BOOLEAN, rc2d_gpu_advanced_options.debugMode);
    SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_VERBOSE_BOOLEAN, rc2d_gpu_advanced_options.verbose);
    SDL_SetBooleanProperty(gpu_props, SDL_PROP_GPU_DEVICE_CREATE_PREFERLOWPOWER_BOOLEAN, rc2d_gpu_advanced_options.preferLowPower);
    
    // Pilote GPU forcé si nécessaire
    switch (rc2d_gpu_advanced_options.driver) 
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
        RC2D_log(RC2D_LOG_CRITICAL, "Error create GPU device : %s", SDL_GetError());
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
    rc2d_gpu_device = SDL_CreateGPUDeviceWithProperties(gpu_props);
    SDL_DestroyProperties(gpu_props);
    if (!rc2d_gpu_device) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Error create GPU device : %s", SDL_GetError());
        return false;
    }
    else
    {
        RC2D_log(RC2D_LOG_INFO, "GPU device created successfully.\n");
    }

    // Associe la fenêtre au GPU
    SDL_ClaimWindowForGPUDevice(rc2d_gpu_device, rc2d_window);

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
        if (SDL_WindowSupportsGPUPresentMode(rc2d_gpu_device, rc2d_window, present_modes[i])) 
        {
            rc2d_gpu_present_mode = present_modes[i];
            break;
        }
    }

    // SDL3 : Configurer le swapchain pour le GPU
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
        if (SDL_WindowSupportsGPUSwapchainComposition(rc2d_gpu_device, rc2d_window, compositions[i])) 
        {
            rc2d_gpu_swapchain_composition = compositions[i];
            break;
        }
    }

    // SDL3 : Set GPU Swapchain parameters
    if (!SDL_SetGPUSwapchainParameters(rc2d_gpu_device, rc2d_window, rc2d_gpu_swapchain_composition, rc2d_gpu_present_mode)) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Could not set swapchain parameters: %s", SDL_GetError());
    }

    // SDL3: Set GPU frames in flight
    if (!SDL_SetGPUAllowedFramesInFlight(rc2d_gpu_device, (Uint32)rc2d_gpu_frames_in_flight)) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to set GPU frames in flight: %s", SDL_GetError());
    }

    /**
     * Calcul initial du viewport GPU et de l'échelle de rendu pour l'ensemble de l'application.
     * Cela permet de s'assurer que le rendu est effectué à la bonne échelle et dans la bonne zone de la fenêtre.
     */
    rc2d_calculate_renderscale_and_gpuviewport();

    /**
     * Recupere les donnees du moniteur qui contient la fenetre window pour regarder 
     * le nombre de HZ du moniteur et lui set les FPS.
     * 
	 * Si les hz n'ont pas etait trouve, FPS par default : 60.
     */
    rc2d_update_fps_based_on_monitor();

    /**
     * Une variable contrôlant les orientations autorisées sur iOS/Android.
     */
#ifdef RC2D_PLATFORM_IOS || RC2D_PLATFORM_ANDROID
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
#endif

    /**
     * Initialiser certains modules internes de RC2D
     */
	//rc2d_keyboard_init();
    rc2d_timer_init();

	return true;
}

bool rc2d_init(void)
{
	// Init GameEngine house
	if (!rc2d_engine())
    {
		return false;
    }

	return true;
}

void rc2d_quit(void)
{
	/**
     * Détruire les ressources internes de RC2D
     */
	rc2d_filesystem_quit();
    rc2d_touch_freeTouchState();

    // Lib OpenSSL Deinitialize
    rc2d_cleanup_openssl();

    // Lib SDL3_ttf Deinitialize
    rc2d_cleanup_sdlttf();

    // Lib SDL3_mixer Deinitialize
    rc2d_cleanup_sdlmixer();

    // TODO: Libérer : rc2d_letterbox_textures
    
    // Destroy GPU device
    if (rc2d_gpu_device != NULL)
    {
        SDL_DestroyGPUDevice(rc2d_gpu_device);
        rc2d_gpu_device = NULL;
    }

    // Destroy window
    if (rc2d_window != NULL)
    {
        SDL_DestroyWindow(rc2d_window);
        rc2d_window = NULL;
    }

    // Cleanup SDL3
	rc2d_cleanup_sdl();
}

bool rc2d_configure(const RC2D_Config* config)
{
    if (config == NULL)
    {
        RC2D_log(RC2D_LOG_ERROR, "RC2D_Config pointer is NULL.");
        return false;
    }

    // Set les informations de l'application
    if (config->appInfo != NULL)
    {
        rc2d_app_info = *(config->appInfo);
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "No RC2D_AppInfo provided. Using default values.");
    }

    // Set les callbacks pour la librairie RC2D
    if (config->callbacks != NULL)
    {
        rc2d_set_callbacks(config->callbacks);
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "No RC2D_Callbacks provided. Some events may not be handled.");
    }

    // Set frame in flight
    if (config->gpuFramesInFlight != RC2D_GPU_FRAMES_LOW_LATENCY &&
        config->gpuFramesInFlight != RC2D_GPU_FRAMES_BALANCED &&
        config->gpuFramesInFlight != RC2D_GPU_FRAMES_HIGH_THROUGHPUT)
    {
        rc2d_gpu_frames_in_flight = config->gpuFramesInFlight;
    }
    else
    {
        RC2D_log(RC2D_LOG_ERROR, "Invalid RC2D_GPUFramesInFlight value.");
        return false;
    }

    // Set GPU advanced options
    if (config->gpuOptions != NULL)
    {
        rc2d_gpu_advanced_options = *(config->gpuOptions);
    }
    else
    {
        RC2D_log(RC2D_LOG_INFO, "No RC2D_GPUAdvancedOptions provided. Using default GPU settings.");
    }

    // Set window size
    if (config->windowWidth > 0)
    {
        rc2d_window_width = config->windowWidth;
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "Invalid window size width provided. Using default values.");
    }

    if (config->windowHeight > 0)
    {
        rc2d_window_height = config->windowHeight;
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "Invalid window size height provided. Using default values.");
    }

    // Set logical size
    if (config->logicalWidth > 0)
    {
        rc2d_logical_width = config->logicalWidth;
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "Invalid logical size width provided. Using default values.");
    }

    if (config->logicalHeight > 0)
    {
        rc2d_logical_height = config->logicalHeight;
    }
    else
    {
        RC2D_log(RC2D_LOG_WARN, "Invalid logical size height provided. Using default values.");
    }
    
    // Set presentation mode : RC2D_PRESENTATION_PIXELART ou RC2D_PRESENTATION_CLASSIC
    if (config->presentationMode != RC2D_PRESENTATION_PIXELART &&
        config->presentationMode != RC2D_PRESENTATION_CLASSIC)
    {
        RC2D_log(RC2D_LOG_ERROR, "Invalid RC2D_PresentationMode: %d", config->presentationMode);
        return false;
    }
    rc2d_presentation_mode = config->presentationMode;

    // Set letterbox texture
    if (config->letterboxTextures != NULL)
    {
        rc2d_letterbox_textures = *(config->letterboxTextures);
    }
    else
    {
        RC2D_log(RC2D_LOG_INFO, "No letterbox textures provided. Default black bars will be used.");
    }

    return true;
}