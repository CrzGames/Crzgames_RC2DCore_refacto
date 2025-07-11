#include <RC2D/RC2D_math.h>
#include <RC2D/RC2D_logger.h>
#include <RC2D/RC2D_memory.h>

#include <SDL3/SDL_stdinc.h> // Require for : SDL_memcpy

#include <stdarg.h> // Require for : va_list, va_start, va_arg, va_end
#include <limits.h> // Require for : UINT_MAX
#include <stdlib.h> // Require for : RAND_MAX

#include <math.h>

/**
 * Calcule le produit vectoriel de trois points pour déterminer la convexité.
 */
static float crossProduct(const RC2D_Point p1, const RC2D_Point p2, const RC2D_Point p3) 
{
    float dx1 = p2.x - p1.x;
    float dy1 = p2.y - p1.y;
    float dx2 = p3.x - p2.x;
    float dy2 = p3.y - p2.y;
    return dx1 * dy2 - dy1 * dx2;
}

/**
 * Calcule le produit scalaire entre un vecteur et les coordonnées spécifiées.
 * 
 * @param {number[]} g - Un vecteur de taille 2.
 * @param {number} x - La coordonnée x.
 * @param {number} y - La coordonnée y.
 * @returns {number} Le produit scalaire entre le vecteur `g` et les coordonnées `(x, y)`.
 */
static double dot(int g[], double x, double y) 
{
    return g[0]*x + g[1]*y;
}

/**
 * Tableau de gradients pour le bruit de Perlin.
 * Chaque gradient est représenté par un vecteur 2D.
 * Le tableau contient 12 gradients pour couvrir toutes les directions possibles.
 */
static int grad3[][2] = {{1,1}, {-1,1}, {1,-1}, {-1,-1},
                 {1,0}, {-1,0}, {1,0}, {-1,0},
                 {0,1}, {0,-1}, {0,1}, {0,-1}};


/**
 * RANDOM GENERATOR
 * IMPLEMENTATION ALGORITHME : MERSENNE TWISTER
 * Wiki : https://fr.wikipedia.org/wiki/Mersenne_Twister
 * MT = Mersenne Twister
 * Functions for algorithme MT : rc2d_math_setSeed, rc2d_math_initializeMT, rc2d_math_extractNumber
 */

/**
 * Initialise la graine du générateur de nombres aléatoires.
 * @param rng Pointeur vers le générateur de nombres aléatoires.
 * @param seed La graine pour initialiser le générateur.
 */
static void rc2d_math_setSeed(RC2D_RandomGenerator *rng, uint32_t seed) 
{
    if (rng == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "Le générateur de nombres aléatoires est NULL dans rc2d_math_setSeed\n");
        return; // Le générateur de nombres aléatoires est NULL
    }

    rng->mt[0] = seed;

    for (rng->index = 1; rng->index < MT_N; rng->index++) 
    {
        rng->mt[rng->index] = (1812433253UL * (rng->mt[rng->index-1] ^ (rng->mt[rng->index-1] >> 30)) + rng->index);
    }
}

/**
 * Réinitialise l'état interne du générateur de nombres aléatoires basé sur l'état actuel.
 * @param rng Pointeur vers le générateur de nombres aléatoires.
 */
static void rc2d_math_initializeMT(RC2D_RandomGenerator *rng) 
{
    int i;
    uint32_t y;
    static const uint32_t mag[2] = {0x0, 0x9908b0dfUL};

    for (i = 0; i < MT_N-1; i++) 
    {
        y = (rng->mt[i] & 0x80000000UL) | (rng->mt[i+1] & 0x7fffffffUL);
        rng->mt[i] = rng->mt[(i + 397) % MT_N] ^ (y >> 1) ^ mag[y & 0x1UL];
    }

    y = (rng->mt[MT_N-1] & 0x80000000UL) | (rng->mt[0] & 0x7fffffffUL);
    rng->mt[MT_N-1] = rng->mt[396] ^ (y >> 1) ^ mag[y & 0x1UL];

    rng->index = 0;
}

/**
 * Extrait un nombre aléatoire de l'état interne du générateur.
 * @param rng Pointeur vers le générateur de nombres aléatoires.
 * @return Un nombre entier aléatoire.
 */
static uint32_t rc2d_math_extractNumber(RC2D_RandomGenerator *rng) 
{
    if (rng == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "Le générateur de nombres aléatoires est NULL dans rc2d_math_extractNumber\n");
        return 0; // Le générateur de nombres aléatoires est NULL
    }

    if (rng->index >= MT_N) 
    {
        rc2d_math_initializeMT(rng);
    }

    uint32_t y = rng->mt[rng->index++];
    y ^= y >> 11;
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= y >> 18;

    return y;
}

/**
 * Crée un nouveau générateur de nombres aléatoires avec une graine unique.
 * Cette fonction alloue de la mémoire pour un nouveau générateur de nombres aléatoires
 * et initialise son état interne avec la graine fournie.
 *
 * @param seed La graine pour initialiser le générateur.
 * @return Pointeur vers le nouveau générateur de nombres aléatoires.
 *         Retourne NULL si l'allocation de mémoire échoue.
 */
static RC2D_RandomGenerator* rc2d_math_newRandomGeneratorSingle(uint32_t seed) 
{
    RC2D_RandomGenerator* rng = (RC2D_RandomGenerator*) RC2D_malloc(sizeof(RC2D_RandomGenerator));

    if (rng) 
    {
        rc2d_math_setSeed(rng, seed);
        return rng;
    }

    return NULL;
}

/**
 * Calcule un point sur une courbe de Bézier en utilisant l'algorithme de De Casteljau.
 *
 * @param points Les points de contrôle de la courbe.
 * @param count Le nombre de points de contrôle.
 * @param t Le paramètre de la courbe, où 0 <= t <= 1.
 * @return Le point sur la courbe.
 */
static RC2D_Point deCasteljau(RC2D_Point* points, int count, double t) 
{
    if (points == NULL)
    {
        RC2D_log(RC2D_LOG_WARN, "Les points de contrôle de la courbe sont NULL dans deCasteljau\n");
        return (RC2D_Point){0, 0}; // Les points de contrôle de la courbe sont NULL
    }

    RC2D_Point* tempPoints = RC2D_malloc(sizeof(RC2D_Point) * count);
    SDL_memcpy(tempPoints, points, sizeof(RC2D_Point) * count);

    for (int r = 1; r < count; ++r) 
    {
        for (int i = 0; i < count - r; ++i) 
        {
            tempPoints[i].x = (1 - t) * tempPoints[i].x + t * tempPoints[i + 1].x;
            tempPoints[i].y = (1 - t) * tempPoints[i].y + t * tempPoints[i + 1].y;
        }
    }

    RC2D_Point result = tempPoints[0];
    RC2D_safe_free(tempPoints);

    return result;
}

/**
 * Subdivise récursivement une courbe de Bézier pour générer des points d'échantillonnage en utilisant l'algorithme de De Casteljau.
 * 
 * @param curve La courbe de Bézier.
 * @param depth La profondeur de la récursion.
 * @param t0 Le paramètre de début de la courbe.
 * @param t1 Le paramètre de fin de la courbe.
 * @param points La liste des points à remplir.
 * @param index L'index actuel dans la liste des points.
 */
static void subdivideBezier(RC2D_BezierCurve* curve, int depth, double t0, double t1, RC2D_Point* points, int* index) 
{
    if (curve == NULL || curve->points == NULL || points == NULL || index == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier ou la liste des points est NULL dans subdivideBezier\n");
        return; // La courbe de Bézier ou la liste des points est NULL
    }

    if (depth == 0) 
    {
        points[(*index)++] = deCasteljau(curve->points, curve->count, t0);
    } 
    else 
    {
        double tm = (t0 + t1) / 2.0;
        subdivideBezier(curve, depth - 1, t0, tm, points, index);
        subdivideBezier(curve, depth - 1, tm, t1, points, index);
    }
}

/**
 * Calcule le carré d'un nombre.
 *
 * @param a Le nombre à élever au carré.
 * @return Le carré de 'a'.
 */
static double sqr(double a)
{
	return a * a;
}

/**
 * Effectue une interpolation cubique entre deux valeurs.
 * Cette fonction utilise une interpolation cubique pour lisser la transition entre deux valeurs.
 */
static int perm[512] = {
    151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142, 
    8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,
    197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149, 
    56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48, 
    27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105, 
    92,41,55,46,245,40,244,102,143,54, 65,25,63,161, 1,216, 
    80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188, 
    159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123, 
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58, 
    17,182,189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 
    70,221,153,101,155,167, 43,172,9,129,22,39,253, 19,98,108, 
    110,79,113,224,232,178,185, 112,104,218,246,97,228,251,34,242, 
    193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239, 
    107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50, 
    45,127, 4,150,254,138,236,205,93,222,114,67,29,24,72,243, 
    141,128,195,78,66,215,61,156,180,
    // Répétition (pour des questions de performance et de mémoire) 
    151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142, 
    8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,
    197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149, 
    56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48, 
    27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105, 
    92,41,55,46,245,40,244,102,143,54, 65,25,63,161, 1,216, 
    80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188, 
    159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123, 
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58, 
    17,182,189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 
    70,221,153,101,155,167, 43,172,9,129,22,39,253, 19,98,108, 
    110,79,113,224,232,178,185, 112,104,218,246,97,228,251,34,242, 
    193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239, 
    107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50, 
    45,127, 4,150,254,138,236,205,93,222,114,67,29,24,72,243, 
    141,128,195,78,66,215,61,156,180
};

/**
 * Fonction de fondu utilisée dans l'interpolation de Perlin pour lisser les transitions.
 * 
 * @param {number} t - La valeur d'entrée à lisser.
 * @returns {number} La valeur lissée.
 */
static double fade(double t) 
{
    // Fonction de fondu de Perlin pour lisser l'interpolation
    return t * t * t * (t * (t * 6 - 15) + 10);
}

/**
 * Calcule le produit scalaire entre un gradient et une distance.
 * 
 * @param {number} ix - La composante entière du point de grille.
 * @param {number} x - La distance du point par rapport au début de la grille.
 * @returns {number} Le produit scalaire entre le gradient de la grille et la distance.
 */
static double dotGridGradient(int ix, double x) 
{
    int g = perm[ix & 255];
    return (x - ix) * g;
}

bool rc2d_math_isConvex(const RC2D_Polygon* polygon) 
{
    if (polygon == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "Le polygone est NULL dans rc2d_math_isConvex\n");
        return false; // Le polygone est NULL
    }

    if (polygon->numVertices < 3) 
    {
        RC2D_log(RC2D_LOG_WARN, "Un polygone ne peut pas avoir moins de 3 points dans rc2d_math_isConvex\n");
        return false; // Un polygone ne peut pas avoir moins de 3 points
    }

    bool gotInitialSign = false;
    float initialCrossProduct = 0;
    bool isConvex = true;

    for (int i = 0; i < polygon->numVertices; ++i) 
    {
        RC2D_Point current = polygon->vertices[i];
        RC2D_Point next = polygon->vertices[(i + 1) % polygon->numVertices];
        RC2D_Point nextnext = polygon->vertices[(i + 2) % polygon->numVertices];

        float cross = crossProduct(current, next, nextnext);

        if (!gotInitialSign) 
        {
            initialCrossProduct = cross;
            gotInitialSign = true;
        } 
        else 
        {
            if ((initialCrossProduct > 0) != (cross > 0)) 
            {
                isConvex = false;
                break;
            }
        }
    }

    return isConvex;
}

RC2D_RandomGenerator* rc2d_math_newRandomGeneratorWithSeed(uint32_t seed) 
{
   return rc2d_math_newRandomGeneratorSingle(seed);
}

RC2D_RandomGenerator* rc2d_math_newRandomGeneratorDouble(uint32_t seed_low, uint32_t seed_high) 
{
    uint64_t seed = ((uint64_t)seed_high << 32) | seed_low;
    return rc2d_math_newRandomGeneratorSingle((uint32_t)seed); // Cast nécessaire si on veut utiliser une seule fonction d'initialisation
}

RC2D_RandomGenerator* rc2d_math_newRandomGeneratorDefault(void) 
{
    /*
     La valeur de départ par défaut utilisée est la paire bas/haut suivante : 
     0xCBBF7A44, 0x0139408D
    */
    return rc2d_math_newRandomGeneratorDouble(0xCBBF7A44, 0x0139408D);
}

void rc2d_math_freeRandomGenerator(RC2D_RandomGenerator* randomGenerator) 
{
    RC2D_safe_free(randomGenerator);
}

double rc2d_math_randomReal(RC2D_RandomGenerator *rng) 
{
    if (rng == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "Le générateur de nombres aléatoires est NULL dans rc2d_math_randomReal\n");
        return 0.0; // Le générateur de nombres aléatoires est NULL
    }

    return (double)rc2d_math_extractNumber(rng) / ((double)UINT32_MAX + 1.0);
}

uint32_t rc2d_math_randomInt(RC2D_RandomGenerator *rng, uint32_t max) 
{
    if (rng == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "Le générateur de nombres aléatoires est NULL dans rc2d_math_randomInt\n");
        return 0; // Le générateur de nombres aléatoires est NULL
    }

    // Assurez-vous que max est positif
    if (max == 0) return 0;

    // Génère un nombre réel dans [0, 1) et ajuste pour l'intervalle [1, max]
    double scaled = rc2d_math_randomReal(rng) * max; 

    return (uint32_t)(scaled + 1);
}

uint32_t rc2d_math_randomIntRange(RC2D_RandomGenerator *rng, uint32_t min, uint32_t max) 
{
    if (rng == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "Le générateur de nombres aléatoires est NULL dans rc2d_math_randomIntRange\n");
        return 0; // Le générateur de nombres aléatoires est NULL
    }

    // Vérifier que min est inférieur ou égal à max
    if (min > max) 
    {
        uint32_t temp = min;
        min = max;
        max = temp;
    }

    // Calcul de l'intervalle
    uint32_t range = max - min + 1;
    double scaled = rc2d_math_randomReal(rng) * range; 

    return min + (uint32_t)scaled;
}

void rc2d_math_setRandomSeedSingle(RC2D_RandomGenerator *rng, uint64_t seed) 
{
    if (rng == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "Le générateur de nombres aléatoires est NULL dans rc2d_math_setRandomSeedSingle\n");
        return; // Le générateur de nombres aléatoires est NULL
    }

    // Assurez-vous que la graine ne dépasse pas 2^53 - 1
    seed &= 0x1FFFFFFFFFFFFFUL;

    // Appelle la fonction existante pour définir la nouvelle graine du générateur
    rc2d_math_setSeed(rng, seed);
}

void rc2d_math_setRandomSeedDouble(RC2D_RandomGenerator *rng, uint32_t low, uint32_t high) 
{
    if (rng == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "Le générateur de nombres aléatoires est NULL dans rc2d_math_setRandomSeedDouble\n");
        return; // Le générateur de nombres aléatoires est NULL
    }

    // Aucune vérification supplémentaire n'est nécessaire pour 'low' et 'high' ici
    // car ils sont déjà de type uint32_t, ce qui garantit qu'ils sont dans l'intervalle [0, 2^32 - 1]

    // Combine les valeurs 'low' et 'high' pour former une graine de 64 bits
    uint64_t seed = ((uint64_t)high << 32) | low;

    // Appelle la fonction existante pour définir la nouvelle graine du générateur
    rc2d_math_setSeed(rng, seed);
}

void rc2d_math_colorFromBytes(int rb, int gb, int bb, int *ab, double *r, double *g, double *b, double *a) 
{
    if (r == NULL || g == NULL || b == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "Les pointeurs de stockage des composantes de couleur sont NULL dans rc2d_math_colorFromBytes\n");
        return; // Les pointeurs de stockage des composantes de couleur sont NULL
    }

    *r = rb / 255.0;
    *g = gb / 255.0;
    *b = bb / 255.0;

    if (ab != NULL && a != NULL) 
    {
        *a = *ab / 255.0;
    }
}

void rc2d_math_colorToBytes(double r, double g, double b, double *a, int *rb, int *gb, int *bb, int *ab) 
{
    if (rb == NULL || gb == NULL || bb == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "Les pointeurs de stockage des composantes de couleur sont NULL dans rc2d_math_colorToBytes\n");
        return; // Les pointeurs de stockage des composantes de couleur sont NULL
    }

    *rb = (int)(r * 255.0);
    *gb = (int)(g * 255.0);
    *bb = (int)(b * 255.0);

    if (a != NULL && ab != NULL) 
    {
        *ab = (int)(*a * 255.0);
    }
}

double rc2d_math_gammaToLinear(double c) 
{
    if (c <= 0.04045) 
    {
        return c / 12.92;
    } 
    else 
    {
        return SDL_pow((c + 0.055) / 1.055, 2.4);
    }
}

void rc2d_math_gammaToLinearRGB(double r, double g, double b, double *lr, double *lg, double *lb) 
{
    if (lr == NULL || lg == NULL || lb == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "Les pointeurs de stockage des composantes de couleur sont NULL dans rc2d_math_gammaToLinearRGB\n");
        return; // Les pointeurs de stockage des composantes de couleur sont NULL
    }

    *lr = rc2d_math_gammaToLinear(r);
    *lg = rc2d_math_gammaToLinear(g);
    *lb = rc2d_math_gammaToLinear(b);
}

double rc2d_math_linearToGamma(double c) 
{
    if (c <= 0.0031308) 
    {
        return 12.92 * c;
    } 
    else 
    {
        return 1.055 * SDL_pow(c, 1.0 / 2.4) - 0.055;
    }
}

void rc2d_math_linearToGammaRGB(double lr, double lg, double lb, double *cr, double *cg, double *cb) 
{
    if (cr == NULL || cg == NULL || cb == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "Les pointeurs de stockage des composantes de couleur sont NULL dans rc2d_math_linearToGammaRGB\n");
        return; // Les pointeurs de stockage des composantes de couleur sont NULL
    }
    
    *cr = rc2d_math_linearToGamma(lr);
    *cg = rc2d_math_linearToGamma(lg);
    *cb = rc2d_math_linearToGamma(lb);
}

RC2D_BezierCurve* rc2d_math_newBezierCurve(int count, const RC2D_Point* points) 
{
	if (points == NULL || count <= 0)
	{
		RC2D_log(RC2D_LOG_ERROR, "rc2d_math_new_BezierCurve: points est NULL ou count <= 0.");
		return NULL;
	}

    // Allouer de la mémoire pour la courbe de Bézier
	RC2D_BezierCurve* curve = RC2D_malloc(sizeof(RC2D_BezierCurve));
    // Allouer de la mémoire pour les points de la courbe
	curve->points = RC2D_malloc(sizeof(RC2D_Point) * count);
    // Initialiser le compteur de points
	curve->count = count;

    // Boucler à travers les points de contrôle et les copier dans la courbe
	for (int i = 0; i < count; ++i) 
	{
		curve->points[i] = points[i];
	}

    // Renvoie le pointeur vers la nouvelle courbe de Bézier
	return curve;
}

void rc2d_math_freeBezierCurve(RC2D_BezierCurve* curve) 
{
    // Libérer le tableau de points
    RC2D_safe_free(curve->points);

    // Libérer la courbe elle-même
    RC2D_safe_free(curve);
}

void rc2d_math_translateBezierCurve(RC2D_BezierCurve* curve, double dx, double dy) 
{
    if (curve == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier est NULL dans rc2d_math_translate_BezierCurve\n");
        return; // La courbe de Bézier est NULL
    }

    for (int i = 0; i < curve->count; i++) 
    {
        curve->points[i].x += dx;
        curve->points[i].y += dy;
    }
}

void rc2d_math_setControlPointBezierCurve(RC2D_BezierCurve* curve, int i, double x, double y) 
{
    if (curve == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier est NULL dans rc2d_math_setControlPoint_BezierCurve\n");
        return; // La courbe de Bézier est NULL
    }

    if (i >= 0 && i < curve->count) 
    {
        // Définition des coordonnées du point de contrôle
        curve->points[i].x = x;
        curve->points[i].y = y;
    }
}

void rc2d_math_scaleBezierCurve(RC2D_BezierCurve* curve, double scale, double ox, double oy) 
{
    if (curve == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier est NULL dans rc2d_math_scale_BezierCurve\n");
        return; // La courbe de Bézier est NULL
    }

    for (int i = 0; i < curve->count; i++) 
    {
        // Calcul de la différence par rapport au point d'origine
        double dx = curve->points[i].x - ox;
        double dy = curve->points[i].y - oy;

        // Mise à l'échelle des coordonnées par le facteur spécifié
        curve->points[i].x = ox + dx * scale;
        curve->points[i].y = oy + dy * scale;
    }
}

void rc2d_math_rotateBezierCurve(RC2D_BezierCurve* curve, double angle, double ox, double oy) 
{
    if (curve == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier est NULL dans rc2d_math_rotate_BezierCurve\n");
        return; // La courbe de Bézier est NULL
    }

    double cos_angle = cos(angle);
    double sin_angle = sin(angle);
    
    for (int i = 0; i < curve->count; i++) 
    {
        // Calcul de la différence par rapport au point d'origine
        double dx = curve->points[i].x - ox;
        double dy = curve->points[i].y - oy;

        // Application de la rotation
        curve->points[i].x = ox + dx * cos_angle - dy * sin_angle;
        curve->points[i].y = oy + dx * sin_angle + dy * cos_angle;
    }
}

void rc2d_math_removeControlPointBezierCurve(RC2D_BezierCurve* curve, int index) 
{
    if (curve == NULL || curve->points == NULL || curve->count <= 1) 
    {
        // Ne rien faire si la courbe est invalide ou s'il ne reste qu'un seul point de contrôle
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier ou les points de contrôle sont NULL ou il ne reste qu'un seul point de contrôle dans rc2d_math_removeControlPoint_BezierCurve\n");
        return;
    }
    
    if (index < 0 || index >= curve->count) 
    {
        // Indice invalide
        RC2D_log(RC2D_LOG_WARN, "L'indice du point de contrôle est invalide dans rc2d_math_removeControlPoint_BezierCurve\n");
        return;
    }

    // Créer un nouveau tableau de points de contrôle avec une taille réduite
    RC2D_Point* newPoints = RC2D_malloc(sizeof(RC2D_Point) * (curve->count - 1));
    
    // Copier tous les points de contrôle sauf celui à supprimer
    for (int i = 0, j = 0; i < curve->count; i++) 
    {
        if (i != index) 
        {
            newPoints[j++] = curve->points[i];
        }
    }
    
    // Libérer l'ancien tableau de points de contrôle et mettre à jour la courbe
    RC2D_safe_free(curve->points);

    curve->points = newPoints;
    curve->count -= 1;
}

void rc2d_math_insertControlPointBezierCurve(RC2D_BezierCurve* curve, double x, double y, int i) 
{
    if (curve == NULL || curve->points == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier ou les points de contrôle sont NULL dans rc2d_math_insertControlPoint_BezierCurve\n");
        return; // Vérification de la validité de la courbe
    }
    
    // Ajustement des indices négatifs
    if (i < 0) 
    {
        i = curve->count + i + 1; // Convertir l'indice négatif en positif
    } 
    else if (i == 0) 
    {
        i = 1; // Assurer que l'indice commence à 1
    }
    
    // Limiter l'indice pour qu'il soit dans la plage valide
    if (i > curve->count) 
    {
        i = curve->count + 1;
    }
    
    // Ajuster l'indice pour la base 0
    i -= 1;
    
    // Créer un nouveau tableau de points de contrôle avec une taille augmentée
    RC2D_Point* newPoints = RC2D_malloc(sizeof(RC2D_Point) * (curve->count + 1));
    
    // Copier les points existants en insérant le nouveau point à l'indice spécifié
    for (int j = 0, k = 0; j < curve->count + 1; j++) 
    {
        if (j == i) 
        {
            newPoints[j] = (RC2D_Point){x, y}; // Insérer le nouveau point
        } 
        else 
        {
            newPoints[j] = curve->points[k++]; // Copier un point existant
        }
    }
    
    // Libérer l'ancien tableau de points et mettre à jour la courbe
    RC2D_safe_free(curve->points);

    curve->points = newPoints;
    curve->count += 1; // Augmenter le nombre de points de contrôle
}

RC2D_BezierCurve* rc2d_math_subdivideBezierCurve(RC2D_BezierCurve* curve, double startpoint, double endpoint) 
{
	if (curve == NULL || curve->points == NULL || startpoint < 0 || startpoint > 1 || endpoint <= startpoint || endpoint > 1) 
	{
		RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier ou les points de contrôle sont NULL ou les points de début/fin sont invalides dans rc2d_math_getSegment_BezierCurve\n");
		return NULL;
	}

	int count = curve->count;

	// Allocation de mémoire temporaire pour les subdivisions
	RC2D_Point* newPointsStart = RC2D_malloc(sizeof(RC2D_Point) * count);
	RC2D_Point* newPointsEnd = RC2D_malloc(sizeof(RC2D_Point) * count);
	SDL_memcpy(newPointsStart, curve->points, sizeof(RC2D_Point) * count);
	SDL_memcpy(newPointsEnd, curve->points, sizeof(RC2D_Point) * count);

	// Subdivision à startpoint
	for (int i = 1; i < count; i++) 
	{
		for (int j = 0; j < count - i; j++) 
		{
			newPointsStart[j].x = (1 - startpoint) * newPointsStart[j].x + startpoint * newPointsStart[j + 1].x;
			newPointsStart[j].y = (1 - startpoint) * newPointsStart[j].y + startpoint * newPointsStart[j + 1].y;
		}
	}

	// Subdivision à endpoint ajusté
	double adjustedEndpoint = (endpoint - startpoint) / (1 - startpoint);
	for (int i = 1; i < count; i++) 
	{
		for (int j = 0; j < count - i; j++) 
		{
			newPointsEnd[j].x = (1 - adjustedEndpoint) * newPointsEnd[j].x + adjustedEndpoint * newPointsEnd[j + 1].x;
			newPointsEnd[j].y = (1 - adjustedEndpoint) * newPointsEnd[j].y + adjustedEndpoint * newPointsEnd[j + 1].y;
		}
	}

	// Création de la nouvelle courbe avec les points du segment
	RC2D_BezierCurve* segmentCurve = rc2d_math_newBezierCurve(count, newPointsStart);

	RC2D_safe_free(newPointsStart);
	RC2D_safe_free(newPointsEnd);

	return segmentCurve;
}

/**
 * Génère une liste de coordonnées pour être utilisée avec une fonction de dessin.
 * 
 * @param curve La courbe de Bézier.
 * @param depth La profondeur de récursion.
 * @param numPoints Un pointeur vers une variable où stocker le nombre de points générés.
 * @return La liste des coordonnées des points sur la courbe.
 */
RC2D_Point* rc2d_math_renderBezierCurve(RC2D_BezierCurve* curve, int depth, int* numPoints) 
{
    if (curve == NULL || curve->points == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier est NULL dans rc2d_math_render_BezierCurve\n");
        return NULL; // La courbe doit avoir au moins deux points de contrôle pour être rendue.
    }

    int countPoints = SDL_pow(2, depth) + 1; // Calcul du nombre de points basé sur la profondeur de récursion
    RC2D_Point* points = RC2D_malloc(sizeof(RC2D_Point) * countPoints);
    *numPoints = countPoints; // Stocker le nombre de points dans la variable de sortie

    int index = 0;
    subdivideBezier(curve, depth, 0.0, 1.0, points, &index);

    return points;
}

RC2D_Point* rc2d_math_renderSegmentBezierCurve(RC2D_BezierCurve* curve, double startpoint, double endpoint, int depth, int* numPoints) 
{
    if (startpoint < 0 || startpoint > 1 || endpoint <= startpoint || endpoint > 1) 
    {
        // Gestion d'erreurs pour des points de début ou de fin invalides
        RC2D_log(RC2D_LOG_WARN, "Les points de début/fin sont invalides dans rc2d_math_renderSegment_BezierCurve\n");
        return NULL;
    }

    int countPoints = SDL_pow(2, depth) + 1; // Calcul du nombre de points basé sur la profondeur de récursion
    RC2D_Point* points = RC2D_malloc(sizeof(RC2D_Point) * countPoints);
    *numPoints = countPoints; // Stocker le nombre de points dans la variable de sortie

    int index = 0;
    double tStep = (endpoint - startpoint) / (countPoints - 1);
    for (int i = 0; i < countPoints; i++) 
    {
        double t = startpoint + tStep * i;
        points[index++] = deCasteljau(curve->points, curve->count, t);
    }

    return points;
}

/**
 * Obtient la dérivée d'une courbe de Bézier.
 *
 * @param curve Un pointeur vers la courbe de Bézier originale.
 * @return Un pointeur vers la courbe de Bézier dérivée.
 */
RC2D_BezierCurve* rc2d_math_getDerivativeBezierCurve(RC2D_BezierCurve* curve) {
    if (curve == NULL || curve->points == NULL || curve->count < 2) 
    {
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier est NULL ou a moins de 2 points de contrôle dans rc2d_math_getDerivative_BezierCurve\n");
        return NULL; // La courbe doit avoir au moins deux points de contrôle pour avoir une dérivée.
    }

    int degree = curve->count - 1; // Le degré de la courbe originale
    RC2D_BezierCurve* derivativeCurve = RC2D_malloc(sizeof(RC2D_BezierCurve));

    derivativeCurve->count = degree; // La courbe dérivée aura un degré de moins.
    derivativeCurve->points = RC2D_malloc(sizeof(RC2D_Point) * derivativeCurve->count);

    for (int i = 0; i < degree; i++) 
    {
        // Calcul des points de contrôle de la courbe dérivée
        derivativeCurve->points[i].x = degree * (curve->points[i+1].x - curve->points[i].x);
        derivativeCurve->points[i].y = degree * (curve->points[i+1].y - curve->points[i].y);
    }

    return derivativeCurve;
}

/**
 * Obtient le degré d'une courbe de Bézier.
 *
 * @param curve Un pointeur vers la courbe de Bézier.
 * @return Le degré de la courbe de Bézier.
 */
int rc2d_math_getDegreeBezierCurve(RC2D_BezierCurve* curve) 
{
    if (curve == NULL || curve->points == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier est NULL dans rc2d_math_getDegree_BezierCurve\n");
        return -1; // Retourne -1 pour indiquer une erreur si la courbe est invalide.
    }

    return curve->count - 1;
}

/**
 * Obtient le nombre de points de contrôle dans une courbe de Bézier.
 *
 * @param curve Un pointeur vers la courbe de Bézier.
 * @return Le nombre de points de contrôle de la courbe de Bézier.
 */
int rc2d_math_getControlPointCountBezierCurve(RC2D_BezierCurve* curve) 
{
    if (curve == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier est NULL dans rc2d_math_getControlPointCount_BezierCurve\n");
        return 0; // Retourne 0 pour indiquer qu'il n'y a aucun point de contrôle si la courbe est invalide.
    }

    return curve->count;
}

/**
 * Obtient les coordonnées du i-ème point de contrôle d'une courbe de Bézier.
 *
 * @param curve Un pointeur vers la courbe de Bézier.
 * @param i L'indice du point de contrôle (commence à 1).
 * @param x Un pointeur vers une variable où stocker la position du point de contrôle sur l'axe des x.
 * @param y Un pointeur vers une variable où stocker la position du point de contrôle sur l'axe des y.
 * @return 0 si le point a été récupéré avec succès, -1 sinon.
 */
int rc2d_math_getControlPointBezierCurve(RC2D_BezierCurve* curve, int i, double* x, double* y) 
{
    if (curve == NULL || i < 1 || i > curve->count) 
    {
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier est NULL ou l'indice est hors limites dans rc2d_math_getControlPoint_BezierCurve\n");
        return -1; // Retourne 0 si la courbe est invalide ou si l'indice est hors limites.
    }

    // Ajustement de l'indice pour la base 0
    i -= 1;

    // Récupération des coordonnées
    if (x != NULL) *x = curve->points[i].x;
    if (y != NULL) *y = curve->points[i].y;

    return 0; // Succès
}

/**
 * Évalue une courbe de Bézier à un paramètre t donné.
 * 
 * @param curve Un pointeur vers la courbe de Bézier.
 * @param t Le paramètre où évaluer la courbe, doit être compris entre 0 et 1.
 * @param x Un pointeur vers une variable où stocker la coordonnée x du point évalué.
 * @param y Un pointeur vers une variable où stocker la coordonnée y du point évalué.
 * @return 0 si l'évaluation a réussi, -1 sinon.
 */
int rc2d_math_evaluateBezierCurve(RC2D_BezierCurve* curve, double t, double* x, double* y) 
{
    if (curve == NULL || curve->points == NULL || t < 0 || t > 1 || curve->count < 2) 
    {
        // InValidation des entrées
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier est NULL ou le paramètre t est invalide dans rc2d_math_evaluate_BezierCurve\n");
        return -1;
    }

    // Appliquer l'algorithme de De Casteljau pour calculer le point sur la courbe
    RC2D_Point point = deCasteljau(curve->points, curve->count, t);

    // Stocker les résultats
    if (x != NULL) *x = point.x;
    if (y != NULL) *y = point.y;

    return 0;
}

double rc2d_math_dist(double x1, double y1, double x2, double y2)
{
	return SDL_sqrt(sqr(y2 - y1) + sqr(x2 - x1));
}

double rc2d_math_angle(double x1, double y1, double x2, double y2)
{
	return SDL_atan2(y2 - y1, x2 - x1) * (180 / SDL_PI_D); // return angle en degres
}

double rc2d_math_cosinus(double x)
{
	return cos(x);
}

double rc2d_math_sinus(double y)
{
	return sin(y);
}

int rc2d_math_normalize(int x, int y)
{
	int l = (x * x + y * y) ^ (int)0.5;

	if (l == 0)
	{
		return 0, 0, 0;
	}
	else
	{
		return x / l, y / l, l;
	}
}

int rc2d_math_random(int min, int max)
{
    int range = max - min + 1;
    int random = SDL_rand(range);
    return min + random;
}

double rc2d_math_random0and1(void)
{
    const Sint32 precision = 1 << 24; // 2^24
    return (double)SDL_rand(precision) / (double)precision;
}

double rc2d_math_lerp(double startValue, double endValue, double progress)
{
    return (1 - progress) * startValue + progress * endValue;
}

double rc2d_math_lerp2(double startValue, double endValue, double progress)
{
    return startValue + (endValue - startValue) * progress;
}

double rc2d_math_cerp(double startValue, double endValue, double progress)
{
	double f = (1 - sin(progress * SDL_PI_D) * 0.5);

	return startValue * (1 - f) + endValue * f;
}

double rc2d_math_noise_1d(double x) 
{
    int i0 = SDL_floor(x);
    int i1 = i0 + 1;

    double x0 = x - i0;
    double x1 = x0 - 1.0;

    double n0, n1;

    // Appliquer la fonction de fondu à x0
    double fade_x0 = fade(x0);

    double t0 = 1.0 - x0 * x0;
    t0 *= t0;
    n0 = t0 * t0 * dotGridGradient(i0, x0);

    double t1 = 1.0 - x1 * x1;
    t1 *= t1;
    n1 = t1 * t1 * dotGridGradient(i1, x1);

    // Interpoler les contributions avec la valeur fondu
    double value = (1.0 - fade_x0) * n0 + fade_x0 * n1;

    // Normaliser le résultat pour le rendre dans l'intervalle [0, 1]
    return value * 0.5 + 0.5;
}

double rc2d_math_noise_2d(double x, double y) 
{
	// Contributions des trois coins
    double n0, n1, n2;

    // Constantes de déformation pour 2D
    double F2 = 0.5 * (SDL_sqrt(3.0) - 1.0);
    double G2 = (3.0 - SDL_sqrt(3.0)) / 6.0;

	// Déformation des cellules (x, y)
    double s = (x + y) * F2;
    int i = SDL_floor(x + s);
    int j = SDL_floor(y + s);
    double t = (i + j) * G2;

    double X0 = i - t; // Décalage non déformé de x, y vers x-y
    double Y0 = j - t;

    double x0 = x - X0; // Les distances x, y de la cellule sont x,y
    double y0 = y - Y0;

    // Pour le 2D simplex, les contributions de chaque coin sont calculées
    int i1, j1; // Offsets pour le second coin de simplex en (i,j) coords
    if(x0 > y0) {i1=1; j1=0;} // bas triangle, XY ordre: (0,0)->(1,0)->(1,1)
    else {i1=0; j1=1;}      // haut triangle, YX ordre: (0,0)->(0,1)->(1,1)

    // Un pas plus loin dans les simplex (i,j) coordonnées
    double x1 = x0 - i1 + G2; // Offsets pour le milieu du coin
    double y1 = y0 - j1 + G2;
    double x2 = x0 - 1.0 + 2.0 * G2; // Offsets pour le dernier coin
    double y2 = y0 - 1.0 + 2.0 * G2;

    // Calcul des indices de gradient à partir de la table de permutation
    int ii = i & 255;
    int jj = j & 255;
    int gi0 = perm[ii+perm[jj]] % 12;
    int gi1 = perm[ii+i1+perm[jj+j1]] % 12;
    int gi2 = perm[ii+1+perm[jj+1]] % 12;

    // Calcul des contributions des trois coins
    double t0 = 0.5 - x0*x0 - y0*y0;
    if(t0<0) n0 = 0.0;
    else {
        t0 *= t0;
        n0 = t0 * t0 * dot(grad3[gi0], x0, y0);  // (x,y) du coin 0
    }

    double t1 = 0.5 - x1*x1 - y1*y1;
    if(t1<0) n1 = 0.0;
    else {
        t1 *= t1;
        n1 = t1 * t1 * dot(grad3[gi1], x1, y1);  // (x,y) du coin 1
    }

    double t2 = 0.5 - x2*x2 - y2*y2;
    if(t2<0) n2 = 0.0;
    else {
        t2 *= t2;
        n2 = t2 * t2 * dot(grad3[gi2], x2, y2);  // (x,y) du coin 2
    }

    // Ajout des contributions de chaque coin pour obtenir le résultat final.
    double result = 70.0 * (n0 + n1 + n2);

    // Normaliser le résultat pour le rendre dans l'intervalle [0, 1]
    return (result + 1.0) / 2.0;
}