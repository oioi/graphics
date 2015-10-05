#ifndef GRAPHICS_LB2
#define GRAPHICS_LB2

#include <vector>
#include "graphics.h"

using namespace graphics;

struct edge
{
   double ymax;
   double xstart;
   double xend;
   double dx;

   hsv_color st_color;
   hsv_color end_color;
   double dh;
   double ds;
   double dv;

   edge(double ymin_, double ymax_, double xstart_, double xend_, const hsv_color &st, const hsv_color &end);
   edge(const edge &other) = default;

   void shift();
   bool operator < (const edge &other) { return xstart < other.xstart; }
};

#endif
