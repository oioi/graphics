#include <iostream>
#include <chrono>
#include <bitset>

#include "graphics.h"

using namespace graphics;
using hclock = std::chrono::high_resolution_clock;

enum positions {
   left   = 1 << 0,
   right  = 1 << 1,
   bottom = 1 << 2,
   top    = 1 << 3
};

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

// Some kind of book implemention of Bresenham's line algorithm.
// Not used, see next line function.
template <typename T>
void line3(bitmap *area, const point<T> &start, const point<T> &end, double hue)
{
   point<long> a {lrint(start.x), lrint(start.y)};
   point<long> b {lrint(end.x), lrint(end.y)};

   const bool steep = abs(b.y - a.y) > abs(b.x - a.x);
   if (steep) { std::swap(a.x, a.y); std::swap(b.x, b.y); }
   if (a.x > b.x) std::swap(a, b);

   const long dx = b.x - a.x;
   const long dy = abs(b.y - a.y);
   const long ystep = a.y < b.y ? 1 : -1;
   long err = 2*dy - dx;

   hsv_color color {hue, 1.0, 0.4};

   for (; a.x < b.x; ++a.x)
   {
      steep ? area->setpixel(point<long> {a.y, a.x}, color.to_rgb()) : area->setpixel(point<long> {a.x, a.y}, color.to_rgb());
      if (err > 0) { a.y += ystep; err += 2*dy - 2*dx; }
      else err += 2*dy;
   }
}

template <typename T>
void line(bitmap *area, const point<T> &start, const point<T> &end, double hue)
{
   point<long> a {lrint(start.x), lrint(start.y)};
   point<long> b {lrint(end.x), lrint(end.y)};

   const long dx = abs(b.x - a.x);
   const long dy = abs(b.y - a.y);
   const long sx = b.x > a.x ? 1 : -1;
   const long sy = b.y > a.y ? 1 : -1;
   long err = dx - dy;

   hsv_color color {hue, 1.0, 0.4};

   for (long e2; ;)
   {
      area->setpixel(a, color.to_rgb());
      if (a == b) break;

      e2 = 2 * err;
      if (e2 > -dy) { err -= dy; a.x += sx; }
      if (e2 < dx)  { err += dx; a.y += sy; }
   }
}

template <typename T>
uint8_t position_code(const area_coord &box, const point<T> &p)
{
   uint8_t code = 0;
   if      (p.x < box.first.x)  code |= left;
   else if (p.x > box.second.x) code |= right;
   if      (p.y < box.first.y)  code |= top;
   else if (p.y > box.second.y) code |= bottom;

   std::cerr << "Point " << p << " code: " << std::bitset<4> {code} << std::endl;
   return code;
}

void clip(const area_coord &box, point<double> &p, double dx, double dy, uint8_t &code)
{
   if (code & left)
   {
      std::cerr << "Left clipping: ";
      p.y += dy * (box.first.x - p.x) / dx;
      p.x = box.first.x;
   }
   else if (code & right)
   {
      std::cerr << "Right clipping: ";
      p.y += dy * (box.second.x - p.x) / dx;
      p.x = box.second.x;
   }
   else if (code & bottom)
   {
      std::cerr << "Bottom clipping: ";
      p.x += dx * (box.second.y - p.y) / dy;
      p.y = box.second.y;
   }
   else if (code & top)
   {
      std::cerr << "Top clipping: ";
      p.x += dx * (box.first.y - p.y) / dy;
      p.y = box.first.y;
   }
   code = position_code(box, p);
}

void clip_line(bitmap *area, point<double> start, point<double> end, double hue)
{
   area_coord box = area->box();
   uint8_t k1 = position_code(box, start);
   uint8_t k2 = position_code(box, end);
   double dx, dy;

   for (;;)
   {
      if (0 == (k1 | k2))
      {
         std::cerr << "K1 = K2 = 0. Line is inside the area." << std::endl;
         return line(area, start, end, hue);
      }

      if (0 != (k1 & k2))
      {
         std::cerr << "K1 & K2 != 0. Line is outside the area." << std::endl;
         return;
      }

      dx = start.x - end.x;
      dy = start.y - end.y;

      if (0 != k1) clip(box, start, dx, dy, k1);
      else clip(box, end, dx, dy, k2);
   }
}

int main(int, char **)
{
   window mainwin {"Lines", {win_width, win_height}};
   bitmap *mainarea = mainwin.get_bitmap();
   bitmap *cliparea = mainwin.get_bitmap({win_width / 2, win_height / 2}, {win_width / 4, win_height / 4});
   cliparea->type = coordinates::absolute;

   srand(hclock::to_time_t(hclock::now()));
   auto xrandom = []() { return rand() % win_width; };
   auto yrandom = []() { return rand() % win_height; };

   for (;;)
   {
      point<> p1(xrandom(), yrandom());
      point<> p2(xrandom(), yrandom());
      std::cerr << std::endl <<  "Line: " << p1 << " -> " << p2 << std::endl;

      mainarea->clear(220);
      cliparea->clear();
      line(mainarea, p1, p2, 320);
      mainwin.update();

      if (wait_event()) break;

      mainarea->clear(220);
      cliparea->clear();
      clip_line(cliparea, p1, p2, 150);
      mainwin.update();

      if (wait_event()) break;
   }

   return 0;
}
