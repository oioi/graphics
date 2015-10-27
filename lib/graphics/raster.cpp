#include <stdexcept>
#include <cstring>
#include "raster.h"

namespace graphics {

void bitmap::clear(uint8_t def)
{
   for (long i = 0; i < mapsize.height; i++)
      memset(framebuf + ((i + offsets.height) * framesize.width) + offsets.width, def, sizeof(uint32_t) * mapsize.width);
}

void bitmap::setpixel(const point<> &p, uint32_t color)
{
   #ifndef GRC_NO_DEBUG
   if (0 > p.x || 0 > p.y) throw std::out_of_range  {"point has negative coordinates"};
   #endif

   if (coordinates::absolute == type)
   {
      #ifndef GRC_NO_DEBUG
      if (p.x < offsets.width ||  p.y < offsets.height ||
         p.x >= offsets.width + mapsize.width || p.y >= offsets.height + mapsize.height)
         throw std::out_of_range {"point's absolute coordinates are not in area"};
      #endif

      framebuf[p.y * framesize.width + p.x] = color;
      return;
   }

   #ifndef GRC_NO_DEBUG
   if (p.x >= mapsize.width || p.y >= mapsize.height)
      throw std::out_of_range {"relative coordinates are too big for this areasize"};
   #endif

   framebuf[(p.y + offsets.height) * framesize.width + (p.x + offsets.width)] = color;
}

}