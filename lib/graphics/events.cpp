#include "SDL.h"
#include "events.h"

int handle_keyevent(const SDL_Event &event)
{
   switch (event.key.keysym.sym)
   {
      case SDLK_ESCAPE: exit(0);
      default: return 0;
   }
}

int wait_keyevent()
{
   SDL_Event event;

   for (;;)
   {
      SDL_WaitEvent(&event);
      switch (event.type)
      {
         case SDL_QUIT: exit(0);
         case SDL_KEYDOWN: return handle_keyevent(event);
      }
   }
}