#include "graphics.h"

#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include <iostream>

using namespace graphics;

enum winsize
{
   win_height = 768,
   win_width = 1024,
};

struct polygon_edge
{
   double ymax;
   double xstart;
   double xend;
   double dx;

   hsv_color st_color;
   hsv_color end_color;
   hsv_color dcolor;

   polygon_edge(const polygon_edge &other) = default;
   polygon_edge(double ymin_, double ymax_, double xstart_, double xend_, const hsv_color &st, const hsv_color &end);

   void shift() { xstart += dx; st_color += dcolor; }
   bool operator <(const polygon_edge &other) { return xstart < other.xstart; }

};

polygon_edge::polygon_edge(double ymin_, double ymax_, double xstart_, double xend_, const hsv_color &st, const hsv_color &end) :
   ymax{ymax_}, xstart{xstart_}, xend{xend_}, st_color{st}, end_color{end}
{
   dx = (xend - xstart) / (ymax - ymin_);
   dcolor = (end_color - st_color) / (ymax - ymin_);
}

using edge_table = std::map<double, std::vector<polygon_edge>>;
using points_vect = std::vector<hsv_point<unsigned>>;

std::ostream & operator <<(std::ostream &out, const edge_table &table)
{
   for (const auto &group : table)
   {
      out << "Group minimum Y: " << group.first << std::endl;

      auto owidth = out.width();
      out.width(10);
      for (const auto &edge : group.second) out << edge.ymax << ' ' << edge.xstart << ' ' << edge.xend << ' ' << std::endl;
      out.width(owidth);
   }
   return out;
}

edge_table build_groups(const points_vect &points, double &ymax)
{
   edge_table edge_groups;

   points_vect::const_iterator it {points.cbegin()};
   points_vect::const_iterator next {it + 1};
   points_vect::const_iterator min, max;
   ymax = 0;

   for (bool run = true; run; ++it, ++next)
   {
      if (points.end() == next)
      {
         next = points.cbegin();
         run = false;
      }

      min = it; max = next;
      if (max->y == min->y) continue; // Skipping horizontal edges

      if (max->y < min->y) std::swap(min, max);
      if (max->y > ymax) ymax = max->y;
      edge_groups[min->y].emplace_back(min->y, max->y, min->x, max->x, *min, *max);
   }

   for (auto &group : edge_groups)
      std::sort(group.second.begin(), group.second.end());
   return edge_groups;
}

void draw_polline(bitmap *area, const std::list<polygon_edge> &active_edges, double y)
{
   auto xs = active_edges.cbegin();
   auto xe = xs;
   bool drawing {true};

   hsv_point<> xit {0, narrow_cast<unsigned, long>(lrint(y))};
   unsigned xend;
   hsv_color dcol;

   for (++xe; xe != active_edges.end(); ++xs, ++xe, drawing = !drawing)
   {
      if (!drawing) continue;

      xit.x = lrint(xs->xstart);
      xit.colcomp() = xs->st_color;
      xend = lrint(xe->xstart);
      dcol = (xe->st_color - xs->st_color) / (xe->xstart - xs->xstart);

      for (; xit.x < xend; xit.x++, xit.colcomp() += dcol) area->setpixel(xit);
   }
}

void polygon(bitmap *area, const points_vect &points)
{
   if (3 > points.size()) throw std::range_error {"Insufficient points for a polygon."};

   double ymax;
   edge_table edge_groups {build_groups(points, ymax)};
   if (0 == edge_groups.size()) throw std::range_error {"Incorrect polygon area"};

   std::list<polygon_edge> active_edges;
   for (double y = edge_groups.cbegin()->first; y <= ymax; y++)
   {
      active_edges.remove_if([y,ymax](const polygon_edge &e) { return (y == e.ymax && e.ymax < ymax); });
      if (edge_groups.end() != edge_groups.find(y))
         for (const auto &edge : edge_groups[y]) active_edges.push_back(edge);

      if (2 > active_edges.size()) throw std::runtime_error {"Less than two active edges."};
      active_edges.sort();

      draw_polline(area, active_edges, y);
      for (auto &edge : active_edges) edge.shift();
   }
}

int main(int, char **)
{
   window mainwin {"some", {win_width, win_height}};
   bitmap *mainarea = mainwin.get_bitmap();
   mainarea->clear();

   points_vect triangle {{200, 100, 150, 1.0, 0.2}, {300, 350, 150, 1.0, 0.9}, {100, 300, 150, 1.0, 0.6}};
   points_vect rectangle {{ 100, 100, 150, 1.0, 0.6 }, { 300, 100, 150, 1.0, 0.9 }, {300, 300, 150, 1.0, 0.75}, {100, 300, 150, 1.0, 0.1}};
   points_vect some {{100, 200, 150, 1.0, 0.2}, {400, 768, 150, 1.0, 0.3}, {768, 600, 150, 1.0, 0.7}, {700, 200, 150, 1.0, 0.2}, {500, 400, 150, 1.0, 0.9}};

   polygon(mainarea, triangle);
   mainwin.update();

   wait_keyevent();
   return 0;
}