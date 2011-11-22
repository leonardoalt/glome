#include <cmath>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <GL/glew.h>
#include <GL/glu.h>

#include "4dmath.hpp"
#include "projectile.hpp"
#include "ship.hpp"
#include "shader.hpp"
#include "meridian.hpp"
#include "world.hpp"
#include "options.hpp"

#include "minimap.hpp"

using namespace std;
using namespace Options;

static GLuint tex_minimap, program_map;
static GLuint tex_object;
static GLint proj_only_uniform;

void
MiniMap::draw(int wstart, World* world, const Matrix4& center)
{
  // Calculate field of vision
  const float radius = 75.0;
  const int t = 160;
  const int b = 10;
  const int l = 10;
  const int r = 160;

  const float cx = 0;
  const float cy = 0;

  const float angle = asin(0.5f * width * sin(FOV * M_PI / 360.0f) / height);
  const float dx = (radius * sin(angle));
  const float ppx0 = (cx - dx) / radius;
  const float ppy = ((cy + (radius * cos(angle))) / radius) * 0.9;
  const float ppx1 = (cx + dx) / radius;

  // Change to minimap display area/projection.
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glViewport(wstart + l, b, r, t);
  glOrtho(-1, 1, -1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);

  // Draw 2D green background.
  glUseProgram(0);
  glDisable(GL_FOG);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glLoadIdentity();

  glColor3ub(14, 164, 3);
  glBindTexture(GL_TEXTURE_2D, tex_minimap);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex2f(-1, -1);
  glTexCoord2f(1, 0);
  glVertex2f(1, -1);
  glTexCoord2f(1, 1);
  glVertex2f(1, 1);
  glTexCoord2f(0, 1);
  glVertex2f(-1, 1);
  glEnd();
  glBindTexture(GL_TEXTURE_2D, 0);

  // Draw field of vision
  glColor3ub(14, 164, 3);
  glBegin(GL_LINES);
  glVertex2f(ppx0, ppy);
  glVertex2f(cx, cy);
  glVertex2f(ppx1, ppy);
  glVertex2f(cx, cy);
  glEnd();

  // Draw ship object
  glBindTexture(GL_TEXTURE_2D, 0);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glColor3ub(255, 255, 255);
  glBegin(GL_TRIANGLE_FAN);
  float a = 0.07;
  glVertex2f(0, a);
  glVertex2f(a, -a);
  glVertex2f(0, -a/2.f);
  glVertex2f(-a, -a);
  glEnd();

  // Draw objects
  glUseProgram(program_map);
  Matrix4 cam = yz_matrix(M_PI / 2) * center;
  cam.loadToGL();

  // Draw shots
  glUniform1i(proj_only_uniform, 1);
  Projectile::draw_in_minimap();
  draw_meridians(cam);

  glUniform1i(proj_only_uniform, 0);
  glBindTexture(GL_TEXTURE_2D, tex_object);
  world->fill_minimap();
  glBindTexture(GL_TEXTURE_2D, 0);

  // Disable 2D
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_FOG);
  glMatrixMode(GL_PROJECTION);
  glViewport(0, 0, width, height);    
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

void MiniMap::draw_dot(const Object& obj)
{
	glPushMatrix();
	obj.transformation().multToGL();
	
	glColor3ub(255, 0, 0);
	glBegin(GL_QUADS);
	glVertex2i(-1, -1);
	glVertex2i(1, -1);
	glVertex2i(1, 1);
	glVertex2i(-1, 1);
	glEnd();
	glPopMatrix();
}
	
void
create_circle_texture(int w, float p, int a0, int a1, GLuint& tex)
{
  int i, j;
  float cx, cy, d, tex_r, tex_r_lim;
  unsigned char* texture = (unsigned char*)malloc(w * w * sizeof(unsigned char));
  cx = cy = (w - 1) / 2.;
  tex_r = w / 2.;
  tex_r_lim = tex_r * p;

  for(i = 0; i < w; ++i)
  {
    for(j = 0; j < w; ++j)
    {
			float x;
      d = sqrt(((i - cx) * (i - cx)) + ((j - cy) * (j - cy)));
      texture[(i * w) + j] = (d > tex_r) ? a0 : ((d < tex_r_lim) ? a1 : ( (x = (tex_r - d) / (tex_r - tex_r_lim), x*x) * a1 ));
    }
  }

  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, w, 0,
  GL_ALPHA, GL_UNSIGNED_BYTE, (GLvoid*)texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  free(texture);
}

void
MiniMap::initialize()
{
  create_circle_texture(256, 0.9, 0, 142, tex_minimap);
  create_circle_texture(16, 0.8, 0, 255, tex_object);

#include "minimap_proj.glsl.hpp"
  program_map = setup_shader(minimap_proj_glsl, minimap_proj_glsl_len);
  proj_only_uniform = glGetUniformLocation(program_map, "proj_only");
}
