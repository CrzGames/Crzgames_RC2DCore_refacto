#include <RC2D/RC2D_math.h>
#include <RC2D/RC2D_logger.h>

#include <SDL3/SDL_stdinc.h> // Require for : SDL_malloc, SDL_free, SDL_memcpy

#include <stdarg.h> // Require for : va_list, va_start, va_arg, va_end
#include <limits.h> // Require for : UINT_MAX

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
 * Vérifie si un polygone est convexe.
 * 
 * @param polygon Le polygone à vérifier.
 * @return true si le polygone est convexe, false sinon.
 */
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
    RC2D_RandomGenerator* rng = (RC2D_RandomGenerator*) SDL_malloc(sizeof(RC2D_RandomGenerator));

    if (rng) 
    {
        rc2d_math_setSeed(rng, seed);
        return rng;
    }

    return NULL;
}

/**
 * Crée un nouveau générateur de nombres aléatoires avec une graine spécifique.
 * @param seed La graine pour initialiser le générateur.
 * @return Pointeur vers le nouveau générateur de nombres aléatoires.
 */
RC2D_RandomGenerator* rc2d_math_newRandomGeneratorWithSeed(uint32_t seed) 
{
   return rc2d_math_newRandomGeneratorSingle(seed);
}

/**
 * Crée un nouveau générateur de nombres aléatoires avec une graine composée de deux entiers.
 * @param seed_low Les 32 bits inférieurs de la graine.
 * @param seed_high Les 32 bits supérieurs de la graine.
 * @return Pointeur vers le nouveau générateur de nombres aléatoires.
 */
RC2D_RandomGenerator* rc2d_math_newRandomGeneratorDouble(uint32_t seed_low, uint32_t seed_high) 
{
    uint64_t seed = ((uint64_t)seed_high << 32) | seed_low;
    return rc2d_math_newRandomGeneratorSingle((uint32_t)seed); // Cast nécessaire si on veut utiliser une seule fonction d'initialisation
}

/**
 * Crée un nouveau générateur de nombres aléatoires avec une graine par défaut.
 * @return Pointeur vers le nouveau générateur de nombres aléatoires.
 */
RC2D_RandomGenerator* rc2d_math_newRandomGeneratorDefault(void) 
{
    /*
     La valeur de départ par défaut utilisée est la paire bas/haut suivante : 
     0xCBBF7A44, 0x0139408D
    */
    return rc2d_math_newRandomGeneratorDouble(0xCBBF7A44, 0x0139408D);
}

/**
 * Libère la mémoire allouée pour un générateur de nombres aléatoires.
 * @param randomGenerator Pointeur vers le générateur de nombres aléatoires à libérer.
 */
void rc2d_math_freeRandomGenerator(RC2D_RandomGenerator* randomGenerator) 
{
    if (randomGenerator != NULL) 
    {
        SDL_free(randomGenerator); // Libérer la mémoire allouée pour le randomGenerator
        randomGenerator = NULL; // Eviter les pointeurs fantomes
    }
}


/**
 * Génère un nombre réel aléatoire dans l'intervalle [0, 1).
 * @param rng Pointeur vers le générateur de nombres aléatoires.
 * @return Un nombre réel aléatoire.
 */
double rc2d_math_randomReal(RC2D_RandomGenerator *rng) 
{
    if (rng == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "Le générateur de nombres aléatoires est NULL dans rc2d_math_randomReal\n");
        return 0.0; // Le générateur de nombres aléatoires est NULL
    }

    return (double)rc2d_math_extractNumber(rng) / ((double)UINT32_MAX + 1.0);
}

/**
 * Génère un nombre entier aléatoire dans l'intervalle [1, max].
 * @param rng Pointeur vers le générateur de nombres aléatoires.
 * @param max La valeur maximale du nombre aléatoire (inclusive).
 * @return Un nombre entier aléatoire.
 */
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

/**
 * Génère un nombre entier aléatoire dans l'intervalle spécifié [min, max].
 * @param rng Pointeur vers le générateur de nombres aléatoires.
 * @param min La valeur minimale du nombre aléatoire (inclusive).
 * @param max La valeur maximale du nombre aléatoire (inclusive).
 * @return Un nombre entier aléatoire.
 */
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

/**
 * Définit la nouvelle graine du générateur de nombres aléatoires en utilisant un seul nombre entier.
 * @param rng Pointeur vers le générateur de nombres aléatoires.
 * @param seed La graine pour initialiser le générateur.
 */
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

/**
 * Définit la nouvelle graine du générateur de nombres aléatoires en combinant deux nombres entiers de 32 bits.
 * @param rng Pointeur vers le générateur de nombres aléatoires.
 * @param low Les 32 bits inférieurs de la graine.
 * @param high Les 32 bits supérieurs de la graine.
 */
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


/**
 * Convertit une couleur de l'intervalle 0..255 à l'intervalle 0..1.
 * 
 * @param rb Composante rouge de la couleur dans l'intervalle 0..255.
 * @param gb Composante verte de la couleur dans l'intervalle 0..255.
 * @param bb Composante bleue de la couleur dans l'intervalle 0..255.
 * @param ab Composante alpha de la couleur dans l'intervalle 0..255; peut être NULL si non spécifié.
 * @param r Pointeur vers le stockage du résultat de la composante rouge dans l'intervalle 0..1.
 * @param g Pointeur vers le stockage du résultat de la composante verte dans l'intervalle 0..1.
 * @param b Pointeur vers le stockage du résultat de la composante bleue dans l'intervalle 0..1.
 * @param a Pointeur vers le stockage du résultat de la composante alpha dans l'intervalle 0..1; ignoré si ab est NULL.
 */
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

/**
 * Convertit une couleur de l'intervalle 0..1 à l'intervalle 0..255.
 * 
 * @param r Composante rouge de la couleur dans l'intervalle 0..1.
 * @param g Composante verte de la couleur dans l'intervalle 0..1.
 * @param b Composante bleue de la couleur dans l'intervalle 0..1.
 * @param a Composante alpha de la couleur dans l'intervalle 0..1; peut être NULL si non spécifié.
 * @param rb Pointeur vers le stockage du résultat de la composante rouge dans l'intervalle 0..255.
 * @param gb Pointeur vers le stockage du résultat de la composante verte dans l'intervalle 0..255.
 * @param bb Pointeur vers le stockage du résultat de la composante bleue dans l'intervalle 0..255.
 * @param ab Pointeur vers le stockage du résultat de la composante alpha dans l'intervalle 0..255; ignoré si a est NULL.
 */
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

/**
 * Convertit une composante de couleur de l'espace gamma (sRVB) en espace linéaire (RVB).
 * 
 * @param c La valeur d'un canal de couleur dans l'espace sRVB à convertir.
 * @return La valeur du canal de couleur dans l'espace RVB linéaire.
 */
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

/**
 * Applique la conversion de gamma à linéaire pour les composantes rouge, verte et bleue d'une couleur.
 * 
 * @param r Le canal rouge de la couleur sRGB à convertir.
 * @param g Le canal vert de la couleur sRGB à convertir.
 * @param b Le canal bleu de la couleur sRGB à convertir.
 * @param lr Pointeur vers le stockage du canal rouge de la couleur convertie dans l'espace RVB linéaire.
 * @param lg Pointeur vers le stockage du canal vert de la couleur convertie dans l'espace RVB linéaire.
 * @param lb Pointeur vers le stockage du canal bleu de la couleur convertie dans l'espace RVB linéaire.
 */
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

/**
 * Convertit une composante de couleur de l'espace linéaire (RVB) en espace gamma (sRVB).
 * 
 * @param c La valeur d'un canal de couleur dans l'espace RVB linéaire à convertir.
 * @return La valeur du canal de couleur dans l'espace sRVB gamma.
 */
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

/**
 * Applique la conversion de linéaire à gamma pour les composantes rouge, verte et bleue d'une couleur.
 * 
 * @param lr Le canal rouge de la couleur RVB linéaire à convertir.
 * @param lg Le canal vert de la couleur RVB linéaire à convertir.
 * @param lb Le canal bleu de la couleur RVB linéaire à convertir.
 * @param cr Pointeur vers le stockage du canal rouge de la couleur convertie dans l'espace sRVB gamma.
 * @param cg Pointeur vers le stockage du canal vert de la couleur convertie dans l'espace sRVB gamma.
 * @param cb Pointeur vers le stockage du canal bleu de la couleur convertie dans l'espace sRVB gamma.
 */
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

/**
 * Crée une nouvelle courbe de Bézier.
 *
 * Cette fonction alloue et initialise une nouvelle courbe de Bézier avec un nombre
 * spécifié de points de contrôle.
 *
 * @param count Le nombre de points de contrôle de la courbe.
 * @param ... Les coordonnées des points de contrôle (double x, double y), répétées 'count' fois.
 * @return Un pointeur vers la nouvelle courbe de Bézier allouée.
 */
RC2D_BezierCurve* rc2d_math_new_BezierCurve(int count, ...) 
{
    va_list args;
    va_start(args, count);

    RC2D_BezierCurve* curve = SDL_malloc(sizeof(RC2D_BezierCurve));
    curve->points = SDL_malloc(sizeof(RC2D_Point) * count);
    curve->count = count;

    for (int i = 0; i < count; i++) 
    {
        double x = va_arg(args, double);
        double y = va_arg(args, double);
        curve->points[i] = (RC2D_Point){x, y};
    }

    va_end(args);

    return curve;
}

/**
 * Libère la mémoire occupée par une courbe de Bézier.
 *
 * @param curve Un pointeur vers la courbe de Bézier à libérer.
 */
void rc2d_math_free_BezierCurve(RC2D_BezierCurve* curve) 
{
    if (curve != NULL) 
    {
        if (curve->points != NULL) 
        {
            SDL_free(curve->points); // Libérer le tableau de points
        }

        SDL_free(curve);            // Libérer la courbe elle-même
        curve = NULL;           // Eviter les pointeurs fantomes
    }
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

    RC2D_Point* tempPoints = SDL_malloc(sizeof(RC2D_Point) * count);
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
    SDL_free(tempPoints);

    return result;
}

/**
 * Déplace la courbe de Bézier par un décalage spécifié.
 *
 * @param curve      Un pointeur vers la courbe de Bézier à déplacer.
 * @param dx         Décalage le long de l'axe x.
 * @param dy         Décalage le long de l'axe y.
 */
void rc2d_math_translate_BezierCurve(RC2D_BezierCurve* curve, double dx, double dy) 
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

/**
 * Définit les coordonnées du i-ème point de contrôle dans une courbe de Bézier.
 *
 * @param curve Un pointeur vers la courbe de Bézier.
 * @param i     L'indice du point de contrôle à définir (commençant à 0).
 * @param x     La position du point de contrôle le long de l'axe x.
 * @param y     La position du point de contrôle le long de l'axe y.
 */
void rc2d_math_setControlPoint_BezierCurve(RC2D_BezierCurve* curve, int i, double x, double y) 
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

/**
 * Met à l'échelle la courbe de Bézier par un facteur donné autour d'un point d'origine spécifié.
 *
 * @param curve Un pointeur vers la courbe de Bézier à mettre à l'échelle.
 * @param scale Le facteur d'échelle.
 * @param ox    La coordonnée x du centre de mise à l'échelle (par défaut à 0).
 * @param oy    La coordonnée y du centre de mise à l'échelle (par défaut à 0).
 */
void rc2d_math_scale_BezierCurve(RC2D_BezierCurve* curve, double scale, double ox, double oy) 
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

/**
 * Fait pivoter la courbe de Bézier d'un angle donné autour d'un point d'origine spécifié.
 *
 * @param curve Un pointeur vers la courbe de Bézier à faire pivoter.
 * @param angle L'angle de rotation en radians.
 * @param ox    La coordonnée x du centre de rotation (par défaut à 0).
 * @param oy    La coordonnée y du centre de rotation (par défaut à 0).
 */
void rc2d_math_rotate_BezierCurve(RC2D_BezierCurve* curve, double angle, double ox, double oy) 
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
 * Supprime le point de contrôle spécifié d'une courbe de Bézier.
 *
 * @param curve Un pointeur vers la courbe de Bézier.
 * @param index L'indice du point de contrôle à supprimer.
 */
void rc2d_math_removeControlPoint_BezierCurve(RC2D_BezierCurve* curve, int index) 
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
    RC2D_Point* newPoints = SDL_malloc(sizeof(RC2D_Point) * (curve->count - 1));
    
    // Copier tous les points de contrôle sauf celui à supprimer
    for (int i = 0, j = 0; i < curve->count; i++) 
    {
        if (i != index) 
        {
            newPoints[j++] = curve->points[i];
        }
    }
    
    // Libérer l'ancien tableau de points de contrôle et mettre à jour la courbe
    SDL_free(curve->points);

    curve->points = newPoints;
    curve->count -= 1;
}

/**
 * Insère un point de contrôle comme le nouveau i-ème point de contrôle.
 * Les points de contrôle existants à partir de i sont décalés d'une position.
 * Les indices commencent à 1. Les indices négatifs s'enroulent : -1 est le dernier point de contrôle,
 * -2 est celui juste avant le dernier, etc.
 *
 * @param curve Un pointeur vers la courbe de Bézier.
 * @param x La position du point de contrôle le long de l'axe des x.
 * @param y La position du point de contrôle le long de l'axe des y.
 * @param i L'indice du point de contrôle. Si i est -1, le point est inséré à la fin.
 */
void rc2d_math_insertControlPoint_BezierCurve(RC2D_BezierCurve* curve, double x, double y, int i) 
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
    RC2D_Point* newPoints = SDL_malloc(sizeof(RC2D_Point) * (curve->count + 1));
    
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
    SDL_free(curve->points);

    curve->points = newPoints;
    curve->count += 1; // Augmenter le nombre de points de contrôle
}

/**
 * Génère une nouvelle courbe de Bézier correspondant à un segment spécifié d'une courbe de Bézier existante.
 *
 * @param curve La courbe de Bézier originale.
 * @param startpoint Le point de départ du segment sur la courbe (0 <= startpoint <= 1).
 * @param endpoint Le point de fin du segment sur la courbe (startpoint < endpoint <= 1).
 * @return Une nouvelle courbe de Bézier correspondant au segment spécifié.
 */
RC2D_BezierCurve* rc2d_math_getSegment_BezierCurve(RC2D_BezierCurve* curve, double startpoint, double endpoint) 
{
    if (curve == NULL || curve->points == NULL || startpoint < 0 || startpoint > 1 || endpoint <= startpoint || endpoint > 1) 
    {
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier ou les points de contrôle sont NULL ou les points de début/fin sont invalides dans rc2d_math_getSegment_BezierCurve\n");
        return NULL; // Validation des entrées
    }

    // Application de l'algorithme de De Casteljau pour trouver les nouveaux points de contrôle
    RC2D_Point* newPointsStart = SDL_malloc(sizeof(RC2D_Point) * curve->count);
    RC2D_Point* newPointsEnd = SDL_malloc(sizeof(RC2D_Point) * curve->count);
    SDL_memcpy(newPointsStart, curve->points, sizeof(RC2D_Point) * curve->count);
    SDL_memcpy(newPointsEnd, curve->points, sizeof(RC2D_Point) * curve->count);

    // Subdivision à startpoint
    for (int i = 1; i < curve->count; i++) 
    {
        for (int j = 0; j < curve->count - i; j++) 
        {
            newPointsStart[j].x = (1 - startpoint) * newPointsStart[j].x + startpoint * newPointsStart[j + 1].x;
            newPointsStart[j].y = (1 - startpoint) * newPointsStart[j].y + startpoint * newPointsStart[j + 1].y;
        }
    }

    // Subdivision à endpoint ajusté en fonction de startpoint
    double adjustedEndpoint = (endpoint - startpoint) / (1 - startpoint);
    for (int i = 1; i < curve->count; i++) 
    {
        for (int j = 0; j < curve->count - i; j++) 
        {
            newPointsEnd[j].x = (1 - adjustedEndpoint) * newPointsEnd[j].x + adjustedEndpoint * newPointsEnd[j + 1].x;
            newPointsEnd[j].y = (1 - adjustedEndpoint) * newPointsEnd[j].y + adjustedEndpoint * newPointsEnd[j + 1].y;
        }
    }

    // Création de la nouvelle courbe de Bézier
    RC2D_BezierCurve* segmentCurve = rc2d_math_new_BezierCurve(curve->count);
    for (int i = 0; i < curve->count; i++) 
    {
        segmentCurve->points[i] = newPointsStart[i];
    }

    SDL_free(newPointsStart);
    SDL_free(newPointsEnd);

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
RC2D_Point* rc2d_math_render_BezierCurve(RC2D_BezierCurve* curve, int depth, int* numPoints) 
{
    if (curve == NULL || curve->points == NULL) 
    {
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier est NULL dans rc2d_math_render_BezierCurve\n");
        return NULL; // La courbe doit avoir au moins deux points de contrôle pour être rendue.
    }

    int countPoints = SDL_pow(2, depth) + 1; // Calcul du nombre de points basé sur la profondeur de récursion
    RC2D_Point* points = SDL_malloc(sizeof(RC2D_Point) * countPoints);
    *numPoints = countPoints; // Stocker le nombre de points dans la variable de sortie

    int index = 0;
    subdivideBezier(curve, depth, 0.0, 1.0, points, &index);

    return points;
}

/**
 * Génère une liste de coordonnées pour un segment spécifié d'une courbe de Bézier.
 *
 * @param curve La courbe de Bézier.
 * @param startpoint Le point de départ du segment sur la courbe (0 <= startpoint <= 1).
 * @param endpoint Le point de fin du segment sur la courbe (startpoint < endpoint <= 1).
 * @param depth La profondeur de la récursion pour la subdivision.
 * @param numPoints Un pointeur vers une variable où stocker le nombre de points générés.
 * @return Un tableau de points (RC2D_Point*) sur le segment spécifié de la courbe.
 */
RC2D_Point* rc2d_math_renderSegment_BezierCurve(RC2D_BezierCurve* curve, double startpoint, double endpoint, int depth, int* numPoints) 
{
    if (startpoint < 0 || startpoint > 1 || endpoint <= startpoint || endpoint > 1) 
    {
        // Gestion d'erreurs pour des points de début ou de fin invalides
        RC2D_log(RC2D_LOG_WARN, "Les points de début/fin sont invalides dans rc2d_math_renderSegment_BezierCurve\n");
        return NULL;
    }

    int countPoints = SDL_pow(2, depth) + 1; // Calcul du nombre de points basé sur la profondeur de récursion
    RC2D_Point* points = SDL_malloc(sizeof(RC2D_Point) * countPoints);
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
RC2D_BezierCurve* rc2d_math_getDerivative_BezierCurve(RC2D_BezierCurve* curve) {
    if (curve == NULL || curve->points == NULL || curve->count < 2) 
    {
        RC2D_log(RC2D_LOG_WARN, "La courbe de Bézier est NULL ou a moins de 2 points de contrôle dans rc2d_math_getDerivative_BezierCurve\n");
        return NULL; // La courbe doit avoir au moins deux points de contrôle pour avoir une dérivée.
    }

    int degree = curve->count - 1; // Le degré de la courbe originale
    RC2D_BezierCurve* derivativeCurve = SDL_malloc(sizeof(RC2D_BezierCurve));

    derivativeCurve->count = degree; // La courbe dérivée aura un degré de moins.
    derivativeCurve->points = SDL_malloc(sizeof(RC2D_Point) * derivativeCurve->count);

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
int rc2d_math_getDegree_BezierCurve(RC2D_BezierCurve* curve) 
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
int rc2d_math_getControlPointCount_BezierCurve(RC2D_BezierCurve* curve) 
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
int rc2d_math_getControlPoint_BezierCurve(RC2D_BezierCurve* curve, int i, double* x, double* y) 
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
int rc2d_math_evaluate_BezierCurve(RC2D_BezierCurve* curve, double t, double* x, double* y) 
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
 * Calcule la distance entre deux points.
 *
 * @param x1 La première coordonnée x.
 * @param y1 La première coordonnée y.
 * @param x2 La seconde coordonnée x.
 * @param y2 La seconde coordonnée y.
 * @return La distance euclidienne entre les deux points.
 */
double rc2d_math_dist(double x1, double y1, double x2, double y2)
{
	return SDL_sqrt(sqr(y2 - y1) + sqr(x2 - x1));
}

/**
 * Calcule l'angle entre deux points.
 *
 * @param x1 La première coordonnée x.
 * @param y1 La première coordonnée y.
 * @param x2 La seconde coordonnée x.
 * @param y2 La seconde coordonnée y.
 * @return L'angle entre les deux points en degrés.
 */
double rc2d_math_angle(double x1, double y1, double x2, double y2)
{
	return SDL_atan2(y2 - y1, x2 - x1) * (180 / SDL_PI_D); // return angle en degres
}

/**
 * Retourne le cosinus d'un angle.
 *
 * @param x L'angle en radians.
 * @return Le cosinus de l'angle.
 */
double rc2d_math_cosinus(double x)
{
	return cos(x);
}

/**
 * Retourne le sinus d'un angle.
 *
 * @param y L'angle en radians.
 * @return Le sinus de l'angle.
 */
double rc2d_math_sinus(double y)
{
	return sin(y);
}

/**
 * Normalise un vecteur 2D.
 *
 * @param x La composante x du vecteur.
 * @param y La composante y du vecteur.
 * @return Les composantes x et y normalisées du vecteur, ainsi que sa longueur originale.
 */
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

/**
 * Génère un nombre aléatoire entre min et max.
 *
 * @param min La valeur minimale.
 * @param max La valeur maximale.
 * @return Un nombre aléatoire entre min et max.
 */
int rc2d_math_random(int min, int max)
{
	int random = SDL_rand() % max + min;

	return random;
}

/**
 * Génère un nombre aléatoire entre 0 et 1.
 *
 * @return Un nombre aléatoire dans l'intervalle [0, 1].
 */
double rc2d_math_random0and1(void)
{
	return (double)SDL_rand() / ((double)RAND_MAX + 1);
}

/**
 * Effectue une interpolation linéaire entre deux valeurs.
 *
 * @param startValue La valeur de départ.
 * @param endValue La valeur de fin.
 * @param progress Le paramètre d'interpolation entre 0 et 1, représentant la progression de l'animation.
 * @return La valeur interpolée.
 */
double rc2d_math_lerp(double startValue, double endValue, double progress)
{
    return (1 - progress) * startValue + progress * endValue;
}

/**
 * Une autre fonction pour l'interpolation linéaire entre deux valeurs.
 *
 * @param startValue La valeur de départ.
 * @param endValue La valeur de fin.
 * @param progress Le paramètre d'interpolation entre 0 et 1, représentant la progression de l'animation.
 * @return La valeur interpolée.
 */
double rc2d_math_lerp2(double startValue, double endValue, double progress)
{
    return startValue + (endValue - startValue) * progress;
}

/**
 * Effectue une interpolation cosinus entre deux valeurs.
 *
 * @param a La valeur de départ.
 * @param b La valeur de fin.
 * @param t Le paramètre d'interpolation entre 0 et 1.
 * @return La valeur interpolée.
 */
double rc2d_math_cerp(double a, double b, double t)
{
	double f = (1 - sin(t * SDL_PI_D) * 0.5);

	return a * (1 - f) + b * f;
}

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


/**
 * Implémente le bruit de Perlin 1D.
 * 
 * Cette fonction génère une valeur de bruit basée sur la coordonnée x fournie.
 * Elle utilise une interpolation linéaire entre deux points de grille avec un
 * "fondu" pour lisser le résultat. Cette approche permet de créer un bruit
 * cohérent qui peut être utilisé pour divers effets comme des textures procédurales
 * ou des mouvements naturels.
 *
 * @param x La coordonnée x de la position pour laquelle générer le bruit.
 * @return La valeur du bruit en 1D, normalisée dans l'intervalle [0, 1].
 */
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

// Gradients pour 2D. Ils pourraient être aléatoires mais sont représentatifs pour un bon répartissement général, elle est utilisée dans rc2d_math_noise_2d
static int grad3[][2] = {{1,1}, {-1,1}, {1,-1}, {-1,-1},
                 {1,0}, {-1,0}, {1,0}, {-1,0},
                 {0,1}, {0,-1}, {0,1}, {0,-1}};

/**
 * Implémente le bruit de Perlin simplex en 2D.
 * 
 * Cette fonction génère une valeur de bruit basée sur les coordonnées x et y fournies.
 * Elle utilise une grille simplex, qui est une manière plus efficace et uniforme de
 * diviser l'espace que la grille régulière utilisée dans le bruit de Perlin classique.
 * Le bruit simplex réduit les artefacts visuels et offre une meilleure performance.
 *
 * @param x La coordonnée x de la position pour laquelle générer le bruit.
 * @param y La coordonnée y de la position pour laquelle générer le bruit.
 * @return La valeur du bruit simplex en 2D, normalisée dans l'intervalle [0, 1].
 */
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