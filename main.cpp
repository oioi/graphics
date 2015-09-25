#include <iostream>
#include <random>

#include "graphics.h"

using namespace graphics;

enum winsize
{
   win_height = 768,
   win_width = 1024,
};

int wait_event()
{
   SDL_Event event;

   for (;;)
   {
      SDL_WaitEvent(&event);      
      switch (event.type)
      {
         case SDL_QUIT: return 1;
         case SDL_KEYDOWN: return 0;
      }
   }
}

void line(bitmap *area, point start, point end)
{
   int dx = abs(end.x - start.x);
   int sx = start.x < end.x ? 1 : -1;
   int dy = abs(end.y - start.y);
   int sy = start.y < end.y ? 1 : -1;

   int err = (dx > dy ? dx : -dy) / 2;


   hsv_color color(150, 1.0, 0.4);
   double v = 0;
   double vs = 0.6 / ((dx > dy ? dx : dy) - 1);

   for (int errt;;)
   {
      area->setpixel(start, color.to_rgb());
      if (start == end) break;
      color.v += vs;

      errt = err;
      if (errt > -dx) { err -= dy; start.x += sx; }
      if (errt < dy) { err += dx; start.y += sy; }
      v += vs;
   }
}

int main(int, char **)
{
   window mainwin("Test", win_width, win_height);
   bitmap *mainarea = mainwin.get_bitmap();
   bitmap *truncarea = mainwin.get_bitmap(win_width / 2, win_height / 2, win_width / 4, win_height / 4);

   std::random_device rd;
   std::mt19937 xgen {rd()};
   std::mt19937 ygen {rd()};   
   std::uniform_int_distribution<> xdist {0, win_width / 2};
   std::uniform_int_distribution<> ydist {0, win_height / 2};   

   auto xrandom = std::bind(xdist, xgen);
   auto yrandom = std::bind(ydist, ygen);   

   for (;;)
   {
      graphics::point p1 (xrandom(), yrandom());
      graphics::point p2 (xrandom(), yrandom());

      mainwin.lock();

      mainarea->clear(220);
      truncarea->clear(250);
      line(truncarea, p1, p2);

      mainwin.unlock();
      mainwin.update();

      if (wait_event()) break;
   }

   return 0;
}

