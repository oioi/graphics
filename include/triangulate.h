#ifndef GRAPHICS_TRIANGULATE_H
#define GRAPHICS_TRIANGULATE_H

/* Based on code by John W. Ratcliff (jratcliff@verant.com) on FlipCode.com */

#include <vector>
#include "point.h"

namespace graphics {

using vertices = std::vector<point<double>>;

vertices decompose(const vertices &vert);                             // Decompose polygon into series of triangles
double area(const vertices &contour);                                 // Area of contour/polygon
bool inside_triangle(point<double> p, const vertices &triangle);      // if point is inside triangle

}

#endif
