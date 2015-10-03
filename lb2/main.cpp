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

edge::edge(double ymin_, double ymax_, double xstart_, double xend_, const hsv_color &st, const hsv_color &end) :
   ymax{ymax_}, xstart{xstart_}, xend{xend_}, st_color{st}, end_color{end}
{
   dx = (xend - xstart) / (ymax - ymin_);
   dh = (end_color.h - st_color.h) / (ymax - ymin_);
   ds = (end_color.s - st_color.s) / (ymax - ymin_);
   dv = (end_color.v - st_color.v) / (ymax - ymin_);
}

void edge::shift()
{
   xstart += dx;
   st_color.h += dh;
   st_color.s += ds;
   st_color.v += dv;
}


using edge_table = std::map<double, std::vector<edge>>;
using points_vect = std::vector<hsv_point<unsigned>>;

edge_table build_groups(const points_vect &points, double &ymax)
{
   edge_table edge_groups;

   points_vect::const_iterator it {points.begin()};
   points_vect::const_iterator next {it + 1};
   points_vect::const_iterator min, max;
   ymax = 0;

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
      if (max->y > ymax) ymax = max->y;
      edge_groups[min->y].emplace_back(min->y, max->y, min->x, max->x, *min, *max);
   }

   for (auto &group : edge_groups)
      std::sort(group.second.begin(), group.second.end());
   return edge_groups;
}

void draw_polregion(bitmap *area, const std::list<edge> &active_edges, double y)
{
   auto xs = active_edges.cbegin();
   auto xe = xs;
   bool drawing = true;

   point<unsigned> xit;
   xit.y = lrint(y);

   unsigned xend;
   hsv_color hdsv, ccol;

   for (++xe; xe != active_edges.end(); ++xs, ++xe, drawing = !drawing)
   {
      if (!drawing) continue;
      xit.x = lrint(xs->xstart);
      xend = lrint(xe->xstart);

      ccol = xs->st_color;
      hdsv = (xe->st_color - xs->st_color) / (xe->xstart - xs->xstart);

      for (; xit.x < xend; xit.x++, ccol += hdsv) area->setpixel(xit, ccol.to_rgb());
   }
}

void polygon(bitmap *area, const points_vect &points)
{
   if (3 > points.size()) throw std::range_error {"Insufficient points for a polygon."};

   double ymax;
   edge_table edge_groups {build_groups(points, ymax)};
   if (0 == edge_groups.size()) throw std::range_error {"Incorrect polygon area."};

   std::list<edge> active_edges;
   for (double y = edge_groups.cbegin()->first; y <= ymax; y++)
   {
      active_edges.remove_if([y](const edge &e) { return y >= e.ymax; });
      if (edge_groups.end() != edge_groups.find(y))
         for (const auto &edge : edge_groups[y]) active_edges.push_back(edge);
      active_edges.sort();

      draw_polregion(area, active_edges, y);
      for (auto &edge : active_edges) edge.shift();
   }
}

int main(int, char **)
{
   window mainwin {"Polygons", {win_width, win_height}};
   bitmap *mainarea = mainwin.get_bitmap();

   mainarea->clear();

//     points_vect points {{100, 200, 150, 1.0, 0.2}, {400, 768, 150, 1.0, 0.3},
//                         {768, 600, 150, 1.0, 0.7}, {700, 200, 150, 1.0, 0.2}, {500, 400, 150, 1.0, 0.9}};
//   points_vect points {{100, 100, 150, 1.0, 0.6}, {300, 100, 150, 1.0, 0.9},
//                       {300, 300, 150, 1.0, 0.75}, {100, 300, 150, 1.0, 0.1}};
   points_vect points {{200, 100, 150, 1.0, 0.2}, {300, 350, 150, 1.0, 0.9}, {100, 300, 150, 1.0, 0.6}};

   polygon(mainarea, points);
   mainwin.update();
   wait_event();

   return 0;
}
