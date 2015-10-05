#ifndef GRAPHICS_POINT_H
#define GRAPHICS_POINT_H

#include <ostream>
#include <type_traits>

#include "color.h"

namespace graphics {

template <typename T = unsigned long, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
struct point
{
   constexpr point (T x_ = 0, T y_ = 0) : x{x_}, y{y_} { }
   constexpr point & operator +=(const point &other) { x += other.x; y += other.y; return *this; }
   constexpr bool operator ==(const point &other) { return (x == other.x and y == other.y); }

   T x, y;
};

template <typename T>
point<T> operator +(const point<T> &a, const point<T> &b)
{
   point<T> res {a};
   return res += b;
}

template <typename T>
std::ostream & operator <<(std::ostream &out, const point<T> &p) {
   return out << '{' << p.x << ',' << p.y << '}'; }

template <typename T = unsigned long>
struct color_point : public point<T>
{
   using point<T>::point;
   virtual uint32_t color() const = 0;
};

template <typename T = unsigned long>
struct hsv_point : public color_point<T>, public hsv_color
{
   constexpr hsv_point(T x, T y, double h = 0, double s = 0, double v = 0, double a = 0):
      color_point<T>{x, y}, hsv_color{h, s, v, a} { }
   constexpr hsv_point(T x, T y, const hsv_color &color) :
      color_point<T>{x, y}, hsv_color{color} { }

   constexpr hsv_point(const hsv_point &other) :
      color_point<T>{other.x, other.y}, hsv_color{other.h, other.s, other.v, other.a} { }

   using color_type = hsv_color;
   uint32_t color() const override { return to_rgb(); }
};

template <typename T>
std::ostream & operator <<(std::ostream &out, const hsv_point<T> &p) 
{
   return out << '{' << p.x << ',' << p.y << '}' 
              << " {" << p.h << ';' << p.s << ';' 
              << p.v << ';' << p.a << '}';
}

using area_coord = std::pair<point<>, point<>>;

} // graphics namespace end

#endif
