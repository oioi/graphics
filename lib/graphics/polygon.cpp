#include <algorithm>
#include <array>

#include "shape.h"
#include "polygon.h"
#include "line.h"

namespace graphics {

polygon_edge::polygon_edge(long ymin_, long ymax_, double xstart_, double xend_, const hsv_color &st, const hsv_color &end) :
   ymax{ymax_}, xstart{xstart_}, xend{xend_}, st_color{st}, end_color{end}
{
   dx = (xend - xstart) / (ymax - ymin_);
   dcolor = (end_color - st_color) / (ymax - ymin_);
}

polygon::polygon(raster_t *rast, const std::initializer_list<ppoint> &list) :
   rasterizer(rast), points(list) 
{
   if (3 > list.size()) throw std::range_error {"Insufficient points for a polygon."};
   points.push_back(*list.begin()); 
}

polygon::polygon(raster_t *rast, const points_vect &vect) :
   rasterizer(rast), points(vect)
{
   if (3 > vect.size()) throw std::range_error {"Insufficient points for a polygon."};
   points.push_back(vect.front());
}

void polygon::draw_contour()
{
   points_vect::const_iterator it {points.cbegin()};
   points_vect::const_iterator next {it + 1};

   for (; points.end() != next; ++it, ++next)
   {
      line edge {raster, *it, *next};
      edge.draw();
   }
}

void polygon::build_groups()
{
   ymax = 0;
   edge_groups.clear();
   points_vect::const_iterator it {points.begin()};
   points_vect::const_iterator next {it + 1};
   points_vect::const_iterator min, max;

   for (; points.end() != next; ++it, ++next)
   {
      min = it;
      max = next;
      if (max->y == min->y) continue; // Skipping horizontal edges

      if (max->y < min->y) std::swap(min, max);
      if (max->y > ymax) ymax = lrint(max->y);
      edge_groups[lrint(min->y)].emplace_back(lrint(min->y), lrint(max->y), min->x, max->x, *min, *max);
   }

   for (auto &group : edge_groups)
      std::sort(group.second.begin(), group.second.end());
}

void polygon::draw_polline(const active_edge_table &active_edges, long y) const
{
   active_edge_table::const_iterator it {active_edges.cbegin()};
   active_edge_table::const_iterator next {it};
   bool drawing {true};

   hsv_point<> xit {0, y};
   long xend;
   hsv_color dcol;

   for (++next; next != active_edges.end(); ++it, ++next, drawing = !drawing)
   {
      if (!drawing) continue;
      xit.x = lrint(it->xstart);
      xit.colcomp() = it->st_color;
      xend = lrint(next->xstart);

      dcol = (next->st_color - it->st_color) / (next->xstart - it->xstart);
      for (; xit.x <= xend; xit.x++, xit.colcomp() += dcol) raster->setpixel(xit);
   }
}

void polygon::render()
{
   if (!built) { built = true; build_groups(); }
   if (0 == edge_groups.size()) throw std::range_error {"Incorrect polygon area"};

   std::list<polygon_edge> active_edges;
   for (long y = edge_groups.cbegin()->first; y <= ymax; y++)
   {
      if (edge_groups.end() != edge_groups.find(y))      
         for (const auto &edge : edge_groups[y]) active_edges.push_back(edge);
      active_edges.remove_if([y, this](const polygon_edge &e) { return (y >= e.ymax && e.ymax < ymax); });

      #ifndef GRC_NO_DEBUG
      if (2 > active_edges.size()) throw std::runtime_error {"Less than two active edges."};
      #endif

      active_edges.sort();
      draw_polline(active_edges, y);
      for (auto &edge : active_edges) edge.shift();
   }
}

area_polygon::points_vect area_polygon::clip_side(raster_t *area, const points_vect &points, position mode)
{
   const area_coord box {area->box()};
   points_vect output;
   auto it = points.cbegin();
   auto next = it + 1;

   posval code1, code2;

   for (; points.end() != next; ++it, ++next)
   {
      code1 = code2 = 0;

      switch (mode)
      {
         case position::left:
            if (it->x < box.first.x) code1 |= mode;
            if (next->x < box.first.x) code2 |= mode;
            break;

         case position::right:
            if (it->x > box.second.x) code1 |= mode;
            if (next->x > box.second.x) code2 |= mode;
            break;

         case position::top:
            if (it->y < box.first.y) code1 |= mode;
            if (next->y < box.first.y) code2 |= mode;
            break;

         case position::bottom:
            if (it->y > box.second.y) code1 |= mode;
            if (next->y > box.second.y) code2 |= mode;
            break;
      }

      if ((0 != code1) && (0 != code2)) continue;
      else if ((0 == code1) && (0 == code2)) output.push_back(*next);
      else if ((0 != code1) && (0 == code2))
      {
         if      (code1 & position::left)   output.push_back(clipline(*it, *next, box.first.x,  clipmode::width));
         else if (code1 & position::right)  output.push_back(clipline(*it, *next, box.second.x, clipmode::width));
         else if (code1 & position::bottom) output.push_back(clipline(*it, *next, box.second.y, clipmode::height));
         else if (code1 & position::top)    output.push_back(clipline(*it, *next, box.first.y,  clipmode::height));
         output.push_back(*next);
      }

      else if ((0 == code1) && (0 != code2))
      {
         if      (code2 & position::left)   output.push_back(clipline(*next, *it, box.first.x,  clipmode::width));
         else if (code2 & position::right)  output.push_back(clipline(*next, *it, box.second.x, clipmode::width));
         else if (code2 & position::bottom) output.push_back(clipline(*next, *it, box.second.y, clipmode::height));
         else if (code2 & position::top)    output.push_back(clipline(*next, *it, box.first.y,  clipmode::height));
      }
   }

   if (0 != output.size()) output.push_back(output.front());
   return output;
}

void area_polygon::areacheck()
{
   static const std::array<position, 4> pos {position::left, position::top, position::right, position::bottom};
   checked = true;

   for (auto & mode : pos)
   {
      points = clip_side(raster, points, mode);
      if (0 == points.size()) { inside = false; return; }
   }
   inside = true;
}

void area_polygon::draw()
{
   if (!checked) areacheck();
   if (inside) polygon::render();
}

void area_polygon::draw_contour()
{
   if (!checked) areacheck();
   if (inside) polygon::draw_contour();
}

}