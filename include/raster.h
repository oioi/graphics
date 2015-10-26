#ifndef GRAPHICS_RASTER_H
#define GRAPHICS_RASTER_H

#include <stdexcept>
#include <cstdint>

#include "point.h"

namespace graphics {

using uint_t = unsigned long;

enum class coordinates {
   absolute,
   relative
};

using posval = uint8_t;
enum class position : posval {
   left   = 1 << 0,
   right  = 1 << 1,
   bottom = 1 << 2,
   top    = 1 << 3
};

inline constexpr position operator &(position A, position B) {
   return static_cast<position>(static_cast<posval>(A) & static_cast<posval>(B)); }

inline constexpr posval operator &(posval val, position pos) {
   return val & static_cast<posval>(pos); }

inline posval & operator |=(posval &val, position pos) {
   return val |= static_cast<posval>(pos); }

template <typename T>
posval position_code(const area_coord &box, const point<T> &p)
{
   posval code = 0;

   if      (p.x < box.first.x)  code |= position::left;
   else if (p.x > box.second.x) code |= position::right;
   if      (p.y < box.first.y)  code |= position::top;
   else if (p.y > box.second.y) code |= position::bottom;

   return code;
}

struct areasize
{
   uint_t width;
   uint_t height;

   areasize(uint_t width_ = 0, uint_t height_ = 0) : width{width_}, height{height_} { }
   areasize(const areasize &other) : width{other.width}, height{other.height} { }
};

class bitmap
{
   public:
      bitmap(uint32_t *buf, areasize mapsize_, const areasize &framesize_, const areasize &offsets_) :
         framebuf{buf}, framesize{framesize_}, mapsize{mapsize_}, offsets{offsets_} { }

      void clear(uint8_t defb = 255);
      void setpixel(const point<> &p, uint32_t color = 0);

      template <typename T> void setpixel(const point<T> &p, uint32_t color = 0) { setpixel({lrint(p.x), lrint(p.y)}, color); }
      template <typename T> void setpixel(const color_point<T> &p) { setpixel(p, p.color()); }

      coordinates type {coordinates::absolute};
      area_coord box() const
      {
         return std::make_pair(
               point<> {offsets.width, offsets.height},
               point<> {offsets.width + mapsize.width - 1, offsets.height + mapsize.height - 1}
         );
      }

   private:
      uint32_t *framebuf;
      areasize framesize;
      areasize mapsize;
      areasize offsets;
};

}

#endif
