/*  Physically Modeled Media Plug-In for The GIMP
 *  Copyright (c) 2000-2002 David A. Bartold
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Code taken from Terraform.
 */


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <math.h>
#include <time.h>
#include <stdlib.h>

#include "support.h"
#include "tworld.h"
#include "tperlin3d.h"
#include "native3d.h"

#define EARTH_RADIUS            6375.0
#define EARTH_CLOUD_HEIGHT         5.0
#define EARTH_ATMOSPHERE_HEIGHT  100.0
#define LENS_ANGLE                70.0

#define SKY_RED      0.01
#define SKY_GREEN    0.04
#define SKY_BLUE     0.18
#define HAZE_RED     0.31
#define HAZE_GREEN   0.35
#define HAZE_BLUE    0.40
#define SUN_RED      0.995
#define SUN_GREEN    0.90
#define SUN_BLUE     0.83
#define CLOUD_RED    1.0
#define CLOUD_GREEN  1.0
#define CLOUD_BLUE   1.0
#define SHADOW_RED   0.0
#define SHADOW_GREEN 0.0
#define SHADOW_BLUE  0.0

#define DEGRAD(x) ((x) * G_PI / 180.0)
#define CLIP(x,min,max) MAX (MIN (x, max), min)

typedef struct TWorld TWorld;
struct TWorld
{
  gint       width;
  gint       height;
  TPerlin3D *clouds;
  gfloat     planet_radius;
  gfloat     cloud_height;
  gfloat     camera_distance;
  TMatrix    transform;
  TVector    camera_location;
  gint       sun_x;
  gint       sun_y;
  gboolean   sun_show;
  gfloat     time;
  gdouble    horizon_color[3];
  gdouble    sky_color[3];
  gdouble    sun_color[3];
  gdouble    cloud_color[3];
  gdouble    shadow_color[3];
  gfloat     gamma;
};

static void
t_world_free (TWorld *world)
{
  t_perlin3d_free (world->clouds);
  g_free (world);
}


static gboolean
sphere_intersect (TWorld  *world,
                  gdouble  radius,
                  TVector  camera_ray,
                  TVector  intersection,
                  gdouble *dist)
{
  TVector point;
  gdouble distance;
  gdouble min_dist;
  gdouble half_chord_sqr;

  /* Sphere is in the center of the world. */
  point[0] = 0.0 - world->camera_location[0];
  point[1] = 0.0 - world->camera_location[1];
  point[2] = 0.0 - world->camera_location[2];

  distance = t_vector_dot_product (camera_ray, point);
  half_chord_sqr =
    SQR (radius) + SQR (distance) - t_vector_magnitude_squared (point);
  if (half_chord_sqr < 0.0001)
    return FALSE;

  min_dist = distance + sqrt (half_chord_sqr);
  if (min_dist < 0.0)
    return FALSE;

  if (dist != NULL)
    *dist = min_dist;

  intersection[0] = world->camera_location[0] + camera_ray[0] * min_dist;
  intersection[1] = world->camera_location[1] + camera_ray[1] * min_dist;
  intersection[2] = world->camera_location[2] + camera_ray[2] * min_dist;

  return TRUE;
}

static void
set_fore (gfloat  r,
          gfloat  g,
          gfloat  b,
          gfloat  gamma,
          guint8 *data)
{
  /*
   * Perform some contrast adjustment and clip the results before applying
   * gamma.  We adjust the contrast because the viewer expects a photograph
   * to be more vivid than the real world.  Without the adjustment, the
   * results won't resemble a photo.  Try removing the contrast adjustment
   * and see for yourself.
   */

  data[0] = TOSCREEN (CLIP (r * 1.6 - 0.2, 0.0, 1.0)) * 255.0;
  data[1] = TOSCREEN (CLIP (g * 1.6 - 0.2, 0.0, 1.0)) * 255.0;
  data[2] = TOSCREEN (CLIP (b * 1.6 - 0.2, 0.0, 1.0)) * 255.0;
}

static void
expose (gfloat *inout_red,
        gfloat *inout_green,
        gfloat *inout_blue,
        gfloat  value,
        gfloat  red,
        gfloat  green,
        gfloat  blue)
{
  *inout_red   = value * red + *inout_red * (1.0 - value);
  *inout_green = value * green + *inout_green * (1.0 - value);
  *inout_blue  = value * blue + *inout_blue * (1.0 - value);
}


static void
interpolate (gfloat *out_red,
             gfloat *out_green,
             gfloat *out_blue,
             gfloat  value,
             gfloat  red1,
             gfloat  green1,
             gfloat  blue1,
             gfloat  red2,
             gfloat  green2,
             gfloat  blue2)
{
  *out_red = value * red2 + (1.0 - value) * red1;
  *out_green = value * green2 + (1.0 - value) * green1;
  *out_blue = value * blue2 + (1.0 - value) * blue1;
}


static void
draw_cloud_point (TWorld *world,
                  gfloat *inout_red,
                  gfloat *inout_green,
                  gfloat *inout_blue,
                  gfloat  value,
                  gint    x,
                  gint    y,
                  gfloat  cloud_x,
                  gfloat  cloud_y,
                  gfloat  shadow_x,
                  gfloat  shadow_y)
{
  gfloat red, green, blue;
  gfloat point1, point2;
  gfloat offset_x, offset_y;

  offset_x = world->time * 0.25;
  offset_y = world->time * 0.33;

  point1 = t_perlin3d_get (world->clouds,
                           cloud_x + offset_x, cloud_y + offset_y,
                           world->time * 2.5);
  point2 = t_perlin3d_get (world->clouds,
                           shadow_x + offset_x, shadow_y + offset_y,
                           world->time * 2.5);

  /* Apply a threshold and exponentiation function */
  if (point1 < 0.525)
    point1 = 0.0;
  else
    point1 = pow ((point1 - 0.525) / (1.0 - 0.525), 0.4);

  if (point2 < 0.56)
    point2 = 0.0;
  else
    point2 = pow ((point2 - 0.56) / (1.0 - 0.56), 0.9);

  /* Apply shadow */
  interpolate (&red, &green, &blue, point2,
               world->cloud_color[0],
               world->cloud_color[1],
               world->cloud_color[2],
               world->shadow_color[0],
               world->shadow_color[1],
               world->shadow_color[2]);

  expose (inout_red, inout_green, inout_blue,
          MIN (value * point1, 1.0),
          red, green, blue);
}


static void
draw_sun (TWorld *world,
          gint    x,
          gint    y,
          gint    sky_height,
          gfloat  sun_size,
          gfloat  sun_red,
          gfloat  sun_green,
          gfloat  sun_blue,
          gfloat *inout_red,
          gfloat *inout_green,
          gfloat *inout_blue)
{
  gdouble dist;
  gdouble distance;

  dist = sqrt (world->width * world->width +
               world->height * world->height);

  distance = sqrt ((world->sun_x - x) * (world->sun_x - x) +
                   (world->sun_y - y) * (world->sun_y - y));

  if (distance < sun_size)
    {
      *inout_red = sun_red;
      *inout_green = sun_green;
      *inout_blue = sun_blue;
    }
  else if (distance < dist)
    {
      gfloat value;

      value = 1.0 - 1.0 /
              pow (200.0, (distance - sun_size) / (dist - sun_size));

      interpolate (inout_red, inout_green, inout_blue,
                   value, sun_red, sun_green, sun_blue,
                   *inout_red, *inout_green, *inout_blue);
   }
}


static void
world_render_sky (TWorld *world,
                  Region *region)
{
  gint    x, y;
  TVector ray, world_ray;
  TVector intersection;
  TVector intersection2;
  gfloat  scale;
  gfloat  offset;

  scale = 1.0 / 100.0;

  ray[2] = world->camera_distance;
  for (y = region->y; y < region->y + region->h; y++)
    {
      guint8 *data;

      ray[1] = (world->height - 1) - y;
      for (x = region->x; x < region->x + region->w; x++)
        {
          gboolean result1, result2;
          gfloat   red, green, blue;
          gdouble  sky_distance;
          gdouble  value;

          ray[0] = x - world->width * 0.5;
          t_matrix_transform (world->transform, ray, world_ray);
          t_vector_normalize (world_ray);

          result1 =
            sphere_intersect (world, world->planet_radius +
                                     world->cloud_height,
                              world_ray, intersection, &sky_distance);

          result2 =
            sphere_intersect (world, world->planet_radius +
                                     world->cloud_height * 0.9,
                              world_ray, intersection2, NULL);

          value = (intersection[1] - world->camera_location[1]) /
                  (world->planet_radius + world->cloud_height -
                   world->camera_location[1]);

          interpolate (&red, &green, &blue, pow (value, 200.0),
                       world->horizon_color[0],
                       world->horizon_color[1],
                       world->horizon_color[2],
                       world->sky_color[0],
                       world->sky_color[1],
                       world->sky_color[2]);

          if (world->sun_show)
            draw_sun (world, x, y,
                      world->height,
                      world->width / 35,
                      world->sun_color[0],
                      world->sun_color[1],
                      world->sun_color[2],
                      &red, &green, &blue);

          if (result1 == TRUE && result2 == TRUE)
            {
              /*
               * Add an offset to the x and z coordinates because
               *  Perlin noise does not accept negative numbers
               */
              offset = world->planet_radius + world->cloud_height;

              draw_cloud_point (world, &red, &green, &blue,
                pow (value, 100.0), x, y,
                (intersection[0] + offset) * scale,
                (intersection[2] + offset) * scale,
                (intersection2[0] + offset) * scale,
                (intersection2[2] + offset) * scale);
            }

          data = &region->data[(y - region->y) * region->rowstride +
                               (x - region->x) * region->bpp];
          set_fore (red, green, blue, world->gamma, data);

          if (region->bpp == 4)
            data[3] = 255;
        }
    }
}


static void
from_screen (gdouble *out,
             GimpRGB *rgb,
             gfloat   gamma)
{
  out[0] = FROMSCREEN (rgb->r);
  out[1] = FROMSCREEN (rgb->g);
  out[2] = FROMSCREEN (rgb->b);
}


static TWorld *
tworld_create (gint          width,
               gint          height,
               SkyValues    *values)
{
  TMatrix  temp1, temp2;
  TWorld  *world;
  gfloat   amplitudes[] = { 1.0, 0.5, 0.25, 0.125, 0.0625,
                            0.03125, 0.05, 0.05, 0.04, 0.0300 };

  world = g_new0 (TWorld, 1);
  world->width = width;
  world->height = height;
  world->clouds = t_perlin3d_new_full (10, 16.0, amplitudes, values->seed);
  world->planet_radius = EARTH_RADIUS;
  world->cloud_height = EARTH_CLOUD_HEIGHT;
  world->camera_distance = width * 0.5 /
                           tan (DEGRAD (LENS_ANGLE) * 0.5);
  world->sun_x = RINT ((width - 1) * values->sun_x);
  world->sun_y = RINT ((height - 1) * values->sun_y);
  world->sun_show = values->sun_show;
  world->time = values->time;
  from_screen (world->horizon_color, &values->horizon_color, values->gamma);
  from_screen (world->sky_color, &values->sky_color, values->gamma);
  from_screen (world->sun_color, &values->sun_color, values->gamma);
  from_screen (world->cloud_color, &values->cloud_color, values->gamma);
  from_screen (world->shadow_color, &values->shadow_color, values->gamma);

  t_matrix_rotate (temp1, values->tilt, 1);
  t_matrix_rotate (temp2, values->rotation, 2);
  t_matrix_combine (temp2, temp1, world->transform);
  t_vector_set (world->camera_location, 0.0, EARTH_RADIUS + 0.2, 0.0);
  world->gamma = values->gamma;

  return world;
}


static void
gimp_rgb_set (GimpRGB *rgb, gdouble r, gdouble g, gdouble b)
{
  rgb->r = r;
  rgb->g = g;
  rgb->b = b;
  rgb->a = 0.0;
}


void
sky_values_init (SkyValues *values,
                 gfloat     gamma)
{
  values->gamma = gamma;
  values->tilt = 0.0;
  values->rotation = 0.0;
  values->seed = -1;
  values->sun_show = TRUE;
  values->sun_x = 0.2;
  values->sun_y = 0.2;
  values->time = 0.0;
  gimp_rgb_set (&values->horizon_color,
    TOSCREEN (HAZE_RED), TOSCREEN (HAZE_GREEN), TOSCREEN (HAZE_BLUE));
  gimp_rgb_set (&values->sky_color,
    TOSCREEN (SKY_RED), TOSCREEN (SKY_GREEN), TOSCREEN (SKY_BLUE));
  gimp_rgb_set (&values->sun_color,
    TOSCREEN (SUN_RED), TOSCREEN (SUN_GREEN), TOSCREEN (SUN_BLUE));
  gimp_rgb_set (&values->cloud_color,
    TOSCREEN (CLOUD_RED), TOSCREEN (CLOUD_GREEN), TOSCREEN (CLOUD_BLUE));
  gimp_rgb_set (&values->shadow_color,
    TOSCREEN (SHADOW_RED), TOSCREEN (SHADOW_GREEN), TOSCREEN (SHADOW_BLUE));
}


#if 0
/* light_shader:
 *
 * data is of type gdouble[9]:
 * [0], [1], [2] = red, green, blue.  Values are between 0.0 and 1.0
 * [3], [4], [5] = Vector of parallel light source (must be normalized)
 * [6], [7], [8] = Ambient, Diffuse, Specular
 */

static void
light_shader (gpointer  data,
              gdouble  *normal,
              gdouble  *xyz,
              gdouble  *color)
{
  gdouble *solid = (gdouble*) data;
  gdouble *light = &solid[3];
  gdouble  dot;
  const gdouble ambient = solid[6];
  const gdouble diffuse = solid[7];
  const gdouble specular = solid[8];

  /* Ambient shading */
  color[0] = ambient * solid[0];
  color[1] = ambient * solid[1];
  color[2] = ambient * solid[2];

  /* Diffuse shading */
  dot = t_vector_dot_product (light, normal);
  if (dot >= 0.0)
    {
      dot = diffuse * dot * dot * dot * dot;
      color[0] += dot * solid[0];
      color[1] += dot * solid[1];
      color[2] += dot * solid[2];
    }

  /* Specular/phong shading */
  {
    TVector mirror;
    TVector camera;

    mirror[0] = -2.0 * dot * normal[0] + light[0];
    mirror[1] = -2.0 * dot * normal[1] + light[1];
    mirror[2] = -2.0 * dot * normal[2] + light[2];

    t_vector_set (camera, xyz[0], xyz[1], xyz[2]);
    t_vector_normalize (camera);
    t_vector_normalize (mirror);

    dot = t_vector_dot_product (camera, mirror);

    if (dot > 0.0)
      {
        dot = specular * pow (dot, 5.0);
        color[0] += dot;
        color[1] += dot;
        color[2] += dot;
      }
  }
}
#endif


/* terrain_shader: Color the terrain
 *
 * data is unused
 */

static void
terrain_shader (gpointer  data,
                gdouble  *normal,
                gdouble  *xyz,
                gdouble  *color)
{
  gdouble  ambient, diffuse, specular;
  gdouble *solid;
  gdouble  light[3] = { -0.57735, 0.57735, -0.57735 };
  gdouble  snow_color[3] = { 0.95, 0.95, 0.95 };
  gdouble  grass_color[3] = { 0.09, 0.3, 0.03 };
  gdouble  dirt_color[3] = { 0.27, 0.24, 0.1 };
  gdouble  dot;
  TVector  mirror;
  TVector  camera;

  /* Determine color of terrain */
  if (normal[1] >= 0.85)
    {
      /* Snow */
      ambient = 0.7;
      diffuse = 0.5;
      specular = 0.5;
      solid = snow_color;
    }
  else if (normal[1] <= 0.55)
    {
      /* Dirt */
      ambient = 0.3;
      diffuse = 0.7;
      specular = 0.0;
      solid = dirt_color;
    }
  else
    {
      /* Grass */
      ambient = 0.3;
      diffuse = 0.5;
      specular = 0.2;
      solid = grass_color;
    }

  /* Ambient shading */
  color[0] = ambient * solid[0];
  color[1] = ambient * solid[1];
  color[2] = ambient * solid[2];

  /* Diffuse shading */
  dot = t_vector_dot_product (light, normal);

  if (dot >= 0.0)
    {
      dot = dot * dot * dot * dot * diffuse;
      color[0] += dot * solid[0];
      color[1] += dot * solid[1];
      color[2] += dot * solid[2];
    }

  /* Specular/phong shading */
  mirror[0] = -2.0 * dot * normal[0] + light[0];
  mirror[1] = -2.0 * dot * normal[1] + light[1];
  mirror[2] = -2.0 * dot * normal[2] + light[2];

  t_vector_set (camera, xyz[0], xyz[1], xyz[2]);
  t_vector_normalize (camera);
  t_vector_normalize (mirror);

  dot = t_vector_dot_product (camera, mirror);

  if (dot > 0.0)
    {
      dot = pow (dot, 5.0) * specular;
      color[0] += dot;
      color[1] += dot;
      color[2] += dot;
    }
}


typedef struct _NativeTerrain NativeTerrain;

struct _NativeTerrain
{
  NativePaint  paint;
  TTerrain    *terrain;
};

/* get_random: Return a random number given two doubles used as seeds */
static gdouble
get_random (gdouble x, gdouble y)
{
  guint32 *a = (guint32*) &x;
  guint32 *b = (guint32*) &y;

  srand (((a[0] + a[1]) ^ 0x2f93a473) + ((b[0] + b[1]) ^ 0xfca814ed));

  return (rand () + 0.0) / RAND_MAX;
}


static gdouble
midpoint (gdouble  location_x,
          gdouble  location_y,
          gdouble  value1,
          gdouble  value2)
{
  gdouble rand;
  gdouble x, x2;
  gdouble mean, deviation;

  x = value1 + value2;
  x2 = SQR (value1) + SQR (value2);

  mean = x / 2.0;
  deviation = sqrt (x2 - x * x / 2.0);

  rand = get_random (location_x, location_y) - 0.5;

  return mean + rand * deviation;
}


static gboolean
split_triangle (gdouble xy_triangle[3][2])
{
  return fabs (xy_triangle[0][0] - xy_triangle[1][0]) >= 1.0 ||
         fabs (xy_triangle[1][0] - xy_triangle[2][0]) >= 1.0 ||
         fabs (xy_triangle[2][0] - xy_triangle[0][0]) >= 1.0 ||
         fabs (xy_triangle[0][1] - xy_triangle[1][1]) >= 1.0 ||
         fabs (xy_triangle[1][1] - xy_triangle[2][1]) >= 1.0 ||
         fabs (xy_triangle[2][1] - xy_triangle[0][1]) >= 1.0;
}


static void
terrain_square_render_b (NativeTerrain *terrain,
                         Region        *region,
                         gint           depth,
                         gdouble        min_x,
                         gdouble        max_x,
                         gdouble        min_y,
                         gdouble        max_y,
                         gdouble        a,
                         gdouble        b,
                         gdouble        c,
                         gdouble        d)
{
  NativePaint *state = &terrain->paint;
  gboolean     split;
  gdouble      points[4][3];
  gdouble      temp_xy[4][2];
  gint         i;

  if (depth == 0)
    return;

  points[0][0] = min_x - 0.5;
  points[0][1] = a;
  points[0][2] = 0.5 - min_y;
  points[1][0] = max_x - 0.5;
  points[1][1] = b;
  points[1][2] = 0.5 - min_y;
  points[2][0] = min_x - 0.5;
  points[2][1] = c;
  points[2][2] = 0.5 - max_y;
  points[3][0] = max_x - 0.5;
  points[3][1] = d;
  points[3][2] = 0.5 - max_y;

  for (i = 0; i < 4; i++)
    {
      gdouble temp_xyz[3];

      state->object_transform (state->object_transform_data,
                               points[i], temp_xyz);
      state->projection (state->projection_data, temp_xyz, temp_xy[i]);
    }

  /* If upper triangle doesn't fit a pixel, split square */
  split = split_triangle (&temp_xy[0]);

  /* If lower triangle doesn't fit a pixel, split square */
  if (split == FALSE)
    split = split_triangle (&temp_xy[1]);

  if (split)
    {
      /* Split square */

      gdouble h, i, j, k, l;
      gdouble mid_x, mid_y;
      gdouble rand;
      gdouble x, x2;
      gdouble mean, deviation;

      mid_x = (min_x + max_x) * 0.5;
      mid_y = (min_y + max_y) * 0.5;

      x = a + b + c + d;
      x2 = a * a + b * b + c * c + d * d;
      mean = x / 4.0;
      deviation = sqrt ((x2 - x * x / 4.0) / 3.0);
      rand = get_random (mid_x, mid_y) - 0.5;

      /*
       * ahb
       * ijk
       * cld
       */

      h = midpoint (mid_x, min_y, a, b);
      i = midpoint (min_x, mid_y, a, c);
      j = mean + rand * deviation;
      k = midpoint (max_x, mid_y, b, d);
      l = midpoint (mid_x, max_y, c, d);

      terrain_square_render_b (terrain, region, depth - 1,
                               min_x, mid_x, min_y, mid_y,
                               a, h, i, j);
      terrain_square_render_b (terrain, region, depth - 1,
                               mid_x, max_x, min_y, mid_y,
                               h, b, j, k);
      terrain_square_render_b (terrain, region, depth - 1,
                               min_x, mid_x, mid_y, max_y,
                               i, j, c, l);
      terrain_square_render_b (terrain, region, depth - 1,
                               mid_x, max_x, mid_y, max_y,
                               j, k, l, d);
    }
  else
    {
      /* Otherwise paint triangles */

      gdouble upper_xyz[3][3];
      gdouble lower_xyz[3][3];

      t_vector_copy (points[0], upper_xyz[0]);
      t_vector_copy (points[1], upper_xyz[1]);
      t_vector_copy (points[2], upper_xyz[2]);

      t_vector_copy (points[1], lower_xyz[0]);
      t_vector_copy (points[3], lower_xyz[1]);
      t_vector_copy (points[2], lower_xyz[2]);

      paint_triangle (state, region, upper_xyz);
      paint_triangle (state, region, lower_xyz);
    }
}


static void
terrain_square_render (NativeTerrain *state,
                       Region        *region,
                       gint           x,
                       gint           y)
{
  gfloat   *pos;
  TTerrain *terrain = state->terrain;

  pos = &terrain->heightfield[y * terrain->width + x];

  terrain_square_render_b (state, region, 14,
                           ((gdouble) x) / terrain->width,
                           ((gdouble) (x + 1)) / terrain->width,
                           ((gdouble) y) / terrain->height,
                           ((gdouble) (y + 1)) / terrain->height,
                           pos[0] * terrain->y_scale_factor,
                           pos[1] * terrain->y_scale_factor,
                           pos[terrain->width] * terrain->y_scale_factor,
                           pos[terrain->width + 1] * terrain->y_scale_factor);
}


static void
terrain_render (NativeTerrain *terrain,
                Region        *region)
{
  gint     x, y;

  for (y = 0; y < terrain->terrain->height - 1; y++)
    for (x = 0; x < terrain->terrain->width - 1; x++)
      terrain_square_render (terrain, region, x, y);
}


void
sky_render (TTerrain     *terrain,
            TPixbuf      *pixbuf,
            SkyValues    *values)
{
  TWorld   *world;
  Region    region;
  int       index;
  int       y;

  if (pixbuf->data == NULL)
    return;

  world = tworld_create (pixbuf->width, pixbuf->height, values);

  region.bpp = 3;
  region.x = 0;
  region.w = pixbuf->width;
  region.h = 1;
  region.rowstride = region.w * region.bpp;

  /* Render the sky background */
  for (y = 0; y < pixbuf->height; y++)
    {
      region.data = &pixbuf->data[region.rowstride * y];
      region.y = y;

      world_render_sky (world, &region);

      /* gtk_widget_queue_draw_area (GTK_WIDGET (drawing_area), 0, y, region.w, 1); */

      /* while (gtk_events_pending ()) */
      /*   gtk_main_iteration (); */
    }

  /* Draw the terrain */

  {
    NativeTerrain  terrain_state;
    NativePaint   *paint = &terrain_state.paint;
    TMatrix        matrix_1;
    gdouble        projection_data[4];

    {
      TMatrix rotate, translate;

      t_matrix_translate (translate, 0.0, -0.25, 4.0);
      t_matrix_rotate (rotate, 30.0, 1);
      t_matrix_combine (rotate, translate, matrix_1);
    }

    projection_data[0] = pixbuf->width;
    projection_data[1] = pixbuf->height;
    projection_data[2] = MIN (pixbuf->width, pixbuf->height) / 1.1;
    projection_data[3] = 5.0;

    terrain_state.terrain        = terrain;
    paint->gamma                 = values->gamma;
    paint->projection            = &perspective_projection;
    paint->object_transform      = &affine_transform;
    paint->shader_transform      = &affine_transform;
    paint->shader                = &terrain_shader;
    paint->projection_data       = &projection_data;
    paint->object_transform_data = matrix_1;
    paint->shader_transform_data = matrix_1;
    paint->shader_data           = NULL;

    region.data = pixbuf->data;
    region.y    = 0;
    region.h    = pixbuf->height;

    /* Initialize the Z buffer */
    region.zbuffer = g_new (gfloat, region.w * region.h);
    for (index = 0; index < region.w * region.h; index++)
      region.zbuffer[index] = 10e20;

    /* Paint terrain */
    terrain_render (&terrain_state, &region);

    g_free (region.zbuffer);
  }

  t_world_free (world);
}


void
sky_preview (TTerrain  *terrain,
             guint8    *data,
             guint      width,
             guint      height,
             SkyValues *values)
{
  TWorld *world;
  Region  region;

  world = tworld_create (width, height, values);

  region.data = data;
  region.bpp = 3;
  region.rowstride = width * region.bpp;
  region.x = 0;
  region.y = 0;
  region.w = width;
  region.h = height;

  world_render_sky (world, &region);

  t_world_free (world);
}
