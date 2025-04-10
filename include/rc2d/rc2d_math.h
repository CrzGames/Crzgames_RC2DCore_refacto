#ifndef RC2D_MATH_H
#define RC2D_MATH_H

#include <stdint.h> // Required for : uint32_t
#include <stdbool.h> // Required for : bool

#define MT_N 624 // Taille de la table de génération de nombres aléatoires de Mersenne Twister

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumération représentant les types d'arc.
 * @typedef {enum} RC2D_ArcType
 * @property {number} RC2D_ARC_OPEN - Arc ouvert.
 * @property {number} RC2D_ARC_CLOSED - Arc fermé.
 */
typedef enum RC2D_ArcType {
    RC2D_ARC_OPEN,
    RC2D_ARC_CLOSED
} RC2D_ArcType;

/**
 * Structure représentant un cercle, utilisée pour les calculs de collision.
 * @typedef {object} RC2D_Circle
 * @property {number} x - La coordonnée x du centre du cercle.
 * @property {number} y - La coordonnée y du centre du cercle.
 * @property {number} rayon - Le rayon du cercle.
 */
typedef struct RC2D_Circle {
	int x;
	int y;
	int rayon;
} RC2D_Circle;

/**
 * Représente un générateur de nombres aléatoires.
 * @typedef {Object} RC2D_RandomGenerator
 * @property {uint32_t[]} mt - Tableau de la table de génération de nombres aléatoires Mersenne Twister.
 * @property {int} index - Index actuel dans le tableau MT.
 * @property {uint32_t} seed_low - Les 32 bits inférieurs de la graine du générateur.
 * @property {uint32_t} seed_high - Les 32 bits supérieurs de la graine du générateur.
 */
typedef struct RC2D_RandomGenerator {
    uint32_t mt[MT_N];
    int index;        
    uint32_t seed_low; // TODO: add seed_low
    uint32_t seed_high; // TODO: add seed_high
} RC2D_RandomGenerator;

/**
 * Représente un point en 2D.
 * @typedef {Object} RC2D_Point
 * @property {number} x - La coordonnée x du point.
 * @property {number} y - La coordonnée y du point.
 */
typedef struct RC2D_Point {
    double x;
    double y;
} RC2D_Point;

/**
 * Structure représentant un segment en 2D.
 * @typedef {Object} RC2D_Segment
 * @property {RC2D_Point} start - Point de départ du segment.
 * @property {RC2D_Point} end - Point de fin du segment.
 */
typedef struct RC2D_Segment {
    RC2D_Point start;
    RC2D_Point end;
} RC2D_Segment;

/**
 * Représente un polygone, défini par une série de sommets (points en 2D).
 * @typedef {Object} RC2D_Polygon
 * @property {RC2D_Point*} vertices - Tableau dynamique de points définissant les sommets du polygone.
 * @property {int} numVertices - Nombre de sommets dans le polygone.
 */
typedef struct RC2D_Polygon {
    RC2D_Point* vertices;
    int numVertices;
} RC2D_Polygon;

/**
 * Représente un vecteur en 2D.
 * @typedef {Object} RC2D_Vector2D
 * @property {double} x - Composante x du vecteur.
 * @property {double} y - Composante y du vecteur.
 */
typedef struct RC2D_Vector2D {
    double x;
    double y;
} RC2D_Vector2D;

/**
 * Structure représentant un rayon dans l'espace 2D, défini par un point d'origine, un vecteur directionnel et une longueur.
 *
 * @typedef {Object} RC2D_Ray
 * @property {RC2D_Point} origin - Le point d'origine du rayon.
 * @property {RC2D_Vector2D} direction - Le vecteur directionnel du rayon, indiquant sa direction et sa magnitude.
 * @property {number} length - La longueur du rayon.
 */
typedef struct RC2D_Ray {
    RC2D_Point origin;
    RC2D_Vector2D direction;
    double length;
} RC2D_Ray;

/**
 * Représente une courbe de Bézier en 2D.
 * @typedef {Object} RC2D_BezierCurve
 * @property {RC2D_Point[]} points - Tableau dynamique de points définissant la courbe de Bézier.
 *                                   Chaque point représente un point de contrôle de la courbe.
 * @property {number} count - Nombre de points dans la courbe.
 */
typedef struct RC2D_BezierCurve {
    RC2D_Point* points;
    int count;
} RC2D_BezierCurve;

// Objet de courbes de Bézier
RC2D_BezierCurve* rc2d_math_new_BezierCurve(int count, ...); // TODO: Changer en -> int count, RC2D_Point* points
void rc2d_math_free_BezierCurve(RC2D_BezierCurve* curve);

// Fonctions pour manipuler les courbes de Bézier
void rc2d_math_translate_BezierCurve(RC2D_BezierCurve* curve, double dx, double dy);
void rc2d_math_setControlPoint_BezierCurve(RC2D_BezierCurve* curve, int i, double x, double y);
void rc2d_math_scale_BezierCurve(RC2D_BezierCurve* curve, double scale, double ox, double oy);
void rc2d_math_rotate_BezierCurve(RC2D_BezierCurve* curve, double angle, double ox, double oy);
void rc2d_math_removeControlPoint_BezierCurve(RC2D_BezierCurve* curve, int index);
void rc2d_math_insertControlPoint_BezierCurve(RC2D_BezierCurve* curve, double x, double y, int i);
RC2D_BezierCurve* rc2d_math_getSegment_BezierCurve(RC2D_BezierCurve* curve, double startpoint, double endpoint);
RC2D_Point* rc2d_math_render_BezierCurve(RC2D_BezierCurve* curve, int depth, int* numPoints);
RC2D_Point* rc2d_math_renderSegment_BezierCurve(RC2D_BezierCurve* curve, double startpoint, double endpoint, int depth, int* numPoints);
RC2D_BezierCurve* rc2d_math_getDerivative_BezierCurve(RC2D_BezierCurve* curve);
int rc2d_math_getDegree_BezierCurve(RC2D_BezierCurve* curve);
int rc2d_math_getControlPointCount_BezierCurve(RC2D_BezierCurve* curve);
int rc2d_math_getControlPoint_BezierCurve(RC2D_BezierCurve* curve, int i, double* x, double* y);
int rc2d_math_evaluate_BezierCurve(RC2D_BezierCurve* curve, double t, double* x, double* y);


// Fonctions pour l'objet RandomGenerator
RC2D_RandomGenerator* rc2d_math_newRandomGeneratorWithSeed(uint32_t seed);
RC2D_RandomGenerator* rc2d_math_newRandomGeneratorDouble(uint32_t seed_low, uint32_t seed_high);
RC2D_RandomGenerator* rc2d_math_newRandomGeneratorDefault(void);
void rc2d_math_freeRandomGenerator(RC2D_RandomGenerator* randomGenerator);

double rc2d_math_randomReal(RC2D_RandomGenerator *rng);
uint32_t rc2d_math_randomInt(RC2D_RandomGenerator *rng, uint32_t max);
uint32_t rc2d_math_randomIntRange(RC2D_RandomGenerator *rng, uint32_t min, uint32_t max);

void rc2d_math_setRandomSeedSingle(RC2D_RandomGenerator *rng, uint64_t seed);
void rc2d_math_setRandomSeedDouble(RC2D_RandomGenerator *rng, uint32_t low, uint32_t high);


// Color 
void rc2d_math_colorFromBytes(int rb, int gb, int bb, int *ab, double *r, double *g, double *b, double *a); 
void rc2d_math_colorToBytes(double r, double g, double b, double *a, int *rb, int *gb, int *bb, int *ab);

double rc2d_math_gammaToLinear(double c);
void rc2d_math_gammaToLinearRGB(double r, double g, double b, double *lr, double *lg, double *lb);

double rc2d_math_linearToGamma(double c); 
void rc2d_math_linearToGammaRGB(double lr, double lg, double lb, double *cr, double *cg, double *cb); 


// Test si un polygone est convexe
bool rc2d_math_isConvex(const RC2D_Polygon* polygon);

// Renvoie la distance entre deux points.
double rc2d_math_dist(double x1, double y1, double x2, double y2);

// Renvoie l'angle entre deux vecteurs en supposant la meme origine. 
double rc2d_math_angle(double x1, double y1, double x2, double y2);

// Cosinus
double rc2d_math_cosinus(double x);

// Sinus
double rc2d_math_sinus(double y);

// Normalise deux nombres
int rc2d_math_normalize(int x, int y);

// Genere un nombre aleatoire entre deux nombres
int rc2d_math_random(int min, int max);

// Genere un nombre aleatoire entre 0 et 1.
double rc2d_math_random0and1(void);

// Interpolation Linear entre deux nombres
double rc2d_math_lerp(double startValue, double endValue, double progress);
double rc2d_math_lerp2(double startValue, double endValue, double progress);

// Interpolation Cosinus entre deux nombres
double rc2d_math_cerp(double a, double b, double t);

// Génère du bruit Simplex Noise à partir d'une dimension ou deux dimensions.
double rc2d_math_noise_2d(double x, double y);
double rc2d_math_noise_1d(double x);

#ifdef __cplusplus
}
#endif

#endif // RC2D_MATH_H