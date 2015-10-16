#include <iostream>
#include <cmath>

#include "cast.h"
#include "color.h"

namespace graphics {

rgb_color::operator uint32_t() const
{
   uint8_t rb = narrow_cast<uint8_t, long>(lrint(r * 255));
   uint8_t gb = narrow_cast<uint8_t, long>(lrint(g * 255));
   uint8_t bb = narrow_cast<uint8_t, long>(lrint(b * 255));
   uint8_t ab = narrow_cast<uint8_t, long>(lrint(a * 255));
   return (ab << 24) | (rb << 16) | (gb << 8) | bb;
}

hsv_color& hsv_color::operator -=(const hsv_color &other)
{
   h -= other.h;
   s -= other.s;
   v -= other.v;
   return *this;
}

hsv_color& hsv_color::operator +=(const hsv_color &other)
{
   h += other.h;
   s += other.s;
   v += other.v;
   return *this;
}

hsv_color operator -(const hsv_color &c1, const hsv_color &c2)
{
   hsv_color result {c1};
   return result -= c2;
}

hsv_color operator /(const hsv_color &c, double delim)
{
   hsv_color result{c};
   result.h /= delim;
   result.s /= delim;
   result.v /= delim;
   return result;
}

hsv_color operator *(const hsv_color &c, double mul)
{
   hsv_color result {c};
   result.h *= mul;
   result.s *= mul;
   result.v *= mul;
   return result;
}

rgb_color hsv_color::to_rgb() const
{
   if (0 == s) return rgb_color{ v, v, v, a };

   double hh = ((h >= 360.0) ? 0.0 : h) / 60.0;
   long i = (long)hh;
   double ff = hh - i;

   double p = v * (1.0 - s);
   double q = v * (1.0 - (s * ff));
   double t = v * (1.0 - (s * (1.0 - ff)));

   switch (i)
   {
   case 0: return rgb_color(v, t, p, a);
   case 1: return rgb_color(q, v, p, a);
   case 2: return rgb_color(p, v, t, a);
   case 3: return rgb_color(p, q, v, a);
   case 4: return rgb_color(t, p, v, a);
   case 5:
   default:
      return rgb_color(v, p, q, a);
   }
}

std::ostream & operator <<(std::ostream &out, const hsv_color &color) {
   return out << '{' << color.h << ',' << color.s << ',' << color.v << '}';
}

}