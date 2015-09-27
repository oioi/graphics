#ifndef SDL_GRAPHICS_H
#define SDL_GRAPHICS_H

#include <vector>
#include <memory>
#include <type_traits>

#include <cmath>
#include <stdint.h>

#include <SDL.h>

namespace graphics {

using uint_t = unsigned int;

struct rgb_color
{
   constexpr rgb_color(double r_ = 0, double g_ = 0, double b_ = 0, double a_ = 0) :
      r{r_}, g{g_}, b{b_}, a{a_} { }
   operator uint32_t() const;

   double r, g, b, a;
};

struct hsv_color
{
   constexpr hsv_color(double h_ = 0, double s_ = 0, double v_ = 0, double a_ = 0) :
      h{h_}, s{s_}, v{v_}, a{a_} { }
   rgb_color to_rgb();

   double h, s, v, a;
};

enum class coordinates {
   absolute,
   relative
};

template <typename T = double, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
struct point
{
   constexpr point(T x_ = 0, T y_ = 0) : x{x_}, y{y_} { }
   constexpr point & operator +=(const point &other) { x += other.x; y += other.y; return *this; }
   constexpr bool operator ==(const point &other) { return (x == other.x and y == other.y); }

   T x, y;
};

using area_coord = std::pair<point<uint_t>, point<uint_t>>;

template <typename T>
point<T> operator +(const point<T> &a, const point<T> &b)
{
   point<T> res {a};
   return res += b;
}

template <typename T>
std::ostream & operator <<(std::ostream &out, const point<T> &p) {
   return out << '{' << p.x << ',' << p.y << '}';  }

struct areasize
{
   uint_t width;
   uint_t height;

   areasize(uint_t width_ = 0, uint_t height_ = 0) : width{width_}, height{height_} { }
   areasize(const areasize &other) : width{other.width}, height{other.height} { }
};

class bitmap
{
   public:
      bitmap(uint32_t *buf, areasize mapsize_, const areasize &framesize_, const areasize &offsets_) :
         framebuf{buf}, framesize{framesize_}, mapsize{mapsize_}, offsets{offsets_} { }

      void clear(uint8_t defb = 255);
      template <typename T> void setpixel(const point<T> &p, const rgb_color &color = {});
      void setpixel(const point<unsigned long> &p, const rgb_color &color = {});

      coordinates type {coordinates::relative};
      area_coord box() const {
         return std::make_pair(point<uint_t>{offsets.width, offsets.height},
                               point<uint_t>{offsets.width + mapsize.width, offsets.height + mapsize.height});
      }

   private:
      uint32_t *framebuf;
      areasize framesize;
      areasize mapsize;
      areasize offsets;
};

template <typename T>
void bitmap::setpixel(const point<T> &p, const rgb_color &color)
{
   if (0 > p.x or 0 > p.y) throw std::out_of_range {"point has negative coordinates."};
   point<unsigned long> pp {static_cast<unsigned long>(std::lrint(p.x)), static_cast<unsigned long>(std::lrint(p.y))};
   setpixel(pp, color);
}

class sdl_main
{
   public:
      sdl_main();
      virtual ~sdl_main();

   private:
      static bool init;
      static unsigned refs;
};

class window : public sdl_main
{
   public:
      window(const char *title, const areasize &winsize_,
             uint_t xpos = SDL_WINDOWPOS_UNDEFINED, uint_t ypos = SDL_WINDOWPOS_UNDEFINED);
      ~window() { SDL_DestroyWindow(win); }

      void lock()   const { SDL_LockSurface(surface); }
      void unlock() const { SDL_UnlockSurface(surface); }
      void update() const { SDL_UpdateWindowSurface(win); }

      bitmap * get_bitmap() { return get_bitmap(winsize); }
      bitmap * get_bitmap(const areasize &mapsize, const areasize &offsets = {});

   private:
      SDL_Window *win;
      SDL_Surface *surface;
      areasize winsize;

      std::vector<std::unique_ptr<bitmap>> areas;
};

} // grpahics namespace end

#endif
