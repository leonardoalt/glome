#include <map>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <stdio.h>
#include <GL/glew.h>
#include <stdint.h>
#include <iostream>
#include "4dmath.hpp"
#include "ship.hpp"
#include "projectile.hpp"

using namespace std;

static int dlist;

void Ship::initialize()
{
  int ret;
  FILE *fd;
  GLuint vbo;
  GLuint ibo;
  uint16_t ilen;
  uint16_t vlen;

  // Load file
  fd = fopen("Hunter0.wire", "rb");
	assert(fd != NULL);
  {
    // Reading 4-D coordinates
    ret = fread(&vlen, sizeof(vlen), 1, fd);
    assert(ret == 1);

    // Create vertex buffer
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vlen * 4 * sizeof(float), NULL, GL_STATIC_DRAW);
    float *vdata = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
		ret = fread(vdata, sizeof(float) * 4, vlen, fd); 
		assert(ret == vlen);
    glUnmapBuffer(GL_ARRAY_BUFFER);
  }

  {
    ret = fread(&ilen, sizeof(ilen), 1, fd);
    assert(ret == 1);

    // Create index buffer
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ilen * sizeof(uint16_t) * 2, NULL, GL_STATIC_DRAW);
    uint16_t *idata = (uint16_t*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE);

    ret = fread(idata, sizeof(uint16_t) * 2, ilen, fd);
    assert(ret == ilen);

    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
  }

  fclose(fd);

  // Create the display list
  dlist = glGenLists(1);
  glNewList(dlist, GL_COMPILE);
  glColor3ub(80, 80, 80);
  glVertexPointer(4, GL_FLOAT, 0, NULL);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glDrawRangeElements(GL_LINES, 0, vlen-1, ilen*2, GL_UNSIGNED_SHORT, NULL);
  glEndList();
}

Ship::Ship():
    Drawable(Matrix4::IDENTITY),
    v_tilt(0.0f),
    h_tilt(0.0f),
    speed(0.0f),
    speed_v(0.0f),
    speed_h(0.0f),
    speed_s(0.0f),
    sps(15),
    shot_count(0),
    r_canon(xw_matrix(0.005) * zw_matrix(-0.015)),
    l_canon(xw_matrix(-0.005) * zw_matrix(-0.015))
{}

void Ship::draw()
{
  glPushMatrix();
  t.multToGL();
  glCallList(dlist);
  glPopMatrix();
}

void Ship::update()
{
  /* Maximum turning delta per frame in radians. */
  const float MAXR = 0.03;

  /* Maximum speed, in radians per second. */
  const float MAXS = 0.0015;
  const float MAXS_V = 0.0004;
  const float MAXS_H = 0.0004;
  const float MAXS_S = 0.02;

  float accel_v = 0.f, accel_h = 0.f, accel_s = 0.f;

  /* Turning */
  float h = h_tilt - h_req;
  float v = v_tilt - v_req;

  /* Limit the turning speed to MAXD rads per frame. */
  if(h > MAXR)
    h = MAXR;
  else if(h < -MAXR)
    h = -MAXR;
  if(v > MAXR)
    v = MAXR;
  else if(v < -MAXR)
    v = -MAXR;

  h_tilt -= h;
  v_tilt -= v;

  /* Handle input commands */
  handle_commands(up, down, speed_v, accel_v, 0.00004);
  handle_commands(right, left, speed_h, accel_h, 0.00004);
  handle_commands(spinl, spinr, speed_s, accel_s, 0.002);

  /* Accelerating */
  accelerate(speed, accel, MAXS);
  accelerate(speed_v, accel_v, MAXS_V);
  accelerate(speed_h, accel_h, MAXS_H);
  accelerate(speed_s, accel_s, MAXS_S);

  /* Shooting */
  shot_count -= sps;
  if(shot_count < 0) {
    if(sh) {
      static bool right = true;
      Projectile::shot(t * (right ? r_canon : l_canon), 0.02 - speed);
      shot_count += 60;
      right = !right;
    }
    else
      shot_count = 0;
  }

  t = t * zw_matrix(speed) * yw_matrix(speed_v) * xw_matrix(speed_h) * xy_matrix(speed_s) * yz_matrix(v_tilt) * rotation(-h_tilt, 0.0, M_SQRT2/2.0, M_SQRT2/2.0);
}

void
Ship::accelerate(float& speed, float& accel, float max)
{
  speed += accel;
  if(speed > max)
    speed = max;
  else if(speed < -max)
    speed = -max;
}

void
Ship::handle_commands(bool k0, bool k1, float& speed, float& accel, float max_accel)
{
  if(k0 || k1)
  {
    accel = max_accel;
    if(k1)
      accel = -accel;
  }
  else
  {
    if(speed > 0.f)
    {
      accel = -max_accel;
      if((speed + accel) < 0.f)
      {
        speed = 0.f;
        accel = 0.f;
      }
    }
    else if(speed < 0.f)
    {
      accel = max_accel;
      if((speed + accel) > 0.f)
      {
        speed = 0.f;
        accel = 0.f;
      }
    }
  }
}

