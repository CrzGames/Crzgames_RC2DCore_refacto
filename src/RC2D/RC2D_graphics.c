#include <RC2D/RC2D_engine.h>
#include <RC2D/RC2D_internal.h>

#include <string.h> // Required for : strcmp()
#include <stdarg.h> // Required for : va_list, va_start, va_end
#include <math.h> // Required for : cosf, sinf, M_PI

#define CIRCLE_POINTS 360 // Nombre de points pour dessiner le cercle / ellipse
#ifndef INT_MIN
#define INT_MIN (-2147483647 - 1)
#endif
#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

static Uint8 rc2d_graphics_alpha = 255;
static RC2D_Color rc2d_graphics_currentBackgroundColor = { 0, 0, 0, 255 };

// Facon dont l'image est filtree lors d'un changement d'echelle.  
void rc2d_graphics_setDefaultFilter(const RC2D_FilterMode filterMode)
{
	if (filterMode == RC2D_FILTER_MODE_NEAREST)
	{
		// Empêche SDL2 de filtrer les contours des images quand elles sont redimensionnées / Pas d'anti-aliasing
		// Indispensable pour du Pixel Art
		// Indispensable pour des effets de Pixel Shaders
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	}
	else if (filterMode == RC2D_FILTER_MODE_LINEAR)
	{
		// Permet à SDL2 de filtrer les contours des images lorsqu'elles sont redimensionnées / Anti-aliasing
		// Par défaut
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	}
}

/**************************************/

/*              RENDERER               */

/**************************************/
/*
En raison des affichages à haute résolution (HIGH DPI), vous pourriez vous retrouver avec un contexte de rendu comportant plus de pixels que la fenêtre qui le contient
Exemple Renderer : 1280 x 760 et Fenetre : 1280 x 760, le renderer peut ce retrouvé avec une densité de pixel de 1.6 par exemple 
et ce retrouver avec une taille de 1280 x 760 * 1.6 = 2048 x 1216
*/
void rc2d_graphics_getRendererOutputSize(int* width, int* height)
{
    int result = SDL_GetRendererOutputSize(rc2d_sdl_renderer, width, height);

    if (result != 0) 
    {
        RC2D_log(RC2D_LOG_ERROR, "Could not get renderer output size: %s", SDL_GetError());
    }
}





/**************************************/

/*              DRAWING               */

/**************************************/

// Clear Screen
void rc2d_graphics_clear(void)
{
    SDL_RenderClear(rc2d_sdl_renderer);
}

// Present draw screen
void rc2d_graphics_present(void)
{
	// Dessine les zones de letterbox si nécessaire
	rc2d_draw_letterbox_zones();

	// Dessine toute la scene
    SDL_RenderPresent(rc2d_sdl_renderer);
}

static void rc2d_draw_letterbox_zones(void)
{
    if (rc2d_letterbox_textures.mode == RC2D_LETTERBOX_NONE) {
        // Ne rien dessiner, les zones sont remplies par défaut en noir
        return;
    }

    // Mode BACKGROUND_FULL ⇒ dessine le fond complet en arrière-plan
    if (rc2d_letterbox_textures.mode == RC2D_LETTERBOX_BACKGROUND_FULL && rc2d_letterbox_textures.background != NULL) {
        rc2d_graphics_draw_texture_fullscreen(rc2d_letterbox_textures.background);
        return;
    }

    // Parcours des zones de letterbox calculées
    for (int i = 0; i < rc2d_letterbox_count; ++i) {
        SDL_Rect area = rc2d_letterbox_areas[i];

        if (rc2d_letterbox_textures.mode == RC2D_LETTERBOX_UNIFORM) {
            if (rc2d_letterbox_textures.uniform != NULL) {
                rc2d_graphics_draw_texture_in_rect(rc2d_letterbox_textures.uniform, &area);
            }
        }
        else if (rc2d_letterbox_textures.mode == RC2D_LETTERBOX_PER_SIDE) {
            // Détermination du côté à partir de la position (simpliste)
            if (area.x == 0 && area.y == 0 && area.h < area.w) {
                if (rc2d_letterbox_textures.top != NULL)
                    rc2d_graphics_draw_texture_in_rect(rc2d_letterbox_textures.top, &area);
            }
            else if (area.x == 0 && area.y + area.h >= rc2d_window_height) {
                if (rc2d_letterbox_textures.bottom != NULL)
                    rc2d_graphics_draw_texture_in_rect(rc2d_letterbox_textures.bottom, &area);
            }
            else if (area.y == 0 && area.w < area.h) {
                if (rc2d_letterbox_textures.left != NULL)
                    rc2d_graphics_draw_texture_in_rect(rc2d_letterbox_textures.left, &area);
            }
            else if (area.x + area.w >= rc2d_window_width) {
                if (rc2d_letterbox_textures.right != NULL)
                    rc2d_graphics_draw_texture_in_rect(rc2d_letterbox_textures.right, &area);
            }
        }
    }
}

// Draw image
void rc2d_graphics_draw(RC2D_Image image, int x, int y, const double r, float sx, float sy, int ox, int oy, bool flipHorizontal, bool flipVertical)
{
	SDL_Rect rectDest;
	rectDest.x = x;
	rectDest.y = y;
	rectDest.w = image.width;
	rectDest.h = image.height;

	// Alpha
	SDL_SetTextureAlphaMod(image.sdl_texture, rc2d_graphics_alpha);

	// Set FLIP X and Y
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	if (flipHorizontal)
	{
		flip = SDL_FLIP_HORIZONTAL;
	}
	if (flipVertical)
	{
		flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);
	}
	
	// Set OFFSETX and Y
	SDL_Point pointOffset;
	pointOffset.x = ox;
	pointOffset.y = oy;

	// Set ScaleX and Y
	SDL_RenderSetScale(rc2d_sdl_renderer, sx, sy);

	// Render Image
	bool offset = false;
	if (pointOffset.x >= 0 && pointOffset.y >= 0)
	{
		offset = true;
	}

	int result;
	if (offset)
	{
		result = SDL_RenderCopyEx(rc2d_sdl_renderer, image.sdl_texture, NULL, &rectDest, r, &pointOffset, flip);
	}
	else
	{
		result = SDL_RenderCopyEx(rc2d_sdl_renderer, image.sdl_texture, NULL, &rectDest, r, NULL, flip);
	}

	// Debug render
	if (result != 0)
	{
		RC2D_log(RC2D_LOG_ERROR, "SDL_RenderCopyEx failed in rc2d_graphics_draw: %s\n", SDL_GetError());
	}

	// Reset le scale du renderer par d�fault pour les dessin qui vont �tre effectuer apr�s.
	SDL_RenderSetScale(rc2d_sdl_renderer, 1, 1);
}

void rc2d_graphics_drawCanvas(RC2D_Canvas canvas, int x, int y, const double r, float sx, float sy, int ox, int oy, bool flipHorizontal, bool flipVertical)
{
	SDL_Rect rectDest;
	rectDest.x = x;
	rectDest.y = y;
	rectDest.w = canvas.width;
	rectDest.h = canvas.height;

	// Alpha
	SDL_SetTextureAlphaMod(canvas.sdl_texture, rc2d_graphics_alpha);

	// Set FLIP X and Y
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	if (flipHorizontal)
	{
		flip = SDL_FLIP_HORIZONTAL;
	}
	if (flipVertical)
	{
		flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);
	}

	// Set OFFSETX and Y
	SDL_Point pointOffset;
	pointOffset.x = ox;
	pointOffset.y = oy;

	// Set ScaleX and Y
	SDL_RenderSetScale(rc2d_sdl_renderer, sx, sy);

	// Render Image
	bool offset = false;
	if (pointOffset.x >= 0 && pointOffset.y >= 0)
	{
		offset = true;
	}

	int result;
	if (offset)
	{
		result = SDL_RenderCopyEx(rc2d_sdl_renderer, canvas.sdl_texture, NULL, &rectDest, r, &pointOffset, flip);
	}
	else
	{
		result = SDL_RenderCopyEx(rc2d_sdl_renderer, canvas.sdl_texture, NULL, &rectDest, r, NULL, flip);
	}

	// Debug render
	if (result != 0)
	{
		RC2D_log(RC2D_LOG_ERROR, "SDL_RenderCopyEx failed: %s\n", SDL_GetError());
	}

	// Reset le scale du renderer par d�fault. pour les dessin qui vont �tre effectuer apr�s.
	SDL_RenderSetScale(rc2d_sdl_renderer, 1.0f, 1.0f);
}

void rc2d_graphics_drawText(RC2D_Text text, int x, int y, const double r, float sx, float sy, int ox, int oy, bool flipHorizontal, bool flipVertical)
{

}

// Primitives
/**
 * Dessine un polygone basé sur un tableau de points.
 * 
 * Cette fonction dessine un polygone fermé sur l'écran en utilisant un tableau de points. 
 * Chaque point est défini par une paire de coordonnées x et y dans le tableau. 
 * Le polygone est automatiquement fermé en reliant le dernier point au premier.
 * 
 * @param const int numPoints Le nombre de points dans le polygone. Doit être au moins 3.
 * @param const int* pointsArray Un tableau contenant les coordonnées des points du polygone. 
 *                               Le tableau doit avoir une taille de 2*numPoints. 
 *                               Chaque paire de valeurs représente les coordonnées x et y d'un point (par exemple, {x1, y1, x2, y2, ..., xn, yn}).
 * @return int Renvoie 0 en cas de succès, un code d'erreur négatif en cas d'échec.
 */
int rc2d_graphics_polygon(const int numPoints, const int* pointsArray) 
{
    if (numPoints < 3) 
	{
        RC2D_log(RC2D_LOG_ERROR, "Un polygone doit avoir au moins 3 points.\n");
        return -1; // Un polygone doit avoir au moins 3 points pour être valide.
    }

    SDL_Point* points = (SDL_Point*)malloc(sizeof(SDL_Point) * (numPoints + 1)); // +1 pour fermer le polygone
    if (!points) 
	{
        return -1; // Échec d'allocation
    }

    for (int i = 0; i < numPoints; ++i) 
	{
        points[i].x = pointsArray[2 * i];
        points[i].y = pointsArray[2 * i + 1];
    }
    points[numPoints] = points[0]; // Ajouter le premier point à la fin pour fermer le polygone

    // Dessiner le contour du polygone fermé
    if (SDL_RenderDrawLines(rc2d_sdl_renderer, points, numPoints + 1) != 0) 
	{
        RC2D_log(RC2D_LOG_ERROR, "SDL_RenderDrawLines failed: %s", SDL_GetError());
    }

    free(points);
    return 0; // Succès
}

int rc2d_graphics_arc(const RC2D_ArcType arcType, const int x, const int y, const int radius, const double angle1, const double angle2, const int segments) 
{
    double angleStep = (angle2 - angle1) / segments;
    double radian1 = angle1 * (M_PI / 180.0);
    double radian2 = angle2 * (M_PI / 180.0);

	// Dessiner l'arc
	int lastX = x + (int)(radius * cos(radian1));
	int lastY = y + (int)(radius * sin(radian1));

	int result = -1;
	for (int i = 1; i <= segments; ++i) 
	{
		double angle = radian1 + i * angleStep;
		int nextX = x + (int)(radius * cos(angle));
		int nextY = y + (int)(radius * sin(angle));

		result = SDL_RenderDrawLine(rc2d_sdl_renderer, lastX, lastY, nextX, nextY);
		if (result != 0)
		{
			RC2D_log(RC2D_LOG_ERROR, "SDL_RenderDrawLine failed in rc2d_graphics_arc: %s\n", SDL_GetError());
		}

		lastX = nextX;
		lastY = nextY;
	}

	// Gérer les cas spécifiques des types d'arc
	if (arcType == RC2D_ARC_CLOSED) 
	{
		result = SDL_RenderDrawLine(rc2d_sdl_renderer, lastX, lastY, x + (int)(radius * cos(radian1)), y + (int)(radius * sin(radian1)));
		if (result != 0)
		{
			RC2D_log(RC2D_LOG_ERROR, "SDL_RenderDrawLine failed in rc2d_graphics_arc: %s\n", SDL_GetError());
		}
	}
	// 'RC2D_ARC_OPEN' ne nécessite rien de plus car les extrémités ne sont pas connectées

    return 0; // Succès
}

/**
 * Dessine une ellipse sur la cible de rendu actuelle.
 * 
 * @param x La position du centre le long de l'axe des x.
 * @param y La position du centre le long de l'axe des y.
 * @param radiusx Le rayon de l'ellipse le long de l'axe des x.
 * @param radiusy Le rayon de l'ellipse le long de l'axe des y.
 * @param segments Le nombre de segments utilisés pour dessiner l'ellipse.
 */
int rc2d_graphics_ellipse(const int x, const int y, const int radiusx, const int radiusy, int segments) 
{
    // Assurez-vous que le nombre de segments est valide.
    if (segments <= 0) 
	{
        segments = CIRCLE_POINTS;
    }

    double angleStep = (2.0 * M_PI) / segments;

	// Dessinez le contour de l'ellipse.
	int prevPx = x + (int)(radiusx * cos(0));
	int prevPy = y + (int)(radiusy * sin(0));

    int result = -1;
	for (int i = 1; i <= segments; ++i) 
	{
		double angle = angleStep * i;
		int px = x + (int)(radiusx * cos(angle));
		int py = y + (int)(radiusy * sin(angle));

		result = SDL_RenderDrawLine(rc2d_sdl_renderer, prevPx, prevPy, px, py);
		if (result != 0) 
		{
			RC2D_log(RC2D_LOG_ERROR, "SDL_RenderDrawLine failed in rc2d_graphics_ellipse: %s\n", SDL_GetError());
		}

		prevPx = px;
		prevPy = py;
	}

    return result;
}

int rc2d_graphics_rectangle(const char* mode, const int x, const int y, const int width, const int height)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;

	int result = -1;
    if (strcmp(mode, "fill") == 0) 
	{
        result = SDL_RenderFillRect(rc2d_sdl_renderer, &rect);
		if (result != 0)
		{
			RC2D_log(RC2D_LOG_ERROR, "SDL_RenderFillRect failed in rc2d_graphics_rectangle: %s\n", SDL_GetError());
		}
    } 
	else if (strcmp(mode, "line") == 0) 
	{
        result = SDL_RenderDrawRect(rc2d_sdl_renderer, &rect);
		if (result != 0)
		{
			RC2D_log(RC2D_LOG_ERROR, "SDL_RenderDrawRect failed in rc2d_graphics_rectangle: %s\n", SDL_GetError());
		}
    }

	return result;
}

/**
 * Dessine ou remplit plusieurs rectangles sur la cible de rendu actuelle.
 * 
 * @param mode Le mode de dessin ("fill" pour remplir les rectangles, "line" pour dessiner uniquement les contours).
 * @param count Le nombre de rectangles à dessiner.
 * @param rectsArray Tableau contenant les coordonnées et dimensions des rectangles (x, y, width, height pour chaque rectangle).
 * @return 0 en cas de succès, un code d'erreur négatif en cas d'échec.
 */
int rc2d_graphics_rectangles(const char* mode, const int count, const int* rectsArray) 
{
    SDL_Rect* rects = (SDL_Rect*)malloc(sizeof(SDL_Rect) * count);
    if (!rects) 
	{
        return -1; // Échec d'allocation
    }

    for (int i = 0; i < count; ++i) 
	{
        rects[i].x = rectsArray[i * 4];
        rects[i].y = rectsArray[i * 4 + 1];
        rects[i].w = rectsArray[i * 4 + 2];
        rects[i].h = rectsArray[i * 4 + 3];
    }

    int result = -1;
    if (strcmp(mode, "fill") == 0) 
	{
        result = SDL_RenderFillRects(rc2d_sdl_renderer, rects, count);
        if (result != 0) 
		{
            RC2D_log(RC2D_LOG_ERROR, "SDL_RenderFillRects failed: %s", SDL_GetError());
        }
    } 
	else if (strcmp(mode, "line") == 0) 
	{
        result = SDL_RenderDrawRects(rc2d_sdl_renderer, rects, count);
        if (result != 0) 
		{
            RC2D_log(RC2D_LOG_ERROR, "SDL_RenderDrawRects failed: %s", SDL_GetError());
        }
    }

    free(rects);
    return result;
}

int rc2d_graphics_circle(const char* mode, const int x, const int y, const int radius) 
{
	int result = -1;

    if (strcmp(mode, "fill") == 0) 
	{
        for (int w = 0; w < radius * 2; w++) 
		{
            for (int h = 0; h < radius * 2; h++)
			{
                int dx = radius - w; // distance horizontale depuis le centre
                int dy = radius - h; // distance verticale depuis le centre

                if ((dx*dx + dy*dy) <= (radius * radius)) 
				{
                    result = SDL_RenderDrawPoint(rc2d_sdl_renderer, x + dx, y + dy);
					if (result != 0)
					{
						RC2D_log(RC2D_LOG_ERROR, "SDL_RenderDrawPoint failed in rc2d_graphics_circle: %s\n", SDL_GetError());
					}
                }
            }
        }
    } 
	else if (strcmp(mode, "line") == 0) 
	{
        for (int i = 0; i < CIRCLE_POINTS; i++) 
		{
            float rad = (float)i * M_PI / 180.0f; // Conversion de degrés en radians
            int px = x + (int)(radius * cosf(rad));
            int py = y + (int)(radius * sinf(rad));

            result = SDL_RenderDrawPoint(rc2d_sdl_renderer, px, py);
			if (result != 0)
			{
				RC2D_log(RC2D_LOG_ERROR, "SDL_RenderDrawPoint failed in rc2d_graphics_circle: %s\n", SDL_GetError());
			}
        }
    }

	return result;
}

/**
 * Dessine plusieurs cercles sur la cible de rendu actuelle, en mode rempli ou en contours, en utilisant un tableau de paramètres.
 * 
 * @param mode Le mode de dessin ("fill" pour des cercles pleins, "line" pour des contours de cercles).
 * @param numCircles Le nombre de cercles à dessiner.
 * @param circlesArray Tableau contenant les paramètres des cercles. Pour chaque cercle : x, y, rayon.
 * @return 0 en cas de succès sur tous les cercles, -1 si une erreur se produit.
 */
int rc2d_graphics_circles(const char* mode, const int numCircles, const int* circlesArray)
{
    int result = -1;

    for (int i = 0; i < numCircles; ++i) 
	{
        int x = circlesArray[i * 3];
        int y = circlesArray[i * 3 + 1];
        int radius = circlesArray[i * 3 + 2];

        if (strcmp(mode, "fill") == 0) 
		{
            for (int w = 0; w < radius * 2; w++) 
			{
                for (int h = 0; h < radius * 2; h++) 
				{
                    int dx = radius - w; // distance horizontale depuis le centre
                    int dy = radius - h; // distance verticale depuis le centre

                    if ((dx*dx + dy*dy) <= (radius * radius)) 
					{
                        if (SDL_RenderDrawPoint(rc2d_sdl_renderer, x + dx, y + dy) != 0) 
						{
                            RC2D_log(RC2D_LOG_ERROR, "SDL_RenderDrawPoint failed in rc2d_graphics_circles: %s\n", SDL_GetError());
                        }
                    }
                }
            }
        } 
		else if (strcmp(mode, "line") == 0) 
		{
            for (int i = 0; i < CIRCLE_POINTS; i++) 
			{ 
                float rad = (float)i * M_PI / 180.0f; // Conversion de degrés en radians
                int px = x + (int)(radius * cosf(rad));
                int py = y + (int)(radius * sinf(rad));

                if (SDL_RenderDrawPoint(rc2d_sdl_renderer, px, py) != 0) 
				{
                    RC2D_log(RC2D_LOG_ERROR, "SDL_RenderDrawPoint failed in rc2d_graphics_circles: %s\n", SDL_GetError());
                }
            }
        }
    }

    return result;
}

int rc2d_graphics_line(const int x1, const int y1, const int x2, const int y2)
{
	int result = SDL_RenderDrawLine(rc2d_sdl_renderer, x1, y1, x2, y2);
	if (result != 0)
	{
		RC2D_log(RC2D_LOG_ERROR, "SDL_RenderDrawLine failed in rc2d_graphics_line: %s\n", SDL_GetError());
	}

	return result;
}

/**
 * Dessine plusieurs lignes connectées sur la cible de rendu actuelle en utilisant un tableau de coordonnées entières.
 * 
 * @param numPoints Le nombre de points définissant les lignes (au moins deux pour une ligne).
 * @param pointsArray Tableau contenant les coordonnées des points définissant les lignes (chaque point est représenté par deux entiers consécutifs pour x et y).
 * @return 0 en cas de succès, un code d'erreur négatif en cas d'échec.
 */
int rc2d_graphics_lines(const int numPoints, const int* pointsArray) 
{
    SDL_Point* points = (SDL_Point*)malloc(sizeof(SDL_Point) * numPoints);
    if (!points) 
	{
		RC2D_log(RC2D_LOG_ERROR, "SDL_Point failed in rc2d_graphics_lines_array: %s\n", SDL_GetError());
        return -1; // Erreur d'allocation
    }

    for (int i = 0; i < numPoints; ++i) 
	{
        points[i].x = pointsArray[i * 2]; // Chaque point a deux coordonnées (x, y)
        points[i].y = pointsArray[i * 2 + 1];
    }

    // Dessine les lignes connectées.
    int result = SDL_RenderDrawLines(rc2d_sdl_renderer, points, numPoints);
    if (result != 0) 
	{
        RC2D_log(RC2D_LOG_ERROR, "SDL_RenderDrawLines failed: %s", SDL_GetError());
    }

    free(points);
    return result;
}

int rc2d_graphics_point(const int x, const int y)
{
	int result = SDL_RenderDrawPoint(rc2d_sdl_renderer, x, y);
	if (result != 0)
	{
		RC2D_log(RC2D_LOG_ERROR, "SDL_RenderDrawPoint failed in rc2d_graphics_point: %s\n", SDL_GetError());
	}

	return result;
}

/**
 * Dessine plusieurs points sur la cible de rendu actuelle en utilisant des coordonnées entières.
 * 
 * @param numPoints Le nombre de points à dessiner.
 * @param pointsArray Tableau contenant les coordonnées des points à dessiner (chaque point est représenté par deux entiers consécutifs pour x et y).
 * @return 0 en cas de succès, un code d'erreur négatif en cas d'échec.
 */
int rc2d_graphics_points(const int numPoints, const int* pointsArray) 
{
    SDL_Point* points = (SDL_Point*)malloc(sizeof(SDL_Point) * numPoints);
    if (!points) 
	{
		RC2D_log(RC2D_LOG_ERROR, "SDL_Point failed in rc2d_graphics_points: %s\n", SDL_GetError());
        return -1; // Erreur d'allocation
    }

    for (int i = 0; i < numPoints; ++i) 
	{
        points[i].x = pointsArray[i * 2]; // Multiplication par 2 car chaque point a deux coordonnées (x, y)
        points[i].y = pointsArray[i * 2 + 1];
    }

    int result = SDL_RenderDrawPoints(rc2d_sdl_renderer, points, numPoints);
    if (result != 0) 
	{
        RC2D_log(RC2D_LOG_ERROR, "SDL_RenderDrawPoints failed: %s", SDL_GetError());
    }

    free(points);
    return result;
}

/**************************************/

/*         OBJECT CREATION            */

/**************************************/

// Image / Surface - Quand il y a besoin de modifier des pixels de l'image.
// Infos Image : Image for SDL2 default OffsetX and offsetY is center
RC2D_ImageData rc2d_graphics_newImageData(const char* filename)
{
	RC2D_ImageData imageData;
	imageData.sdl_surface = NULL;

	// Load image (surface)
	imageData.sdl_surface = IMG_Load(filename);

	if (imageData.sdl_surface == NULL)
	{
		RC2D_log(RC2D_LOG_ERROR, "rc2d_graphics_newImageData : Unable to create surface from %s ! SDL Error : %s \n", filename, SDL_GetError());
	}
	else
	{
		imageData.width = imageData.sdl_surface->w;
		imageData.height = imageData.sdl_surface->h;
	}

	return imageData;
}
void rc2d_graphics_freeImageData(RC2D_ImageData imageData)
{
	if (imageData.sdl_surface != NULL)
	{
		SDL_FreeSurface(imageData.sdl_surface);
		imageData.sdl_surface = NULL;
	}
}

// Image / Texture
// Infos Image : Image for SDL2 default OffsetX and offsetY is center
RC2D_Image rc2d_graphics_newImage(const char* filename)
{
	RC2D_Image image;
	SDL_Texture* texture = NULL;

	texture = IMG_LoadTexture(rc2d_sdl_renderer, filename);
	if (texture == NULL)
	{
		RC2D_log(RC2D_LOG_ERROR, "Unable to create texture from %s ! SDL Error : %s \n", filename, SDL_GetError());
	}
	else
	{
		image.sdl_texture = texture;
		SDL_QueryTexture(image.sdl_texture, NULL, NULL, &image.width, &image.height);
	}

	return image;
}
void rc2d_graphics_freeImage(RC2D_Image image)
{
	if (image.sdl_texture != NULL)
	{
		SDL_DestroyTexture(image.sdl_texture);
		image.sdl_texture = NULL;
	}
}

// Canvas - If == 0 THEN max height and width to screen drawable
RC2D_Canvas rc2d_graphics_newCanvas(int width, int height)
{
	RC2D_Canvas canvas;
	SDL_Texture* textureTarget = NULL;

	// Un nouveau Canvas dont les dimensions sont egales a la taille de la fenetre en pixels.
	if (width == 0 || height == 0)
	{
		canvas.width = rc2d_sdl_renderer_width;
		canvas.height = rc2d_sdl_renderer_height;
	}
	// SINON retourne un canvas avec la taille demande
	else
	{
		canvas.width = width;
		canvas.height = height;
	}

	textureTarget = SDL_CreateTexture(rc2d_sdl_renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET, canvas.width, canvas.height);

	if (textureTarget == NULL)
	{
		RC2D_log(RC2D_LOG_ERROR, "Texture failed to rc2d_graphics_newCanvas : %s \n", SDL_GetError());
	}
	else
	{
		canvas.sdl_texture = textureTarget;
	}

	return canvas;
}
void rc2d_graphics_freeCanvas(RC2D_Canvas canvas)
{
	if (canvas.sdl_texture != NULL)
	{
		SDL_DestroyTexture(canvas.sdl_texture);
		canvas.sdl_texture = NULL;
	}
}

// Font
RC2D_Font rc2d_graphics_newFont(const char* filename, int size)
{
	RC2D_Font font;
	font.sdl_font = NULL;

	font.sdl_font = TTF_OpenFont(filename, size);
	font.size = size;

	if (font.sdl_font == NULL)
	{
		RC2D_log(RC2D_LOG_ERROR, "rc2d_graphics_newFont failed, Unable to create font : %s", TTF_GetError());
	}

	return font;
}
void rc2d_graphics_freeFont(RC2D_Font font)
{
	if (font.sdl_font != NULL)
	{
		TTF_CloseFont(font.sdl_font);
		font.sdl_font = NULL;
	}
}

// Text
RC2D_Text rc2d_graphics_newText(RC2D_Font font, const char* textString, RC2D_Color coloredText)
{
	RC2D_Text text;
	
	SDL_Surface* surface = NULL;
	SDL_Color textColor = { coloredText.r, coloredText.g, coloredText.b, coloredText.a };

	surface = TTF_RenderUTF8_Blended(font.sdl_font, textString, textColor);

	// TODO: Finir la fonction

	return text;
}

// Quad
RC2D_Quad rc2d_graphics_newQuad(int x, int y, int width, int height, RC2D_Image image)
{
	RC2D_Quad quad;
	quad.height = 10;

	return quad;
}




/**************************************/

/*         STATE GRAPHICS            */
/**************************************/

// Canvas
void rc2d_graphics_setCanvas(RC2D_Canvas canvas)
{
	// Reactive le dessin sur l'ecran principal (main backbuffer)
	if (canvas.sdl_texture == NULL)
	{
		SDL_SetRenderTarget(rc2d_sdl_renderer, NULL);
	}
	// SINON operations de dessin directement sur le canvas
	else
	{
		SDL_SetRenderTarget(rc2d_sdl_renderer, canvas.sdl_texture);
	}
}

// Set Font
void rc2d_graphic_setFont(RC2D_Font font)
{
	TTF_SetFontStyle(font.sdl_font, TTF_STYLE_NORMAL);
	TTF_SetFontSize(font.sdl_font, font.size);
}

// Set FontSize
void rc2d_graphic_setFontSize(RC2D_Font font, int fontSize)
{
	TTF_SetFontStyle(font.sdl_font, TTF_STYLE_NORMAL);
	TTF_SetFontSize(font.sdl_font, fontSize);
}

// Get height to screen drawable.
int rc2d_graphics_getHeight(void)
{
	return rc2d_sdl_renderer_height;
}

// Get width to screen drawable.
int rc2d_graphics_getWidth(void)
{
	return rc2d_sdl_renderer_width;
}

// Definit la couleur utilisee pour le dessin.	
void rc2d_graphics_setColor(const RC2D_Color color)
{
	SDL_SetRenderDrawColor(rc2d_sdl_renderer, color.r, color.g, color.b, color.a);
	rc2d_graphics_alpha = color.a;
}

// Definit la couleur d'arriere-plan.
void rc2d_graphics_setBackgroundColor(const RC2D_Color color)
{
	rc2d_graphics_currentBackgroundColor = color;
}
void rc2d_graphics_setCurrentBackgroundColorInternal(void)
{
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = rc2d_graphics_getWidth();
	rect.h = rc2d_graphics_getHeight();

	SDL_SetRenderDrawColor(rc2d_sdl_renderer, rc2d_graphics_currentBackgroundColor.r, rc2d_graphics_currentBackgroundColor.g, 
						   rc2d_graphics_currentBackgroundColor.b, rc2d_graphics_currentBackgroundColor.a);
	SDL_RenderFillRect(rc2d_sdl_renderer, &rect);
}

// Determinez si un moteur de rendu prend en charge l'utilisation de cibles de rendu.
bool rc2d_graphics_isSupportedCanvas(void)
{
	if (SDL_RenderTargetSupported(rc2d_sdl_renderer) == SDL_TRUE)
		return true;
	
	return false;
}

void rc2d_graphics_getRendererInfo(void)
{
	SDL_RendererInfo rendererInfo;

	int infos = SDL_GetRendererInfo(rc2d_sdl_renderer, &rendererInfo);

	if (infos != 0)
	{
		RC2D_log(RC2D_LOG_ERROR, "SDL_GetRendererInfo failed rc2d_graphics_getRendererInfo : %s\n", SDL_GetError());
	}
}

void rc2d_graphics_getRenderDriverInfo(void)
{
	SDL_RendererInfo rendererInfo;

	int infos = SDL_GetRenderDriverInfo(0, &rendererInfo);

	if (infos != 0)
	{
		RC2D_log(RC2D_LOG_ERROR, "SDL_GetRenderDriverInfo failed rc2d_graphics_getRenderDriverInfo : %s\n", SDL_GetError());
	}
}

void rc2d_graphics_getPixel(RC2D_ImageData imageData, const RC2D_Point point, const RC2D_Color color)
{

}

void rc2d_graphics_setPixel(RC2D_ImageData imageData, const RC2D_Point point, const RC2D_Color color)
{

}

void rc2d_graphics_setBlendMode(const RC2D_BlendMode blendMode) 
{
    SDL_BlendMode sdlBlendMode;

    // Mapper les modes de mélange RC2D aux modes de mélange SDL
    switch (blendMode) 
	{
        case RC2D_BLEND_MODE_ALPHA:
            sdlBlendMode = SDL_BLENDMODE_BLEND;
            break;
        case RC2D_BLEND_MODE_ADD:
            sdlBlendMode = SDL_BLENDMODE_ADD;
            break;
        case RC2D_BLEND_MODE_MULTIPLY:
            sdlBlendMode = SDL_BLENDMODE_MOD;
            break;
		case RC2D_BLEND_MODE_NONE:
			sdlBlendMode = SDL_BLENDMODE_NONE;
			break;
        default:
            sdlBlendMode = SDL_BLENDMODE_NONE;
            break;
    }

    // Configurer le mode de mélange pour le rendu global
    SDL_SetRenderDrawBlendMode(rc2d_sdl_renderer, sdlBlendMode);
}



/*

	COORDINATE SYSTEM

*/
void rc2d_graphics_scale(float scaleX, float scaleY)
{
	SDL_RenderSetScale(rc2d_sdl_renderer, scaleX, scaleY);
}