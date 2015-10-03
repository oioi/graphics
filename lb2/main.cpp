#include <map>
#include <list>
#include <algorithm>
#include <iomanip>

#include "main.h"

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

using edge_table = std::map<double, std::vector<edge>>;
using points_vect = std::vector<point<unsigned>>;

edge_table build_groups(const points_vect &points)
{
   edge_table edge_groups;

   points_vect::const_iterator it {points.cbegin()};
   points_vect::const_iterator next {it + 1};
   points_vect::const_iterator min, max;

   for (bool run = true; run; ++it, ++next)
   {
      if (points.end() == next) 
      {
         next = points.begin();
         run = false;
      }

      min = it; max = next;
      if (max->y == min->y) continue;
      if (max->y < min->y) std::swap(min, max);
      edge_groups[min->y].emplace_back(min->y, max->y, min->x, max->x);
   }

   for (auto &group : edge_groups)
      std::sort(group.second.begin(), group.second.end());

   std::cerr << "Edge table" << std::endl;
   for (const auto &group : edge_groups)
   {
      for (const auto &edge : group.second)
         std::cerr << std::setw(10) << group.first 
                   << std::setw(10) << edge.ymax 
                   << std::setw(10) << edge.xstart 
                   << std::setw(10) << edge.xend 
                   << std::setw(10) << edge.dx << std::endl;
   }

   return edge_groups;
}

void draw_polregion(bitmap *area, const std::list<edge> active_edges, double y)
{
   auto xs = active_edges.cbegin();
   auto xe = xs;
   bool drawing = true;

   point<unsigned> xit, xend;
   xit.y = xend.y = lrint(y);

   for (++xe; xe != active_edges.end(); ++xs, ++xe, drawing = !drawing)
   {
      if (!drawing) continue;
      xit.x = lrint(xs->xstart);
      xend.x = lrint(xe->xstart);

      for (; xit.x <= xend.x; xit.x++) area->setpixel(xit);
   }
}

void polygon(bitmap *area, points_vect &points)
{
   if (3 > points.size()) throw std::range_error {"Insufficient points for a polygon."};
   edge_table edge_groups {build_groups(points)};
   if (0 == edge_groups.size()) throw std::range_error {"Incorrect polygon area."};

   std::list<edge> active_edges;
   double ymax {};
   for (const auto &p : points) if (p.y > ymax) ymax = p.y;

   for (double y = edge_groups.cbegin()->first; y <= ymax; y++)
   {
      active_edges.remove_if([y](const edge &e) { return y >= e.ymax; });

      if (edge_groups.end() != edge_groups.find(y))
         for (const auto &edge : edge_groups[y]) active_edges.push_back(edge);
      active_edges.sort();

      draw_polregion(area, active_edges, y);
      for (auto &edge : active_edges) edge.xstart += edge.dx;
   }
}

int main(int, char **)
{
   window mainwin {"Polygons", {win_width, win_height}};
   bitmap *mainarea = mainwin.get_bitmap();

   mainarea->clear();

   points_vect points {{10, 20}, {40, 80}, {80, 60}, {70, 20}, {50, 40}};

//   points_vect points {{100, 100}, {300, 100}, {300, 300}, {100, 300}};

//   points_vect points {{200, 100}, {300, 300}, {100, 300}};

   polygon(mainarea, points);
   mainwin.update();

   wait_event();



   return 0;
}
