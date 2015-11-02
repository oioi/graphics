#include <iostream>
#include <bitset>
#include <cmath>

#include "line.h"

namespace graphics {

hsv_point<double> clipline(const hsv_point<double> &p1, const hsv_point<double> &p2, double newval, clipmode mode)
{
   double dx = p2.x - p1.x;
   double dy = p2.y - p1.y;
   double dz = p2.z - p1.z;

   hsv_point<double> result {p1};
   hsv_color dcol {p2.colcomp() - p1.colcomp()};

   switch (mode)
   {
      case clipmode::width:
         result.y += dy * (newval - result.x) / dx;
         result.z += dz * (newval - result.x) / dx;
         result.colcomp() += dcol * (newval - result.x) / dx;
         result.x = newval;
         break;

      case clipmode::height:
         result.x += dx * (newval - result.y) / dy;
         result.z += dz * (newval - result.y) / dy;
         result.colcomp() += dcol * (newval - result.y) / dy;
         result.y = newval;
         break;

      case clipmode::depth:
         result.x += dx * (newval - result.z) / dz;
         result.y += dy * (newval - result.z) / dz;
         result.colcomp() += dcol * (newval - result.z) / dz;
         result.z = newval;
         break;
   }

   return result;
}

bool area_line::areacheck(const area_coord &box, hsv_point<double> &start, hsv_point<double> &end)
{
   for (posval k1, k2;;)
   {
      k1 = position_code(box, start);
      k2 = position_code(box, end);

      if (0 == (k1 | k2))
      {
         #ifndef GRC_NO_DEBUG
         std::cerr << "K1 = K2 = 0. Line is inside the area." << std::endl;
         #endif
         return true;
      }

      if (0 != (k1 & k2))
      {
         #ifndef GRC_NO_DEBUG
         std::cerr << "K1 & K2 != 0. Line is outside the area." << std::endl;
         #endif
         return false;
      }

      if (0 != k1)
      {
         if      (k1 & position::left)   start = clipline(start, end, box.first.x,  clipmode::width);
         else if (k1 & position::right)  start = clipline(start, end, box.second.x, clipmode::width);
         else if (k1 & position::bottom) start = clipline(start, end, box.second.y, clipmode::height);
         else if (k1 & position::top)    start = clipline(start, end, box.first.y,  clipmode::height);
      }

      else
      {
         if      (k2 & position::left)   end = clipline(end, start, box.first.x,  clipmode::width);
         else if (k2 & position::right)  end = clipline(end, start, box.second.x, clipmode::width);
         else if (k2 & position::bottom) end = clipline(end, start, box.second.y, clipmode::height);
         else if (k2 & position::top)    end = clipline(end, start, box.first.y,  clipmode::height);
      }
   }
}

void line::render()
{
   #ifndef GRC_NO_DEBUG
   if (0 > start.x || 0 > start.y || 0 > end.x || 0 > end.y)
      throw std::range_error {"Line's points have negative coordinates."};
   #endif

   hsv_point<> rstart {lrint(start.x), lrint(start.y), start};
   hsv_point<> rend {lrint(end.x), lrint(end.y), end};

   const long dx = (rend.x > rstart.x) ? rend.x - rstart.x : rstart.x - rend.x;
   const long dy = (rend.y > rstart.y) ? rend.y - rstart.y : rstart.y - rend.y;
   const long sx = rend.x > rstart.x ? 1 : -1;
   const long sy = rend.y > rstart.y ? 1 : -1;

   long err = dx - dy;
   hsv_color dcol {(rend - rstart) / (dx > dy ? dx : dy)};

   for (long e2; ; rstart.colcomp() += dcol)
   {
      raster->setpixel(rstart);
      if (rstart == rend) break;

      e2 = 2 * err;
      if (e2 > -dy) { err -= dy; rstart.x += sx; }
      if (e2 < dx)  { err += dx; rstart.y += sy; }
   }
}

}
