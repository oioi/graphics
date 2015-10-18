#ifndef GRAPHICS_COLOR_H
#define GRAPHICS_COLOR_H

#include <cstdint>

namespace graphics {

struct rgb_color
{
   // NOTE: Needs invariant checks? R[0..1], G[0..1], B[0..1]
   constexpr rgb_color(double r_ = 0, double g_ = 0, double b_ = 0, double a_ = 0) :
      r{r_}, g{g_}, b{b_}, a{a_} { }
   explicit operator uint32_t() const;

   double r, g, b, a;
};

struct hsv_color
{
   // NOTE: All constructors probably need invariant checks. H[0..360], S[0..1], V[0..1]
   constexpr hsv_color(double h_ = 0, double s_ = 0, double v_ = 0, double a_ = 0) :
      h{h_}, s{s_}, v{v_}, a{a_} { }
   // Actually needs a constructor accepting uint32_t argument

   rgb_color to_rgb() const;
   explicit operator uint32_t() const { return static_cast<uint32_t>(to_rgb()); }

   // Have some actual meaning, only if interpolating. Doesn't check anything.
   hsv_color & operator -=(const hsv_color &other);
   hsv_color & operator +=(const hsv_color &other);

   double h, s, v, a;
};

std::ostream & operator<<(std::ostream &out, const hsv_color &color);
hsv_color operator -(const hsv_color &c1, const hsv_color &c2);
hsv_color operator /(const hsv_color &c, double delim);
hsv_color operator *(const hsv_color &c, double mul);

}

#endif
