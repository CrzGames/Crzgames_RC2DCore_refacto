#ifndef RC2D_GRAPHICS_H
#define RC2D_GRAPHICS_H

#include <SDL3/SDL_stdinc.h> // Pour Uint8
#include <stdbool.h> // Required for : bool

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Structure représentant une couleur RGBA.
 * 
 * Cette structure est utilisée pour définir une couleur en termes de rouge, vert, bleu et alpha.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_Color {
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
} RC2D_Color;

/**
 * \brief Mode de dessin pour les formes graphiques.
 *
 * Définit si une forme est dessinée en mode rempli ou en contour.
 *
 * \since Cette énumération est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_DrawMode {
    RC2D_DRAWMODE_FILL, ///< Mode rempli : la forme est complètement remplie.
    RC2D_DRAWMODE_LINE  ///< Mode contour : seule la bordure de la forme est dessinée.
} RC2D_DrawMode;

/**
 * \brief Efface l'écran pour préparer le rendu d'une nouvelle frame.
 *
 * Cette fonction acquiert un command buffer, la texture de swapchain, et commence une passe de rendu.
 * Elle doit être appelée avant toute opération de rendu dans une frame.
 *
 * \threadsafety Cette fonction doit être appelée depuis le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_graphics_clear(void);

/**
 * \brief Présente le rendu à l'écran.
 *
 * Cette fonction termine la passe de rendu, soumet le command buffer au GPU, et présente la frame.
 * Elle doit être appelée après toutes les opérations de rendu dans une frame.
 *
 * \threadsafety Cette fonction doit être appelée depuis le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_graphics_present(void);

/**
 * \brief Définit la couleur globale utilisée pour les opérations de dessin.
 *
 * Cette fonction règle la couleur (R, G, B, A) utilisée pour dessiner les formes, comme les rectangles.
 * Les valeurs sont des entiers non signés de 8 bits (0 à 255).
 *
 * \param color Couleur RGBA (composantes r, g, b, a entre 0 et 255).
 *
 * \threadsafety Cette fonction doit être appelée depuis le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_graphics_setColor(RC2D_Color color);

/**
 * \brief Dessine un rectangle à l'écran.
 *
 * Cette fonction dessine un rectangle à la position (x, y) avec la largeur et la hauteur spécifiées.
 * Le mode de dessin peut être RC2D_DRAWMODE_FILL (rempli) ou RC2D_DRAWMODE_LINE (contour).
 *
 * \param mode Mode de dessin (RC2D_DRAWMODE_FILL ou RC2D_DRAWMODE_LINE).
 * \param x Position x en pixels logiques.
 * \param y Position y en pixels logiques.
 * \param width Largeur en pixels logiques.
 * \param height Hauteur en pixels logiques.
 *
 * \threadsafety Cette fonction doit être appelée depuis le thread principal.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
void rc2d_graphics_rectangle(RC2D_DrawMode mode, float x, float y, float width, float height);

#ifdef __cplusplus
}
#endif

#endif // RC2D_GRAPHICS_H