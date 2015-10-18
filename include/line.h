#ifndef GRAPHICS_LINE_H
#define GRAPHICS_LINE_H

#include "point.h"
#include "shape.h"
#include "cast.h"

namespace graphics {

// Line class, that doesn't check if it's coordinates fit into provided rasterizer areasize.
class line : public shape, protected rasterizer
{
   public:
      using ptype = unsigned long;
      using lpoint = hsv_point<ptype>;

      template <typename T>
      line(raster_t *rast, const hsv_point<T> &p1, const hsv_point<T> &p2) :
         rasterizer(rast), start{p1}, end{p2} { }

      // Some kind of generic interface for any type of color point. Inteface specifies color() call,
      // so internal HSV color type should have a constructor accepting uint32_t initializer.
      template <typename T>
      line(raster_t *rast, const color_point<T> &p1, const color_point<T> &p2) :
         rasterizer(rast), start{p1, p1.color()}, end{p2, p2.color()} { }

      virtual void draw() override { render(); }

   protected:
      lpoint start;
      lpoint end;

      void render();
};

// Checks if it fits.
class area_line : protected line
{
   public:
      using ptype = unsigned long;
      using lpoint = hsv_point<ptype>;

      template <typename T>
      area_line(raster_t *rast, const hsv_point<T> &p1, const hsv_point<T> &p2) :
         line(rast, p1, p2), orig_start{p1}, orig_end{p2} { }

      template <typename T2>
      area_line(raster_t *rast, const color_point<T2> &p1, const color_point<T2> &p2) :
         line(rast, p1, p2), orig_start{p1, p1.color()}, orig_end{p2, p2.color()} { }

      void draw() override;

   private:
      bool checked {false};
      bool inside {false};

      lpoint orig_start;
      lpoint orig_end;
};

}

#endif
