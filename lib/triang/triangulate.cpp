#include <memory>
#include "triangulate.h"

/* Based on code by John W. Ratcliff (jratcliff@verant.com) on FlipCode.com
 * I didn't actually checked this code, just rewrited. 
 * Looks like very basic ear-clipping algorithm. Needs to be checked. */

namespace graphics {

double area(const vertices &contour)
{
   int n = contour.size();
   double a = 0;

   for (int p = n - 1, q = 0; q < n; p = q++)
      a += contour[p].x * contour[q].y - contour[q].x * contour[p].y;
   return a * 0.5l;
}

bool inside_triangle(point<double> p, const vertices &tri)
{
   double ap = (tri[2].x - tri[1].x) * (p.y - tri[1].y) - (tri[2].y - tri[1].y) * (p.x - tri[1].x);
   double bp = (tri[0].x - tri[2].x) * (p.y - tri[2].y) - (tri[0].y - tri[2].y) * (p.x - tri[2].x);
   double cp = (tri[1].x - tri[0].x) * (p.y - tri[0].y) - (tri[1].y - tri[0].y) * (p.x - tri[0].x);
   return ((ap >= 0.0l) && (bp >= 0.0l) && (cp >= 0.0l));
}

bool snip(const vertices &contour, int u, int v, int w, int n, int *V)
{
   static const double E = 0.0000000001l;
   double ax = contour[V[u]].x;
   double ay = contour[V[u]].y;

   double bx = contour[V[v]].x;
   double by = contour[V[v]].y;

   double cx = contour[V[w]].x;
   double cy = contour[V[w]].y;
   if (E > (((bx - ax) * (cy - ay)) - ((by - ay) * (cx - ax )))) return false;


   for (int p = 0; p < n; p++)
   {
      if ((p == u) or (p == v) or (p == w)) continue;
      if (inside_triangle(contour[V[p]], {ax, ay, bx, by, cx, cy})) return false;
   }
   return true;
}

vertices decompose(const vertices &contour)
{
   int n = contour.size();
   if (n < 3) return vertices{};

   int *V = new int[n];
   std::unique_ptr<int> hold{V};

   if (0.0l < area(contour))
      for (int v = 0; v < n; v++) V[v] = v;
   else
      for (int v = 0; v < n; v++) V[v] = (n - 1) - v;

   int nv = n;
   int count = 2 * nv;
   vertices result;

   for (int m = 0, v = nv - 1; nv > 2; )
   {
      if (0 >= (count--)) throw std::runtime_error {"triangulation error."};
      int u = v;     if (nv <= u) u = 0;
      v = u + 1;     if (nv <= v) v = 0;
      int w = v + 1; if (nv <= w) w = 0;

      if (snip(contour, u, v, w, nv, V))
      {
         result.push_back(contour[V[u]]);
         result.push_back(contour[V[v]]);
         result.push_back(contour[V[w]]);

         m++;
         for (int s = v, t = v + 1; t < nv; s++, t++) V[s] = V[t]; nv--;
         count = 2 * nv;
      }
   }

   return result;
}

}
