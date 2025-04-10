#ifndef RC2D_GRAPHICS_H
#define RC2D_GRAPHICS_H

#include "rc2d/rc2d_math.h" // Required for : RC2D_ArcType

#include <SDL.h>
#include <SDL_ttf.h>

#include <stdbool.h> // Required for : bool

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumération des modes de filtrage utilisables pour déterminer comment les textures sont redimensionnées.
 *
 * @typedef {enum} RC2D_FilterMode
 * @property {number} RC2D_FILTER_MODE_NEAREST - Mode de filtrage le plus proche (non anti-aliasé).
 * @property {number} RC2D_FILTER_MODE_LINEAR - Mode de filtrage linéaire (anti-aliasé).
 */
typedef enum RC2D_FilterMode {
	RC2D_FILTER_MODE_NEAREST,
	RC2D_FILTER_MODE_LINEAR
} RC2D_FilterMode;

/**
 * Enumération des modes de mélange utilisables pour déterminer comment les couleurs sont combinées.
 *
 * @typedef {enum} RC2D_BlendMode
 * @property {number} RC2D_BLEND_MODE_NONE - Mode de mélange standard sans transparence.
 * @property {number} RC2D_BLEND_MODE_ALPHA - Mode de mélange standard utilisant l'alpha pour l'opacité.
 * @property {number} RC2D_BLEND_MODE_ADD - Additionne les valeurs de couleur de la source et de la destination.
 * @property {number} RC2D_BLEND_MODE_MULTIPLY - Multiplie les valeurs de couleur de la source et de la destination.
 */
typedef enum RC2D_BlendMode {
	RC2D_BLEND_MODE_NONE,
	RC2D_BLEND_MODE_ALPHA,
	RC2D_BLEND_MODE_ADD,
	RC2D_BLEND_MODE_MULTIPLY,
} RC2D_BlendMode;

/**
 * Structure représentant une couleur RGBA.
 * @typedef {object} RC2D_Color
 * @property {number} r - Composante rouge (0-255).
 * @property {number} g - Composante verte (0-255).
 * @property {number} b - Composante bleue (0-255).
 * @property {number} a - Composante alpha (transparence) (0-255).
 */
typedef struct RC2D_Color {
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
} RC2D_Color;

/**
 * Structure représentant une image.
 * @typedef {object} RC2D_Image
 * @property {number} width - Largeur de l'image.
 * @property {number} height - Hauteur de l'image.
 * @property {SDL_Texture} sdl_texture - Pointeur vers la texture SDL de l'image.
 */
typedef struct RC2D_Image {
	int width;
	int height;
	SDL_Texture* sdl_texture;
} RC2D_Image;

/**
 * Structure représentant les données d'une image.
 * @typedef {object} RC2D_ImageData
 * @property {SDL_Surface} sdl_surface - Pointeur vers la surface SDL de l'image.
 * @property {number} pixelFormat - Format de pixel (SDL_PIXELFORMAT_*).
 */
typedef struct RC2D_ImageData {
	int width;
	int height;
	SDL_Surface* sdl_surface;
	int pixelFormat;
	Uint8* mask; // 1 pour solide, 0 pour transparent
} RC2D_ImageData;

/**
 * Structure représentant un canevas.
 * @typedef {object} RC2D_Canvas
 * @property {number} width - Largeur du canevas.
 * @property {number} height - Hauteur du canevas.
 * @property {SDL_Texture} sdl_texture - Pointeur vers la texture SDL du canevas.
 */
typedef struct RC2D_Canvas {
	int width;
	int height;
	SDL_Texture* sdl_texture;
} RC2D_Canvas;

/**
 * Structure représentant une police de caractères.
 * @typedef {object} RC2D_Font
 * @property {TTF_Font} sdl_font - Pointeur vers la police de caractères SDL.
 * @property {number} size - Taille de la police.
 */
typedef struct RC2D_Font {
	TTF_Font* sdl_font;
	int size;
} RC2D_Font;

/**
 * Structure représentant un texte rendu.
 * @typedef {object} RC2D_Text
 * @property {RC2D_Font} font - Police de caractères du texte.
 * @property {string} text - Chaîne de texte.
 * @property {number} width - Largeur du texte rendu.
 * @property {number} height - Hauteur du texte rendu.
 */
typedef struct RC2D_Text {
	RC2D_Font font;
	char* text;
	int width;
	int height;
} RC2D_Text;

/**
 * Structure représentant un quad.
 * @typedef {object} RC2D_Quad
 * @property {number} width - Largeur du quad.
 * @property {number} height - Hauteur du quad.
 * @property {number} widthDest - Largeur de destination du quad.
 * @property {number} heightDest - Hauteur de destination du quad.
 */
typedef struct RC2D_Quad {
	int width;
	int height;
	int widthDest;
	int heightDest;
} RC2D_Quad;

// Functions internal for the engine
void rc2d_graphics_setCurrentBackgroundColorInternal(void);

//
#ifdef __EMSCRIPTEN__
// Sert seulement a appeler depuis le code javascript via le template JS de RC2D pour emscripten HTML5
void rc2d_graphics_resizeWindowForCanvas(int width, int height, double devicePixelRatio);
#endif

// Get renderer output size pixels physical (real)
void rc2d_graphics_getRendererOutputSize(int* width, int* height);


// OBJECT CREATION
RC2D_Image rc2d_graphics_newImage(const char* filename);
void rc2d_graphics_freeImage(RC2D_Image image);

RC2D_ImageData rc2d_graphics_newImageData(const char* filename);
void rc2d_graphics_freeImageData(RC2D_ImageData imageData);

RC2D_Canvas rc2d_graphics_newCanvas(int width, int height);
void rc2d_graphics_freeCanvas(RC2D_Canvas canvas);

RC2D_Font rc2d_graphics_newFont(const char* filename, int size);
void rc2d_graphics_freeFont(RC2D_Font font);

RC2D_Text rc2d_graphics_newText(RC2D_Font font, const char* textString, RC2D_Color coloredText);

RC2D_Quad rc2d_graphics_newQuad(int x, int y, int width, int height, RC2D_Image image);


// GRAPHICS STATE
void rc2d_graphics_setCanvas(RC2D_Canvas canvas);
void rc2d_graphic_setFont(RC2D_Font font);
void rc2d_graphic_setFontSize(RC2D_Font font, int fontSize);
void rc2d_graphics_setColor(const RC2D_Color color);
void rc2d_graphics_setBackgroundColor(const RC2D_Color color);
void rc2d_graphics_setDefaultFilter(const RC2D_FilterMode filterMode);
void rc2d_graphics_setPixel(RC2D_ImageData imageData, const RC2D_Point point, const RC2D_Color color); // No finish
void rc2d_graphics_setBlendMode(const RC2D_BlendMode blendMode);

bool rc2d_graphics_isSupportedCanvas(void);

void rc2d_graphics_getRendererInfo(void); // No finish
void rc2d_graphics_getRenderDriverInfo(void); // No finish
int rc2d_graphics_getHeight(void);
int rc2d_graphics_getWidth(void);
void rc2d_graphics_getPixel(RC2D_ImageData imageData, const RC2D_Point point, const RC2D_Color color); // No finish


// DRAWING
void rc2d_graphics_clear(void);
void rc2d_graphics_present(void);

int rc2d_graphics_arc(const RC2D_ArcType arcType, const int x, const int y, const int radius, const double angle1, const double angle2, const int segments);

int rc2d_graphics_circle(const char* mode, const int x, const int y, const int radius);
int rc2d_graphics_circles(const char* mode, const int numCircles, const int* circlesArray);

int rc2d_graphics_ellipse(const int x, const int y, const int radiusx, const int radiusy, int segments);

int rc2d_graphics_line(const int x1, const int y1, const int x2, const int y2);
int rc2d_graphics_lines(const int numPoints, const int* pointsArray);

int rc2d_graphics_point(const int x, const int y);
int rc2d_graphics_points(const int numPoints, const int* pointsArray);

int rc2d_graphics_polygon(const int numPoints, const int* pointsArray);

int rc2d_graphics_rectangle(const char* mode, const int x, const int y, const int width, const int height);
int rc2d_graphics_rectangles(const char* mode, const int count, const int* rectsArray);

void rc2d_graphics_draw(RC2D_Image image, int x, int y, const double r, float sx, float sy, int ox, int oy, bool flipHorizontal, bool flipVertical);
void rc2d_graphics_drawImageData(RC2D_ImageData imageData, int x, int y, const double r, float sx, float sy, int ox, int oy, bool flipHorizontal, bool flipVertical);
void rc2d_graphics_drawQuad(RC2D_Quad quad, int x, int y, const double r, float sx, float sy, int ox, int oy, bool flipHorizontal, bool flipVertical);
void rc2d_graphics_drawCanvas(RC2D_Canvas canvas, int x, int y, const double r, float sx, float sy, int ox, int oy, bool flipHorizontal, bool flipVertical);
void rc2d_graphics_drawText(RC2D_Text text, int x, int y, const double r, float sx, float sy, int ox, int oy, bool flipHorizontal, bool flipVertical);


// COORDINATE SYSTEM
void rc2d_graphics_scale(float scaleX, float scaleY);

#ifdef __cplusplus
}
#endif

#endif // RC2D_GRAPHICS_H