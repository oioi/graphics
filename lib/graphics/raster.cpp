#include <stdexcept>
#include <cstring>
#include "raster.h"

namespace graphics {

void bitmap::clear(uint8_t defb)
{
   for (unsigned i = 0; i < mapsize.height; i++)
      memset(framebuf + ((i + offsets.height) * framesize.width) + offsets.width, defb, sizeof(uint32_t) * mapsize.width);
}

void bitmap::setpixel(const point<> &p, uint32_t color)
{
   if (coordinates::absolute == type)
   {
      if (p.x < offsets.width or p.y < offsets.height or
          p.x >= offsets.width + mapsize.width or p.y >= offsets.height + mapsize.height)
         throw std::out_of_range {"point's absoulute coordinates are not in area"};
      framebuf[p.y * framesize.width + p.x] = color;
      return;
   }

   if (p.x >= mapsize.width or p.y >= mapsize.height)
      throw std::out_of_range {"relative coordinates are too big for this area size"};
   framebuf[(p.y + offsets.height) * framesize.width + (p.x + offsets.width)] = color;
}

}
