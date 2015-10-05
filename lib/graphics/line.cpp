#include <iostream>
#include <bitset>
#include <cmath>

#include "line.h"

namespace graphics {

enum positions {
   left   = 1 << 0,
   right  = 1 << 1,
   bottom = 1 << 2,
   top    = 1 << 3
};   

uint8_t position_code(const area_coord &box, const line::lpoint &p)
{
   uint8_t code = 0;

   if      (p.x < box.first.x)  code |= left;
   else if (p.x > box.second.x) code |= right;
   if      (p.y < box.first.y)  code |= top;
   else if (p.y > box.second.y) code |= bottom;

   std::cerr << "Point " << p << " code: " << std::bitset<4> {code} << std::endl;
   return code;
}

void clip(const area_coord &box, line::lpoint &p, double dx, double dy, double dv, uint8_t &code)
{
   if (code & left)
   {
      std::cerr << "Left clipping: ";
      p.y += dy * (box.first.x - p.x) / dx;
      p.v += dv * (box.first.x - p.x) / dx;
      p.x = box.first.x;
   }
   else if (code & right)
   {
      std::cerr << "Right clipping: ";
      p.y += dy * (box.second.x - p.x) / dx;
      p.v += dv * (box.second.x - p.x) / dx;
      p.x = box.second.x;
   }
   else if (code & bottom)
   {
      std::cerr << "Bottom clipping: ";
      p.x += dx * (box.second.y - p.y) / dy;
      p.v += dv * (box.second.y - p.y) / dy;
      p.y = box.second.y;
   }
   else if (code & top)
   {
      std::cerr << "Top clipping: ";
      p.x += dx * (box.first.y - p.y) / dy;
      p.v += dv * (box.first.y - p.y) / dy;
      p.y = box.first.y;
   }

   std::cout << "Brightness: " << p.v << ' ';
   code = position_code(box, p);
}   

void line::render()
{
   if (start.x < 0 or start.y < 0 or end.x < 0 or end.y < 0)
      throw std::out_of_range {"line has points with negative coordinates."};

   point<> a {static_cast<unsigned long>(lrint(start.x)), static_cast<unsigned long>(lrint(start.y))};
   point<> b {static_cast<unsigned long>(lrint(end.x)), static_cast<unsigned long>(lrint(end.y))};

   const long dx = (b.x > a.x) ? b.x - a.x : a.x - b.x;
   const long dy = (b.y > a.y) ? b.y - a.y : a.y - b.y;
   const long sx = b.x > a.x ? 1 : -1;
   const long sy = b.y > a.y ? 1 : -1;

   long err = dx - dy;
   hsv_color color {start};
   double vs = (end.v - start.v) / ((dx > dy ? dx : dy) - 1);

   for (long e2; ;)
   {
      raster->setpixel(a, color.to_rgb());
      if (a == b) break;

      e2 = 2 * err;
      if (e2 > -dy) { err -= dy; a.x += sx; }
      if (e2 < dx)  { err += dx; a.y += sy; }
      color.v += vs;
   }   
}

void line::areacheck()
{
   prepared = true;
   area_coord box = raster->box();
   uint8_t k1 = position_code(box, start);
   uint8_t k2 = position_code(box, end);

   double dx, dy, dv;

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

      dx = start.x - end.x;
      dy = start.y - end.y;
      dv = start.v - end.v;

      if (0 != k1) clip(box, start, dx, dy, dv, k1);
      else clip(box, end, dx, dy, dv, k2);      
   }
}

}
