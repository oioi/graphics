#ifndef GRAPHICS_WINDOW_H
#define GRAPHICS_WINDOW_H

#include <mutex>
#include <vector>
#include <memory>

#include <SDL2/SDL.h>

#include "raster.h"

namespace graphics {

class sdl_main
{
   protected:
      sdl_main();
      ~sdl_main();

   private:
      static bool init;
      static unsigned refs;
};

class window : private sdl_main
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

}

#endif
