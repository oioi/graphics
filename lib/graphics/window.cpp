#include <stdexcept>
#include "window.h"

namespace graphics {

bool sdl_main::init {false};
unsigned sdl_main::refs {0};

sdl_main::sdl_main()
{
   refs++;
   if (init) return;

   if (0 != SDL_Init(SDL_INIT_VIDEO))
   {
      refs--;
      throw std::runtime_error {"SDL Init failed."};
   }
   init = true;
}

sdl_main::~sdl_main()
{
   if (0 != --refs) return;
   SDL_Quit();
   init = false;
}

window::window(const char *title, const areasize &winsize_, uint_t xpos, uint_t ypos) : winsize {winsize_}
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

}