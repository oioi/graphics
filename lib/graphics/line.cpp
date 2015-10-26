#include <iostream>
#include <bitset>
#include <cmath>

#include "line.h"

namespace graphics {

void clipline(const area_coord &box, hsv_point<double> &p, double dx, double dy, const hsv_color &dcol, posval &code, bool recheck)
{
   if (code & position::left)
   {
      std::cerr << "Left clipping: ";
      p.y += dy * (box.first.x - p.x) / dx;
      p.colcomp() += dcol * (box.first.x - p.x) / dx;
      p.x = box.first.x;
   }
   else if (code & position::right)
   {
      std::cerr << "Right clipping: ";
      p.y += dy * (box.second.x - p.x) / dx;
      p.colcomp() += dcol * (box.second.x - p.x) / dx;
      p.x = box.second.x;
   }
   else if (code & position::bottom)
   {
      std::cerr << "Bottom clipping: ";
      p.x += dx * (box.second.y - p.y) / dy;
      p.colcomp() += dcol * (box.second.y - p.y) / dy;
      p.y = box.second.y;
   }
   else if (code & position::top)
   {
      std::cerr << "Top clipping: ";
      p.x += dx * (box.first.y - p.y) / dy;
      p.colcomp() += dcol * (box.first.y - p.y) / dy;
      p.y = box.first.y;
   }

   std::cout << p << std::endl;
   if (recheck) code = position_code(box, p);
}

bool line_areacheck(const area_coord &box, hsv_point<double> &start, hsv_point<double> &end)
{
   posval k1 = position_code(box, start);
   posval k2 = position_code(box, end);

   for (;;)
   {
      if (0 == (k1 | k2))
      {
         std::cerr << "K1 = K2 = 0. Line is inside the area." << std::endl;
         return true;
      }

      if (0 != (k1 & k2))
      {
         std::cerr << "K1 & K2 != 0. Line is outside the area." << std::endl;
         return false;
      }

      if (0 != k1) clipline(box, start, start.x - end.x, start.y - end.y, start - end, k1, true);
      else clipline(box, end, start.x - end.x, start.y - end.y, start - end, k2, true);
   }
}

void area_line::draw()
{
   if (!checked)
   {
      checked = true;
      start = orig_start;
      end = orig_end;
      inside = line_areacheck(raster->box(), start, end);
   }

   if (inside) render();
}

void line::render()
{
   if (0 > start.x || 0 > start.y || 0 > end.x || 0 > end.y)
      throw std::range_error {"Line's points have negative coordinates."};

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
