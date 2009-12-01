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
 */


#include <math.h>
#include <glib.h>
#include "gensubdiv.h"
#include "filters.h"
#include "trandom.h"

#define EMPTY 0.0

#define STILL_NORMAL    1
#define STILL_SEAMLESS  0

static gint
preferred_size (gint n)
{
  gint v;

  for (v = 1; v < n; v <<= 1);

  return v;
}

TTerrain *
t_terrain_generate_subdiv (gint   method,
                           gint   size,
                           gfloat scale,
                           gint   seed)
{
  return t_terrain_generate_subdiv_seed (method, size, scale, seed, NULL, -1, -1, -1, -1);
}


TTerrain *
t_terrain_generate_subdiv_seed (gint      method,
                                gint      n,
                                gfloat    scale,
                                gint      seed,
				TTerrain *torg,
				gint      sel_x1,
				gint      sel_y1,
				gint      sel_w, 
				gint      sel_h)
{
  return t_terrain_generate_subdiv_seed_ns (method, n, n, scale, seed, 
		                            torg, sel_x1, sel_y1, sel_w, sel_h);
}


/*
 * t_terrain_generate_subdiv_seed_ns: generate to non_square size
 * Even though we only generate square terrains we're passing size_x and 
 * size_y here so that we can return an appropriately sized terrain 
 * when we zoom
 */
TTerrain *
t_terrain_generate_subdiv_seed_ns (gint      method,
                                   gint      size_x,
                                   gint      size_y,
                                   gfloat    scale,
                                   gint      seed,
				   TTerrain *torg,
				   gint      sel_x1,
				   gint      sel_y1,
				   gint      sel_w, 
				   gint      sel_h)
{
  GtkObject *terrain_object = NULL;
  TTerrain  *terrain = NULL;
  TRandom   *gauss;
  gboolean   doinit = TRUE;
  int        size = preferred_size (MAX(size_x, size_y));

  if (torg != NULL)
    {
    int lim=size*size;
    int rc; 
    int i;

    g_return_val_if_fail (torg != NULL, NULL);
    g_return_val_if_fail (sel_w > 10, NULL);
    g_return_val_if_fail (sel_h > 10, NULL);

    terrain = torg;
    rc = t_terrain_seed (terrain, size, size, sel_x1, sel_y1, sel_w, sel_h);

    if (rc == 0)
      {
      doinit = FALSE;

      // scale seeded points
      for (i=0; i<lim; i++)
        if (terrain->heightfield[i] != EMPTY)
          terrain->heightfield[i] *= size;
      }
    else
      {
      return torg;
      }
    }
  else
    {
    terrain_object = t_terrain_new (size, size);
    terrain = T_TERRAIN (terrain_object);
    }

  gauss = t_random_new (seed);

  switch (method)
    {
      case 0: /* Recursive Square */
        terrain = t_terrain_generate_recursive_square (terrain, gauss, size, scale, doinit);
        break;

      case 1: /* Recursive Diamond */
        terrain = t_terrain_generate_recursive_diamond (terrain, gauss, size, scale, doinit);
        break;

      case 2: /* Recursive Plasma */
        terrain = t_terrain_generate_recursive_plasma (terrain, gauss, size, scale, doinit);
        break;

      case 3: /* Offset */
        terrain = t_terrain_generate_offset (terrain, gauss, size, scale);
        break;

      case 4: /* Midpoint */
        terrain = t_terrain_generate_midpoint (terrain, gauss, size, scale);
        break;

      case 5: /* Diamond square */
        terrain = t_terrain_generate_diamond_square (terrain, gauss, size, scale);
        break;

      default:
        terrain = NULL;
    }

  t_random_free (gauss);

  if (size != size_x || size != size_y)
    {
    int off_x = (size - size_x)/2;
    int off_y = (size - size_y)/2;

    t_terrain_crop (terrain, off_x, off_y, 
		    size_x + off_x - 1, size_y + off_y - 1);
    }

  t_terrain_normalize (terrain, FALSE);
  t_terrain_set_modified (terrain, TRUE);

  return terrain;
}

static void
process_recursive_square (TTerrain *terrain,
                          TRandom *gauss,
                          gint x1, gint y1, gint x2, gint y2, 
                          gfloat max_delta, gfloat scale_factor)
{
  gint    cx = (x1 + x2) / 2;      /* center   */
  gint    cy = (y1 + y2) / 2;
  gint    dx = ABS (x2 - x1);      /* distance */
  gint    dy = ABS (y2 - y1);
  gint    pos;

  gfloat *data = terrain->heightfield;
  gint    width = terrain->width;

  /* square has reached minimum size; we're done */
  if (dx <= 1 && dy <= 1)
    return;

  /* center point */
  data[cy * width + cx] =
    (data[y1 * width + x1] + data[y2 * width + x1] +
     data[y2 * width + x2] + data[y1 * width + x2]) * 0.25 +
     t_random_gauss (gauss) * max_delta;

  /* process vertex points which haven't been touched yet */
  pos = cy * width + x1;
  if (data[pos] == EMPTY)
    data[pos] = (data[y1 * width + x1] + data[y2 * width + x1]) * 0.5 +
                t_random_gauss (gauss) * max_delta;

  pos = y1 * width + cx;
  if (data[pos] == EMPTY)
    data[pos] = (data[y1 * width + x1] + data[y1 * width + x2]) * 0.5 +
                t_random_gauss (gauss) * max_delta;

  pos = y2 * width + cx;
  if (data[pos] == EMPTY)
    data[pos] = (data[y2 * width + x1] + data[y2 * width + x2]) * 0.5 +
                t_random_gauss (gauss) * max_delta;

  pos = cy * width + x2;
  if (data[pos] == EMPTY)
    data[pos] = (data[y1 * width + x2] + data[y2 * width + x2]) * 0.5 +
                t_random_gauss (gauss) * max_delta;

  /* process subsquares recursively */
  process_recursive_square (terrain, gauss, x1, y1, cx, cy,
                            max_delta * scale_factor, scale_factor);
  process_recursive_square (terrain, gauss, cx, y1, x2, cy,
                            max_delta * scale_factor, scale_factor);
  process_recursive_square (terrain, gauss, x1, cy, cx, y2,
                            max_delta * scale_factor, scale_factor);
  process_recursive_square (terrain, gauss, cx, cy, x2, y2,
                            max_delta * scale_factor, scale_factor);
}

TTerrain *
t_terrain_generate_recursive_square (TTerrain *terrain, 
		                     TRandom  *gauss, 
		                     gint      size,
                                     gfloat    scale_factor,
				     gboolean  doinit)
{ 
  int        center;
  float      init_delta;
  gfloat    *data;

  data = terrain->heightfield;
  init_delta = size;
  center = (size - 1) >> 1;

  if (doinit)
    {
    data[0] = t_random_gauss (gauss) * init_delta;
    data[center + center * size] = t_random_gauss (gauss) * init_delta;
    data[center * size] = t_random_gauss (gauss) * init_delta;
    data[center] = t_random_gauss (gauss) * init_delta;
    data[center + (size - 1) * size] = t_random_gauss (gauss) * init_delta;
    data[(size - 1) + center * size] = t_random_gauss (gauss) * init_delta;
    data[size * size - 1] = t_random_gauss (gauss) * init_delta;
    }

  // process subsquares recursively
  process_recursive_square (terrain, gauss,
                            0, 0, center, center,
                            init_delta, scale_factor);
  process_recursive_square (terrain, gauss,
                            center, 0, size - 1, center,
                            init_delta, scale_factor);
  process_recursive_square (terrain, gauss,
                            0, center, center, size - 1,
                            init_delta, scale_factor);
  process_recursive_square (terrain, gauss,
                            center, center, size - 1, size - 1,
                            init_delta, scale_factor);

  return terrain;
}

static void
random_dot_recursive_diamond (TTerrain *terrain,
                              TRandom  *gauss,
                              gint      x1,
                              gint      y1,
                              gint      x2,
                              gint      y2,
                              gfloat    max_delta)
{
  gint    cx = (x1 + x2) / 2;
  gint    cy = (y1 + y2) / 2;
  gint    noise = 20;
  gfloat *data;

  data = terrain->heightfield;

  /* We don't want to overwrite anything. If you want to know why,
   * just remove the if statement and see what happens. It won't
   * kill your computer, I swear.
   */
  if (data[cy * terrain->width + cx] == EMPTY)
    {
      double dist, average;

      /* Calculate the distance between the two points. */
      dist = sqrt ((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1));

      /* Multiply with the roughness factor. */
      dist = sqrt (dist * noise);

      /* Get the average value. */
      average = (data [y1 * terrain->width + x1] +
                 data [y2 * terrain->width + x2]) * 0.5;

      /* Add random disturbance. */
      average += t_random_gauss (gauss) * max_delta;

      data[cy * terrain->width + cx] = average;
    }
}

/*
 *  process_recursive_diamond: process a given square recursiveley
 *  Xs are the existing points. 1-5 are the points we're going
 *   to add, and A-D are the four new quadrants.
 *
 *    X   1   X
 *      A   B
 *    2   5   3
 *      C   D
 *    X   4   X
 */
static void
process_recursive_diamond (TTerrain *terrain,
                           TRandom  *gauss,
                           gint      x1,
                           gint      y1,
                           gint      x2,
                           gint      y2,
                           gfloat    max_delta,
                           gfloat    scale_factor)
{
  gint cx = (x1 + x2) >> 1;
  gint cy = (y1 + y2) >> 1;

  /* Add the points */
  random_dot_recursive_diamond (terrain, gauss, x1, y1, x2, y1,
                                max_delta); /* 1 */
  random_dot_recursive_diamond (terrain, gauss, x1, y1, x1, y2,
                                max_delta); /* 2 */
  random_dot_recursive_diamond (terrain, gauss, x2, y1, x2, y2,
                                max_delta); /* 3 */
  random_dot_recursive_diamond (terrain, gauss, x1, y2, x2, y2,
                                max_delta); /* 4 */

  /* 5th/center point */
  switch ((int)(t_random_gauss (gauss) * 2.0))
    {
      case 0:
        random_dot_recursive_diamond (terrain, gauss, x1, y1, x2, y2,
                                      max_delta);
        break;

      default:
        random_dot_recursive_diamond (terrain, gauss, x1, y2, x2, y1,
                                      max_delta);
        break;
    }

  /* If we haven't hit the bottom yet, recurse. */
  if (x2 - x1 > 2)
    {
      process_recursive_diamond (terrain, gauss, x1, y1, cx, cy,
                                 max_delta * scale_factor,
                                 scale_factor); /* A */
      process_recursive_diamond (terrain, gauss, cx, y1, x2, cy,
                                 max_delta *scale_factor,
                                 scale_factor); /* B */
      process_recursive_diamond (terrain, gauss, x1, cy, cx, y2,
                                 max_delta * scale_factor,
                                 scale_factor); /* C */
      process_recursive_diamond (terrain, gauss, cx, cy, x2, y2,
                                 max_delta * scale_factor,
                                 scale_factor); /* D */
    }
}

TTerrain *
t_terrain_generate_recursive_diamond (TTerrain *terrain, 
		                      TRandom  *gauss, 
		                      gint     size,
                                      gfloat   scale_factor,
                                      gboolean doinit)
{
  int        center;
  float      init_delta;
  gfloat    *data;

  data = terrain->heightfield;
  init_delta = size;
  center = (size - 1) >> 1;

  if (doinit)
    {
    data[0] = t_random_gauss (gauss) * init_delta;
    data[(size - 1) * size] = t_random_gauss (gauss) * init_delta;
    data[size - 1] = t_random_gauss (gauss) * init_delta;
    data[(size - 1) * size + (size - 1)] = t_random_gauss (gauss) * init_delta;
    }

  process_recursive_diamond (terrain, gauss, 0, 0, size - 1, size - 1,
                             init_delta, scale_factor);

  return terrain;
}

static void
process_recursive_plasma (TTerrain *terrain,
                          TRandom  *gauss,
                          gint      x1,
                          gint      y1,
                          gint      x2,
                          gint      y2,
                          gfloat    max_delta,
                          gfloat    scale_factor,
                          gint      type)
{
  gint    cx, cy;
  gint    width, height;
  gint    last_line;
  gfloat *data;

  data = terrain->heightfield;
  width = terrain->width;
  height = terrain->height;
  cx = (x1 + x2) / 2;
  cy = (y1 + y2) / 2;
  last_line = (height - 1) * width;

  if (cx > x1)
    {
      if (data[y1 * width + x1] == EMPTY)
        {
          data[y1 * width + cx] =
            (data[y1 * width + x2] + data[y1 * width + x2]) * 0.5 +
            t_random_gauss (gauss) * max_delta;

          if (y1 == 0)
            if (type == STILL_SEAMLESS)
              data[last_line + cx] = data[y1 * width + cx];
        }

      if (y1 < y2 && data[y2 * width + cx] == EMPTY)
        data[y2 * width + cx] =
          (data[y2 * width + x1] + data[y2 * width + x2]) * 0.5 +
          t_random_gauss (gauss) * max_delta;
    }

  if (cy > y1) 
    {
      if (data[cy * width + x1] == EMPTY)
        {
          data[cy * width + x1] =
            (data[y1 * width + x1] + data[y2 * width + x1]) * 0.5 +
            t_random_gauss (gauss) * max_delta;

          if (x1 == 0)
            if (type == STILL_SEAMLESS)
              data[cy * width + (width - 1)] = data[cy * width + x1];
        }

      if (x1 < x2 && data[cy * width + x2] == EMPTY)
        data[cy * width + x2] =
          (data[y1 * width + x2] + data[y2 * width + x2]) * 0.5 +
          t_random_gauss (gauss) * max_delta;
    }

  if (cx > x1 && cy > y1)
    {
      if (data[cy * width + cx] == EMPTY)
        data[cy * width + cx] =
          (data[y1 * width + x1] + data[y2 * width + x1] +
           data[y1 * width + x2] + data[y2 * width + x2]) * 0.25 +
           t_random_gauss (gauss) * ((y2 - y1) + (x2 - x1));
    }

  if ((cx > x1 && cx < x2) || (cy > y1 && cy < y2))
    {
      process_recursive_plasma (terrain, gauss, x1, y1, cx, cy,
                                max_delta * scale_factor, scale_factor, type);
      process_recursive_plasma (terrain, gauss, x1, cy, cx, y2,
                                max_delta * scale_factor, scale_factor, type);
      process_recursive_plasma (terrain, gauss, cx, y1, x2, cy,
                                max_delta * scale_factor, scale_factor, type);
      process_recursive_plasma (terrain, gauss, cx, cy, x2, y2,
                                max_delta * scale_factor, scale_factor, type);
    }
}

/*
 *  gereateRecursivePlasma: subdivide and assign
 *      taken from an old Usenet posting, original author unknown
*/

TTerrain *
t_terrain_generate_recursive_plasma (TTerrain *terrain,
		                     TRandom  *gauss, 
				     gint     size,
                                     gfloat   scale_factor,
                                     gboolean doinit)
{
  float      init_delta = size;
  gfloat    *data = terrain->heightfield;
  gint       type = STILL_NORMAL;  /* Type can be either STILL_NORMAL or STILL_SEAMLESS */


  if (doinit)
    {
    data[0] = t_random_gauss (gauss) * init_delta;
    data[size - 1] = t_random_gauss (gauss) * init_delta;
    data[(size - 1) * terrain->width] = t_random_gauss (gauss) * init_delta;
    data[(size - 1) * terrain->width + (size - 1)] = t_random_gauss (gauss) * init_delta;
    }

  /* Call recursive function */
  process_recursive_plasma (terrain, gauss, 0, 0, size - 1, size - 1,
                            init_delta, scale_factor, type);

  return terrain;
}

/**
 * Plasma example - offset square midpoint subdivision on a grid.
 * This method is similar to standard subdivision, but it is 'offset'
 * in that the new points at each level are offset from the previous
 * level by half the square size.  The original corner points at level
 * x are only used once to form a weighted average, and then become the
 * center points of the new squares:
 *
 * It should be clear that this method includes some non-local context
 * when calculating new heights.  With standard subdivision, the only
 * heights that can ever influence the area inside the original square
 * are the original corner points.  With offset squares, most of the
 * new corner points lie outside the original square and therefore
 * are influenced by more distant points.  This feature can be a problem,
 * because if you don't want the map to be toroidal (wrapped in both
 * x and y), you need to generate a large number of points outside the
 * final terrain area.  For this example, I just wrap x and y.
 *
 * Original copyright notice:
 * This code was inspired by looking at Tim Clark's (?) Mars demo. You are 
 * free to use my code, my ideas, whatever. I have benefited enormously from 
 * the free information on the Internet, and I would like to keep that 
 * process going. James McNeill (mcneja@wwc.edu)
 */

TTerrain *
t_terrain_generate_offset (TTerrain *terrain,
		           TRandom  *gauss, 
		           gint      size,
                           gfloat    scale_factor)
{
  gint       x1, y1; 
  gint       square_size; 
  gint       row_offset;
  gfloat     max_delta = size;
  gint       width = terrain->width;
  gfloat    *data = terrain->heightfield;;

  row_offset = 0;  /* start at zero for first row */
  for (square_size = size; square_size > 1; square_size /= 2)
    {
      for (x1 = row_offset; x1 < size; x1 += square_size)
        {
          for (y1 = row_offset; y1 < size; y1 += square_size)
            {
              // Get the four corner points.
              gint x2 = (x1 + square_size) & (size - 1);
              gint y2 = (y1 + square_size) & (size - 1);
              gint x3, y3;

              gfloat i1 = data[y1 * width + x1];
              gfloat i2 = data[y1 * width + x2];
              gfloat i3 = data[y2 * width + x1];
              gfloat i4 = data[y2 * width + x2];

              /* Obtain new points by averaging the corner points. */
              gfloat p1 = ((i1 * 9) + (i2 * 3) + (i3 * 3) + (i4)) / 16;
              gfloat p2 = ((i1 * 3) + (i2 * 9) + (i3) + (i4 * 3)) / 16;
              gfloat p3 = ((i1 * 3) + (i2) + (i3 * 9) + (i4 * 3)) / 16;
              gfloat p4 = ((i1) + (i2 * 3) + (i3 * 3) + (i4 * 9)) / 16;
    
              /* Add a random offset to each new point. */
              p1 += t_random_gauss (gauss) * max_delta;
              p2 += t_random_gauss (gauss) * max_delta;
              p3 += t_random_gauss (gauss) * max_delta;
              p4 += t_random_gauss (gauss) * max_delta;
    
              // Write out the generated points.
              x3 = (x1 + square_size / 4) & (size - 1);
              y3 = (y1 + square_size / 4) & (size - 1);
              x2 = (x3 + square_size / 2) & (size - 1);
              y2 = (y3 + square_size / 2) & (size - 1);
    
              data[y3 * width + x3] = p1;
              data[y3 * width + x2] = p2;
              data[y2 * width + x3] = p3;
              data[y2 * width + x2] = p4;
            }
        }
      row_offset = square_size / 4;  /* set offset for next row */
      max_delta *= scale_factor;
    }

  return terrain;
}

TTerrain *
t_terrain_generate_midpoint (TTerrain *terrain,
		             TRandom  *gauss, 
			     gint      size,
                             gfloat    scale_factor)
{
  gint       i, j, i2, j2, i3, j3, h, h2;
  gfloat     a, b, c, d;
  gfloat     max_delta = size;
  gint       width = terrain->width;
  gfloat    *data = terrain->heightfield;

  for (h = size; h > 1; h = h2)
    {
      h2 = h / 2;
      for (i = 0; i < size; i += h)
        {
          i2 = ((i + h) & (size - 1));
          for (j = 0; j < size; j += h)
            {
              j2 = ((j + h) & (size - 1));
              i3 = i + h2; 
              j3 = j + h2;

              a = data[j * width + i];
              b = data[j * width + i2];
              c = data[j2 * width + i];
              d = data[j2 * width + i2];

              /* center */
              data[j3 * width + i3] =
                (a + b + c + d) * 0.25 +
                t_random_gauss (gauss) * max_delta;

              /* top, left, right, bottom */
              if (i == 0)
                data[j * width + i3] =
                  (a + b) * 0.5 +
                  t_random_gauss (gauss) * max_delta;

              if (j == 0)
                data[j3 * width + i] =
                  (a + c) * 0.5 +
                  t_random_gauss (gauss) * max_delta;

              data[j3 * width + i2] =
                (b + d) * 0.5 +
                t_random_gauss (gauss) * max_delta;

              data[j2 * width + i3] =
                (c + d) * 0.5 +
                t_random_gauss (gauss) * max_delta;
            }
        }

      max_delta *= scale_factor;
    }

  return terrain;
}

/*
 *  avg: used by Diamond generation routine
 */
static float
avg (TTerrain *terrain,
     gint      x,
     gint      y,
     gint      strut)
{
  gfloat *data = terrain->heightfield;
  gint    width = terrain->width;
  gint    mask = width - 1;
  gint    height = terrain->height;

  if (x == 0)
    return (data[((y - strut) & mask) * width + x] +
            data[((y + strut) & mask) * width + x] +
            data[y * width + ((x + strut) & mask)]) / 3.0;
  else if (x == width - 1)
    return (data[((y - strut) & mask) * width + x] +
            data[((y + strut) & mask) * width + x] +
            data[y * width + ((x - strut) & mask)]) / 3.0;
  else if (y == 0)
    return (data[y * width + ((x - strut) & mask)] +
            data[y * width + ((x + strut) & mask)] +
            data[((y + strut) & mask) * width + x]) / 3.0;
  else if (y == height - 1)
    return (data[y * width + ((x - strut) & mask)] +
            data[y * width + ((x + strut) & mask)] +
            data[((y - strut) & mask) * width + x]) / 3.0;
  else
    return (data[y * width + ((x - strut) & mask)] +
            data[y * width + ((x + strut) & mask)] +
            data[((y - strut) & mask) * width + x] +
            data[((y + strut) & mask) * width + x]) / 4.0;
}

/*
 *  avg2: used by Diamond generation routine
 */
static float
avg2 (TTerrain *terrain, int i, int j, int strut)
{
  int     tstrut = strut / 2;
  int     width = terrain->width;
  int     mask = width - 1;
  gfloat *data = terrain->heightfield;

  return (data[((j - tstrut) & mask) * width + ((i - tstrut) & mask)] +
          data[((j + tstrut) & mask) * width + ((i - tstrut) & mask)] +
          data[((j - tstrut) & mask) * width + ((i + tstrut) & mask)] +
          data[((j + tstrut) & mask) * width + ((i + tstrut) & mask)]) * 0.25;
}


/**
 * Plasma example - diamond-square midpoint subdivision on a grid.
 * The surface is iteratively computed by calculating the center point of
 * the initial square (where the four corners of the surface are the
 * corners of the square), then of the resulting diamonds, then of
 * squares again, etc.
 * 
 * Code adapted from 'fillSurf' example posted to Usenet by
 * Paul Martz (pmartz@dsd.es.com).  
 * 
 * Original copyright notice:
 * Use this code anyway you want as long as you don't sell it for money.
 */

TTerrain *
t_terrain_generate_diamond_square (TTerrain *terrain, 
		                   TRandom  *gauss,
				   gint      size,
                                   gfloat    scale_factor)
{
  gint       x, y, strut, tstrut; 
  gboolean   oddline; 
  float      max_delta = size;
  gfloat    *data = terrain->heightfield;;

  /* initialize things */
  strut = size / 2;
    
  /* create fractal surface from seeded values */
  tstrut = strut;
  while (tstrut > 0)
    {
      oddline = FALSE;
      for (x = 0; x < size; x += tstrut)
        {
          oddline = !oddline;

          y = oddline ? tstrut : 0;
          for (; y < size; y += tstrut)
            {
              data[y * size + x] = avg (terrain, x, y, tstrut) +
                                   t_random_gauss (gauss) * max_delta;
              y += tstrut;
            }
        }

      if (tstrut / 2 != 0)
        for (x = tstrut / 2; x < size; x += tstrut)
          for (y = tstrut / 2; y < size; y += tstrut)
            data[y * size + x] = avg2 (terrain, x, y, tstrut) +
                                 t_random_gauss (gauss) * max_delta;

      tstrut /= 2;
      max_delta *= scale_factor;
    }

  return terrain;
}
