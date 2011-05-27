#pragma once

#include "object.hpp"

class Ship: public Object
{
public:
  static void initialize();

  Ship();
  void draw();
  void update();

  inline void rotate(float rx, float ry)
  {
    h_req = rx / 100.0;
    v_req = -ry / 100.0;
  }

  inline void move(float a)
  {
    accel = a;
  }

  inline void move_up(bool a)
  {
    up = a;
  }

  inline void move_down(bool a)
  {
    down = a;
  }

  inline void move_left(bool a)
  {
    left = a;
  }

  inline void move_right(bool a)
  {
    right = a;
  }

  inline void move_spinl(bool a)
  {
    spinl = a;
  }

  inline void move_spinr(bool a)
  {
    spinr = a;
  }

  inline void shot(bool a)
  {
    sh = a;
  }

private:
  inline static void handle_commands(bool, bool, float &, float &, float);
  inline static void accelerate(float &, float &, float);

  float v_req, h_req;
  float v_tilt, h_tilt;
  float accel, speed;
  float speed_v;
  float speed_h;
  float speed_s;
  bool up;
  bool down;
  bool left;
  bool right;
  bool spinl;
  bool spinr;
  bool sh;
  int sps;
  int shot_count;

  Matrix4 r_canon, l_canon;
};

