#ifndef GRAPHICS_WINDOW_H
#define GRAPHICS_WINDOW_H

#include <vector>
#include <memory>

#include <SDL.h>
#include <SDL_ttf.h>

#include "raster.h"

namespace graphics {

class sdl_main
{
   protected:
      sdl_main();
      ~sdl_main();

      static TTF_Font *font;

   private:
      // In theory this needs some kind of synchronisation mechanism.
      // Since no multi-threaded use meant - we don't care.
      static bool init;
      static unsigned refs;
};

class window : private sdl_main
{
   public:
      window(const char *title, const areasize &winsize_,
             long xpos = SDL_WINDOWPOS_UNDEFINED, long ypos = SDL_WINDOWPOS_UNDEFINED);
      ~window();

      void update() const;

      bitmap * get_bitmap() { return get_bitmap(winsize); }
      bitmap * get_bitmap(const areasize &mapsize, const areasize &offsets = {});

      void write_text(const char *string);

   private:
      SDL_Window *win;
      SDL_Renderer *renderer;
      SDL_Surface *screen;
      SDL_Texture *texture;

      areasize winsize;
      std::vector<std::unique_ptr<bitmap>> areas;
};

}

#endif
