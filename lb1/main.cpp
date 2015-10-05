#include <chrono>
#include <iostream>

#include "graphics.h"

using namespace graphics;
using hclock = std::chrono::high_resolution_clock;

enum winsize
{
   win_height = 768,
   win_width = 1024,
};

int handle_keyevent(const SDL_Event &event)
{
   switch (event.key.keysym.sym)
   {
      case SDLK_ESCAPE: return 1;
      default: return 0;
   }
}

int wait_event()
{
   SDL_Event event;

   for (;;)
   {
      SDL_WaitEvent(&event);      
      switch (event.type)
      {
         case SDL_QUIT: return 1;
         case SDL_KEYDOWN: return handle_keyevent(event);
      }
   }
}

int main(void)
{
   window mainwin {"Lines", {win_width, win_height}};
   bitmap *mainarea = mainwin.get_bitmap();
   bitmap *cliparea = mainwin.get_bitmap({win_width / 2, win_height / 2}, {win_width / 4, win_height / 4});

   srand(hclock::to_time_t(hclock::now()));
   auto xrandom = []() { return rand() % win_width; };
   auto yrandom = []() { return rand() % win_height; };

   for (;;)
   {
      hsv_point<> p1(xrandom(), yrandom(), 0, 1.0, 0.2);
      hsv_point<> p2(xrandom(), yrandom(), 0, 1.0, 1.0);
      std::cerr << std::endl <<  "Line: " << p1 << " -> " << p2 << std::endl;

      mainarea->clear(220);
      cliparea->clear();

      line first {mainarea, p1, p2};
      first.draw();
      mainwin.update();

      if (wait_event()) break;

      mainarea->clear(220);
      cliparea->clear();

      p1.h = p2.h = 150;
      line second{cliparea, p1, p2};
      second.draw();
      mainwin.update();

      if (wait_event()) break;
   }

   return 0;
}

