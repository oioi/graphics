#ifndef GRAPHICS_COLOR_H
#define GRAPHICS_COLOR_H

#include <cstdint>

namespace graphics {

struct rgb_color
{
   constexpr rgb_color(double r_ = 0, double g_ = 0, double b_ = 0, double a_ = 0) :
      r{r_}, g{g_}, b{b_}, a{a_} { }
   operator uint32_t() const;

   double r, g, b, a;
};

struct hsv_color
{
   constexpr hsv_color(double h_ = 0, double s_ = 0, double v_ = 0, double a_ = 0):
      h{h_}, s{s_}, v{v_}, a{a_} { }
   rgb_color to_rgb() const;

   hsv_color & operator -=(const hsv_color &other);
   hsv_color & operator +=(const hsv_color &other);

   double h, s, v, a;
};

hsv_color operator -(const hsv_color &c1, const hsv_color &c2);

template <typename T> hsv_color operator /(const hsv_color &c1, T delim)
{
   hsv_color result {c1};
   result.h /= delim;
   result.s /= delim;
   result.v /= delim;
   result.a /= delim;
   return result;
}

} // graphics namespace end

#endif
