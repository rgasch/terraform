/* Terraform - (C) 1997-2002 Robert Gasch (r.gasch@chello.nl)
 *  - http://terraform.sourceforge.net
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
 *  This file copyright (c) 2001 David A. Bartold
 */

#include <math.h>
#include "native3d.h"

#define CLIP(x,min,max) MAX (MIN (x, max), min)

/* Clockwise winding means out */

/* (u, v) => (x, y, z) => (x, y, z) => (x, y, z) => (x, y) */
/*      object       deform      transform    projection   */

static void
draw_triangle (Region  *region,
               int      x,
               int      y,
               gdouble  z,
               gdouble *color,
               gdouble  gamma)
{
  int data_index, zbuffer_index;

  if (x < region->x ||
      x >= region->x + region->w ||
      y < region->y ||
      y >= region->y + region->h)
    return;

  data_index = region->rowstride * (y - region->y) +
               (x - region->x) * region->bpp;
  zbuffer_index = region->w * (y - region->y) + (x - region->x);

  if (z < region->zbuffer[zbuffer_index])
    {
      region->zbuffer[zbuffer_index] = z;
      region->data[data_index + 0] = (int) RINT (TOSCREEN (CLIP (color[0], 0.0, 1.0)) * 255);
      region->data[data_index + 1] = (int) RINT (TOSCREEN (CLIP (color[1], 0.0, 1.0)) * 255);
      region->data[data_index + 2] = (int) RINT (TOSCREEN (CLIP (color[2], 0.0, 1.0)) * 255);
    }
}

void
paint_triangle (NativePaint  *state,
                Region       *region,
                gdouble       triangle[3][3])
{
  int     index;
  gdouble normal[3];
  gdouble color[3];
  gdouble xy_point[2];
  gdouble shader_xyz[3];
  gdouble object_xyz[3];
  gdouble z;
  gdouble vector1[3];
  gdouble vector2[3];
  gdouble xyz[3];

  /* Find an average xyz point */
  for (index = 0; index < 3; index++)
    xyz[index] = (triangle[0][index] +
                  triangle[1][index] +
                  triangle[2][index]) / 3.0;

  state->shader_transform (state->shader_transform_data,
                           xyz, shader_xyz);

  state->object_transform (state->object_transform_data,
                           xyz, object_xyz);

  /* Compute the normal */
  for (index = 0; index < 3; index++)
    {
      vector1[index] = triangle[1][index] - triangle[0][index];
      vector2[index] = triangle[2][index] - triangle[0][index];
    }

  t_vector_cross_product (vector1, vector2, normal);
  t_vector_normalize (normal);

  state->shader (state->shader_data, normal, shader_xyz, color);

  state->projection (state->projection_data, object_xyz, xy_point);

  /* Compute the distance (squared) */
  z = object_xyz[0] * object_xyz[0] +
      object_xyz[1] * object_xyz[1] +
      object_xyz[2] * object_xyz[2];

  draw_triangle (region,
                 (int) RINT (xy_point[0]),
                 (int) RINT (xy_point[1]),
                 z,
                 color,
                 state->gamma);
}

static void
point2_copy (gdouble *in, gdouble *out)
{
  out[0] = in[0];
  out[1] = in[1];
}

static void
process_triangle (Region       *region,
                  NativeObject *object,
                  gdouble       uv_triangle[3][2],
                  int           depth)
{
  NativePaint *state = &object->paint;
  gdouble      transformed[3][3];
  gdouble      xy_triangle[3][2];
  gdouble      xyz_triangle[3][3];
  int          point;
  int          behind_camera_count;
  gboolean     split_triangle;

  /* Don't recurse too deeply */
  if (depth == 0)
    return;

  behind_camera_count = 0;
  for (point = 0; point < 3; point++)
    {
      object->object (object->object_data, uv_triangle[point],
                     xyz_triangle[point]);
      state->object_transform (state->object_transform_data,
                               xyz_triangle[point], transformed[point]);

      /* Ignore points behind camera */
      if (transformed[point][2] <= EPSILON)
        behind_camera_count++;
      else
        state->projection (state->projection_data,
                           transformed[point], xy_triangle[point]);
    }

  split_triangle = 0;

  /* Case 1: All points are behind the camera.  Discard triangle. */
  if (behind_camera_count == 3)
    return;

  /* Case 2: Some points are behind the camera.  Split triangle. */
  if (behind_camera_count != 0)
    split_triangle = 1;

  /* Case 3: No points are behind the camera.  Discard triangle
   * if it doesn't intersect the screen.  Split the triangle
   * if the onscreen triangle is greater than a pixel.
   */
  if (behind_camera_count == 0)
    {
      int min_x, min_y, max_x, max_y;

      /* Find onscreen bounds of triangle */
      min_x = (int) floor (xy_triangle[0][0]);
      max_x = (int) ceil (xy_triangle[0][0]);
      min_y = (int) floor (xy_triangle[0][1]);
      max_y = (int) ceil (xy_triangle[0][1]);
      for (point = 1; point < 3; point++)
        {
          int small_x, small_y, big_x, big_y;

          small_x = (int) floor (xy_triangle[point][0]);
          big_x = (int) ceil (xy_triangle[point][0]);
          small_y = (int) floor (xy_triangle[point][1]);
          big_y = (int) ceil (xy_triangle[point][1]);

          if (small_x < min_x) min_x = small_x;
          if (big_x > max_x) max_x = big_x;
          if (small_y < min_y) min_y = small_y;
          if (big_y > max_y) max_y = big_y;
        }

      /* Discard triangle if it doesn't intersect screen. */
      if (min_x > region->x + region->w - 1 || max_x < region->x ||
          min_y > region->y + region->h - 1 || max_y < region->y)
        return;

      /* If triangle is bigger than a pixel, split it. */
      if (fabs (xy_triangle[0][0] - xy_triangle[1][0]) >= 1.0 ||
          fabs (xy_triangle[1][0] - xy_triangle[2][0]) >= 1.0 ||
          fabs (xy_triangle[2][0] - xy_triangle[0][0]) >= 1.0 ||
          fabs (xy_triangle[0][1] - xy_triangle[1][1]) >= 1.0 ||
          fabs (xy_triangle[1][1] - xy_triangle[2][1]) >= 1.0 ||
          fabs (xy_triangle[2][1] - xy_triangle[0][1]) >= 1.0)
        split_triangle = 1;
    }

  if (split_triangle)
    {
      gdouble uv_midpoints[3][2];
      gdouble uv_subtriangle[3][2];
      int     index;

      /* Find midpoints of a triangle's edges */
      for (index = 0; index < 2; index++)
        {
          uv_midpoints[0][index] =
            (uv_triangle[0][index] + uv_triangle[1][index]) * 0.5;
          uv_midpoints[1][index] =
            (uv_triangle[1][index] + uv_triangle[2][index]) * 0.5;
          uv_midpoints[2][index] =
            (uv_triangle[2][index] + uv_triangle[0][index]) * 0.5;
        }

      point2_copy (uv_midpoints[0], uv_subtriangle[0]);
      point2_copy (uv_midpoints[2], uv_subtriangle[1]);
      point2_copy (uv_triangle[0], uv_subtriangle[2]);

      process_triangle (region, object, uv_subtriangle, depth - 1);

      point2_copy (uv_triangle[1], uv_subtriangle[1]);
      point2_copy (uv_midpoints[1], uv_subtriangle[2]);

      process_triangle (region, object, uv_subtriangle, depth - 1);

      point2_copy (uv_midpoints[2], uv_subtriangle[0]);
      point2_copy (uv_midpoints[0], uv_subtriangle[1]);

      process_triangle (region, object, uv_subtriangle, depth - 1);

      point2_copy (uv_midpoints[1], uv_subtriangle[1]);
      point2_copy (uv_triangle[2], uv_subtriangle[2]);

      process_triangle (region, object, uv_subtriangle, depth - 1);
    }
  else
    {
      paint_triangle (state, region, xyz_triangle);
    }
}

void
paint_object (Region       *region,
              NativeObject *object)
{
  const int max_depth = 12;
  const int u_divisions = 10;
  const int v_divisions = 10;
  gdouble   uv_triangle[3][2]; /* 3 points of (u, v) pairs */
  int       u_index, v_index;

  for (u_index = 0; u_index < u_divisions; u_index++)
    {
      for (v_index = 0; v_index < v_divisions; v_index++)
        {
          gdouble u_min = ((gdouble) u_index) / u_divisions;
          gdouble u_max = ((gdouble) (u_index + 1)) / u_divisions;
          gdouble v_min = ((gdouble) v_index) / v_divisions;
          gdouble v_max = ((gdouble) (v_index + 1)) / v_divisions;

          uv_triangle[0][0] = u_max;
          uv_triangle[0][1] = v_min;

          uv_triangle[1][0] = u_min;
          uv_triangle[1][1] = v_max;

          uv_triangle[2][0] = u_min;
          uv_triangle[2][1] = v_min;

          process_triangle (region, object, uv_triangle, max_depth);

          uv_triangle[1][0] = u_max;
          uv_triangle[1][1] = v_max;

          uv_triangle[2][0] = u_min;
          uv_triangle[2][1] = v_max;

          process_triangle (region, object, uv_triangle, max_depth);
        }
    }
}


/* sphere: Draw a unit sphere at the origin
 *
 * data is unused
 */

void
sphere (gpointer  data,
        gdouble  *uv,
        gdouble  *xyz)
{
  gdouble size;

  xyz[1] = uv[0] * 2.0 - 1.0;
  size = sqrt (1.0 - xyz[1] * xyz[1]);
  xyz[0] = cos (uv[1] * 2.0 * G_PI) * size;
  xyz[2] = sin (uv[1] * 2.0 * G_PI) * size;
}


/* plane: Draw a unit plane on the xz axis around the origin
 *
 * data is unused
 */

void
plane (gpointer  data,
       gdouble  *uv,
       gdouble  *xyz)
{
  xyz[0] = uv[0] - 0.5;
  xyz[1] = 0.0;
  xyz[2] = uv[1] - 0.5;
}


/* isomorphic_projection:
 *
 * data is of type gdouble[3]:
 * [0] = Screen width
 * [1] = Screen height
 * [2] = Screen scale (typically it's the minimum of [0] and [1])
 */

void
isomorphic_projection (gpointer  data,
                       gdouble  *xyz,
                       gdouble  *xy)
{
  gdouble *info = (gdouble*) data;

  xy[0] = 0.5 * info[0] + xyz[0] * info[2];
  xy[1] = 0.5 * info[1] - xyz[1] * info[2];
}


/* perspective_projection:
 *
 * data is of type gdouble[4]:
 * [0] = Screen width
 * [1] = Screen height
 * [2] = Screen scale (typically it's the minimum of [0] and [1])
 * [3] = Perspective distance (corresponds to camera angle)
 */

void
perspective_projection (gpointer  data,
                        gdouble  *xyz,
                        gdouble  *xy)
{
  gdouble *info = (gdouble*) data;
  gdouble  x, y;

  x = xyz[0] * info[3] / xyz[2];
  y = xyz[1] * info[3] / xyz[2];

  xy[0] = 0.5 * info[0] + x * info[2];
  xy[1] = 0.5 * info[1] - y * info[2];
}


/* affine_transform:
 *
 * data is of type TMatrix: the matrix used for the transform
 */

void
affine_transform (gpointer  data,
                  gdouble  *xyz1,
                  gdouble  *xyz2)
{
  t_matrix_transform (data, xyz1, xyz2);
}


/* solid_shader:
 *
 * data is of type gdouble[3]:
 * [0], [1], [2] = red, green, blue.  Values are between 0.0 and 1.0
 */

void
solid_shader (gpointer  data,
              gdouble  *normal,
              gdouble  *xyz,
              gdouble  *color)
{
  gdouble *solid = (gdouble*) data;

  color[0] = solid[0];
  color[1] = solid[1];
  color[2] = solid[2];
}
