#include "graphics.h"
#include <iostream>

using namespace graphics;

enum winsize
{
   win_height = 660,
   win_width = 880
};

polygon::points_vect make_figure(window *win, bitmap *map)
{
   SDL_Event event;
   polygon::points_vect figure;

   for (;;)
   {
      SDL_WaitEvent(&event);

      switch (event.type)
      {
         case SDL_QUIT: exit(0); break;
         case SDL_KEYDOWN:

         switch (event.key.keysym.sym)
         {
            case SDLK_RETURN: if (0 != figure.size()) return figure;
            case SDLK_ESCAPE: exit(0);
         }
         break;

         case SDL_MOUSEBUTTONDOWN:

            if (event.button.button == SDL_BUTTON_LEFT)
            {
               polygon::points_vect::value_type it {
                  narrow_cast<double, Sint32>(event.button.x), narrow_cast<double, Sint32>(event.button.y), 
                  {150, 1.0, event.button.x / (double) win_width} };

               if (0 != figure.size())
               {
                  line edge {map, figure.back(), it};
                  edge.draw();
                  win->update();
               }

               figure.push_back(it);
            }
         break;
      }
   }
}

int main(int, char **)
{
   window mainwin {"Polygons", {win_width, win_height}};
   bitmap *mainarea = mainwin.get_bitmap();
   bitmap *cliparea = mainwin.get_bitmap({win_width / 2, win_height / 2}, {win_width / 4, win_height / 4});
   auto clear = [mainarea, cliparea]() { mainarea->clear(220); cliparea->clear(); };

   for (;;)
   {
      clear();
      mainwin.update();

      polygon::points_vect figure {make_figure(&mainwin, mainarea)};
      polygon pol1 {mainarea, figure};
      pol1.draw_contour();

      mainwin.update();
      if (basic_wait_keyevent()) break;

      for (auto &p : figure) std::cerr << p << std::endl;

      clear();
      area_polygon pol2 {cliparea, figure};
      pol2.draw_contour();

      mainwin.update();
      if (basic_wait_keyevent()) break;

      clear();
      pol2.draw();

      mainwin.update();
      if (basic_wait_keyevent()) break;
   }

   return 0;
}
