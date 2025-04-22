#ifndef RC2D_INTERNAL_H
#define RC2D_INTERNAL_H

#include <RC2D/RC2D_engine.h>
#include <RC2D/RC2D_math.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>

/**
 * IMPORTANT : 
 * Module interne de la bibliothèque RC2D.
 * Pour ne pas exposer certaines fonctions et variables à l'utilisateur final.
 */

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

// Fenetre SDL3 
extern SDL_Window* rc2d_window;

// Dispositif GPU SDL3 pour le rendu matériel
extern SDL_GPUDevice* rc2d_gpu_device;
extern SDL_GPUPresentMode rc2d_gpu_present_mode;
extern SDL_GPUSwapchainComposition rc2d_gpu_swapchain_composition;

// Événement SDL3 pour capturer les entrées, changements de fenêtre..etc.
extern SDL_Event rc2d_event;

/**
 * \brief Variables de la boucle de jeu.
 * 
 * Ces variables sont utilisées pour gérer le taux de rafraîchissement, 
 * le temps écoulé entre les frames, 
 * et l'état de la boucle de jeu.
 */
extern int rc2d_fps;
extern double rc2d_delta_time;
extern bool rc2d_game_is_running;
extern Uint64 rc2d_last_frame_time;

/**
 * \brief Cela concerne le letterbox/pillarbox interne pour le rendu.
 * Les textures de letterbox sont utilisées pour remplir les zones de letterbox/pillarbox
 * Le nombre de zones de letterbox/pillarbox est utilisé pour déterminer combien de zones sont présentes.
 */
extern RC2D_LetterboxTextures rc2d_letterbox_textures;
extern RC2D_Rect rc2d_letterbox_areas[4];
extern int rc2d_letterbox_count;

/**
 * @brief Échelle de rendu interne
 */
extern float rc2d_render_scale;

/**
 * @brief 
 */
int rc2d_init(void);
void rc2d_quit(void);
SDL_AppResult rc2d_processevent(void);
void rc2d_deltatimeframerates_start(void);
void rc2d_deltatimeframerates_end(void);
bool rc2d_configure(const RC2D_Config* config);
extern RC2D_Callbacks rc2d_callbacks_engine;

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

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_INTERNAL_H