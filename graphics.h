#ifndef SDL_GRAPHICS_H
#define SDL_GRAPHICS_H

#include <vector>
#include <memory>

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

struct point
{
   constexpr point(uint_t x_ = 0, uint_t y_ = 0) : x{x_}, y{y_} { }
   point & operator +=(const point &other) { x += other.x; y += other.y; return *this; }
   bool operator ==(const point &other) { return (x == other.x and y == other.y); }

   uint_t x, y;
};
point operator +(const point &a, const point &b);

class bitmap
{
   public:
      bitmap(uint32_t *buf, uint_t width_, uint_t height_, uint_t frame_width_, uint_t frame_height_,
                uint_t xoffset_ = 0, uint_t yoffset_ = 0) :
         framebuf{buf}, frame_width{frame_width_}, frame_height{frame_height_}, 
         width{width_}, height{height_}, xoffset{xoffset_}, yoffset{yoffset_} { }

      void clear(uint8_t defb = 255);
      void setpixel(const point &p, const rgb_color &color = rgb_color{});

      coordinates type {coordinates::relative};

   private:
      uint32_t *framebuf;
      uint_t frame_width;
      uint_t frame_height;

      uint_t width;
      uint_t height;

      uint_t xoffset;
      uint_t yoffset;
};

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
      window(const char *title, uint_t width_, uint_t height_, 
            uint_t xpos = SDL_WINDOWPOS_UNDEFINED, uint_t ypos = SDL_WINDOWPOS_UNDEFINED);
      ~window() { SDL_DestroyWindow(win); }

      void lock()   const { SDL_LockSurface(surface); }
      void unlock() const { SDL_UnlockSurface(surface); }
      void update() const { SDL_UpdateWindowSurface(win); }

      bitmap * get_bitmap() { return get_bitmap(width, height); }
      bitmap * get_bitmap(uint_t area_width, uint_t area_hight, uint_t xoffset = 0, uint_t yoffset = 0);

   private:
      SDL_Window *win;
      SDL_Surface *surface;
      uint_t width;
      uint_t height;

      std::vector<std::unique_ptr<bitmap>> areas;
};

} // graphics namespace end

#endif
