#ifndef GRAPHICS_LINE_H
#define GRAPHICS_LINE_H

#include "point.h"
#include "shape.h"
#include "cast.h"

namespace graphics {

class line : public shape, private rasterizer
{
   public:
      using lpoint = hsv_point<double>;
      void draw() override { if (!prepared) areacheck(); if (inside) render(); }

      line(raster_t *rast, const lpoint &p1, const lpoint &p2) :
         rasterizer{rast}, start{p1}, end{p2} { }

      template <typename T> line(raster_t *rast, const hsv_point<T> &p1, const hsv_point<T> &p2) : rasterizer{rast},
         start{narrow_cast<T, double>(p1.x), narrow_cast<T, double>(p1.y), p1},
         end{narrow_cast<T, double>(p2.x), narrow_cast<T, double>(p2.y), p2} { }      

   private:
      bool prepared {false};
      bool inside {false};

      lpoint start;
      lpoint end;

      void render();
      void areacheck();
};

}

#endif
