#include <criterion/criterion.h>
#include "rc2d/rc2d_collision.h"

Test(rc2d_collision, pointInAABB_inside) {
    RC2D_Point point = {5, 5};
    RC2D_AABB box = {0, 0, 10, 10};
    cr_assert(rc2d_collision_pointInAABB(point, box));
}

Test(rc2d_collision, pointInAABB_outside) {
    RC2D_Point point = {15, 15};
    RC2D_AABB box = {0, 0, 10, 10};
    cr_assert_not(rc2d_collision_pointInAABB(point, box));
}

Test(rc2d_collision, pointInCircle_inside) {
    RC2D_Point point = {3, 4};
    RC2D_Circle circle = {0, 0, 5};
    cr_assert(rc2d_collision_pointInCircle(point, circle));
}

Test(rc2d_collision, pointInCircle_outside) {
    RC2D_Point point = {6, 6};
    RC2D_Circle circle = {0, 0, 5};
    cr_assert_not(rc2d_collision_pointInCircle(point, circle));
}

Test(rc2d_collision, betweenTwoAABB_overlap) {
    RC2D_AABB box1 = {0, 0, 10, 10};
    RC2D_AABB box2 = {5, 5, 10, 10};
    cr_assert(rc2d_collision_betweenTwoAABB(box1, box2));
}

Test(rc2d_collision, betweenTwoAABB_no_overlap) {
    RC2D_AABB box1 = {0, 0, 10, 10};
    RC2D_AABB box2 = {20, 20, 5, 5};
    cr_assert_not(rc2d_collision_betweenTwoAABB(box1, box2));
}

Test(rc2d_collision, betweenTwoCircle_overlap) {
    RC2D_Circle c1 = {0, 0, 5};
    RC2D_Circle c2 = {5, 0, 5};
    cr_assert(rc2d_collision_betweenTwoCircle(c1, c2));
}

Test(rc2d_collision, betweenTwoCircle_no_overlap) {
    RC2D_Circle c1 = {0, 0, 5};
    RC2D_Circle c2 = {20, 0, 5};
    cr_assert_not(rc2d_collision_betweenTwoCircle(c1, c2));
}

Test(rc2d_collision, betweenAABBCircle_overlap) {
    RC2D_AABB box = {0, 0, 10, 10};
    RC2D_Circle circle = {5, 5, 3};
    cr_assert(rc2d_collision_betweenAABBCircle(box, circle));
}

Test(rc2d_collision, betweenAABBCircle_no_overlap) {
    RC2D_AABB box = {0, 0, 10, 10};
    RC2D_Circle circle = {20, 20, 3};
    cr_assert_not(rc2d_collision_betweenAABBCircle(box, circle));
}