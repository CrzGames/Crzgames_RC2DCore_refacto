#ifndef RC2D_INTERNAL_H
#define RC2D_INTERNAL_H

#include <RC2D/RC2D_engine.h>
#include <RC2D/RC2D_math.h>
#include <RC2D/RC2D_gpu.h>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_mutex.h>

/**
 * IMPORTANT: 
 * Module interne de la bibliothèque RC2D.
 * Pour ne pas exposer certaines fonctions, struct, enums et variables à l'utilisateur final.
 */

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Structure représentant une entrée de shader de calcul dans le moteur RC2D.
 *
 * Cette structure est utilisée pour gérer les shaders de calcul chargés par le moteur,
 * y compris leur nom de fichier, le pointeur vers le shader chargé et le timestamp
 * de la dernière modification du fichier.
 * 
 * \note Cela est utilisé pour le rechargement à chaud des shaders via RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
 * si défini à 1.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_ComputeShaderEntry {
    /**
     * Nom du fichier du shader (e.g., "test.compute").
     */
    char* filename;
    
    /**
     * Pointeur vers le shader de calcul chargé.
     */
    RC2D_GPUComputePipeline* shader;

    /**
     * Timestamp de la dernière modification du fichier shader.
     */
    SDL_Time lastModified;
} RC2D_ComputeShaderEntry;

/**
 * \brief Structure représentant une entrée de shader dans le moteur RC2D.
 *
 * Cette structure est utilisée pour gérer les shaders chargés par le moteur,
 * y compris leur nom de fichier, le pointeur vers le shader chargé et le timestamp
 * de la dernière modification du fichier.
 * 
 * \note Cela est utilisé pour le rechargement à chaud des shaders via RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
 * si défini à 1.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_GraphicsShaderEntry {
    /**
     * Nom du fichier du shader (e.g., "test.vertex" ou "test.fragment").
     * 
     * IMPORTANT:
     * \note Le pointeur doit être libéré en interne dans RC2D.
     */
    const char* filename;
    
    /**
     * Pointeur vers le shader graphique chargé.
     * 
     * IMPORTANT:
     * \note Ce pointeur pointe vers le shader graphique de l'utilisateur,
     * il ne doit pas être libéré manuellement en interne, mais par l'utilisateur
     * lorsqu'il n'est plus nécessaire, via : SDL_ReleaseGPUShader().
     */
    RC2D_GPUShader* shader;

    /**
     * Timestamp de la dernière modification du fichier shader.
     */
    SDL_Time lastModified;
} RC2D_GraphicsShaderEntry;

/**
 * \brief Structure représentant une entrée de pipeline graphique dans le moteur RC2D.
 *
 * Cette structure est utilisée pour gérer les pipelines graphiques chargés par le moteur,
 * y compris les nom de fichiers des shaders associés et le pointeur vers le pipeline graphique.
 *
 * \note Cela est utilisé pour le rechargement à chaud des shaders via RC2D_GPU_SHADER_HOT_RELOAD_ENABLED
 * si défini à 1.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_GraphicsPipelineEntry {
    /**
     * Nom du fichier du shader de vertex (e.g., "test.vertex").
     * 
     * IMPORTANT:
     * \note Le pointeur doit être libéré en interne dans RC2D.
     */
    const char* vertex_shader_filename;

    /**
     * Nom du fichier du shader de fragment (e.g., "test.fragment").
     * 
     * IMPORTANT:
     * \note Le pointeur doit être libéré en interne dans RC2D.
     */
    const char* fragment_shader_filename;

    /**
     * Pointeur vers le pipeline graphique chargé.
     * 
     * IMPORTANT:
     * \note Ce pointeur pointe vers le pipeline graphique de l'utilisateur,
     * il ne doit pas être libéré manuellement en interne, mais par l'utilisateur
     * lorsqu'il n'est plus nécessaire, via : SDL_ReleaseGPUGraphicsPipeline().
     */
    RC2D_GPUGraphicsPipeline* graphicsPipeline;
} RC2D_GraphicsPipelineEntry;

/**
 * \brief Structure regroupant l'état global du moteur RC2D.
 *
 * Cette structure encapsule toutes les variables nécessaires pour gérer l'état du moteur,
 * y compris la configuration, les ressources SDL, et les paramètres d'exécution.
 *
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_EngineState {
    // Pointeur vers la configuration utilisateur
    RC2D_EngineConfig* config;

    // SDL : Window
    SDL_Window* window;

    /**
     * SDL GPU
     * 
     * Cette structure contient :
     * - Pointeur vers le périphérique GPU SDL (SDL_GPUDevice)
     * - Mode de présentation du GPU (SDL_GPUPresentMode)
     * - Composition de la swapchain du GPU (SDL_GPUSwapchainComposition)
     * - Pointeur vers le tampon de commandes actuel (SDL_GPUCommandBuffer)
     * - Pointeur vers la texture de swapchain actuelle (SDL_GPUTexture)
     * - Pointeur vers le rendu actuel (SDL_GPURenderPass)
     * - Pointeur vers la vue actuelle (SDL_GPUViewport)
     * - Pointeur vers le pipeline graphique actuel (SDL_GPUGraphicsPipeline)
     */
    SDL_GPUDevice* gpu_device;
    SDL_GPUPresentMode gpu_present_mode;
    SDL_GPUSwapchainComposition gpu_swapchain_composition;
    SDL_GPUCommandBuffer* gpu_current_command_buffer;
    SDL_GPUTexture* gpu_current_swapchain_texture;
    SDL_GPURenderPass* gpu_current_render_pass;
    SDL_GPUViewport* gpu_current_viewport;

    /**
     * Mise en cache des shaders graphiques
     * 
     * Cette structure contient :
     * - Tableau dynamique des shaders graphics (vertex/fragment) chargés
     * - Nombre de shaders graphics (vertex/fragment) chargés
     * - Mutex pour protéger l'accès aux shaders graphics (vertex/fragment) chargés
     */
    RC2D_GraphicsShaderEntry* gpu_graphics_shaders_cache;
    int gpu_graphics_shader_count;
    SDL_Mutex* gpu_graphics_shader_mutex;

    /**
     * Mise en cache des pipelines graphiques lié aux shaders graphics (vertex/fragment)
     * 
     * Cette structure contient :
     * - Tableau dynamique des pipelines graphiques chargés
     * - Nombre de pipelines graphiques chargés
     * - Mutex pour protéger l'accès aux pipelines graphiques chargés
     */
    RC2D_GraphicsPipelineEntry* gpu_graphics_pipelines_cache;
    int gpu_graphics_pipeline_count;
    SDL_Mutex* gpu_graphics_pipeline_mutex;

    /**
     * Mise en cache des shaders de calcul
     * 
     * Cette structure contient :
     * - Tableau dynamique des shaders de calcul chargés
     * - Nombre de shaders de calcul chargés
     * - Mutex pour protéger l'accès aux shaders de calcul chargés
     */
    RC2D_ComputeShaderEntry* gpu_compute_shaders_cache;
    int gpu_compute_shader_count;
    SDL_Mutex* gpu_compute_shader_mutex;

    /**
     * Pour indiquer si le rendu doit être sauté
     */
    bool skip_rendering;
    
    // RC2D : État d'exécution
    int fps;
    double delta_time;
    bool game_is_running;
    Uint64 last_frame_time;

    // RC2D : Echelle de rendu
    float render_scale;

    // RC2D : Letterbox / Pillarbox
    RC2D_LetterboxTextures letterbox_textures;
    RC2D_Rect letterbox_areas[4]; // [0]: gauche, [1]: droite, [2]: haut, [3]: bas
    int letterbox_count;

    RC2D_GPUTexture* letterbox_uniform_texture;

    RC2D_GPUTexture* letterbox_top_texture;
    RC2D_GPUTexture* letterbox_bottom_texture;
    RC2D_GPUTexture* letterbox_left_texture;
    RC2D_GPUTexture* letterbox_right_texture;

    RC2D_GPUTexture* letterbox_background_texture;

    // Pour RC2D_LETTERBOX_SHADER
    RC2D_GPUGraphicsPipeline letterbox_shader_pipeline;
    RC2D_GPUShader* letterbox_vertex_shader;
    RC2D_GPUShader* letterbox_fragment_shader;
} RC2D_EngineState;

/**
 * \brief Instance globale du moteur RC2D.
 *
 * Cette instance contient toutes les informations nécessaires pour gérer l'état
 * et le comportement du moteur RC2D tout au long de l'exécution de l'application.
 *
 * \since Cette variable est disponible depuis RC2D 1.0.0.
 */
extern RC2D_EngineState rc2d_engine_state;

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
bool rc2d_engine_init(void);

/**
 * \brief Libère les ressources allouées par le moteur RC2D.
 * 
 * Cette fonction doit être appelée avant de quitter l'application pour éviter les fuites de mémoire.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_engine_quit(void);

/**
 * \brief Traite les événements SDL3.
 *
 * Cette fonction traite les événements SDL3 et appelle les callbacks appropriés.
 * 
 * \param {SDL_Event*} event - Pointeur vers l'événement SDL à traiter.
 * \return {SDL_AppResult} - Le résultat du traitement des événements.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
SDL_AppResult rc2d_engine_processevent(SDL_Event *event);

/**
 * \brief Capture le temps au début de la frame actuelle et donc calcule 
 * le delta time à chaque frame.
 * 
 * \note Cette fonction doit être appelée en premier dans la callback principale 
 * de l'application (SDL_AppIterate) pour calculer le delta time.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_engine_deltatime_start(void);

/**
 * \brief Permet de faire une pause entre chaque frame pour atteindre le FPS cible.
 * 
 * Elle permet de fixer le tickrate de la callback principale de l'application (SDL_AppIterate).
 * 
 * \note Si SDL_HINT_MAIN_CALLBACK_RATE est défini, cette fonction ne fait rien.
 * Sinon, elle attend le temps nécessaire pour atteindre le FPS cible.
 * SDL_HINT_MAIN_CALLBACK_RATE peux ne pas marcher sur toutes les plateformes.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_engine_deltatime_end(void);

/**
 * \brief Configure le moteur RC2D avec les paramètres spécifiés.
 * 
 * Cette fonction configure le moteur RC2D en utilisant la structure de configuration fournie.
 * Elle doit être appelée avant d'initialiser le moteur. 
 * 
 * \note si config == NULL, la configuration par défaut sera utilisée.
 * 
 * \param {RC2D_Config*} config - Pointeur vers la structure de configuration à utiliser.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_engine_configure(const RC2D_EngineConfig* config);

/**
 * \brief Initialisation de l'assertion RC2D.
 * 
 * Cette fonction initialise le module d'assertion de RC2D. 
 * Elle doit être appelée avant d'utiliser les macros d'assertion.
 * 
 * \threadsafety Il est possible d'appeler cette fonction en toute sécurité à partir de n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_assert_init(void);

/**
 * \brief Initialise le timer de haute précision utilisé pour mesurer le temps écoulé. 
 * 
 * Cette fonction est appelée automatiquement lors de l'initialisation du système de timer de RC2D 
 * et ne doit généralement pas être appelée directement.
 * 
 * \threadsafety Il est possible d'appeler cette fonction en toute sécurité à partir de n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_timer_init(void);

void rc2d_gpu_hotReloadGraphicsShadersAndGraphicsPipeline(void);
void rc2d_gpu_hotReloadComputeShader(void);

#if RC2D_ONNX_MODULE_ENABLED
/**
 * \brief Initialise le module ONNX de RC2D.
 * 
 * Cette fonction initialise le module ONNX de RC2D. Elle doit être appelée avant d'utiliser les fonctionnalités ONNX.
 * 
 * \return true si l'initialisation a réussi, false sinon.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
bool rc2d_onnx_init(void);

/**
 * \brief Libère les ressources allouées par le module ONNX de RC2D.
 * 
 * Cette fonction doit être appelée avant de quitter l'application pour éviter les fuites de mémoire.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_onnx_cleanup(void);
#endif

/**
 * \brief Initialise la bibliothèque cimgui avec le backend SDL3 GPU.
 *
 * Cette fonction configure le contexte ImGui, initialise les backends SDL3 et SDLGPU3,
 * et applique un style par défaut.
 *
 * \return true si l'initialisation a réussi, false sinon.
 *
 * \since RC2D 1.0.0
 */
bool rc2d_cimgui_init(void);

/**
 * \brief Nettoie les ressources de cimgui.
 *
 * Cette fonction libère les backends SDL3 et SDLGPU3, puis détruit le contexte ImGui.
 *
 * \since RC2D 1.0.0
 */
void rc2d_cimgui_cleanup(void);

/**
 * \brief Prépare une nouvelle frame pour ImGui.
 *
 * Cette fonction doit être appelée au début de chaque frame avant tout rendu ImGui.
 *
 * \since RC2D 1.0.0
 */
void rc2d_cimgui_newFrame(void);

/**
 * \brief Rend les données ImGui sur le GPU.
 *
 * Cette fonction finalise le rendu ImGui et soumet les commandes de dessin au GPU.
 *
 * \since RC2D 1.0.0
 */
void rc2d_cimgui_draw(void);

/**
 * \brief Traite un événement SDL pour ImGui.
 *
 * Cette fonction passe les événements SDL à ImGui pour gérer les entrées utilisateur.
 *
 * \param event Pointeur vers l'événement SDL à traiter.
 * \return true si ImGui a consommé l'événement, false sinon.
 *
 * \since RC2D 1.0.0
 */
bool rc2d_cimgui_processEvent(SDL_Event* event);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_INTERNAL_H