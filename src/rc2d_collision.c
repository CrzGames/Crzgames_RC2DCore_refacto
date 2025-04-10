#include "rc2d/rc2d_collision.h"
#include "rc2d/rc2d_logger.h"

#include <math.h> // Required for : sqrt, fabs
#include <float.h> // Required for : FLT_MAX

/**
 * Vérifie si un point est à l'intérieur d'un polygone.
 * 
 * Cette fonction utilise l'algorithme de ray-casting pour déterminer si le point donné est à l'intérieur du polygone.
 * Le principe est de tracer une ligne horizontale à partir du point vers l'extérieur du polygone et de compter combien
 * de fois cette ligne coupe les arêtes du polygone. Si le nombre de coupures est impair, le point est à l'intérieur.
 * 
 * @param point Le point à vérifier.
 * @param polygon Le polygone dans lequel vérifier le point.
 * @return true si le point est à l'intérieur du polygone, false sinon.
 */
bool rc2d_collision_pointInPolygon(const RC2D_Point point, const RC2D_Polygon* polygon) 
{
    if (polygon == NULL || polygon->numVertices < 3) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Le polygone est invalide ou ne contient pas suffisamment de sommets dans rc2d_collision_pointInPolygon().\n");
        return false;
    }

    for (int i = 0, j = polygon->numVertices - 1; i < polygon->numVertices; j = i++) 
    {
        if (((polygon->vertices[i].y > point.y) != (polygon->vertices[j].y > point.y)) &&
            (point.x < (polygon->vertices[j].x - polygon->vertices[i].x) * (point.y - polygon->vertices[i].y) / (polygon->vertices[j].y - polygon->vertices[i].y) + polygon->vertices[i].x)) 
        {
            return true;
        }
    }

    return false;
}

/**
 * Vérifie si un point est à l'intérieur d'une boîte englobante alignée sur les axes (AABB).
 *
 * @param x La coordonnée x du point.
 * @param y La coordonnée y du point.
 * @param box La boîte englobante avec laquelle vérifier la collision.
 * @return true si le point est à l'intérieur de la boîte, false sinon.
 */
bool rc2d_collision_pointInAABB(const RC2D_Point point, const RC2D_AABB box)
{
    // Vérifier si le point est à l'intérieur de la boîte englobante
    if (point.x >= box.x
        && point.x < box.x + box.w
        && point.y >= box.y
        && point.y < box.y + box.h)
        return true;

    return false;
}

/**
 * Vérifie si un point est à l'intérieur d'un cercle.
 *
 * @param x La coordonnée x du point.
 * @param y La coordonnée y du point.
 * @param circle Le cercle avec lequel vérifier la collision.
 * @return true si le point est à l'intérieur du cercle, false sinon.
 */
bool rc2d_collision_pointInCircle(const RC2D_Point point, const RC2D_Circle circle)
{
    // Calculer la distance entre le point et le centre du cercle
    int d2 = (point.x - circle.x) * (point.x - circle.x) + (point.y - circle.y) * (point.y - circle.y);

    // Vérifier si la distance est inférieure ou égale au rayon du cercle
    if (d2 > circle.rayon * circle.rayon)
        return false;

    return true;
}

/**
 * Vérifie si deux boîtes englobantes alignées sur les axes (AABB) se chevauchent.
 *
 * @param box1 La première boîte englobante.
 * @param box2 La deuxième boîte englobante.
 * @return true si les boîtes se chevauchent, false sinon.
 */
bool rc2d_collision_betweenTwoAABB(const RC2D_AABB box1, const RC2D_AABB box2)
{
    // Vérifier si les boîtes englobantes se chevauchent
    if ((box2.x >= box1.x + box1.w)      // trop a droite
        || (box2.x + box2.w <= box1.x) // trop a gauche
        || (box2.y >= box1.y + box1.h) // trop en bas
        || (box2.y + box2.h <= box1.y))  // trop en haut
        return false;

    return true;
}

/**
 * Vérifie si deux cercles se chevauchent.
 *
 * @param circle1 Le premier cercle.
 * @param circle2 Le deuxième cercle.
 * @return true si les cercles se chevauchent, false sinon.
 */
bool rc2d_collision_betweenTwoCircle(const RC2D_Circle circle1, const RC2D_Circle circle2)
{
    // Calculer la distance entre les centres des deux cercles
    int d2 = (circle1.x - circle2.x) * (circle1.x - circle2.x) + (circle1.y - circle2.y) * (circle1.y - circle2.y);

    // Vérifier si la distance est inférieure ou égale à la somme des rayons des deux cercles
    if (d2 > (circle1.rayon + circle2.rayon) * (circle1.rayon + circle2.rayon))
        return false;

    return true;
}

/**
 * Vérifie si un cercle et une boîte englobante alignée sur les axes (AABB) se chevauchent.
 *
 * Cette fonction détermine le point le plus proche sur la boîte AABB au centre du cercle,
 * puis calcule la distance entre ce point et le centre du cercle. Si la distance est inférieure
 * ou égale au rayon du cercle, cela signifie qu'il y a une collision entre le cercle et la boîte AABB.
 *
 * @param box Un objet RC2D_AABB représentant la boîte englobante, contenant les champs x, y (coordonnées du coin supérieur gauche), w (largeur), et h (hauteur).
 * @param circle Un objet RC2D_Circle représentant le cercle, contenant les champs x, y (coordonnées du centre) et rayon.
 * @return Retourne true si le cercle et la boîte AABB se chevauchent, false sinon.
 */
bool rc2d_collision_betweenAABBCircle(const RC2D_AABB box, const RC2D_Circle circle) 
{
    // Trouver le point le plus proche sur la boîte AABB au centre du cercle
    int closestX = (circle.x < box.x) ? box.x : (circle.x > box.x + box.w) ? box.x + box.w : circle.x;
    int closestY = (circle.y < box.y) ? box.y : (circle.y > box.y + box.h) ? box.y + box.h : circle.y;

    // Calculer la distance entre ce point et le centre du cercle
    int distanceX = circle.x - closestX;
    int distanceY = circle.y - closestY;
    int distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);

    // Si la distance est inférieure ou égale au carré du rayon du cercle, il y a collision
    return distanceSquared <= (circle.rayon * circle.rayon);
}

/**
 * @brief Vérifie si un cercle et un segment se chevauchent.
 *
 * @details Cette fonction détermine si un cercle spécifié par son centre et son rayon
 * intersecte ou touche un segment. La collision est détectée
 * par le calcul de la distance perpendiculaire du centre du cercle à la ligne
 * formée par le segment, et en vérifiant ensuite si cette distance est inférieure
 * ou égale au rayon du cercle. De plus, la fonction vérifie si le cercle touche
 * l'une des extrémités du segment ou si le point d'intersection projeté est situé
 * entre les deux points définissant le segment.
 *
 * @param segment Le segment avec lequel tester la collision.
 * @param circle Le cercle avec lequel tester la collision.
 * @return true si le cercle et le segment se chevauchent, false sinon.
 */
bool rc2d_collision_betweenCircleSegment(const RC2D_Segment segment, const RC2D_Circle circle)
{
    // Calcul des composantes des vecteurs pour les points de départ et de fin du segment
    int ABx = segment.end.x - segment.start.x;
    int ABy = segment.end.y - segment.start.y;
    int ACx = circle.x - segment.start.x;
    int ACy = circle.y - segment.start.y;
    int BCx = circle.x - segment.end.x;
    int BCy = circle.y - segment.end.y;

    // Calcul du produit scalaire pour déterminer la position relative de circle par rapport à point et point2
    int dotABAC = ABx * ACx + ABy * ACy;
    int dotABBC = -ABx * BCx - ABy * BCy;

    // Calcul de la norme du vecteur AB
    float normAB = sqrt(ABx * ABx + ABy * ABy);

    // Calcul de la distance du centre de circle à la droite formée par point et point2
    float distance = fabs(ABx * ACy - ABy * ACx) / normAB;

    // Vérification si la distance est inférieure au rayon du cercle
    if (distance > circle.rayon) 
    {
        return false;
    }

    // Si le cercle touche la droite, vérifier si le point de contact est entre point et point2
    if (dotABAC > 0 && dotABBC > 0) 
    {
        return true;
    }

    // Vérification supplémentaire si le cercle touche les extrémités du segment
    if (sqrt(ACx * ACx + ACy * ACy) <= circle.rayon || sqrt(BCx * BCx + BCy * BCy) <= circle.rayon) 
    {
        return true;
    }

    return false;
}

/**
 * Calcule le déterminant à partir de quatre valeurs.
 * 
 * @param {float} ax - La composante x du premier vecteur.
 * @param {float} ay - La composante y du premier vecteur.
 * @param {float} bx - La composante x du deuxième vecteur.
 * @param {float} by - La composante y du deuxième vecteur.
 * @returns {float} Le déterminant calculé à partir des composantes données.
 * @algorithm Calcul du déterminant pour déterminer la position relative.
 */
static float det(float ax, float ay, float bx, float by) 
{
    return ax * by - ay * bx;
}

/**
 * @brief Vérifie si deux segments se croisent.
 *
 * @details Cette fonction vérifie la collision entre deux segments en calculant
 * et en comparant les déterminants pour déterminer la position relative des segments.
 * Si les produits des déterminants ont des signes opposés, les segments peuvent se croiser.
 * La fonction vérifie ensuite si les points d'intersection projetés se situent entre
 * les points de départ et de fin des segments pour confirmer la collision.
 *
 * @param segment1 Le premier segment pour le test de collision.
 * @param segment2 Le deuxième segment pour le test de collision.
 * @return true si les segments se croisent, false sinon.
 */
bool rc2d_collision_betweenTwoSegment(const RC2D_Segment segment1, const RC2D_Segment segment2) 
{
    // Calcul des vecteurs AB et OP représentant les segments
    RC2D_Point AB = {segment1.end.x - segment1.start.x, segment1.end.y - segment1.start.y};
    RC2D_Point OP = {segment2.end.x - segment2.start.x, segment2.end.y - segment2.start.y};

    // Calcul des vecteurs AP et AO représentant les vecteurs de l'extrémité de segment1 vers les extrémités de segment2
    RC2D_Point AP = {segment2.start.x - segment1.start.x, segment2.start.y - segment1.start.y};
    RC2D_Point BP = {segment2.end.x - segment1.start.x, segment2.end.y - segment1.start.y};

    // Calcul des produits vectoriels pour déterminer les relations de position
    float detABAP = det(AB.x, AB.y, AP.x, AP.y);
    float detABBP = det(AB.x, AB.y, BP.x, BP.y);

    // Vérifier si les produits vectoriels entre les points de segments opposés sont de signes opposés
    if (detABAP * detABBP > 0) 
    {
        return false; // Les points de segment2 sont du même côté de segment1, pas de collision
    }

    // Calcul des vecteurs AO et BO représentant les vecteurs de l'extrémité de segment2 vers les extrémités de segment1
    RC2D_Point AO = {segment1.start.x - segment2.start.x, segment1.start.y - segment2.start.y};
    RC2D_Point BO = {segment1.end.x - segment2.start.x, segment1.end.y - segment2.start.y};

    // Calcul des produits vectoriels pour déterminer les relations de position
    float detOPAO = det(OP.x, OP.y, AO.x, AO.y);
    float detOPBO = det(OP.x, OP.y, BO.x, BO.y);

    // Vérifier si les produits vectoriels entre les points de segments opposés sont de signes opposés
    if (detOPAO * detOPBO > 0) 
    {
        return false; // Les points de segment1 sont du même côté de segment2, pas de collision
    }

    // Si les produits vectoriels entre les points de segments opposés sont de signes opposés, il y a collision
    return true;
}

/**
 * Calcule la normale d'un segment défini par deux points.
 * 
 * @param {RC2D_Point} p1 - Le premier point du segment.
 * @param {RC2D_Point} p2 - Le deuxième point du segment.
 * @return {RC2D_Vector2D} La normale calculée.
 * @algorithm Algorithme de calcul de la normale à partir d'un segment.
 */
static RC2D_Vector2D calculateNormal(RC2D_Point p1, RC2D_Point p2) 
{
    RC2D_Vector2D edge = {p2.x - p1.x, p2.y - p1.y};
    RC2D_Vector2D normal = {-edge.y, edge.x};

    // Normaliser le vecteur normal
    double length = sqrt(normal.x * normal.x + normal.y * normal.y);
    normal.x /= length;
    normal.y /= length;

    return normal;
}

/**
 * Vérifie s'il y a un chevauchement entre deux intervalles.
 * 
 * @param {double} minA - La valeur minimale de l'intervalle A.
 * @param {double} maxA - La valeur maximale de l'intervalle A.
 * @param {double} minB - La valeur minimale de l'intervalle B.
 * @param {double} maxB - La valeur maximale de l'intervalle B.
 * @return {bool} `true` s'il y a un chevauchement, sinon `false`.
 * @algorithm Algorithme de vérification du chevauchement entre deux intervalles.
 */
static bool checkOverlap(double minA, double maxA, double minB, double maxB) 
{
    return !(minA > maxB || minB > maxA);
}

/**
 * Projette les points d'un polygone sur un axe et retourne les valeurs minimales et maximales de la projection.
 * 
 * @param {RC2D_Polygon*} polygon - Le polygone à projeter.
 * @param {RC2D_Vector2D} axis - L'axe sur lequel projeter le polygone.
 * @param {double*} min - Pointeur vers la variable pour stocker la valeur minimale de la projection.
 * @param {double*} max - Pointeur vers la variable pour stocker la valeur maximale de la projection.
 * @algorithm Algorithme de projection d'un polygone sur un axe.
 */
static void projectPolygon(RC2D_Polygon* polygon, RC2D_Vector2D axis, double* min, double* max) 
{
    if (polygon == NULL || polygon->numVertices < 3) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Le polygone est invalide ou ne contient pas suffisamment de sommets dans projectPolygon().\n");
        return;
    }

    *min = INFINITY;
    *max = -INFINITY;

    for (int i = 0; i < polygon->numVertices; i++) 
    {
        // Projection du point sur l'axe : dot product
        double projection = (polygon->vertices[i].x * axis.x + polygon->vertices[i].y * axis.y);
        if (projection < *min) *min = projection;
        if (projection > *max) *max = projection;
    }
}

/**
 * Vérifie s'il y a une collision entre deux polygones en utilisant le test de séparation d'axes (SAT).
 * 
 * @param {RC2D_Polygon*} poly1 - Le premier polygone.
 * @param {RC2D_Polygon*} poly2 - Le deuxième polygone.
 * @return {bool} `true` s'il y a collision, sinon `false`.
 * @algorithm Algorithme de test de collision entre deux polygones en utilisant SAT.
 */
bool rc2d_collision_betweenTwoPolygon(const RC2D_Polygon* poly1, const RC2D_Polygon* poly2) 
{
    if (poly1 == NULL || poly2 == NULL || poly1->numVertices < 3 || poly2->numVertices < 3) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Les polygones sont invalides ou ne contiennent pas suffisamment de sommets dans rc2d_collision_betweenTwoPolygon().\n");
        return false;
    }

    // Vérifier si les polygones sont convexes (nécessaire pour SAT)
    if (!rc2d_math_isConvex(poly1) ||
        !rc2d_math_isConvex(poly2)) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Un des deux polygones n'est pas convexe.\n");
        return false;
    }

    // Axes du premier polygone
    for (int i = 0; i < poly1->numVertices; i++) 
    {
        RC2D_Vector2D axis = calculateNormal(poly1->vertices[i], poly1->vertices[(i + 1) % poly1->numVertices]);
        double minA, maxA, minB, maxB;
        projectPolygon(poly1, axis, &minA, &maxA);
        projectPolygon(poly2, axis, &minB, &maxB);

        if (!checkOverlap(minA, maxA, minB, maxB)) 
        {
            return false; // Pas de collision
        }
    }

    // Axes du deuxieme polygone
    for (int i = 0; i < poly2->numVertices; i++) 
    {
        // Calculer l'axe normal pour l'arête courante de poly2
        RC2D_Vector2D axis = calculateNormal(poly2->vertices[i], poly2->vertices[(i + 1) % poly2->numVertices]);

        // Projeter les deux polygones sur cet axe
        double minA, maxA, minB, maxB;
        projectPolygon(poly1, axis, &minA, &maxA);
        projectPolygon(poly2, axis, &minB, &maxB);

        // Vérifier s'il y a une séparation sur cet axe
        if (!checkOverlap(minA, maxA, minB, maxB)) 
        {
            return false; // Pas de collision si une séparation est trouvée
        }
    }

    return true; // Collision détectée
}

/**
 * Vérifie la collision entre un polygone et un cercle. Cette fonction effectue trois vérifications :
 * 1. Si un sommet du polygone est à l'intérieur du cercle.
 * 2. Si un bord du polygone intersecte avec le cercle.
 * 3. Si le centre du cercle est à l'intérieur du polygone.
 * 
 * Cette approche complète assure une détection précise pour divers scénarios d'intersection.
 *
 * @param polygon Un pointeur vers une structure RC2D_Polygon représentant le polygone à vérifier.
 * @param circle Une structure RC2D_Circle représentant le cercle à vérifier.
 * @return true s'il y a une collision entre le polygone et le cercle, false autrement.
 */
bool rc2d_collision_betweenPolygonCircle(const RC2D_Polygon* polygon, const RC2D_Circle circle) 
{
    if (polygon == NULL || polygon->numVertices < 3) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Le polygone est invalide ou ne contient pas suffisamment de sommets dans rc2d_collision_betweenPolygonCircle().\n");
        return false;
    }

    // Check if any vertex of the polygon is inside the circle
    for (int i = 0; i < polygon->numVertices; i++) 
    {
        if (rc2d_collision_pointInCircle(polygon->vertices[i], circle))
        {
            return true;
        }
    }

    // Check for edge-circle intersection
    for (int i = 0; i < polygon->numVertices; i++) 
    {
        RC2D_Point start = polygon->vertices[i];
        RC2D_Point end = polygon->vertices[(i + 1) % polygon->numVertices];
        RC2D_Segment edge = {start, end};

        if (rc2d_collision_betweenCircleSegment(edge, circle)) 
        {
            return true;
        }
    }

    // Additional check: circle's center inside the polygon
    if (rc2d_collision_pointInPolygon((RC2D_Point){circle.x, circle.y}, polygon)) 
    {
        return true;
    }

    return false;
}

/**
 * Vérifie si un segment et un polygone se chevauchent.
 *
 * Cette fonction détermine si un segment spécifié par deux points
 * intersecte ou touche un polygone. La collision est détectée
 * en vérifiant l'intersection entre le segment et chaque arête du polygone.
 *
 * @param segment Le segment avec lequel tester la collision.
 * @param polygon Le polygone avec lequel tester la collision.
 * @return true si le segment et le polygone se chevauchent, false sinon.
 */
bool rc2d_collision_betweenPolygonSegment(const RC2D_Segment segment, const RC2D_Polygon* polygon)
{
    if (polygon == NULL || polygon->numVertices < 3) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Le polygone est invalide ou ne contient pas suffisamment de sommets dans rc2d_collision_betweenPolygonSegment().\n");
        return false;
    }

    // Vérifier l'intersection entre le segment et chaque arête du polygone
    for (int i = 0; i < polygon->numVertices; i++) 
    {
        // Obtenir le point de départ et de fin de l'arête du polygone
        RC2D_Point start = polygon->vertices[i];
        RC2D_Point end = polygon->vertices[(i + 1) % polygon->numVertices];
        RC2D_Segment edge = {start, end};

        // Utiliser la fonction existante pour vérifier l'intersection entre deux segments
        if (rc2d_collision_betweenTwoSegment(segment, edge)) 
        {
            return true; // Intersection détectée
        }
    }

    // Vérifier si un point du segment est à l'intérieur du polygone
    // Cela couvre le cas où le segment est entièrement à l'intérieur du polygone
    if (rc2d_collision_pointInPolygon(segment.start, polygon) || rc2d_collision_pointInPolygon(segment.end, polygon)) 
    {
        return true;
    }

    return false; // Aucune intersection trouvée
}

/**
 * Calcule la distance entre deux points dans un espace 2D.
 * 
 * @param {RC2D_Point} a - Le premier point.
 * @param {RC2D_Point} b - Le deuxième point.
 * @return {float} La distance entre les deux points.
 * @algorithm Algorithme de la distance euclidienne.
 */
static float rc2d_math_distanceBetweenPoints(const RC2D_Point a, const RC2D_Point b) 
{
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return sqrt(dx * dx + dy * dy);
}

/**
 * Échange les valeurs de deux variables de type float.
 * 
 * @param {float*} a - Pointeur vers la première variable.
 * @param {float*} b - Pointeur vers la deuxième variable.
 */
static void swap(float* a, float* b) 
{
    float temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Effectue un test de collision entre un rayon et un segment.
 * 
 * @param {RC2D_Ray} ray - Le rayon utilisé pour le test de collision.
 * @param {RC2D_Segment} segment - Le segment avec lequel tester la collision.
 * @param {RC2D_Point*} intersection - Pointeur vers une structure pour stocker les coordonnées du point d'intersection si une collision est détectée.
 * @return {bool} `true` si une collision est détectée, sinon `false`.
 */
bool rc2d_collision_raycastSegment(const RC2D_Ray ray, const RC2D_Segment segment, RC2D_Point* intersection) 
{
    RC2D_Vector2D v1 = {ray.origin.x - segment.start.x, ray.origin.y - segment.start.y};
    RC2D_Vector2D v2 = {segment.end.x - segment.start.x, segment.end.y - segment.start.y};
    RC2D_Vector2D v3 = {-ray.direction.y, ray.direction.x};

    double dot = v2.x * v3.x + v2.y * v3.y;
    if (fabs(dot) < 0.000001) 
    {
        return false; // Les lignes sont parallèles
    }

    double t1 = (v2.x * v1.y - v2.y * v1.x) / dot;
    double t2 = (v1.x * v3.x + v1.y * v3.y) / dot;

    if (t1 >= 0.0 && t2 >= 0.0 && t2 <= 1.0 && t1 <= ray.length) 
    {
        intersection->x = ray.origin.x + t1 * ray.direction.x;
        intersection->y = ray.origin.y + t1 * ray.direction.y;
        return true;
    }

    return false;
}

/**
 * Effectue un test de collision entre un rayon et une boîte englobante axis-aligned (AABB).
 * 
 * @param {RC2D_Ray} ray - Le rayon utilisé pour le test de collision.
 * @param {RC2D_AABB} box - La boîte englobante AABB avec laquelle tester la collision.
 * @param {RC2D_Point*} intersection - Pointeur vers une structure pour stocker les coordonnées du point d'intersection si une collision est détectée.
 * @return {bool} `true` si une collision est détectée, sinon `false`.
 */
bool rc2d_collision_raycastAABB(const RC2D_Ray ray, const RC2D_AABB box, RC2D_Point* intersection) 
{
    // Calculer le point de fin du rayon basé sur sa longueur
    double endX = ray.origin.x + ray.direction.x * ray.length;
    double endY = ray.origin.y + ray.direction.y * ray.length;

    // Calculer les paramètres d'intersection le long de l'axe x
    double tminX = (box.x - ray.origin.x) / ray.direction.x;
    double tmaxX = (box.x + box.w - ray.origin.x) / ray.direction.x;

    // Trier les paramètres d'intersection pour s'assurer que tminX est le plus petit
    if (tminX > tmaxX) 
    {
        double temp = tminX;
        tminX = tmaxX;
        tmaxX = temp;
    }

    // Calculer les paramètres d'intersection le long de l'axe y
    double tminY = (box.y - ray.origin.y) / ray.direction.y;
    double tmaxY = (box.y + box.h - ray.origin.y) / ray.direction.y;

    // Trier les paramètres d'intersection pour s'assurer que tminY est le plus petit
    if (tminY > tmaxY) 
    {
        double temp = tminY;
        tminY = tmaxY;
        tmaxY = temp;
    }

    // Trouver le paramètre d'intersection le plus grand parmi les paramètres d'intersection minimaux
    double tmin = fmax(tminX, tminY);

    // Vérifier si le point d'intersection est au-delà du point de fin du rayon
    if (tmin > ray.length)
        return false;

    // Trouver le paramètre d'intersection le plus petit parmi les paramètres d'intersection maximaux
    double tmax = fmin(tmaxX, tmaxY);

    // Vérifier si les intervalles d'intersection se chevauchent
    if (tmax < 0 || tmin > tmax)
        return false;

    // Calculer les coordonnées du point d'intersection
    intersection->x = ray.origin.x + tmin * ray.direction.x;
    intersection->y = ray.origin.y + tmin * ray.direction.y;

    return true;
}

/**
 * Effectue un test de collision entre un rayon et un cercle.
 * 
 * @param {RC2D_Ray} ray - Le rayon utilisé pour le test de collision.
 * @param {RC2D_Circle} circle - Le cercle avec lequel tester la collision.
 * @param {RC2D_Point*} intersection - Pointeur vers une structure pour stocker les coordonnées du point d'intersection si une collision est détectée.
 * @return {bool} `true` si une collision est détectée, sinon `false`.
 */
bool rc2d_collision_raycastCircle(const RC2D_Ray ray, const RC2D_Circle circle, RC2D_Point* intersection) 
{
    // Calculer le point de fin du rayon basé sur sa longueur
    double endX = ray.origin.x + ray.direction.x * ray.length;
    double endY = ray.origin.y + ray.direction.y * ray.length;
    
    // Calculer la distance entre l'origine du rayon et le centre du cercle
    RC2D_Vector2D L = {circle.x - ray.origin.x, circle.y - ray.origin.y};
    double tca = L.x * ray.direction.x + L.y * ray.direction.y;
    
    // Si la distance est négative et que le cercle est derrière le rayon, pas de collision
    if (tca < 0)
        return false;
    
    // Calculer la distance entre le point d'impact le plus proche du rayon et le centre du cercle
    double d2 = L.x * L.x + L.y * L.y - tca * tca;
    double radius2 = circle.rayon * circle.rayon;

    // Vérifier si le rayon intersecte le cercle en comparant les distances
    if (d2 > radius2) 
        return false;

    // Calculer la distance entre le point d'impact le plus proche et le point d'intersection
    double thc = sqrt(radius2 - d2);
    double t0 = tca - thc;

    // Si t0 est négatif, l'intersection est derrière le rayon, ajuster pour obtenir l'intersection la plus proche
    if (t0 < 0) 
        t0 += 2 * thc;

    // Vérifier si le point d'intersection est au-delà du point de fin du rayon
    if (t0 > ray.length)
        return false;

    // Calculer les coordonnées du point d'intersection
    intersection->x = ray.origin.x + t0 * ray.direction.x;
    intersection->y = ray.origin.y + t0 * ray.direction.y;

    return true;
}

/**
 * Trace une ligne basée sur un rayon et vérifie la collision avec des pixels solides dans une image.
 * 
 * Utilise l'algorithme de tracé de ligne de Bresenham pour parcourir chaque pixel le long du chemin du rayon.
 * Vérifie si un pixel sur le trajet est solide (non transparent) en utilisant un masque de collision
 * stocké dans `imageData`. Si un pixel solide est rencontré, la fonction retourne `true` et définit
 * le point d'intersection au premier pixel solide rencontré.
 * 
 * @param {RC2D_ImageData*} imageData - Données de l'image incluant la largeur, la hauteur et le masque de collision.
 * @param {RC2D_Ray} ray - Rayon utilisé pour tracer la ligne, incluant l'origine, la direction et la longueur du rayon.
 * @param {RC2D_Point*} intersection - Pointeur vers une structure pour stocker le point d'intersection si une collision est détectée.
 * @return {bool} `true` si une collision est détectée avec un pixel solide, sinon `false`.
 */
static bool traceLineAndCheckCollision(const RC2D_ImageData* imageData, const RC2D_Ray ray, RC2D_Point* intersection) 
{
    if (imageData == NULL || imageData->mask == NULL) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Les données de l'image sont invalides dans traceLineAndCheckCollision().\n");
        return false;
    }

    // Calculer le point de fin du rayon
    int x1 = (int)(ray.origin.x + ray.direction.x * ray.length);
    int y1 = (int)(ray.origin.y + ray.direction.y * ray.length);

    int x0 = (int)ray.origin.x;
    int y0 = (int)ray.origin.y;

    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    for (;;) 
    {
        if (x0 >= 0 && x0 < imageData->width && y0 >= 0 && y0 < imageData->height) 
        {
            if (imageData->mask[y0 * imageData->width + x0]) 
            {
                if (intersection) 
                {
                    intersection->x = x0;
                    intersection->y = y0;
                }

                return true; // Collision détectée
            }
        }

        if (x0 == x1 && y0 == y1) 
            break;

        e2 = 2 * err;
        if (e2 >= dy) 
        { 
            err += dy; x0 += sx; 
        }
        if (e2 <= dx) 
        { 
            err += dx; y0 += sy; 
        }
    }

    return false; // Aucune collision détectée
}

/**
 * Effectue un raycasting pixel parfait pour détecter les collisions avec une image en utilisant un masque de collision.
 * 
 * Trace un rayon depuis l'origine du rayon dans la direction spécifiée jusqu'à une certaine longueur, puis vérifie
 * s'il y a une collision avec des pixels solides (non transparents) dans l'image représentée par `imageData`.
 * Si une collision est détectée, les coordonnées du point d'intersection sont stockées dans la structure `intersection`.
 * 
 * @param {RC2D_ImageData*} imageData - Données de l'image incluant la largeur, la hauteur et le masque de collision.
 * @param {RC2D_Ray} ray - Rayon utilisé pour effectuer le raycasting, avec une origine et une direction.
 * @param {RC2D_Point*} intersection - Pointeur vers une structure pour stocker les coordonnées du point d'intersection si une collision est détectée.
 * @return {bool} `true` si une collision avec un pixel solide est détectée, sinon `false`.
 */
bool rc2d_collision_raycastPixelPerfect(const RC2D_ImageData* imageData, const RC2D_Ray ray, RC2D_Point* intersection)
{
    if (imageData == NULL || imageData->mask == NULL) 
    {
        rc2d_log(RC2D_LOG_ERROR, "Les données de l'image sont invalides dans rc2d_collision_raycastPixelPerfect().\n");
        return false;
    }

    return traceLineAndCheckCollision(imageData, ray, intersection);
}