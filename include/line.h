#ifndef GRAPHICS_LINE_H
#define GRAPHICS_LINE_H

#include "point.h"
#include "shape.h"
#include "cast.h"

namespace graphics {

enum class clipmode
{
   width  = 1,
   height = 2,
   depth  = 3
};

// Line class, that doesn't check if it's coordinates fit into provided rasterizer areasize.
class line : public shape, protected rasterizer
{
   public:
      using ptype = double;
      using lpoint = hsv_point<ptype>;

      template <typename T>
      line(raster_t *rast, const hsv_point<T> &p1, const hsv_point<T> &p2) :
         rasterizer(rast), start(p1), end(p2) { }

      // Some kind of generic interface for any type of color point. Inteface specifies color() call,
      // so internal HSV color type should have a constructor accepting uint32_t initializer.
      template <typename T>
      line(raster_t *rast, const color_point<T> &p1, const color_point<T> &p2) :
         rasterizer(rast), start(p1, p1.color()), end(p2, p2.color()) { }

      virtual void draw() override { render(); }

   protected:
      void render();

      lpoint start;
      lpoint end;
};

// Checks if it fits.
class area_line : protected line
{
   public:
      using line::line;
      void draw() override { if (areacheck(raster->box(), start, end)) render(); }

   private:
      bool areacheck(const area_coord &box, hsv_point<double> &start, hsv_point<double> &end);
};

hsv_point<double> clipline(const hsv_point<double> &p1, const hsv_point<double> &p2, double newval, clipmode mode);

}

#endif
