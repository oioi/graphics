#ifndef GRAPHICS_LINE_H
#define GRAPHICS_LINE_H

#include "point.h"
#include "shape.h"
#include "cast.h"

namespace graphics {

class line : public shape, private rasterizer
{
   public:
      // Maybe we will have to clip line to fit area, so we're using double as internal point type.
      using lpoint = hsv_point<double>;
      void draw() override { if (!prepared) areacheck(); if (inside) render(); }

      line(raster_t *rast, const lpoint &p1, const lpoint &p2) :
         rasterizer(rast), start{p1}, end{p2} { }

      template <typename T>
      line(raster_t *rast, const hsv_point<T> &p1, const hsv_point<T> &p2) : rasterizer(rast),
         start{narrow_cast<double, T>(p1.x), narrow_cast<double, T>(p1.y), p1},
         end{narrow_cast<double, T>(p2.x), narrow_cast<double, T>(p2.y), p2} { }

      // Some kind of generic interface for any type of color point. Inteface specifies color() call,
      // so internal HSV color type should have a constructor accepting uint32_t initializer.
      template <typename T>
      line(raster_t *rast, const color_point<T> &p1, const color_point<T> &p2) :
         rasterizer(rast), start{p1, p1.color()}, end{p2, p2.color()} { }

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
