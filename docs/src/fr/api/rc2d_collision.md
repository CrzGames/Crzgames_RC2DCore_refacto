
# Collision Module API Documentation

Welcome to the RC2D Collision API documentation. This section outlines the functionalities provided by the RC2D game engine for collision detection, including tests for point-in-shape and collision between geometric shapes.

## Structures

### `RC2D_Circle`

Represents a circle with a position and radius.

- **Fields:**
  - `int x, y;` - The x and y coordinates of the circle's center.
  - `int radius;` - The radius of the circle.

```c
typedef struct RC2D_Circle {
  int x;
  int y;
  int radius;
} RC2D_Circle;
```

### `RC2D_AABB`

Represents an Axis-Aligned Bounding Box.

- **Fields:**
  - `int x, y;` - The top-left corner coordinates of the box.
  - `int w, h;` - The width and height of the box.

```c
typedef struct RC2D_AABB {
  int x;
  int y;
  int w;
  int h;
} RC2D_AABB;
```

## API Functions

### Checking if a Point is Inside an AABB

Determines whether a given point (with coordinates `x`, `y`) is inside an AABB.

- `x`: The x-coordinate of the point.
- `y`: The y-coordinate of the point.
- `RC2D_AABB box`: The AABB structure to check against.

```c
bool rc2d_collision_pointInAABB(int x, int y, RC2D_AABB box);
```

### Collision Between Two AABBs

Checks if two AABBs intersect.

- `RC2D_AABB box1`: The first AABB structure.
- `RC2D_AABB box2`: The second AABB structure to check for intersection with the first.

```c
bool rc2d_collision_betweenTwoAABB(RC2D_AABB box1, RC2D_AABB box2);
```

### Checking if a Point is Inside a Circle

Determines whether a given point is inside a circle.

- `x`: The x-coordinate of the point.
- `y`: The y-coordinate of the point.
- `RC2D_Circle C`: The circle structure to check against.

```c
bool rc2d_collision_pointInCircle(int x, int y, RC2D_Circle C);
```

### Collision Between Two Circles

Checks if two circles touch each other.

- `RC2D_Circle C1`: The first circle structure.
- `RC2D_Circle C2`: The second circle structure to check for collision with the first.

```c
bool rc2d_collision_betweenTwoCircle(RC2D_Circle C1, RC2D_Circle C2);
```

## Conclusion

This documentation provides an overview of the collision functionalities available in the RC2D game engine. For more detailed information or assistance, please refer to the RC2D game engine support resources.
