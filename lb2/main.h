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

   edge(double ymin_, double ymax_, double xstart_, double xend_) :
      ymax{ymax_}, xstart{xstart_}, xend{xend_}, dx{(xend_ - xstart_) / (ymax_ - ymin_)} { }

   operator < (const edge &other) { return xstart < other.xstart; }
};

#endif
