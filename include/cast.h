#ifndef GRAPHICS_CAST_H
#define GRAPHICS_CAST_H

#include <stdexcept>

namespace graphics {

template <typename to, typename from>
to narrow_cast(from value)
{
   auto result = static_cast<to>(value);
   if (static_cast<from>(result) != value)
      throw std::runtime_error {"narrow_cast failed"};
   return result;
}

}

#endif
