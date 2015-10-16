#include <iostream>
#include <bitset>
#include <cmath>

#include "line.h"

namespace graphics {

void clipline(const area_coord &box, line::lpoint &p, double dx, double dy, const line::lpoint::color_type &dcol, uint8_t &code)
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

void line::render()
{
   if (start.x < 0 || start.y < 0 || end.x < 0 || end.y < 0)
      throw std::out_of_range {"line has points with negative coordinates"};

   hsv_point<> a {static_cast<unsigned long>(lrint(start.x)), static_cast<unsigned long>(lrint(start.y)), start};
   hsv_point<> b {static_cast<unsigned long>(lrint(end.x)), static_cast<unsigned long>(lrint(end.y)), end};

   const long dx = (b.x > a.x) ? b.x - a.x : a.x - b.x;
   const long dy = (b.y > a.y) ? b.y - a.y : a.y - b.y;
   const long sx = b.x > a.x ? 1 : -1;
   const long sy = b.y > a.y ? 1 : -1;

   long err = dx - dy;
   hsv_color dcol {(end - start) / (dx > dy ? dx : dy)};

   for (long e2; ; a.colcomp() += dcol)
   {
	   raster->setpixel(a);
      if (a == b) break;

      e2 = 2 * err;
      if (e2 > -dy) { err -= dy; a.x += sx; }
      if (e2 < dx)  { err += dx; a.y += sy; }
   }
}

void line::areacheck()
{
   prepared = true;
   area_coord box = raster->box();
   uint8_t k1 = position_code(box, start);
   uint8_t k2 = position_code(box, end);

   for (;;)
   {
      if (0 == (k1 | k2))
      {
         std::cerr << "K1 = K2 = 0. Line is inside the area." << std::endl;
         inside = true;
         return;
      }

      if (0 != (k1 & k2))
      {
         std::cerr << "K1 & K2 != 0. Line is outside the area." << std::endl;
         return;
      }

      if (0 != k1) clipline(box, start, start.x - end.x, start.y - end.y, start - end, k1);
      else clipline(box, end, start.x - end.x, start.y - end.y, start - end, k2);
   }
}

}