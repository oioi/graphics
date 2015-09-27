#include <iostream>
#include <stdexcept>
#include <cstring>

#include "graphics.h"

namespace graphics {

bool sdl_main::init = false;
unsigned sdl_main::refs = 0;

// thread safety? who cares.
sdl_main::sdl_main()
{
   refs++;
   if (init) return;

   if (0 != SDL_Init(SDL_INIT_VIDEO))
   {
      refs--;
      throw std::runtime_error {"SDL init failed."};
   }
}

sdl_main::~sdl_main()
{
   if (0 == --refs) SDL_Quit();
   init = false;
}

window::window(const char *title, const areasize &winsize_, uint_t xpos, uint_t ypos)
   : winsize{winsize_}
{
   if (nullptr == (win = SDL_CreateWindow(title, xpos, ypos, winsize.width, winsize.height, SDL_WINDOW_SHOWN)))
      throw std::runtime_error {std::string {"Could not create window: "} + SDL_GetError()};

   if (nullptr == (surface = SDL_GetWindowSurface(win)))
      throw std::runtime_error {std::string {"Could not get window surface: "} + SDL_GetError()};
}

bitmap * window::get_bitmap(const areasize &mapsize, const areasize &offsets)
{
   bitmap *newarea = new bitmap(static_cast<uint32_t *>(surface->pixels), mapsize, winsize, offsets);
   areas.push_back(std::unique_ptr<bitmap> {newarea});
   return newarea;
}

void bitmap::clear(uint8_t defb)
{
   for (unsigned i = 0; i < mapsize.height; i++)
      memset(framebuf + ((i + offsets.height) * framesize.width) + offsets.width, defb, sizeof(uint32_t) * mapsize.width);
}

void bitmap::setpixel(const point<unsigned long> &p, const rgb_color &color)
{
   if (coordinates::absolute == type)
   {
      if (p.x < offsets.width or p.y < offsets.height or
          p.x > offsets.width + mapsize.width or p.y > offsets.height + mapsize.height)
         throw std::out_of_range {"point's absoulute coordinates are not in area"};
      framebuf[p.y * framesize.width + p.x] = color;
      return;
   }

   if (p.x > mapsize.width or p.y > mapsize.height)
      throw std::out_of_range {"relative coordinates are too big for this area size"};
   framebuf[(p.y + offsets.height) * framesize.width + (p.x + offsets.width)] = color;
}

rgb_color::operator uint32_t() const
{
   uint8_t rb = (r * 255);
   uint8_t gb = (g * 255);
   uint8_t bb = (b * 255);
   uint8_t ab = (a * 255);   

   return (ab << 24) | (rb << 16) | (gb << 8) | bb;
}

rgb_color hsv_color::to_rgb()
{
   if (0 == s) return rgb_color(v, v, v, a);

   double hh = ((h > 360.0) ? 0.0 : h) / 60.0;
   long i = (long) hh;
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
              return rgb_color(v, t, p, a);
   }
}

} // graphics namespace end
