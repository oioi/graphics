#include <Eigen/Dense>
#include <iostream>

#include "graphics.h"


using namespace graphics;

enum move_dir
{
   front = 0,
   back  = 1,
   left  = 2,
   right = 3
};

class camera
{
   public:
      camera(double fovy, double aspect, double znear, double zfar);
      Eigen::Vector3d project(const Eigen::Vector3d &point);

      bool move_dirs[4] {};

      void m_pitch(double angle);
      void m_yaw(double angle) { yaw += angle; }

      void buildM();
      const Eigen::Matrix4d & getMp() const { return Mp; }
      const Eigen::Matrix4d & getMv() const { return Mv; }
      double getNear() const { return znear; }
      double getFar() const { return zfar; }

      const Eigen::Vector3d & getPos() const { return camera_pos; }
      const Eigen::Vector3d & getFront() const { return camera_front; }

   private:
      double yaw       {90.0};
      double pitch     {0.0};
      double max_pitch {45.0};
      double fovy;

      double znear;
      double zfar;

      Eigen::Vector3d camera_pos   {0.0, 0.0, 0.0};
      Eigen::Vector3d camera_front {0.0, 0.0, 1.0};
      Eigen::Vector3d camera_up    {0.0, 1.0, 0.0};

      Eigen::Matrix4d Mp;
      Eigen::Matrix4d Mv;

      void build_frontv();
      void move_camera();
};

camera::camera(double fovy_, double aspect, double znear_, double zfar_) : fovy{fovy_}, znear{znear_}, zfar{zfar_}
{
   const double f = 1 / tan(fovy * M_PI / 360.0);

   Mp <<  f / aspect,           0,                0,                              0,
               0,               f,                0,                              0,
               0,               0, (zfar + znear) / (znear - zfar), (2 * zfar * znear) / (znear - zfar),
               0,               0,               -1,                              0;
   buildM();
}

void camera::build_frontv()
{
   camera_front[0] = cos(yaw * (M_PI / 180.0)) * cos(pitch * (M_PI / 180.0));
   camera_front[1] = sin(pitch * (M_PI / 180.0));
   camera_front[2] = sin(yaw * (M_PI / 180.0)) * cos(pitch * (M_PI / 180.0));
   camera_front.normalize();
}

void camera::move_camera()
{
   int dirs = move_dirs[0] + move_dirs[1] + move_dirs[2] + move_dirs[3];
   if (0 == dirs || 4 == dirs) return;

   double move_speed = 0.05;
   if (2 == dirs) move_speed *= (1 / sqrt(2.0));

   Eigen::Vector3d frontv {camera_front[0], 0, camera_front[2]};
   frontv.normalize();
   Eigen::Vector3d rightv {frontv.cross(camera_up)};

   if (move_dirs[front]) camera_pos += frontv * move_speed;
   if (move_dirs[back]) camera_pos -= frontv * move_speed;
   if (move_dirs[right]) camera_pos += rightv * move_speed;
   if (move_dirs[left]) camera_pos -= rightv * move_speed;
}

void camera::buildM()
{
   build_frontv();
   move_camera();

   const Eigen::Vector3d Z = ((camera_pos + camera_front) - camera_pos).normalized();
   const Eigen::Vector3d X = (Z.cross(camera_up)).normalized();
   const Eigen::Vector3d Y = X.cross(Z);

   Mv << X[0],  X[1],  X[2], -X.dot(camera_pos),
         Y[0],  Y[1],  Y[2], -Y.dot(camera_pos),
        -Z[0], -Z[1], -Z[2],  Z.dot(camera_pos),
            0,     0,     0,         1;
}

void camera::m_pitch(double angle)
{
   pitch += angle;
   if (pitch > max_pitch) pitch = max_pitch;
   else if (pitch < -max_pitch) pitch = -max_pitch;
}

int handle_key(const SDL_Event &event, camera &cam, bool state)
{
   switch (event.key.keysym.sym)
   {
      case SDLK_ESCAPE: return 1;

      case SDLK_w: cam.move_dirs[front] = state; break;
      case SDLK_s: cam.move_dirs[back] = state; break;
      case SDLK_d: cam.move_dirs[right] = state; break;
      case SDLK_a: cam.move_dirs[left] = state; break;
      default: return 0;
   }

   return 0;
}

void handle_mouse(const SDL_Event &event, camera &cam)
{
   static const double msens = 0.01;
   if (0 != event.motion.xrel) cam.m_yaw(event.motion.xrel * msens);
   if (0 != event.motion.yrel) cam.m_pitch(-event.motion.yrel * msens);
}

void clip_depth(Eigen::Vector4d &p, double dx, double dy, double dz, double depth)
{
   p[0] += dx * (depth - p[2]) / dz;
   p[1] += dy * (depth - p[2]) / dz;
   p[2] = depth;
}

void pline(const Eigen::Vector3d &p1, const Eigen::Vector3d &p2, const camera &cam, bitmap *raster)
{
   enum winsize { win_width = 1280, win_height = 720 };
   const Eigen::Matrix4d &Mv = cam.getMv();
   Eigen::Vector4d dp1 = Mv * Eigen::Vector4d {p1[0], p1[1], p1[2], 1.0};
   Eigen::Vector4d dp2 = Mv * Eigen::Vector4d {p2[0], p2[1], p2[2], 1.0};

   double near = -cam.getNear();
   double far = -cam.getFar();

   if ((dp1[2] > near && dp2[2] > near) ||
       (dp1[2] < far && dp2[2] < far )) return;

   double dx = dp2[0] - dp1[0];
   double dy = dp2[1] - dp1[1];
   double dz = dp2[2] - dp1[2];

   if (dp1[2] > near) clip_depth(dp1, dx, dy, dz, near);
   if (dp1[2] < far) clip_depth(dp1, dx, dy, dz, far);

   if (dp2[2] > near) clip_depth(dp2, dx, dy, dz, near);
   if (dp2[2] < far) clip_depth(dp2, dx, dy, dz, far);

   const Eigen::Matrix4d &Mp = cam.getMp();
   dp1 = Mp * dp1;
   dp2 = Mp * dp2;

   dp1 /= (dp1[3] == 0) ? 1 : dp1[3];
   dp2 /= (dp2[3] == 0) ? 1 : dp2[3];

   if ((dp1[0] < -1.0 && dp2[0] < -1.0) ||
       (dp1[0] > 1.0 && dp2[0] > 1.0) ||
       (dp1[1] > 1.0 && dp2[1] > 1.0) ||
       (dp1[1] < -1.0 && dp1[1] > 1.0)) return;

   area_line line {raster, 
      hsv_point<double>{(dp1[0] + 1) * win_width / 2, (-dp1[1] + 1) * win_height / 2, 0, 0, 0}, 
      hsv_point<double>{(dp2[0] + 1) * win_width / 2, (-dp2[1] + 1) * win_height / 2, 0, 0, 0}
   };
   line.draw();
}

int main(int, char **)
{
   enum winsize { win_width = 1280, win_height = 720 };

   camera cam {65.0, 16.0 / 9.0, 0.1, 100.0};
   const size_t bufsize {512};
   char buffer [bufsize];

   SDL_ShowCursor(0);
   SDL_SetRelativeMouseMode(SDL_TRUE);
   window mainwin {"Polygons", {win_width, win_height}};
   bitmap *mainarea = mainwin.get_bitmap();

   mainarea->clear();

   uint32_t ticks;
   uint32_t ticks_per_frame = 1000 / 60;

   for (bool running = true; running;)
   {
      ticks = SDL_GetTicks();

      pline({-2.0, -2.0, 6.0}, {2.0, -2.0, 6.0}, cam, mainarea);
      pline({2.0, -2.0, 6.0}, {2.0, -2.0, 10.0}, cam, mainarea);
      pline({2.0, -2.0, 10.0}, {-2.0, -2.0, 10.0}, cam, mainarea);
      pline({-2.0, -2.0, 10.0}, {-2.0, -2.0, 6.0}, cam, mainarea);

      pline({-2.0, 2.0, 6.0}, {2.0,  2.0, 6.0}, cam, mainarea);
      pline({2.0,  2.0, 6.0}, {2.0,  2.0, 10.0}, cam, mainarea);
      pline({2.0,  2.0, 10.0}, {-2.0, 2.0, 10.0}, cam, mainarea);
      pline({-2.0, 2.0, 10.0}, {-2.0, 2.0, 6.0}, cam, mainarea);

      pline({-2.0, -2.0, 6.0}, {-2.0, 2.0, 6.0}, cam, mainarea);
      pline({2.0, -2.0, 6.0}, {2.0,  2.0, 6.0}, cam, mainarea);
      pline({2.0, -2.0, 10.0}, {2.0,  2.0, 10.0}, cam, mainarea);
      pline({-2.0, -2.0, 10.0}, {-2.0, 2.0, 10.0}, cam, mainarea);



      pline({3.0, -2.0, 6.0}, {7.0, -2.0, 6.0}, cam, mainarea);
      pline({7.0, -2.0, 6.0}, {7.0, -2.0, 10.0}, cam, mainarea);
      pline({7.0, -2.0, 10.0}, {3.0, -2.0, 10.0}, cam, mainarea);
      pline({3.0, -2.0, 10.0}, {3.0, -2.0, 6.0}, cam, mainarea);

      pline({3.0, -2.0, 6.0}, {5.0, 2.0, 8.0}, cam, mainarea);
      pline({7.0, -2.0, 6.0}, {5.0, 2.0, 8.0}, cam, mainarea);
      pline({7.0, -2.0, 10.0}, {5.0, 2.0, 8.0}, cam, mainarea);
      pline({3.0, -2.0, 10.0}, {5.0, 2.0, 8.0}, cam, mainarea);

      const Eigen::Vector3d &camera_pos = cam.getPos();
      const Eigen::Vector3d &camera_front = cam.getFront();

      snprintf(buffer, bufsize, "Camera position: %f %f %f     Front vector: %f %f %f",
         camera_pos[0], camera_pos[1], camera_pos[2], camera_front[0], camera_front[1], camera_front[2]);
      mainwin.write_text(buffer);

      mainwin.update();

      for (;;)
      {
         SDL_Event event;
         if (0 == SDL_PollEvent(&event)) break;

         switch (event.type)
         {
            case SDL_QUIT: running = false; break;
            case SDL_KEYDOWN: if (handle_key(event, cam, true)) running = false; break;
            case SDL_KEYUP: handle_key(event, cam, false); break;
            case SDL_MOUSEMOTION: handle_mouse(event, cam); break;
         }
      }

      cam.buildM();
      mainarea->clear();

      uint32_t frameticks = SDL_GetTicks() - ticks;
      if (frameticks < ticks_per_frame) 
      {
         std::cerr << "\rFrame delayed for " << ticks_per_frame - frameticks <<"ms.";
         SDL_Delay(ticks_per_frame - frameticks);
      }
   }

   return 0;
}
