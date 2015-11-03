#include <stdexcept>
#include "window.h"

namespace graphics {

bool sdl_main::init {false};
unsigned sdl_main::refs {0};
TTF_Font * sdl_main::font {nullptr};

sdl_main::sdl_main()
{
   if (init) { refs++; return; }

   if (0 != SDL_Init(SDL_INIT_VIDEO))
      throw std::runtime_error {"SDL Init failed."};

   if (-1 == TTF_Init())
      throw std::runtime_error {std::string {"TTF Init failed: "} + TTF_GetError()};
   if (nullptr == (font = TTF_OpenFont("arial.ttf", 14)))
      throw std::runtime_error {std::string {"Could not open fontfile: "} + TTF_GetError()};

   refs++;
   init = true;
}

sdl_main::~sdl_main()
{
   if (0 != --refs) return;
   TTF_CloseFont(font);
   TTF_Quit();
   SDL_Quit();
   init = false;
}

window::window(const char *title, const areasize &winsize_, long xpos, long ypos) : winsize{winsize_}
{
   if (nullptr == (win = SDL_CreateWindow(title, xpos, ypos, winsize.width, winsize.height, SDL_WINDOW_SHOWN)))
      throw std::runtime_error {std::string {"Could not create window: "} + SDL_GetError()};
   if (nullptr == (renderer = SDL_CreateRenderer(win, -1, 0)))
      throw std::runtime_error {std::string {"Could not create window renderer: "} + SDL_GetError()};

   if (nullptr == (screen = SDL_CreateRGBSurface(0, winsize.width, winsize.height, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000)))
      throw std::runtime_error {std::string {"Could not create screen RGB surface: "} + SDL_GetError()};
   if (nullptr == (texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, winsize.width, winsize.height)))
      throw std::runtime_error {std::string {"Could not create screen GPU texture: "} + SDL_GetError()};
}

window::~window()
{
   SDL_DestroyTexture(texture);
   SDL_FreeSurface(screen);
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(win);
}

void window::update() const
{
   SDL_UpdateTexture(texture, nullptr, screen->pixels, screen->pitch);
   SDL_RenderClear(renderer);
   SDL_RenderCopy(renderer, texture, nullptr, nullptr);
   SDL_RenderPresent(renderer);
}

bitmap * window::get_bitmap(const areasize &mapsize, const areasize &offsets)
{
   bitmap *newarea = new bitmap(static_cast<uint32_t *>(screen->pixels), mapsize, winsize, offsets);
   areas.push_back(std::unique_ptr<bitmap> {newarea});
   return newarea;
}

void window::write_text(const char *string)
{
   SDL_Surface *text = TTF_RenderText_Solid(font, string, {0, 0, 0});
   SDL_Rect location = {0, 0, 0, 0};
   SDL_BlitSurface(text, nullptr, screen, &location);
   SDL_FreeSurface(text);
}

}
