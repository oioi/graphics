#ifndef GRAPHICS_POLYGON_H
#define GRAPHICS_POLYGON_H

#include <map>
#include <list>
#include <vector>
#include <initializer_list>

#include "point.h"
#include "shape.h"

namespace graphics {

struct polygon_edge
{
   long ymax;
   double xstart;
   double xend;
   double dx;

   hsv_color st_color;
   hsv_color end_color;
   hsv_color dcolor;

   polygon_edge(const polygon_edge &other) = default;
   polygon_edge(long ymin_, long ymax_, double xstart_, double xend_, const hsv_color &st, const hsv_color &end);

   void shift() { xstart += dx; st_color += dcolor; }
   bool operator <(const polygon_edge &other) { return xstart < other.xstart; }
};

class polygon : public shape, protected rasterizer
{
   public:
      using ptype = double;
      using ppoint = hsv_point<ptype>;
      using points_vect = std::vector<ppoint>;
      using edge_table = std::map<long, std::vector<polygon_edge>>;
      using active_edge_table = std::list<polygon_edge>;

      polygon(raster_t *rast, const std::initializer_list<ppoint> &list);
      polygon(raster_t *rast, const points_vect &vect);

      virtual void draw() override { render(); }
      virtual void draw_contour();

   protected:
      points_vect points;
      bool changed {true};

      void render();

   private:
      long ymax;
      edge_table edge_groups;

      void build_groups();
      void draw_polline(const active_edge_table &active_edges, long y) const;
};

class area_polygon : protected polygon
{
   public:
      area_polygon(raster_t *rast, const std::initializer_list<ppoint> &list) :
         polygon(rast, list), origin_points {list} { origin_points.push_back(*list.begin()); }
      area_polygon(raster_t *rast, const points_vect &vect) :
         polygon(rast, vect), origin_points(vect) { origin_points.push_back(vect.front()); }

      void draw() override;
      void draw_contour() override;

   private:
      bool checked {false};
      bool inside {false};
      points_vect origin_points;

      void clip();
      points_vect clip_side(raster_t *area, const points_vect &points, position mode);
};

}

#endif
