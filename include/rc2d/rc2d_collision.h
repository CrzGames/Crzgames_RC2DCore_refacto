#ifndef RC2D_COLLISION_H
#define RC2D_COLLISION_H

#include "rc2d/rc2d_math.h" 
#include "rc2d/rc2d_graphics.h"

#include <stdbool.h> // Required for : bool

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure représentant une boîte englobante alignée sur les axes (AABB),
 * utilisée pour les calculs de collision.
 * @typedef {object} RC2D_AABB
 * @property {number} x - La coordonnée x du coin supérieur gauche de la boîte.
 * @property {number} y - La coordonnée y du coin supérieur gauche de la boîte.
 * @property {number} w - La largeur de la boîte.
 * @property {number} h - La hauteur de la boîte.
 */
typedef struct RC2D_AABB {
	int x;
	int y;
	int w;
	int h;
} RC2D_AABB;

// Collision Point in Shape
bool rc2d_collision_pointInAABB(const RC2D_Point point, const RC2D_AABB box);
bool rc2d_collision_pointInCircle(const RC2D_Point point, const RC2D_Circle circle);
bool rc2d_collision_pointInPolygon(const RC2D_Point point, const RC2D_Polygon* polygon);

// Collision Shape vs Shape
bool rc2d_collision_betweenTwoAABB(const RC2D_AABB box1, const RC2D_AABB box2);
bool rc2d_collision_betweenTwoCircle(const RC2D_Circle circle1, const RC2D_Circle circle2);
bool rc2d_collision_betweenAABBCircle(const RC2D_AABB box, const RC2D_Circle circle);
bool rc2d_collision_betweenCircleSegment(const RC2D_Segment segment, const RC2D_Circle circle);
bool rc2d_collision_betweenTwoSegment(const RC2D_Segment segment1, const RC2D_Segment segment2);
bool rc2d_collision_betweenTwoPolygon(const RC2D_Polygon* poly1, const RC2D_Polygon* poly2);
bool rc2d_collision_betweenPolygonCircle(const RC2D_Polygon* polygon, const RC2D_Circle circle);
bool rc2d_collision_betweenPolygonSegment(const RC2D_Segment segment, const RC2D_Polygon* polygon);

// Collision Raycast vs Shape or Image
// TODO: Tester les fonctions de collision raycast
bool rc2d_collision_raycastCircle(const RC2D_Ray ray, const RC2D_Circle circle, RC2D_Point* intersection);
bool rc2d_collision_raycastAABB(const RC2D_Ray ray, const RC2D_AABB box, RC2D_Point* intersection);
bool rc2d_collision_raycastSegment(const RC2D_Ray ray, const RC2D_Segment segment, RC2D_Point* intersection);
bool rc2d_collision_raycastPixelPerfect(const RC2D_ImageData* imageData, const RC2D_Ray ray, RC2D_Point* intersection);

#ifdef __cplusplus
}
#endif

#endif // RC2D_COLLISION_H
