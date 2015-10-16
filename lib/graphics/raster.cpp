#include <stdexcept>
#include <cstring>
#include "raster.h"

namespace graphics {

void bitmap::clear(uint8_t def)
{
   for (unsigned i = 0; i < mapsize.height; i++)
      memset(framebuf + ((i + offsets.height) * framesize.width) + offsets.width, def, sizeof(uint32_t) * mapsize.width);
}

void bitmap::setpixel(const point<> &p, uint32_t color)
{
   if (coordinates::absolute == type)
   {
      if (p.x < offsets.width ||  p.y < offsets.height ||
         p.x >= offsets.width + mapsize.width || p.y >= offsets.height + mapsize.height)
         throw std::out_of_range {"point's absolute coordinates are not in area"};
      framebuf[p.y * framesize.width + p.x] = color;
      return;
   }

   if (p.x >= mapsize.width || p.y >= mapsize.height)
      throw std::out_of_range {"relative coordinates are too big for this areasize"};
   framebuf[(p.y + offsets.height) * framesize.width + (p.x + offsets.width)] = color;
}

}