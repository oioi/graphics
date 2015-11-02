#include <SDL.h>
#include "events.h"

int basic_wait_keyevent()
{
   SDL_Event event;

   for (;;)
   {
      SDL_WaitEvent(&event);
      switch (event.type)
      {
         case SDL_QUIT: return 1;
         case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
               case SDLK_ESCAPE: return 1;
               default: return 0;
            }
      }
   }
}
