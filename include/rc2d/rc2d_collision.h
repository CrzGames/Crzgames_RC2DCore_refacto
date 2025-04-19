#ifndef RC2D_COLLISION_H
#define RC2D_COLLISION_H

#include <RC2D/RC2D_math.h>
#include <RC2D/RC2D_graphics.h>

#include <stdbool.h> // Required for : bool

/* Configuration pour les définitions de fonctions C, même lors de l'utilisation de C++ */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Vérifie si un point est à l'intérieur d'une boîte englobante alignée sur les axes (AABB).
 *
 * Cette fonction détermine si un point donné se trouve à l'intérieur d'un rectangle
 * défini par la structure RC2D_AABB. Elle vérifie que les coordonnées du point sont comprises
 * entre les bornes horizontales et verticales définies par les champs x, y, w et h de la boîte.
 *
 * \param {RC2D_Point} point - Le point à tester.
 * \param {RC2D_AABB} box - La boîte englobante définie par un coin supérieur gauche (x, y), une largeur (w) et une hauteur (h).
 * \return {bool} - `true` si le point est à l'intérieur de la boîte, sinon `false`.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *  
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_collision_pointInCircle
 * \see rc2d_collision_pointInPolygon
 */
bool rc2d_collision_pointInAABB(const RC2D_Point point, const RC2D_AABB box);

/**
 * @brief Vérifie si un point est à l'intérieur d'un cercle.
 *
 * Cette fonction détermine si un point donné se trouve à l'intérieur d'un cercle défini
 * par la structure RC2D_Circle. Le test est basé sur la distance euclidienne entre le point
 * et le centre du cercle. Si cette distance est inférieure ou égale au rayon du cercle, alors
 * le point est considéré comme étant à l'intérieur.
 *
 * \param {RC2D_Point} point - Le point à tester.
 * \param {RC2D_Circle} circle - Le cercle défini par un centre (x, y) et un rayon.
 * \return {bool} - `true` si le point est à l'intérieur du cercle, sinon `false`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 *     
 * \see rc2d_collision_pointInAABB
 * \see rc2d_collision_betweenTwoCircle
 */
bool rc2d_collision_pointInCircle(const RC2D_Point point, const RC2D_Circle circle);

/**
 * \brief Vérifie si un point est à l'intérieur d'un polygone.
 *
 * Cette fonction utilise l'algorithme de ray-casting (ou crossing number) pour déterminer
 * si un point donné se trouve à l'intérieur d'un polygone. Elle trace un rayon horizontal à
 * partir du point vers la droite et compte combien d'arêtes du polygone ce rayon croise.
 * Si le nombre de croisements est impair, alors le point est considéré comme étant à l'intérieur
 * du polygone.
 *
 * \note Le polygone doit avoir au moins trois sommets pour être valide. Si ce n’est pas le cas,
 * la fonction retourne `false`.
 *
 * \param {RC2D_Point} point - Le point à tester.
 * \param {RC2D_Polygon*} polygon - Pointeur vers la structure représentant le polygone.
 * \return {bool} - `true` si le point est à l'intérieur du polygone, sinon `false`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_collision_betweenPolygonCircle
 * \see rc2d_collision_betweenPolygonSegment
 */
bool rc2d_collision_pointInPolygon(const RC2D_Point point, const RC2D_Polygon* polygon);

/**
 * \brief Vérifie si deux boîtes englobantes (AABB) se chevauchent.
 *
 * Cette fonction teste si deux rectangles définis par leurs coins supérieurs gauches,
 * largeurs et hauteurs respectives se recouvrent partiellement ou totalement.
 * Le test est basé sur la non-superposition des extrémités (séparation par les axes X ou Y).
 * Si l’un des rectangles est entièrement à gauche, à droite, au-dessus ou en dessous de l’autre,
 * alors ils ne se chevauchent pas.
 *
 * \param {RC2D_AABB} box1 - Première boîte, définie par son coin supérieur gauche (x, y), largeur (w) et hauteur (h).
 * \param {RC2D_AABB} box2 - Deuxième boîte, définie par son coin supérieur gauche (x, y), largeur (w) et hauteur (h).
 * \return {bool} - `true` si les boîtes se chevauchent, sinon `false`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 *  
 * \see rc2d_collision_pointInAABB
 * \see rc2d_collision_betweenAABBCircle
 */

bool rc2d_collision_betweenTwoAABB(const RC2D_AABB box1, const RC2D_AABB box2);

/**
 * \brief Vérifie si deux cercles se chevauchent.
 *
 * Cette fonction détermine si deux cercles entrent en collision ou se touchent.
 * Elle calcule la distance carrée entre les centres des deux cercles, puis la compare
 * au carré de la somme de leurs rayons. Cela évite l’utilisation coûteuse de `sqrt` pour optimiser les performances.
 *
 * \param {RC2D_Circle} circle1 - Premier cercle, défini par son centre (x, y) et rayon.
 * \param {RC2D_Circle} circle2 - Deuxième cercle, défini par son centre (x, y) et rayon.
 * \return {bool} - `true` si les cercles se chevauchent ou se touchent, sinon `false`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 *  
 * \see rc2d_collision_pointInCircle
 * \see rc2d_collision_betweenAABBCircle
 */
bool rc2d_collision_betweenTwoCircle(const RC2D_Circle circle1, const RC2D_Circle circle2);

/**
 * \brief Vérifie si un cercle et une boîte AABB se chevauchent.
 *
 * Cette fonction calcule le point le plus proche sur le rectangle AABB par rapport
 * au centre du cercle, en pinçant chaque coordonnée du centre du cercle à l'intérieur
 * des limites de la boîte. Elle calcule ensuite la distance entre ce point et le centre du cercle.
 * Si cette distance est inférieure ou égale au rayon, il y a collision.
 *
 * \param {RC2D_AABB} box - Rectangle défini par (x, y, w, h).
 * \param {RC2D_Circle} circle - Cercle défini par son centre (x, y) et son rayon.
 * \return {bool} - `true` s’il y a collision entre le cercle et la boîte, sinon `false`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 *
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 *  
 * \see rc2d_collision_betweenTwoAABB
 * \see rc2d_collision_pointInCircle
 */

bool rc2d_collision_betweenAABBCircle(const RC2D_AABB box, const RC2D_Circle circle);

/**
 * \brief Vérifie si un cercle et un segment se chevauchent.
 *
 * Cette fonction détecte une collision entre un cercle (défini par son centre et son rayon)
 * et un segment de droite (défini par deux points). Elle commence par calculer la distance
 * perpendiculaire du centre du cercle à la ligne formée par le segment. Si cette distance est
 * supérieure au rayon, il n'y a pas de collision.
 * Ensuite, elle vérifie si le point de projection du centre du cercle sur le segment se trouve bien
 * entre les extrémités du segment (en utilisant des produits scalaires). Enfin, elle effectue
 * une vérification supplémentaire : si le centre du cercle est suffisamment proche d'une des extrémités
 * du segment (c'est-à-dire que la distance est inférieure ou égale au rayon), la collision est validée.
 *
 * \note Ce test est robuste même lorsque le cercle est tangent ou recouvre partiellement le segment.
 *
 * \param {RC2D_Segment} segment - Le segment à tester, défini par deux points (start et end).
 * \param {RC2D_Circle} circle - Le cercle à tester, défini par un centre (x, y) et un rayon.
 * \return {bool} - `true` s'il y a chevauchement entre le cercle et le segment, sinon `false`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 *
 * \see rc2d_collision_pointInCircle
 * \see rc2d_collision_betweenTwoSegment
 * \see rc2d_collision_betweenPolygonCircle
 */
bool rc2d_collision_betweenCircleSegment(const RC2D_Segment segment, const RC2D_Circle circle);

/**
 * \brief Vérifie si deux segments 2D se croisent ou se touchent.
 *
 * Cette fonction vérifie l’intersection de deux segments en utilisant des produits vectoriels
 * (déterminants). Elle décompose chaque segment en vecteurs, puis calcule le signe des déterminants
 * formés par ces vecteurs et leurs positions relatives.
 * Si les produits croisés des vecteurs indiquent que les extrémités d’un segment se trouvent de part
 * et d’autre de l’autre segment, une intersection est probable. Ensuite, la fonction vérifie si
 * cette intersection se trouve bien dans les deux segments en contrôlant leurs projections.
 * 
 * \note Cette méthode est précise pour détecter toutes les situations de croisement ou de contact entre deux segments.
 *
 * \param {RC2D_Segment} segment1 - Premier segment, défini par deux points (start et end).
 * \param {RC2D_Segment} segment2 - Deuxième segment, défini par deux points (start et end).
 * \return {bool} - `true` si les deux segments se croisent ou se touchent, sinon `false`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 *
 * \see rc2d_collision_betweenPolygonSegment
 * \see rc2d_collision_raycastSegment
 * \see rc2d_collision_betweenCircleSegment
 */
bool rc2d_collision_betweenTwoSegment(const RC2D_Segment segment1, const RC2D_Segment segment2);

/**
 * \brief Vérifie si deux polygones convexes entrent en collision à l'aide de l'algorithme SAT (Separating Axis Theorem).
 *
 * Cette fonction applique le théorème des axes séparateurs (SAT) pour détecter les collisions
 * entre deux polygones convexes. Elle consiste à projeter les sommets des deux polygones sur
 * les axes normaux à chacune des arêtes des deux polygones, puis à vérifier les intervalles de projection.
 * Si un axe est trouvé où les projections ne se chevauchent pas, les polygones ne sont pas en collision.
 * 
 * \note Cette méthode est fiable pour des formes convexes, mais n'est pas adaptée aux polygones concaves.
 *
 * \param {RC2D_Polygon*} poly1 - Le premier polygone à tester, avec au moins 3 sommets.
 * \param {RC2D_Polygon*} poly2 - Le second polygone à tester, avec au moins 3 sommets.
 * \return {bool} - `true` s’il y a collision, sinon `false`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * @see rc2d_collision_betweenPolygonCircle
 * @see rc2d_math_isConvex
 */
bool rc2d_collision_betweenTwoPolygon(const RC2D_Polygon* poly1, const RC2D_Polygon* poly2);

/**
 * \brief Vérifie si un polygone et un cercle se chevauchent.
 *
 * Cette fonction combine plusieurs techniques pour détecter avec précision une collision
 * entre un polygone (potentiellement concave) et un cercle. Elle vérifie :
 * 1. Si un sommet du polygone est dans le cercle (`rc2d_collision_pointInCircle`)
 * 2. Si une arête du polygone touche le cercle (`rc2d_collision_betweenCircleSegment`)
 * 3. Si le centre du cercle est à l'intérieur du polygone (`rc2d_collision_pointInPolygon`)
 * 
 * \note Ces trois tests permettent de couvrir la plupart des scénarios d’intersection entre ces deux formes.
 *
 * \param {RC2D_Polygon*} polygon - Le polygone à tester, défini par un ensemble de sommets.
 * \param {RC2D_Circle} circle - Le cercle à tester, défini par un centre (x, y) et un rayon.
 * \return {bool} - `true` s’il y a une collision, sinon `false`.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0. 
 *
 * \see rc2d_collision_pointInPolygon
 * \see rc2d_collision_betweenCircleSegment
 * \see rc2d_collision_pointInCircle
 */
bool rc2d_collision_betweenPolygonCircle(const RC2D_Polygon* polygon, const RC2D_Circle circle);

/**
 * \brief Vérifie si un segment touche ou traverse un polygone.
 *
 * Cette fonction vérifie si un segment entre en collision avec un polygone.
 * Elle procède en testant d'abord les intersections entre le segment donné
 * et toutes les arêtes du polygone en utilisant `rc2d_collision_betweenTwoSegment`.
 * Ensuite, elle vérifie si l’un des points du segment est contenu dans le polygone
 * via `rc2d_collision_pointInPolygon`, ce qui permet de détecter un segment totalement intérieur.
 *
 * \param {RC2D_Segment} segment - Le segment à tester (deux points).
 * \param {RC2D_Polygon*} polygon - Le polygone cible, défini par un tableau de sommets.
 * \return {bool} - `true` si le segment croise ou est contenu dans le polygone, sinon `false`.
 *
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0. 
 *
 * \see rc2d_collision_betweenTwoSegment
 * \see rc2d_collision_pointInPolygon
 */
bool rc2d_collision_betweenPolygonSegment(const RC2D_Segment segment, const RC2D_Polygon* polygon);

// TODO: Tester les fonctions de collision raycast
/**
 * \brief Détecte si un rayon intersecte un cercle, et retourne le point d’intersection s’il existe.
 *
 * Cette fonction utilise les équations paramétriques du rayon et l’équation du cercle
 * pour déterminer si une intersection existe. Elle calcule la distance projetée entre le centre du cercle
 * et le rayon, puis vérifie si cette distance est inférieure au rayon du cercle.
 * Si le cercle est situé derrière le rayon (selon sa direction), aucune intersection n’est retournée.
 * 
 * \note En cas de collision, le point d’intersection est retourné via le pointeur `intersection`.
 *
 * \param {RC2D_Ray} ray - Le rayon défini par une origine, une direction (vectorielle), et une longueur maximale.
 * \param {RC2D_Circle} circle - Le cercle défini par son centre (x, y) et son rayon.
 * \param {RC2D_Point*} intersection - Un pointeur vers une structure pour stocker les coordonnées du point d’intersection si détectée.
 * \return {bool} - `true` si une collision est détectée dans la portée du rayon, sinon `false`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_collision_pointInCircle
 * \see rc2d_collision_betweenCircleSegment
 */
bool rc2d_collision_raycastCircle(const RC2D_Ray ray, const RC2D_Circle circle, RC2D_Point* intersection);

/**
 * \brief Détecte si un rayon intersecte une boîte englobante AABB.
 *
 * Cette fonction repose sur le calcul des intervalles d’entrée et de sortie du rayon
 * en fonction de ses coordonnées et de celles de la boîte AABB.
 * Elle utilise la méthode du “slab testing” : calculer les valeurs de `t` (paramètre du rayon)
 * à l’entrée et sortie de chaque face de l’AABB. Ensuite, elle vérifie si l’intervalle d’intersection
 * est valide et dans la portée du rayon.
 * 
 * \note En cas de collision, le point d’intersection est retourné via le pointeur `intersection`.
 *
 * \note param {RC2D_Ray} ray - Le rayon à tester, avec origine, direction, et longueur maximale.
 * \note param {RC2D_AABB} box - La boîte à tester, définie par un coin supérieur gauche (x, y), largeur et hauteur.
 * \note param {RC2D_Point*} intersection - Pointeur vers une structure pour stocker les coordonnées de l’intersection, si trouvée.
 * \return {bool} - `true` si une intersection existe dans la portée du rayon, sinon `false`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0. 
 *
 * \see rc2d_collision_pointInAABB
 * \see rc2d_collision_raycastCircle
 */
bool rc2d_collision_raycastAABB(const RC2D_Ray ray, const RC2D_AABB box, RC2D_Point* intersection);

/**
 * \brief Détecte une intersection entre un rayon et un segment.
 *
 * Cette fonction vérifie si un rayon touche un segment en utilisant une formule paramétrique.
 * Elle repose sur le produit croisé entre les vecteurs formés par le segment et le rayon,
 * permettant de détecter les cas d’intersection non colinéaires.
 * Si les deux vecteurs ne sont pas parallèles, elle calcule les coefficients `t1` (longueur sur le rayon)
 * et `t2` (position sur le segment) pour vérifier si l’intersection est comprise dans les deux.
 *
 * \note En cas de collision, le point d’intersection est retourné via le pointeur `intersection`.
 *
 * \param {RC2D_Ray} ray - Le rayon, défini par une origine, direction et longueur.
 * \param {RC2D_Segment} segment - Le segment défini par deux points (start, end).
 * \param {RC2D_Point*} intersection - Pointeur vers une structure pour stocker le point d’intersection s’il existe.
 * \return {bool} - `true` si une intersection existe, sinon `false`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 * 
 * \see rc2d_collision_betweenTwoSegment
 * \see rc2d_collision_raycastAABB
 */
bool rc2d_collision_raycastSegment(const RC2D_Ray ray, const RC2D_Segment segment, RC2D_Point* intersection);

/**
 * \brief Effectue une détection de collision pixel-par-pixel entre un rayon et une image.
 *
 * Cette fonction simule le trajet d’un rayon pixel par pixel à l’aide de l’algorithme
 * de tracé de Bresenham. Elle teste chaque point du trajet pour vérifier s’il correspond à
 * un pixel solide dans une image, en utilisant un masque de collision binaire (`imageData->mask`).
 * Elle est idéale pour des collisions précises sur des sprites irréguliers (formes découpées).
 * Dès qu’un pixel non transparent est rencontré, la collision est validée et le point est retourné.
 *
 * \note En cas de collision, le point d’intersection est retourné via le pointeur `intersection`.
 * 
 * \param {RC2D_ImageData*} imageData - Données de l’image incluant la largeur, la hauteur, et le masque de collision.
 * \param {RC2D_Ray} ray - Rayon utilisé pour balayer les pixels, défini par une origine, direction et longueur.
 * \param {RC2D_Point*} intersection - Si une collision a lieu, ce pointeur contiendra les coordonnées du premier pixel touché.
 * \return {bool} - `true` si une collision est détectée avec un pixel solide, sinon `false`.
 * 
 * \threadsafety Cette fonction peut être appelée depuis n'importe quel thread.
 * 
 * \since Cette fonction est disponible depuis RC2D 1.0.0.
 *
 * \see rc2d_collision_pointInAABB
 * \see rc2d_collision_raycastCircle
 */
bool rc2d_collision_raycastPixelPerfect(const RC2D_ImageData* imageData, const RC2D_Ray ray, RC2D_Point* intersection);

/* Termine les définitions de fonctions C lors de l'utilisation de C++ */
#ifdef __cplusplus
}
#endif

#endif // RC2D_COLLISION_H
