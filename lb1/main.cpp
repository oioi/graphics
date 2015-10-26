#include <iostream>
#include <ctime>
#include "graphics.h"

using namespace graphics;

enum winsize
{
   win_height = 768,
   win_width = 1024,
};

int main(int, char **)
{
   window mainwin{ "Lines",{ win_width, win_height } };
   bitmap *mainarea = mainwin.get_bitmap();
   bitmap *cliparea = mainwin.get_bitmap({win_width / 2, win_height / 2}, {win_width / 4, win_height / 4});

   srand(narrow_cast<unsigned, time_t>(time(nullptr)));
   auto xrandom = []() { return rand() % win_width; };
   auto yrandom = []() { return rand() % win_height; };
   auto clear = [mainarea, cliparea]() { mainarea->clear(220); cliparea->clear(); };

   for (;;)
   {
      hsv_point<> p1(xrandom(), yrandom(), 0, 1.0, 0.2);
      hsv_point<> p2(xrandom(), yrandom(), 350, 1.0, 1.0);
      std::cerr << std::endl << "Line: " << p1 << " -> " << p2 << std::endl;

      clear();
      line first {mainarea, p1, p2};
      first.draw();
      mainwin.update();

      if (wait_keyevent()) break;

      clear();
      area_line second{cliparea, p1, p2};
      second.draw();
      mainwin.update();

      if (wait_keyevent()) break;
   }

   return 0;
}