#include <iostream>
#include <bitset>
#include <cmath>

#include "line.h"

namespace graphics {

void clipline(const area_coord &box, hsv_point<double> &p, double dx, double dy, const hsv_color &dcol, uint8_t &code)
{
   if (code & left)
   {
      std::cerr << "Left clipping: ";
      p.y += dy * (box.first.x - p.x) / dx;
      p.colcomp() += dcol * (box.first.x - p.x) / dx;
      p.x = box.first.x;
   }
   else if (code & right)
   {
      std::cerr << "Right clipping: ";
      p.y += dy * (box.second.x - p.x) / dx;
      p.colcomp() += dcol * (box.second.x - p.x) / dx;
      p.x = box.second.x;
   }
   else if (code & bottom)
   {
      std::cerr << "Bottom clipping: ";
      p.x += dx * (box.second.y - p.y) / dy;
      p.colcomp() += dcol * (box.second.y - p.y) / dy;
      p.y = box.second.y;
   }
   else if (code & top)
   {
      std::cerr << "Top clipping: ";
      p.x += dx * (box.first.y - p.y) / dy;
      p.colcomp() += dcol * (box.first.y - p.y) / dy;
      p.y = box.first.y;
   }

   std::cout << p << std::endl;
   code = position_code(box, p);
}

bool line_areacheck(const area_coord &box, hsv_point<> &start, hsv_point<> &end)
{
   hsv_point<double> first {start};
   hsv_point<double> second {end};

   uint8_t k1 = position_code(box, start);
   uint8_t k2 = position_code(box, end);

   for (;;)
   {
      if (0 == (k1 | k2))
      {
         std::cerr << "K1 = K2 = 0. Line is inside the area." << std::endl;
         start = {static_cast<unsigned long>(lrint(first.x)), static_cast<unsigned long>(lrint(first.y)), first};
         end = {static_cast<unsigned long>(lrint(second.x)), static_cast<unsigned long>(lrint(second.y)), second};
         return true;
      }

      if (0 != (k1 & k2))
      {
         std::cerr << "K1 & K2 != 0. Line is outside the area." << std::endl;
         return false;
      }

      if (0 != k1) clipline(box, first, first.x - second.x, first.y - second.y, first - second, k1);
      else clipline(box, second, first.x - second.x, first.y - second.y, first - second, k2);
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
   hsv_point<> it = start;

   const long dx = (end.x > start.x) ? end.x - start.x : start.x - end.x;
   const long dy = (end.y > start.y) ? end.y - start.y : start.y - end.y;
   const long sx = end.x > start.x ? 1 : -1;
   const long sy = end.y > start.y ? 1 : -1;

   long err = dx - dy;
   hsv_color dcol {(end - start) / (dx > dy ? dx : dy)};

   for (long e2; ; it.colcomp() += dcol)
   {
	   raster->setpixel(it);
      if (it == end) break;

      e2 = 2 * err;
      if (e2 > -dy) { err -= dy; it.x += sx; }
      if (e2 < dx)  { err += dx; it.y += sy; }
   }
}

}
