#ifndef RC2D_MATH_H
#define RC2D_MATH_H

#include <stdint.h> // Required for : uint32_t
#include <stdbool.h> // Required for : bool

#define MT_N 624 // Taille de la table de génération de nombres aléatoires de Mersenne Twister

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Structure représentant un point 2D avec des coordonnées flottantes.
 * 
 * Cette structure est utilisée pour représenter des points dans un espace 2D,
 * avec des coordonnées x et y de type flottant.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct {
    /**
     * \brief La coordonnée x du point.
     */
    float x;

    /**
     * \brief La coordonnée y du point.
     */
    float y;
} RC2D_Vertex;

/**
 * \brief Structure représentant une boîte englobante alignée sur les axes (AABB).
 * 
 * Cette structure est utilisée pour représenter un rectangle dans un espace 2D,
 * défini par sa position (x, y) et ses dimensions (width, height).
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_AABB {
    /**
     * \brief La coordonnée x du coin supérieur gauche de la boîte.
     */
	int x;

    /**
     * \brief La coordonnée y du coin supérieur gauche de la boîte.
     */
	int y;

    /**
     * \brief La largeur de la boîte.
     */
	int width;

    /**
     * \brief La hauteur de la boîte.
     */
	int height;
} RC2D_AABB;

/**
 * Alias pour la structure RC2D_AABB, utilisée pour représenter un rectangle.
 * @typedef {RC2D_AABB} RC2D_Rect
 */
typedef struct RC2D_AABB RC2D_Rect;

/**
 * \brief Enumération représentant les types d'arcs dans l'espace 2D.
 * 
 * Cette énumération est utilisée pour spécifier si un arc est ouvert ou fermé.
 * Un arc ouvert n'inclut pas les points de début et de fin, tandis qu'un arc fermé les inclut.
 * 
 * \since Cette énumération est disponible depuis RC2D 1.0.0.
 */
typedef enum RC2D_ArcType {
    /**
     * \brief Arc ouvert, n'inclut pas les points de début et de fin.
     */
    RC2D_ARC_OPEN,

    /**
     * \brief Arc fermé, inclut les points de début et de fin.
     */
    RC2D_ARC_CLOSED
} RC2D_ArcType;

/**
 * \brief Représente un cercle en 2D.
 * 
 * Cette structure est utilisée pour représenter un cercle dans un espace 2D,
 * défini par son centre (x, y) et son rayon.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_Circle {
    /**
     * \brief La coordonnée x du centre du cercle.
     */
	int x;

    /**
     * \brief La coordonnée y du centre du cercle.
     */
	int y;

    /**
     * \brief Le rayon du cercle.
     */
	int rayon;
} RC2D_Circle;

/**
 * \brief Représente un générateur de nombres aléatoires basé sur l'algorithme Mersenne Twister.
 * 
 * Cette structure est utilisée pour générer des nombres pseudo-aléatoires
 * en utilisant l'algorithme Mersenne Twister. Elle contient un tableau de
 * nombres entiers (mt) pour stocker l'état interne du générateur,
 * un index pour suivre la position actuelle dans le tableau, et deux champs
 * pour les graines (seed_low et seed_high) qui peuvent être utilisés pour
 * initialiser le générateur avec des valeurs spécifiques.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_RandomGenerator {
    /**
     * \brief Tableau de nombres entiers représentant l'état interne du générateur.
     * 
     * Ce tableau est utilisé pour stocker les nombres générés par l'algorithme Mersenne Twister.
     */
    uint32_t mt[MT_N];

    /**
     * \brief Index actuel dans le tableau mt.
     * 
     * Cet index est utilisé pour suivre la position actuelle dans le tableau
     * lors de la génération de nombres aléatoires.
     */
    int index;
    
    /**
     * TODO: add seed_low
     * \brief Graine basse pour initialiser le générateur.
     * 
     * Ce champ peut être utilisé pour initialiser le générateur avec une valeur spécifique,
     * permettant de reproduire la séquence de nombres aléatoires.
     */
    uint32_t seed_low;

    /**
     * TODO: add seed_high
     * \brief Graine haute pour initialiser le générateur.
     * 
     * Ce champ peut être utilisé pour initialiser le générateur avec une valeur spécifique,
     * permettant de reproduire la séquence de nombres aléatoires.
     */
    uint32_t seed_high;
} RC2D_RandomGenerator;

/**
 * \brief Représente un point en 2D.
 * 
 * Cette structure est utilisée pour représenter un point dans un espace 2D,
 * avec des coordonnées x et y de type double.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_Point {
    /**
     * \brief La coordonnée x du point.
     */
    double x;

    /**
     * \brief La coordonnée y du point.
     */
    double y;
} RC2D_Point;

/**
 * \brief Représente un segment en 2D, défini par deux points (start et end).
 * 
 * Cette structure est utilisée pour représenter un segment de ligne dans un espace 2D,
 * avec un point de départ (start) et un point de fin (end), chacun étant un point en 2D.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_Segment {
    /**
     * \brief Le point de départ du segment.
     */
    RC2D_Point start;

    /**
     * \brief Le point de fin du segment.
     */
    RC2D_Point end;
} RC2D_Segment;

/**
 * \brief Représente un polygone en 2D.
 * 
 * Cette structure est utilisée pour représenter un polygone dans un espace 2D,
 * défini par un tableau dynamique de points (vertices) et le nombre de sommets (numVertices).
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_Polygon {
    /**
     * \brief Tableau dynamique de points représentant les sommets du polygone.
     * 
     * Chaque point est un RC2D_Point, et le tableau peut être de taille variable.
     */
    RC2D_Point* vertices;

    /**
     * \brief Le nombre de sommets dans le polygone.
     * 
     * Ce champ indique combien de points sont présents dans le tableau vertices.
     */
    int numVertices;
} RC2D_Polygon;

/**
 * \brief Représente un vecteur en 2D.
 * 
 * Cette structure est utilisée pour représenter un vecteur dans un espace 2D,
 * avec des composantes x et y de type double. Elle est souvent utilisée pour
 * décrire des directions ou des déplacements dans l'espace 2D.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_Vector2D {
    /**
     * \brief La composante x du vecteur.
     * 
     * Représente la direction horizontale du vecteur.
     */
    double x;

    /**
     * \brief La composante y du vecteur.
     * 
     * Représente la direction verticale du vecteur.
     */
    double y;
} RC2D_Vector2D;

/**
 * \brief Représente un rayon en 2D, défini par une origine et une direction.
 * 
 * Cette structure est utilisée pour représenter un rayon dans un espace 2D,
 * avec un point d'origine (origin) et une direction (direction) représentée par un vecteur 2D.
 * Le rayon a également une longueur (length) qui peut être utilisée pour limiter la portée du rayon.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_Ray {
    /**
     * \brief Le point d'origine du rayon.
     * 
     * Représente le point de départ du rayon dans l'espace 2D.
     */
    RC2D_Point origin;

    /**
     * \brief La direction du rayon, représentée par un vecteur 2D.
     * 
     * Ce vecteur indique la direction dans laquelle le rayon s'étend à partir de son origine.
     */
    RC2D_Vector2D direction;

    /**
     * \brief La longueur du rayon.
     * 
     * Ce champ indique jusqu'où le rayon s'étend à partir de son origine dans la direction spécifiée.
     */
    double length;
} RC2D_Ray;

/**
 * \brief Représente une courbe de Bézier en 2D.
 * 
 * Cette structure est utilisée pour représenter une courbe de Bézier, qui est définie par un ensemble de points de contrôle.
 * La courbe est représentée par un tableau dynamique de points (points) et le nombre de points (count) qui la composent.
 * 
 * \since Cette structure est disponible depuis RC2D 1.0.0.
 */
typedef struct RC2D_BezierCurve {
    /**
     * \brief Tableau dynamique de points de contrôle pour la courbe de Bézier.
     * 
     * Chaque point est un RC2D_Point, et le tableau peut être de taille variable.
     */
    RC2D_Point* points;

    /**
     * \brief Le nombre de points de contrôle dans la courbe de Bézier.
     * 
     * Ce champ indique combien de points sont présents dans le tableau points.
     */
    int count;
} RC2D_BezierCurve;

/**
 * \brief Crée une nouvelle courbe de Bézier.
 *
 * Cette fonction alloue et initialise une nouvelle courbe de Bézier avec un nombre
 * spécifié de points de contrôle.
 *
 * @param {int} count - Le nombre de points de contrôle pour la courbe de Bézier.
 * @param {RC2D_Point*} points - Tableau de points de contrôle pour la courbe de Bézier.
 * @return {RC2D_BezierCurve*} - Pointeur vers la nouvelle courbe de Bézier allouée.
 * 
 * \note Le pointeur retourné doit être libéré par l'appelant avec `rc2d_math_freeBezierCurve`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *  
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_free_BezierCurve
 */
RC2D_BezierCurve* rc2d_math_newBezierCurve(int count, const RC2D_Point* points);

/**
 * \brief Libère la mémoire occupée par une courbe de Bézier.
 *
 * @param {RC2D_BezierCurve*} curve - Pointeur vers la courbe de Bézier à libérer.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_new_BezierCurve
 */
void rc2d_math_freeBezierCurve(RC2D_BezierCurve* curve);


// Fonctions pour manipuler les courbes de Bézier
/**
 * \brief Déplace la courbe de Bézier par un décalage spécifié.
 *
 * @param {RC2D_BezierCurve*} curve - Pointeur vers la courbe de Bézier à déplacer.
 * @param {double} dx - Décalage le long de l'axe x.
 * @param {double} dy - Décalage le long de l'axe y.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_scale_BezierCurve
 * \see rc2d_math_rotate_BezierCurve
 */
void rc2d_math_translateBezierCurve(RC2D_BezierCurve* curve, double dx, double dy);

/**
 * \brief Définit les coordonnées du i-ème point de contrôle dans une courbe de Bézier.
 *
 * @param {RC2D_BezierCurve*} curve - Pointeur vers la courbe de Bézier.
 * @param {int} i - L'indice du point de contrôle à définir (commençant à 0).
 * @param {double} x - La position du point de contrôle le long de l'axe x.
 * @param {double} y - La position du point de contrôle le long de l'axe y.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_getControlPoint_BezierCurve
 * \see rc2d_math_removeControlPoint_BezierCurve
 * \see rc2d_math_insertControlPoint_BezierCurve
 */
void rc2d_math_setControlPointBezierCurve(RC2D_BezierCurve* curve, int i, double x, double y);

/**
 * \brief Met à l'échelle la courbe de Bézier par un facteur donné autour d'un point d'origine spécifié.
 *
 * @param {RC2D_BezierCurve*} curve - Un pointeur vers la courbe de Bézier à mettre à l'échelle.
 * @param {double} scale - Le facteur d'échelle.
 * @param {double} ox - La coordonnée x du centre de mise à l'échelle (par défaut à 0).
 * @param {double} oy - La coordonnée y du centre de mise à l'échelle (par défaut à 0).
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_translate_BezierCurve
 * \see rc2d_math_rotate_BezierCurve
 */
void rc2d_math_scaleBezierCurve(RC2D_BezierCurve* curve, double scale, double ox, double oy);

/**
 * \brief Fait pivoter la courbe de Bézier autour d'un point d'origine spécifié par un angle donné.
 *
 * @param {RC2D_BezierCurve*} curve - Un pointeur vers la courbe de Bézier à faire pivoter.
 * @param {double} angle - L'angle de rotation en radians.
 * @param {double} ox - La coordonnée x du centre de rotation (par défaut à 0).
 * @param {double} oy - La coordonnée y du centre de rotation (par défaut à 0).
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_translate_BezierCurve
 * \see rc2d_math_scale_BezierCurve
 */
void rc2d_math_rotateBezierCurve(RC2D_BezierCurve* curve, double angle, double ox, double oy);

/**
 * \brief Supprime un point de contrôle de la courbe de Bézier à l'index spécifié.
 *
 * @param {RC2D_BezierCurve*} curve - Pointeur vers la courbe de Bézier.
 * @param {int} index - L'index du point de contrôle à supprimer.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_setControlPoint_BezierCurve
 * \see rc2d_math_insertControlPoint_BezierCurve
 */
void rc2d_math_removeControlPointBezierCurve(RC2D_BezierCurve* curve, int index);

/**
 * \brief Insère un point de contrôle à une position spécifique dans la courbe de Bézier.
 * 
 * Les points de contrôle existants à partir de i sont décalés d'une position.
 * Les indices commencent à 1. Les indices négatifs s'enroulent : -1 est le dernier point de contrôle,
 * -2 est celui juste avant le dernier, etc.
 * 
 * @param {RC2D_BezierCurve*} curve - Pointeur vers la courbe de Bézier.
 * @param {double} x - La position du point de contrôle le long de l'axe x.
 * @param {double} y - La position du point de contrôle le long de l'axe y.
 * @param {int} i - L'index où insérer le nouveau point de contrôle, si i est -1, le point est inséré à la fin.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_setControlPoint_BezierCurve
 * \see rc2d_math_removeControlPoint_BezierCurve
 */
void rc2d_math_insertControlPointBezierCurve(RC2D_BezierCurve* curve, double x, double y, int i);

/**
 * \brief Génère une nouvelle courbe de Bézier correspondant à un segment défini de la courbe d'origine.
 *
 * Cette fonction applique l'algorithme de De Casteljau pour extraire un sous-segment de la courbe de Bézier d'origine,
 * défini par les paramètres `startpoint` et `endpoint`, tous deux compris entre 0 et 1.
 * Elle retourne une nouvelle courbe de Bézier contenant de nouveaux points de contrôle équivalents au segment [startpoint, endpoint].
 *
 * Cette opération est utile pour découper dynamiquement des portions de courbes,
 * par exemple pour des animations, morphings ou autres effets visuels progressifs.
 *
 * \param {RC2D_BezierCurve*} curve - La courbe de Bézier originale à partir de laquelle extraire un segment.
 * \param {double} startpoint - La position de début sur la courbe (entre 0.0 et 1.0).
 * \param {double} endpoint - La position de fin sur la courbe (entre startpoint et 1.0).
 * \return {RC2D_BezierCurve*} - Une nouvelle courbe de Bézier représentant le segment défini, ou NULL en cas d'erreur.
 *
 * \note Le pointeur retourné doit être libéré par l'appelant avec `rc2d_math_freeBezierCurve`.
 * 
 * \since This function is available since RC2D 1.0.0.
 *
 * \see rc2d_math_evaluateBezier
 */
RC2D_BezierCurve* rc2d_math_subdivideBezierCurve(RC2D_BezierCurve* curve, double startpoint, double endpoint);

/**
 * \brief Génère une liste de coordonnées pour être utilisée avec une fonction de dessin.
 * Cette fonction génère un tableau de points représentant la courbe de Bézier
 * en utilisant l'algorithme de De Casteljau pour subdiviser la courbe à une profondeur donnée.
 *
 * \param {RC2D_BezierCurve*} curve - La courbe de Bézier à rendre.
 * \param {int} depth - La profondeur de la récursion pour la subdivision.
 * \param {int*} numPoints - Un pointeur vers une variable où stocker le nombre de points générés.
 * \return {RC2D_Point*} - Un tableau de points (RC2D_Point*) représentant la courbe de Bézier.
 * 
 * \note Ce tableau doit être libéré par l'appelant avec `RC2D_free()`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_renderSegmentBezierCurve
 */
RC2D_Point* rc2d_math_renderBezierCurve(RC2D_BezierCurve* curve, int depth, int* numPoints);

/**
 * \brief Génère une liste de coordonnées pour un segment spécifié d'une courbe de Bézier.
 *
 * \param {RC2D_BezierCurve*} curve - La courbe de Bézier.
 * \param startpoint Le point de départ du segment sur la courbe (0 <= startpoint <= 1).
 * \param endpoint Le point de fin du segment sur la courbe (startpoint < endpoint <= 1).
 * \param depth La profondeur de la récursion pour la subdivision.
 * \param numPoints Un pointeur vers une variable où stocker le nombre de points générés.
 * \return {RC2D_Point*} - Un tableau de points (RC2D_Point*) sur le segment spécifié de la courbe.
 * 
 * \note Ce tableau doit être libéré par l'appelant avec `RC2D_free()`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_renderBezierCurve
 */
RC2D_Point* rc2d_math_renderSegmentBezierCurve(RC2D_BezierCurve* curve, double startpoint, double endpoint, int depth, int* numPoints);
RC2D_BezierCurve* rc2d_math_getDerivativeBezierCurve(RC2D_BezierCurve* curve);
int rc2d_math_getDegreeBezierCurve(RC2D_BezierCurve* curve);
int rc2d_math_getControlPointCountBezierCurve(RC2D_BezierCurve* curve);
int rc2d_math_getControlPointBezierCurve(RC2D_BezierCurve* curve, int i, double* x, double* y);
int rc2d_math_evaluateBezierCurve(RC2D_BezierCurve* curve, double t, double* x, double* y);


/**
 * \brief Crée un nouveau générateur de nombres aléatoires avec une graine spécifique.
 * 
 * @param {uint32_t} seed - La graine pour initialiser le générateur.
 * @return {RC2D_RandomGenerator*} - Pointeur vers le nouveau générateur de nombres aléatoires.
 * 
 * \note Le pointeur retourné doit être libéré par l'appelant avec `rc2d_math_freeRandomGenerator`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_freeRandomGenerator
 * \see rc2d_math_newRandomGeneratorDouble
 * \see rc2d_math_newRandomGeneratorDefault
 */
RC2D_RandomGenerator* rc2d_math_newRandomGeneratorWithSeed(uint32_t seed);

/**
 * \brief Crée un nouveau générateur de nombres aléatoires avec une graine composée de deux entiers.
 * 
 * @param {uint32_t} seed_low - Les 32 bits inférieurs de la graine.
 * @param {uint32_t} seed_high - Les 32 bits supérieurs de la graine.
 * @return {RC2D_RandomGenerator*} - Pointeur vers le nouveau générateur de nombres aléatoires.
 * 
 * \note Le pointeur retourné doit être libéré par l'appelant avec `rc2d_math_freeRandomGenerator`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_freeRandomGenerator
 * \see rc2d_math_newRandomGeneratorWithSeed
 * \see rc2d_math_newRandomGeneratorDefault
 */
RC2D_RandomGenerator* rc2d_math_newRandomGeneratorDouble(uint32_t seed_low, uint32_t seed_high);

/**
 * \brief Crée un nouveau générateur de nombres aléatoires avec une graine par défaut.
 * 
 * @return {RC2D_RandomGenerator*} - Pointeur vers le nouveau générateur de nombres aléatoires.
 * 
 * \note Le pointeur retourné doit être libéré par l'appelant avec `rc2d_math_freeRandomGenerator`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_freeRandomGenerator
 * \see rc2d_math_newRandomGeneratorWithSeed
 * \see rc2d_math_newRandomGeneratorDouble
 */
RC2D_RandomGenerator* rc2d_math_newRandomGeneratorDefault(void);

/**
 * \brief Libère la mémoire allouée pour un générateur de nombres aléatoires.
 * 
 * @param {RC2D_RandomGenerator*} - randomGenerator - Pointeur vers le générateur de nombres aléatoires à libérer.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_newRandomGeneratorWithSeed
 * \see rc2d_math_newRandomGeneratorDouble
 * \see rc2d_math_newRandomGeneratorDefault
 */
void rc2d_math_freeRandomGenerator(RC2D_RandomGenerator* randomGenerator);


/**
 * \brief Génère un nombre réel aléatoire dans l'intervalle [0, 1).
 * 
 * @param {RC2D_RandomGenerator*} rng - Pointeur vers le générateur de nombres aléatoires.
 * @return {double} - Un nombre réel aléatoire.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_randomInt
 * \see rc2d_math_randomIntRange
 */
double rc2d_math_randomReal(RC2D_RandomGenerator *rng);

/**
 * \brief Génère un nombre entier aléatoire dans l'intervalle [1, max].
 * 
 * @param {RC2D_RandomGenerator*} rng - Pointeur vers le générateur de nombres aléatoires.
 * @param {uint32_t} max - La valeur maximale du nombre aléatoire (inclusive).
 * @return {uint32_t} - Un nombre entier aléatoire.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_randomReal
 * \see rc2d_math_randomIntRange
 */
uint32_t rc2d_math_randomInt(RC2D_RandomGenerator *rng, uint32_t max);

/**
 * \brief Génère un nombre entier aléatoire dans l'intervalle spécifié [min, max].
 * 
 * @param {RC2D_RandomGenerator*} rng - Pointeur vers le générateur de nombres aléatoires.
 * @param {uint32_t} min - La valeur minimale du nombre aléatoire (inclusive).
 * @param {uint32_t} max - La valeur maximale du nombre aléatoire (inclusive).
 * @return {uint32_t} - Un nombre entier aléatoire.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_randomReal
 * \see rc2d_math_randomInt
 */
uint32_t rc2d_math_randomIntRange(RC2D_RandomGenerator *rng, uint32_t min, uint32_t max);

/**
 * \brief Définit la nouvelle graine du générateur de nombres aléatoires en utilisant un seul nombre entier.
 * 
 * @param {RC2D_RandomGenerator*} rng - Pointeur vers le générateur de nombres aléatoires.
 * @param {uint64_t} seed - La graine pour initialiser le générateur.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_setRandomSeedDouble
 */
void rc2d_math_setRandomSeedSingle(RC2D_RandomGenerator *rng, uint64_t seed);

/**
 * \brief Définit la nouvelle graine du générateur de nombres aléatoires en combinant deux nombres entiers de 32 bits.
 * 
 * @param {RC2D_RandomGenerator*} rng - Pointeur vers le générateur de nombres aléatoires.
 * @param {uint32_t} low - Les 32 bits inférieurs de la graine.
 * @param {uint32_t} high - Les 32 bits supérieurs de la graine.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_setRandomSeedSingle
 */
void rc2d_math_setRandomSeedDouble(RC2D_RandomGenerator *rng, uint32_t low, uint32_t high);


// Color
/**
 * \brief Convertit une couleur de l'intervalle 0..255 à l'intervalle 0..1.
 * 
 * @param {int} rb - Composante rouge de la couleur dans l'intervalle 0..255.
 * @param {int} gb - Composante verte de la couleur dans l'intervalle 0..255.
 * @param {int} bb - Composante bleue de la couleur dans l'intervalle 0..255.
 * @param {int*} ab - Pointeur vers la composante alpha de la couleur dans l'intervalle 0..255; peut être NULL si l'alpha n'est pas utilisé.
 * @param {double*} r - Pointeur vers le stockage du résultat de la composante rouge dans l'intervalle 0..1.
 * @param {double*} g - Pointeur vers le stockage du résultat de la composante verte dans l'intervalle 0..1.
 * @param {double*} b - Pointeur vers le stockage du résultat de la composante bleue dans l'intervalle 0..1.
 * @param {double*} a - Pointeur vers le stockage du résultat de la composante alpha dans l'intervalle 0..1; peut être NULL si l'alpha n'est pas utilisé.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_colorToBytes
 */ 
void rc2d_math_colorFromBytes(int rb, int gb, int bb, int *ab, double *r, double *g, double *b, double *a);

/**
 * \brief Convertit une couleur de l'intervalle 0..1 à l'intervalle 0..255.
 * 
 * @param {double} r - Composante rouge de la couleur dans l'intervalle 0..1.
 * @param {double} g - Composante verte de la couleur dans l'intervalle 0..1.
 * @param {double} b - Composante bleue de la couleur dans l'intervalle 0..1.
 * @param {double*} a - Pointeur vers la composante alpha de la couleur dans l'intervalle 0..1; peut être NULL si l'alpha n'est pas utilisé.
 * @param {int*} rb - Pointeur vers le stockage du résultat de la composante rouge dans l'intervalle 0..255.
 * @param {int*} gb - Pointeur vers le stockage du résultat de la composante verte dans l'intervalle 0..255.
 * @param {int*} bb - Pointeur vers le stockage du résultat de la composante bleue dans l'intervalle 0..255.
 * @param {int*} ab - Pointeur vers le stockage du résultat de la composante alpha dans l'intervalle 0..255; peut être NULL si l'alpha n'est pas utilisé.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_colorFromBytes
 */
void rc2d_math_colorToBytes(double r, double g, double b, double *a, int *rb, int *gb, int *bb, int *ab);

/**
 * \brief Convertit une composante de couleur de l'espace gamma (sRVB) en espace linéaire (RVB).
 * 
 * @param {double} c - La valeur d'un canal de couleur dans l'espace sRVB à convertir.
 * @return {double} - La valeur du canal de couleur dans l'espace RVB linéaire.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_linearToGamma
 * \see rc2d_math_gammaToLinearRGB
 * \see rc2d_math_linearToGammaRGB
 */
double rc2d_math_gammaToLinear(double c);

/**
 * \brief Convertit une couleur de l'espace gamma (sRVB) en espace linéaire (RVB).
 * 
 * @param {double} r - Composante rouge de la couleur dans l'espace sRVB.
 * @param {double} g - Composante verte de la couleur dans l'espace sRVB.
 * @param {double} b - Composante bleue de la couleur dans l'espace sRVB.
 * @param {double*} lr - Pointeur vers le stockage du résultat de la composante rouge dans l'espace RVB linéaire.
 * @param {double*} lg - Pointeur vers le stockage du résultat de la composante verte dans l'espace RVB linéaire.
 * @param {double*} lb - Pointeur vers le stockage du résultat de la composante bleue dans l'espace RVB linéaire.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_gammaToLinear
 * \see rc2d_math_linearToGamma
 * \see rc2d_math_linearToGammaRGB
 */
void rc2d_math_gammaToLinearRGB(double r, double g, double b, double *lr, double *lg, double *lb);

/**
 * \brief Convertit une composante de couleur de l'espace linéaire (RVB) en espace gamma (sRVB).
 * 
 * @param {double} c - La valeur d'un canal de couleur dans l'espace RVB linéaire à convertir.
 * @return {double} - La valeur du canal de couleur dans l'espace sRVB.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_gammaToLinear
 * \see rc2d_math_gammaToLinearRGB
 * \see rc2d_math_linearToGammaRGB
 */
double rc2d_math_linearToGamma(double c);

/**
 * \brief Applique la conversion de linéaire à gamma pour les composantes rouge, verte et bleue d'une couleur.
 * 
 * @param {double} lr - Le canal rouge de la couleur RVB linéaire à convertir.
 * @param {double} lg - Le canal vert de la couleur RVB linéaire à convertir.
 * @param {double} lb - Le canal bleu de la couleur RVB linéaire à convertir.
 * @param {double*} cr - Pointeur vers le stockage du canal rouge de la couleur convertie dans l'espace sRVB gamma.
 * @param {double*} cg - Pointeur vers le stockage du canal vert de la couleur convertie dans l'espace sRVB gamma.
 * @param {double*} cb - Pointeur vers le stockage du canal bleu de la couleur convertie dans l'espace sRVB gamma.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_gammaToLinear
 * \see rc2d_math_gammaToLinearRGB
 * \see rc2d_math_linearToGamma
 */
void rc2d_math_linearToGammaRGB(double lr, double lg, double lb, double *cr, double *cg, double *cb); 


/**
 * \brief Vérifie si un polygone est convexe.
 * 
 * @param {const RC2D_Polygon*} polygon - Le polygone à vérifier.
 * @return {true} - Si le polygone est convexe, false sinon.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
bool rc2d_math_isConvex(const RC2D_Polygon* polygon);

/**
 * \brief Calcule la distance entre deux points.
 *
 * @param {double} x1 - La première coordonnée x.
 * @param {double} y1 - La première coordonnée y.
 * @param {double} x2 - La seconde coordonnée x.
 * @param {double} y2 - La seconde coordonnée y.
 * @return {double} - La distance euclidienne entre les deux points.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
double rc2d_math_dist(double x1, double y1, double x2, double y2);

/**
 * \brief Calcule l'angle entre deux points.
 *
 * @param {double} x1 - La première coordonnée x.
 * @param {double} y1 - La première coordonnée y.
 * @param {double} x2 - La seconde coordonnée x.
 * @param {double} y2 - La seconde coordonnée y.
 * @return {double} - L'angle en degrés entre les deux points.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
double rc2d_math_angle(double x1, double y1, double x2, double y2);

/**
 * \brief Retourne le cosinus d'un angle.
 *
 * @param {double} x - L'angle en radians.
 * @return {double} - Le cosinus de l'angle.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_sinus
 */
double rc2d_math_cosinus(double x);

/**
 * \brief Retourne le sinus d'un angle.
 *
 * @param {double} y - L'angle en radians.
 * @return {double} - Le sinus de l'angle.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_cosinus
 */
double rc2d_math_sinus(double y);

/**
 * \brief Normalise un vecteur 2D.
 *
 * @param {int} x - La composante x du vecteur.
 * @param {int} y - La composante y du vecteur.
 * @return {int} - Les composantes x et y normalisées du vecteur, ainsi que sa longueur originale.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 */
int rc2d_math_normalize(int x, int y);

/**
 * \brief Génère un nombre aléatoire entre min et max.
 *
 * @param {int} min - La valeur minimale.
 * @param {int} max - La valeur maximale.
 * @return {int} - Un nombre aléatoire entre min et max.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_random0and1
 */
int rc2d_math_random(int min, int max);

/**
 * \brief Génère un nombre aléatoire entre 0 et 1.
 *
 * @return {double} - Un nombre aléatoire dans l'intervalle [0, 1].
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_random
 */
double rc2d_math_random0and1(void);

/**
 * \brief Effectue une interpolation linéaire entre deux valeurs.
 *
 * @param {double} startValue -La valeur de départ.
 * @param {double} endValue - La valeur de fin.
 * @param {double} progress - Le paramètre d'interpolation entre 0 et 1, représentant la progression de l'animation.
 * @return {double} - La valeur interpolée.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_lerp2
 * \see rc2d_math_cerp
 */
double rc2d_math_lerp(double startValue, double endValue, double progress);

/**
 * \brief Effectue une interpolation linéaire entre deux valeurs en 2D.
 *
 * @param {double} startValue - La valeur de départ.
 * @param {double} endValue - La valeur de fin.
 * @param {double} progress - Le paramètre d'interpolation entre 0 et 1, représentant la progression de l'animation.
 * @return {double} - La valeur interpolée.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_lerp
 * \see rc2d_math_cerp
 */
double rc2d_math_lerp2(double startValue, double endValue, double progress);

/**
 * \brief Effectue une interpolation cosinus entre deux valeurs.
 *
 * @param {double} startValue - La valeur de départ.
 * @param {double} endValue - La valeur de fin.
 * @param {double} progress - Le paramètre d'interpolation entre 0 et 1, représentant la progression de l'animation.
 * @return {double} - La valeur interpolée.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_lerp
 * \see rc2d_math_lerp2
 */
double rc2d_math_cerp(double startValue, double endValue, double progress);

/**
 * \brief Implémente le bruit de Perlin 1D.
 * 
 * Cette fonction génère une valeur de bruit basée sur la coordonnée x fournie.
 * Elle utilise une interpolation linéaire entre deux points de grille avec un
 * "fondu" pour lisser le résultat. Cette approche permet de créer un bruit
 * cohérent qui peut être utilisé pour divers effets comme des textures procédurales
 * ou des mouvements naturels.
 *
 * @param {double} x - La coordonnée x de la position pour laquelle générer le bruit.
 * @return {double} - La valeur du bruit en 1D, normalisée dans l'intervalle [0, 1].
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_noise_2d
 */
double rc2d_math_noise_1d(double x);

/**
 * \brief  Implémente le bruit de Perlin simplex en 2D.
 * 
 * Cette fonction génère une valeur de bruit basée sur les coordonnées x et y fournies.
 * Elle utilise une grille simplex, qui est une manière plus efficace et uniforme de
 * diviser l'espace que la grille régulière utilisée dans le bruit de Perlin classique.
 * Le bruit simplex réduit les artefacts visuels et offre une meilleure performance.
 *
 * @param {double} x - La coordonnée x de la position pour laquelle générer le bruit.
 * @param {double} y - La coordonnée y de la position pour laquelle générer le bruit.
 * @return {double} - La valeur du bruit en 2D, normalisée dans l'intervalle [0, 1].
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_math_noise_1d
 */
double rc2d_math_noise_2d(double x, double y);

#ifdef __cplusplus
}
#endif

#endif // RC2D_MATH_H