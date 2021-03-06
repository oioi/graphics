#ifndef GRAPHICS_POINT_H
#define GRAPHICS_POINT_H

#include <ostream>
#include <type_traits>

#include "cast.h"
#include "color.h"

namespace graphics {

template <typename T = long, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
struct point
{
   constexpr point(T x_ = 0, T y_ = 0, T z_ = 0): x{x_}, y{y_}, z{z_} { }

   template <typename T2> point(T2 x_, T2 y_, T2 z_) :
      x{narrow_cast<T, T2>(x_)}, y{narrow_cast<T, T2>(y_)}, z{narrow_cast<T, T2>(z_)} { }
   template <typename T2> point(const point<T2> &other) : point(other.x, other.y, other.z) { }

   point & operator +=(const point &other) { x += other.x; y += other.y; z += other.z; return *this; }
   constexpr bool operator ==(const point &other) const { return (x == other.x && y == other.y && z == other.z); }
   constexpr bool operator !=(const point &other) const { return !(*this == other); }

   T x, y, z;
};

template <typename T>
point<T> operator +(const point<T> &a, const point<T> &b)
{
   point<T> res {a};
   return res += b;
}

template <typename T>
std::ostream & operator <<(std::ostream &out, const point<T> &p) {
   return out << '{' << p.x << ',' << p.y << ',' << p.z << '}';
}

template <typename T = long>
struct color_point : public point<T>
{
   using point<T>::point;
   virtual uint32_t color() const = 0;
};

template <typename T = long>
struct hsv_point : public color_point<T>, public hsv_color
{
   constexpr hsv_point(T x = 0, T y = 0, T z = 0, double h = 0, double s = 0, double v = 0, double a = 0) :
      color_point<T>(x, y, z), hsv_color(h, s, v, a) { }
   constexpr hsv_point(T x, T y, const hsv_color &color) :
      color_point<T>(x, y), hsv_color(color) { }

   constexpr hsv_point(const hsv_point &other) :
      color_point<T>(other.x, other.y, other.z), hsv_color(other.h, other.s, other.v, other.a) { }

   // Point constructor will perform potentially narrowing conversion in a safe manner
   template <typename T2>
   constexpr hsv_point(const hsv_point<T2> &other) :
      color_point<T>(other.x, other.y, other.z), hsv_color(other) { }

   using color_type = hsv_color;
   uint32_t color() const override { return static_cast<uint32_t>(to_rgb()); }

   hsv_color & colcomp() { return *this; }
   const hsv_color & colcomp() const { return *this; }
};

template <typename T>
std::ostream & operator <<(std::ostream &out, const hsv_point<T> &p)
{
   return out << '{' << p.x << ',' << p.y << ',' << p.z << '}'
              << " {" << p.h << ';' << p.s << ';' << p.v << ';' << p.a  << '}';
}

using area_coord = std::pair<point<>, point<>>;

}

#endif
