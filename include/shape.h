#ifndef GRAPHICS_SHAPE_H
#define GRAPHICS_SHAPE_H

#include "raster.h"

namespace graphics {

using raster_t = bitmap;

class shape
{
   public:
      virtual void draw() = 0;
};

class rasterizer
{
   protected:
      rasterizer(raster_t *raster_) : raster{raster_} { }
      bitmap *raster;
};

}

#endif