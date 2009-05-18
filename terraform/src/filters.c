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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "filters.h"
#include "gensubdiv.h"
#include "writer.h"
#include "trandom.h"

/*
 * This macro updates the terrain taking into account the
 * selection.
 */
#define APPLY(terrain,pos,value)                                    \
if (terrain->selection == NULL)                                     \
    terrain->heightfield[pos] = value;                              \
  else                                                              \
    terrain->heightfield[pos] =                                     \
      terrain->heightfield[pos] * (1.0 - terrain->selection[pos]) + \
      value * terrain->selection[pos];

/*
 * Other utility macros
 */
#define INTERPOLATE(x,y,frac)  (1-(frac))*(x) + (frac)*(y)

#define X_WRAP(x)  x = ((x + terrain->width) % terrain->width)
#define Y_WRAP(y)  y = ((y + terrain->height) % terrain->height)

#define X_CLIP(x)  x = (x<0 ? 0 : ((x>=terrain->width)?(terrain->width-1):x))
#define Y_CLIP(y)  y = (y<0 ? 0 : ((y>=terrain->height)?(terrain->height-1):y))

#define ATAN2(y,x) ((x==0 && y==0)?0:atan2(y,x))
#define NO_FLOW    -987654321  /* marker for 'undefined' flowmap */

void
t_terrain_rough_map (TTerrain *terrain)
{
  gint    x, y, xx, yy; 
  gfloat *data;

  data = g_new0 (gfloat, terrain->width*terrain->height);

  for (x=0; x<terrain->width; x++)
    for (y=0; y<terrain->height; y++)
      {
        gfloat elv = terrain->heightfield[y*terrain->width+x];
        gfloat diff = 0;
        gint   count = 0;

        for (xx=MAX(x-1,0); xx<MIN(terrain->width,x+2); xx++)
          for (yy=MAX(y-1,0); yy<MIN(terrain->height,y+2); yy++)
            if (xx!=x || yy!=y)
              {
                diff += ABS(elv - terrain->heightfield[yy*terrain->width+xx]);
	        count++;
	      }

        data[y*terrain->width+x] = diff/count;
      }

  g_free (terrain->heightfield);
  terrain->heightfield = data;
  t_terrain_normalize (terrain, FALSE);

  t_terrain_set_modified (terrain, TRUE);
}

void
t_terrain_fill (TTerrain *terrain,
                gfloat    elevation,
                gfloat    factor)
{
  gint    i, size;

  size = terrain->width * terrain->height;

  for (i = 0; i < size; i++)
    if (terrain->heightfield[i] < elevation)
      {
        gfloat value;

        value =
          elevation - (elevation - terrain->heightfield[i]) * (1.0 - factor);

        APPLY (terrain, i, value);
      }

  t_terrain_set_modified (terrain, TRUE);
}

static int
fold_sort (const void *_a,
           const void *_b)
{
  const gfloat *a = _a;
  const gfloat *b = _b;

  if (*a < *b)
    return -1;
  else if (*a > *b)
    return 1;
  return 0;
}

/**
 *  fold: fold height field edges under sealevel.
 *  How: Take a slice 'margin' long, fill it with the ABS 
 *  difference between it's height and the sealevel, sort it and 
 *  then subtract it from the HF elevation at that point 
 *  (starting with the largest values on the HF outside), 
 *  scaling the value subtracted in a linear fashion (0..1).
 */

void
t_terrain_fold (TTerrain *terrain,
                gint      margin)
{
  gint    x, y, width, height;
  gfloat *data;
  gint    pos;
  gfloat  pstep;
  gfloat  slice[margin]; 

  if (margin == 0)
    return;

  width = terrain->width;
  height = terrain->height;
  data = terrain->heightfield;

  pstep = 1.0 / margin;

  /* Top and bottom. */
  for (x = 0; x < width; x++)
    {
      /* Top. */
      pos = x;
      for (y = 0; y < margin; y++)
        {
          slice[y] = data[pos] - terrain->sealevel;

          if (slice[y] < 0.0)
            slice[y] = 0.0;

          pos += width;
        }

      qsort ((void *)slice, (size_t)margin, (size_t)sizeof (gfloat),
             fold_sort);

      pos = x;
      for (y = 0; y < margin; y++)
        {
          gfloat v = data[pos] - slice[margin - 1 - y] * pstep * (margin - y);

	  APPLY (terrain, pos, v);
          if (data[pos] < 0.0)
            data[pos] = 0.0;

          pos += width;
        }

      /* Bottom. */
      pos = (height - 1) * width + x;
      for (y = 0; y < margin; y++)
        {
          slice[y] = data[pos] - terrain->sealevel;
          if (slice[y] < 0.0)
            slice[y] = 0.0;

          pos -= width;
        }
      qsort ((void *)slice, (size_t)margin, (size_t)sizeof (gfloat),
             fold_sort);

      pos = (height - 1) * width + x;
      for (y = 0; y < margin; y++)
        {
	  gfloat v = data[pos] - slice[margin - 1 - y] * pstep * (margin - y);

	  APPLY (terrain, pos, v);
          if (data[pos] < 0.0)
            data[pos] = 0.0;

          pos -= width;
        }
    }

  /* Left and right sides. */
  for (y = 0; y < height; y++)
    {
      /* Left side. */
      pos = y * width;
      for (x = 0; x < margin; x++)
        {
          slice[x] = data[pos] - terrain->sealevel;
          if (slice[x] < 0.0)
            slice[x] = 0.0;

          pos++;
        }
      qsort ((void *)slice, (size_t)margin, (size_t)sizeof (gfloat),
             fold_sort);

      pos = y * width;
      for (x = 0; x < margin; x++)
        {
          gfloat v = data[pos] - slice[margin - 1 - x] * pstep * (margin - x);

	  APPLY (terrain, pos, v);
          if (data[pos] < 0.0)
            data[pos] = 0.0;

          pos++;
        }

      /* Right. */
      pos = y * width + (width - 1);
      for (x = 0; x < margin; x++)
        {
          slice[x] = data[pos] - terrain->sealevel;
          if (slice[x] < 0.0)
            slice[x] = 0;

          pos--;
        }

      qsort ((void *)slice, (size_t)margin, (size_t)sizeof (gfloat),
             fold_sort);

      pos = y * width + (width - 1);
      for (x = 0; x < margin; x++)
        {
          gfloat v = data[pos] - slice[margin - 1 - x] * pstep * (margin - x);

	  APPLY (terrain, pos, v);
          if (data[pos] < 0.0)
            data[pos] = 0.0;

          pos--;
        }
    }

  t_terrain_set_modified (terrain, TRUE);
}

/**
 *  radial_scale: pick a point (centerx, centery) and scale the points 
 *      where distance is mindist<=distance<=maxdist linarly.  The formula
 *      we'll use for a nice sloping smoothing factor is (-cos(x*3)/2)+0.5.
 */

void
t_terrain_radial_scale (TTerrain *terrain,
                        gfloat    center_x, 
                        gfloat    center_y, 
                        gfloat    scale_factor, 
                        gfloat    min_dist,
                        gfloat    max_dist, 
                        gfloat    smooth_factor,
			gint      frequency)
{
  gint    pos, x, y, i;
  gfloat  max_size;
  gfloat  band_width;
  gfloat *data;

  g_return_if_fail (center_x >= 0.0);
  g_return_if_fail (center_y >= 0.0);
  g_return_if_fail (center_x <= 1.0);
  g_return_if_fail (center_y <= 1.0);
  g_return_if_fail (scale_factor > 0.0);
  g_return_if_fail (min_dist >= 0.0);
  g_return_if_fail (max_dist >= 0.0);
  g_return_if_fail (min_dist <= 1.0);
  g_return_if_fail (max_dist <= 1.0);
  g_return_if_fail (smooth_factor >= 0.005);
  g_return_if_fail (smooth_factor <= 1.0);

  if (fabs (min_dist - max_dist) < 0.01)
    return;

  max_size = MAX (terrain->width, terrain->height);

  data = terrain->heightfield;

  center_x *= terrain->width;
  center_y *= terrain->height;
  min_dist *= max_size;
  max_dist *= max_size;
  band_width = (max_dist-min_dist)/frequency;

  for (i=0; i<frequency; i++)
    {
    gfloat min_d = min_dist + band_width * i;
    gfloat max_d = min_dist + band_width * (i+1);
    gfloat s_low = min_d + (max_d - min_d) * 0.5 * smooth_factor;
    gfloat s_high = max_d - (max_d - min_d) * 0.5 * smooth_factor;
    gfloat cd = min_d + band_width/2;

    pos = 0;
    for (y = 0; y < terrain->height; y++)
      for (x = 0; x < terrain->width; x++)
      {
        gfloat point_old, point;
        gfloat distance;

        distance = sqrt ((x - center_x) * (x - center_x) +
                         (y - center_y) * (y - center_y));

        point_old = data[pos];

        /* Lower half of scaling area. */
        if (distance >= min_d && distance <= cd)
          {
            point = (distance - min_d) / (cd - min_d);

            point = point_old * point * scale_factor + point_old * (1 - point);

            if (distance <= s_low)
              {
                gfloat temp;

                temp = point;
                point = (distance - min_d) / (s_low - min_d);
                point = -(cos (point * 3.0) * 0.5) + 0.5;
                point = point_old * (1.0 - point) + temp * point;
              }

            APPLY (terrain, pos, point);
          }
        else if (distance <= max_d && distance >= cd)
          {
            /* Upper half of scaling area. */
            point = (max_d - distance) / (max_d - cd);

            /* Increase linearly from higher end to middle. */
            point = point_old * point * scale_factor + point_old * (1 - point);

            if (distance > s_high)
              {
                gfloat temp;

                temp = point;
                point = (max_d - distance) / (max_d - s_high);
                point = -(cos (point * 3.0) * 0.5) + 0.5;
                point = point_old * (1.0 - point) + temp * point;
              }

            APPLY (terrain, pos, point);
          }

        pos++;
      }
    }

  t_terrain_set_modified (terrain, TRUE);
}

/*
 *  gaussian_hill: create a gaussian hill according to paramters 
 */

void
t_terrain_gaussian_hill (TTerrain *terrain,
                         gfloat    center_x,
                         gfloat    center_y,
                         gfloat    radius,
                         gfloat    radius_factor,
                         gfloat    hscale,
                         gfloat    smooth_factor,
                         gfloat    delta_scale)
{
  gint    x, y;
  gint    pos;
  gint    width, height;
  gfloat *data;
  gfloat  fxmin, fxmax, fymin, fymax;
  gfloat  sdist, sdistr;
  gint    xmin, xmax, ymin, ymax;
  gint    xcent, ycent;

  g_return_if_fail (center_x >= 0.0);
  g_return_if_fail (center_y >= 0.0);
  g_return_if_fail (center_x <= 1.0);
  g_return_if_fail (center_y <= 1.0);
  g_return_if_fail (smooth_factor >= 0.0);
  g_return_if_fail (smooth_factor <= 1.0);
  g_return_if_fail (delta_scale >= 0.0);
  g_return_if_fail (delta_scale <= 1.0);

  data = terrain->heightfield;
  width = terrain->width;
  height = terrain->height;

  /* calculate max and min coordinates on 0 .. 1 scale */
  fxmin = center_x - radius;
  fxmax = center_x + radius;
  fymin = center_y - radius;
  fymax = center_y + radius;
  if (fxmin < 0.0) fxmin = 0.0;
  if (fxmax > 1.0) fxmax = 1.0;
  if (fymin < 0.0) fymin = 0.0;
  if (fymax > 1.0) fymax = 1.0;

  /* translate everything to subscript scale (0 .. n) */
  xmin = (gint) (fxmin * width);
  xmax = (gint) (fxmax * width);
  ymin = (gint) (fymin * height);
  ymax = (gint) (fymax * height);
  xcent = (gint) (center_x * width);
  ycent = (gint) (center_y * width);
  radius *= ((width + height) * 0.5);
  sdist = radius * smooth_factor;
  sdistr = radius - sdist;

  radius_factor = radius_factor * radius_factor;
  for (y = ymin; y < ymax; y++) 
    {
      gfloat ya;

      ya = (((gfloat) y + 0.5) / height) - center_y;
      ya = ya * ya;

      pos = y * terrain->width + xmin;
      for (x = xmin; x < xmax; x++) 
        {
          gfloat xa, distance, temp;

          distance = sqrt ((xcent - x) * (xcent - x) +
                           (ycent - y) * (ycent - y));
          if (distance <= radius)
            {
              gfloat radius;

              temp = data[pos];
              xa = (((gfloat) x + 0.5) / width) - center_x;
              xa = xa * xa;
              radius = xa + ya;

              /* see if we should be smoothing */
              if (distance > sdistr)
                {
                  gfloat sf;

                  distance -= sdistr;
                  sf = 1.0 - distance / sdist;
                  temp += delta_scale * sf * hscale *
                          exp (-radius / radius_factor);
                }
              else
                temp += delta_scale * hscale * exp (-radius / radius_factor);

              APPLY (terrain, pos, temp);
            }

          pos++;
        } 
    } 

  t_terrain_set_modified (terrain, TRUE);
}

/**
 *  mirror: mirror the height field on one of it's axis.
 *  Type can be 1, 2, 3, 4 for x-axis, y-axis, top (left
 *  to bottom right) or bottom (left to top right)
 */
void
t_terrain_mirror (TTerrain *terrain,
                  gint      type)
{
  gint      pos1, pos2;
  gint      middle, x, y, width, height;
  gint      limx, limy;
  gfloat   *data;
  gfloat   *river;
  gfloat   *selection;
  gfloat    temp;
  GArray   *objects;
  TTerrainRiverCoords *coords;

  width = terrain->width;
  height = terrain->height;
  limx = width - 1;
  limy = height - 1;
  data = terrain->heightfield;
  selection = terrain->selection;
  river = terrain->riverfield;
  objects = terrain->objects;

  if (type == 0) /* Mirror over the Y Axis */
    {
      middle = width >> 1;
      for (y = 0; y < height; y++)
        {
          pos1 = y * width;
          pos2 = pos1 + width - 1;

          for (x = 0; x < middle; x++)
            {
              temp = data[pos1];
              data[pos1] = data[pos2];
              data[pos2] = temp;

              if (selection != NULL)
                {
                  temp = selection[pos1];
                  selection[pos1] = selection[pos2];
                  selection[pos2] = temp;
                }

              if (river != NULL)
                {
                  temp = river[pos1];
                  river[pos1] = river[pos2];
                  river[pos2] = temp;
                }

            pos1++;
            pos2--;
            }
        }

      if(river!=NULL && terrain->riversources->len>0)
        for(y=0; y<terrain->riversources->len; y++)
          {
          coords = &g_array_index (terrain->riversources,TTerrainRiverCoords,y);
          coords->x = 1.0 - coords->x;
          }

      if (objects->len > 0)
        {
        gint i;
        gint size = objects->len;

	for (i=0; i<size; i++)
          {
	    TTerrainObject *object;
	    gfloat px, py;

            object = &g_array_index (objects, TTerrainObject, i);
            px = object->x;
            py = object->y;
	    object->x = 0.5 + (0.5 - px);
	    object->y = py;

            px = object->ox;
            py = object->oy;
            object->ox = limx/2 + (limx/2 - px);
            object->oy = py;
          }
        }
    }
  else if (type == 1) /* Mirror across the X Axis */
    {
      middle = height >> 1;
      for (y = 0; y < middle; y++)
        {
          pos1 = y * width;
          pos2 = (height - y - 1) * width;

          for (x = 0; x < width; x++)
            {
              temp = data[pos1];
              data[pos1] = data[pos2];
              data[pos2] = temp;

              if (selection != NULL)
                {
                  temp = selection[pos1];
                  selection[pos1] = selection[pos2];
                  selection[pos2] = temp;
                }

              if (river != NULL)
                {
                  temp = river[pos1];
                  river[pos1] = river[pos2];
                  river[pos2] = temp;
	        }

            pos1++;
            pos2++;
            }
        }

      if(river!=NULL && terrain->riversources->len>0)
        for(y=0; y<terrain->riversources->len; y++)
          {
          coords = &g_array_index (terrain->riversources,TTerrainRiverCoords,y);
          coords->y = 1.0 - coords->y;
          }

      if (objects->len > 0)
        {
        gint i;
        gint size = objects->len;

	for (i=0; i<size; i++)
          {
	    TTerrainObject *object;
	    gfloat px, py;

            object = &g_array_index (objects, TTerrainObject, i);
            px = object->x;
            py = object->y;
	    object->x = px;
	    object->y = 0.5 + (0.5 - py);

            px = object->ox;
            py = object->oy;
            object->ox = px;
            object->oy = limy/2 + (limy/2 - py);
          }
        }
    }
  else if (type == 2 || type == 3)
    {
      g_return_if_fail (width == height);

      if (type == 2) /* Top left to bottom right */
        {
          for (y = 0; y < height; y++)
            for (x = y; x < width; x++)
              {
                pos1 = y * width + x;
                pos2 = x * width + y;

                temp = data[pos1];
                data[pos1] = data[pos2];
                data[pos2] = temp;

                if (selection != NULL)
                  {
                    temp = selection[pos1];
                    selection[pos1] = selection[pos2];
                    selection[pos2] = temp;
                  }

                if (river != NULL)
                  {
                    temp = river[pos1];
                    river[pos1] = river[pos2];
                    river[pos2] = temp;
	          }
	        }

          if(river!=NULL && terrain->riversources->len>0)
            for(y=0; y<terrain->riversources->len; y++)
              {
              coords = &g_array_index (terrain->riversources,TTerrainRiverCoords,y);
              temp = coords->y;
              coords->y = coords->x;
              coords->x = temp;
              }


          if (objects->len > 0)
            {
            gint i;
            gint size = objects->len;

	    for (i=0; i<size; i++)
              {
	        TTerrainObject *object;
	        gfloat px, py;

                object = &g_array_index (objects, TTerrainObject, i);
                px = object->x;
                py = object->y;
	        object->x = py;
	        object->y = px;

                px = object->ox;
                py = object->oy;
                object->ox = py;
                object->oy = px;
	      }
            }

        }
      else /* Bottom left to top right */
        {
          for (y = 0; y < height; y++)
            for (x = 0; x < width - y; x++)
              {
                pos1 = y * width + x;
                pos2 = (width - x - 1) * width + (height - y - 1);

                temp = data[pos1];
                data[pos1] = data[pos2];
                data[pos2] = temp;

                if (selection != NULL)
                  {
                    temp = selection[pos1];
                    selection[pos1] = selection[pos2];
                    selection[pos2] = temp;
                  }

                if (river != NULL)
                  {
                    temp = river[pos1];
                    river[pos1] = river[pos2];
                    river[pos2] = temp;
	          }
	        }

          if(river!=NULL && terrain->riversources->len>0)
            for(y=0; y<terrain->riversources->len; y++)
              {
              coords = &g_array_index (terrain->riversources,TTerrainRiverCoords,y);
              temp = coords->y;
              coords->y = 1.0 - coords->x;
              coords->x = 1.0 - temp;
              }

          if (objects->len > 0)
            {
            gint i;
            gint size = objects->len;

	    for (i=0; i<size; i++)
              {
	        TTerrainObject *object;
	        gfloat px, py;

                object = &g_array_index (objects, TTerrainObject, i);
                px = object->x;
                py = object->y;
	        object->x = 1-py;
	        object->y = 1-px;

                px = object->ox;
                py = object->oy;
                object->ox = limx-py;
                object->oy = limy-px;
              }
            }

        }
    }

  t_terrain_set_modified (terrain, TRUE);
}

/*
 * Add craters to a heightfield array 
 * Copyright (C) 1995 by Heiko Eissfeldt
 * heiko@colossus.escape.de
 * modified JAN 1996 John Beale for use with HF-Lab  
 * modified JUN 1999 Robert Gasch for use with Terraform
 * modified DEC 2000 David A. Bartold for use with Terraform 0.8.x+
 */


#define alpha        (35.25 / 180.0 * M_PI) /* sphere segment cutoff angle */
#define crater_depth 0.85                   /* 0.9 */

static double a1 = crater_depth, b, d;

/*
 * crater_profile: method to determine the z-elevation dependent on normalized
 *      squared radial coordinate 
 */

static gdouble
crater_profile (gdouble nsq_rad)
{
  static double thickness;
  gdouble       radial_coord;

  thickness = 50.0; /* controls the wall thickeness */
  radial_coord = sqrt (fabs (nsq_rad));

  if (radial_coord > b)
    /* outer region gauss distribution */
    return d * exp (-thickness * (radial_coord - b) * (radial_coord - b));
  else 
    /* inner region sphere segment */
    return a1 - sqrt (1.0 - nsq_rad);
}

#define pow4(x)     ((x) * (x) * (x) * (x))
#define inner_scale 1.0
#define outer_scale (1.0 - inner_scale * pow4 (border)) / pow4 (1.0 - border)

/* 
 * crater_dissolve: function to control the weight of the crater profile as
 * opposed to the underlying terrain elevation 
 */
static gdouble
crater_dissolve (gdouble nsq_rad)
{
  if (nsq_rad > 0.6 * 0.6)
    return 1.0 - 0.9 * exp (-30.0 * (sqrt (nsq_rad) - 0.6) *
                                    (sqrt (nsq_rad) - 0.6));
  else 
    return 0.1 * exp (-25.0 * (sqrt (nsq_rad) - 0.6) *
                              (sqrt (nsq_rad) - 0.6));
}

void
t_terrain_craters (TTerrain *terrain,
                   gint      count,
                   gboolean  wrap,
                   gfloat    height_scale,
                   gfloat    radius_scale,
                   gfloat    crater_coverage,
                   gint      seed,
                   gfloat    center_x,
                   gfloat    center_y)
{
  gint        i, j, k, xloc, yloc, crater_size;
  gint        sq_radius;
  gdouble     nsq_rad, weight;
  gint        lev_samples, samples;
  gdouble     level_with, level_pure;
  gdouble     crater_scale;           /* vertical crater scaling factor */
  gdouble     c, d2, shift;
  gdouble     b1, b2, b3;             /* radius scaling params  */
  gint        mesh_size;              /* geometric mean of width and height */
  gint        data_size;
  gfloat     *data_orig;
  TRandom    *rnd, *rnd_seed;

  g_return_if_fail (count != 1 || (center_x >= 0.0 && center_y >= 0.0));
  g_return_if_fail (count != 1 || (center_x <= 1.0 && center_y <= 1.0));

  data_size = terrain->width * terrain->height;
  data_orig = g_new (gfloat, data_size);

  rnd = t_random_new (seed);
  rnd_seed = t_random_new (t_random_rnd (rnd, 0.0, 1.0));
  b1 = 10.0;
  b2 = 1.0 / (b1 * b1 * b1 * b1);
  b3 = 1.0 / b1;
  mesh_size = (gint) sqrt (terrain->width * terrain->height);
  b = sin (alpha);
  d = a1 - cos (alpha);

  /* copy the terrain */
  memcpy (data_orig, terrain->heightfield, data_size * sizeof (gfloat)); 

  for (k = 0; k < count; k++)
    {
      gint     ii, jj;
      TRandom *rnd_loop;

      /*
       * allocating a new random number thread for every crater means
       * that we also keep the same sequence of random numbers when
       * changing the number of craters. We scale the random number 
       * seed from 0 to MAXINT (32 bits).
       */

      rnd_loop = t_random_new (t_random_rnd (rnd_seed, 0, 2147483647));

      /* pick a cratersize according to a power law distribution */
      /* greatest first. So great craters never eliminate small ones */
      /* c = (double)(count - k + 1) / count + b3; */
      /* craters appear in random order */

      c = t_random_rnd1 (rnd_loop) + b3;   /* c is in the range b3 ... b3+1 */
      d2 = b2 / (c * c * c * c);      /* d2 is in the range 0 ... 1 */

      //if (count == 1) d2 = 1.0;   /* single craters set to max. size */

      if (count == 1)
        {
          xloc = (gint) (center_x * terrain->width);
          yloc = (gint) (center_y * terrain->height);
          crater_size = (gint) (radius_scale *
                                (terrain->width + terrain->height) * 0.5);
        }
      else
        {
          /* pick a random location for the crater */
          xloc = (gint) (t_random_rnd1 (rnd_loop) * (terrain->width - 1));
          yloc = (gint) (t_random_rnd1 (rnd_loop) * (terrain->height - 1));

          crater_size = 3.0 + (gint) (d2 * crater_coverage *
                                      mesh_size * radius_scale);

          /* Is crater outside of selection?  If so, don't create it. */
          if (terrain->selection != NULL &&
              terrain->selection[yloc * terrain->width + xloc] < 0.5)
            continue;
        }

/* macro to determine the height dependent on crater size */
#define CRATER_SCALE ((height_scale * \
        pow ((crater_size / (3.0 + crater_coverage * mesh_size)), 0.9)) \
        / 256.0 * pow (mesh_size / 256.0, 0.1) / crater_coverage * 80.0)

      crater_scale = CRATER_SCALE;     /* vertical crater scaling factor */

      /* what is the mean height of this plot */
      samples = lev_samples = MAX ((crater_size * crater_size) / 5.0, 1.0);
      level_with = level_pure = 0.0;
      while (samples) 
        {
          i = (gint) (t_random_rnd1 (rnd_loop) * 2.0 * crater_size -
                      crater_size);
          j = (gint) (t_random_rnd1 (rnd_loop) * (2.0 * crater_size) -
                      crater_size);

          if (i * i + j * j > crater_size * crater_size)
            continue;
          ii = xloc + i;
          jj = yloc + j;

          /* handle wrapping details... */
          if (wrap || (ii >= 0 && ii < terrain->width &&
                       jj >= 0 && jj < terrain->height)) 
            {
              X_WRAP (ii);
              Y_WRAP (jj);
              level_with += terrain->heightfield[jj * terrain->width + ii];
              level_pure += data_orig[jj * terrain->width + ii];

              samples--;
            }
        }
      level_with /= lev_samples;
      level_pure /= lev_samples;

      /* Now lets create the crater. */

      /* In order to do it efficiently, we calculate for one octant
       * only and use eightfold symmetry, if possible.
       * Main diagonals and axes have a four fold symmetry only.
       * The center point has to be treated single.
       */

#define SQUEEZE 1.3

/* this macro calculates the coordinates, does clipping and modifies
 * the elevation at this spot due to shift and weight.
 * hfOrig() contains the crater-free underground. p_HF() contains the result.
 * level_with: average altitude of cratered surface in region
 * level_pure: average altitude of uncratered surface
 */

#define SHIFT(x,y) \
  { \
    ii = xloc + (x); jj = yloc + (y); \
    if (wrap || ((ii >= 0) && (ii < terrain->width) && \
                  (jj >= 0) && (jj < terrain->height))) \
    {\
      X_WRAP (ii);  Y_WRAP (jj); \
      terrain->heightfield[jj * terrain->width + ii] = \
      (shift + terrain->heightfield[jj * terrain->width + ii] * weight + \
      (level_with + (data_orig[jj * terrain->width + ii] - level_pure) / \
      SQUEEZE) * (1.0 - weight)); \
    } \
  }

/* macro to do four points at once. Points are rotated by 90 degrees. */
#define FOURFOLD(i,j)   { SHIFT (i,j) SHIFT (-j,i) SHIFT (-i,-j) SHIFT (j,-i) }

/* get eightfold symmetry by mirroring fourfold symmetry along the x==y axe */
#define EIGHTFOLD       { FOURFOLD (i,j) FOURFOLD (j,i) }


      /* The loop covers a triangle (except the center point)
       * Eg crater_size is 3, coordinates are shown as i, j
       *
       *              3,3 j
       *         2,2  3,2 |
       *    1,1  2,1  3,1 v
       * x  1,0  2,0  3,0
       * ^          <-i
       * |
       * center point
       *
       * 2,1 , 3,2 and 3,1 have eightfold symmetry.
       * 1,0 , 2,0 , 3,0 , 1,1 , 2,2 and 3,3 have fourfold symmetry.
       */

      for (i = crater_size; i > 0; i--)
        {
          for (j = i; j >= 0; j--)
            {
              /* check if outside */
              sq_radius = i * i + j * j;
              nsq_rad = (gdouble) sq_radius / crater_size / crater_size;
              if (nsq_rad > 1.0) continue;

              /* inside the crater area */
              shift = crater_scale * crater_profile (nsq_rad);
              weight = crater_dissolve (nsq_rad);

              if (i == j || j == 0)
                FOURFOLD (i, j)
              else
                EIGHTFOLD
            }
        }
      /* the center point */
      shift = crater_scale * crater_profile (0.0);
      weight = crater_dissolve (0.0);
      SHIFT (0,0)

      t_random_free (rnd_loop);
    }

  g_free (data_orig);
  t_random_free (rnd_seed);
  t_random_free (rnd);

  t_terrain_set_modified (terrain, TRUE);
}

static gint
t_terrain_upslope_sfd (TTerrain *terrain,
                       gint      given_x,
                       gint      given_y,
                       gfloat   *cache);
static gfloat
t_terrain_upslope_mfd (TTerrain *terrain,
                       gint      given_x,
                       gint      given_y,
                       gfloat   *cache,
                       gboolean *hitcache);
/*
 * t_terrain_all_rivers: create a river according to paramters 
 *
 * contributed to Marcus Mueller <marcusmm@web.de>
 */
void
t_terrain_all_rivers (TTerrain *terrain,
		      gfloat    power,
		      gint      method)
{
  gint       x, y, x2, y2;
  gint       pos;
  gint       width, height;
  gfloat     nlevel,threshold;
  gboolean   cont;
  gint      *flowmap = NULL;
  gfloat    *data;
  gfloat    *cache = NULL;
  gfloat    *upcells;
  gboolean  *hitcache = NULL;
  TTerrain  *terrain_upslope = NULL;

  g_return_if_fail (power >= 0.0);
  g_return_if_fail (power <= 1.0);

  width = terrain->width;
  height = terrain->height;

  if (terrain->riverfield == NULL)
    terrain->riverfield = g_new0 (gfloat, width*height);

  if(method == UPSLOPE_SFD || method == UPSLOPE_MFD)
    {
      terrain_upslope = t_terrain_clone( terrain );
      cache = g_new0 (gfloat, width*height);
      for (pos = 0; pos < width*height; ++pos)
	cache[pos] = NO_FLOW;

    }
  if(method == UPSLOPE_MFD)
    {
      hitcache = g_new0 (gboolean, width*height);
    }
  
  data = terrain->heightfield;
  upcells = g_new0 (gfloat, width*height);

  if(method == OWN_COUNTING)
    {
      flowmap = g_new0 (gint, width*height);

      /* build flowmap */
      for(y=1; y<height-1; y++)
	for(x=1; x<width-1; x++)
	  {
	    nlevel = data[x+y*width];
	    flowmap[x+y*width] = 9;

	    for(y2=y-1; y2<=y+1; y2++)
		for(x2=x-1; x2<=x+1; x2++)
		  {
                    /*  use only 4 directions  */
		    if( /*((x2+y2)&1 != (x+y)&1) &&*/ nlevel>data[x2+y2*width])
		      {
			flowmap[x+y*width] = (x2+1-x)+(y2+1-y)*8;
			nlevel = data[x2+y2*width];
		      }
		  }
	  }
    } 

  /* build upcells map */
  nlevel = 0.0;
  for(y=1; y<height-1; y++)
    {
      for(x=1; x<width-1; x++)
	{
	switch(method)
	  {
	  case OWN_COUNTING:
	    x2 = x;
	    y2 = y;

	    while(terrain->heightfield[x2+y2*width]>terrain->sealevel &&
		  flowmap[x2+y2*width]!=9 && 
		  x2>0 && y2>0 && 
		  x2<(width-1) && y2<(height-1))
	      {
		upcells[x2+y2*width]++;
		pos = flowmap[x2+y2*width];
		x2 += (pos&7)-1;
		y2 += pos/8-1;
	      }

	    break;

	  case UPSLOPE_SFD:
	    upcells[x+y*width] =
	      1.0 * t_terrain_upslope_sfd (terrain_upslope, x,y, cache);
	    break;

	  case UPSLOPE_MFD:
	    upcells[x+y*width] =
	      t_terrain_upslope_mfd (terrain_upslope, x,y, cache, hitcache);
	    break;
	  } 
	  /* 
	   * if(upcells[x+y*width]!=0.0)
	   *   printf("(%d,%d):%f\n",x,y,upcells[x+y*width]);
	   */
	  if (upcells[x+y*width] > nlevel)
	    nlevel = upcells[x+y*width];
	} 
    } 

  pos = 0;
  power *= 0.99999; /* workaround for 1.0 problems... */
  threshold = pow(MAX(width,height),1.0)/power/10;

  if(method == UPSLOPE_SFD)
    {
      threshold = pow(1.0-power,2.0) * MAX(width,height);
    }
  else
  if(method == UPSLOPE_MFD)
    {
      threshold = pow(1.0-power,2.0) * width*height;
    }

  /* printf("max upcells=%f; threshold=%f\n", nlevel, threshold); */

  for(cont=TRUE; cont==TRUE; threshold++)
    {
      cont = FALSE;

      for(y=1; y<height-1; y++)
	for(x=1; x<width-1; x++)
	    {
              gint off = y*width+x;

	      if (terrain->selection == NULL ||
                  terrain->selection[off] > 0.5)
                {
	        if(upcells[off] > threshold)
		  {
		    if(threshold+1 > upcells[off] &&
		       terrain->riverfield[off] < terrain->heightfield[off])
		      {
		        t_terrain_river (terrain,(1.0*x)/width,(1.0*y)/height);
		      }
		    cont = TRUE;
		  }
                }
	    }
    } /* end while cont (higher upcells available in selection) */

  g_free (upcells);

  if(method == OWN_COUNTING)
    g_free (flowmap);

  if(method == UPSLOPE_SFD || method == UPSLOPE_MFD)
    g_free (cache);

  if(method == UPSLOPE_MFD)
      g_free (hitcache);

  return;
}


/*
 * t_terrain_river: create a river according to paramters 
 *
 * contributed to Marcus Mueller <marcusmm@web.de>
 */
void
t_terrain_river (TTerrain *terrain,
		 gfloat    center_x,
		 gfloat    center_y)
{
  gint    x, y, x2, y2;
  gint    pos;
  gint    width, height;
  gfloat *data;
  gfloat *river;
  gfloat  nlevel, tlevel, sealevel;
  gint    last;
  gboolean end,flooding;
  TTerrainRiverCoords source;
#define ADDING 0.0001
#define RIVERDEPTH 0.0005
#define RF_OFFSET 0.01
  static gfloat adding = ADDING;

  gfloat fill_lake (gint x, gint y, gint *x2, gint *y2)
    {
      gfloat level;

      /* recursive "next"-function to fill lake at "level" */
      void next( gint xt, gint yt)
	{
	  river[xt+yt*width]=level;
	  
	  if( MAX(data[xt+yt*width],river[xt+yt*width]) <level )
	    {
	      if(xt<=0 || yt<=0 || xt>=width-1 || yt>=height-1 )
		{ /* border touched */
		  if(MAX(data[xt+yt*width],river[xt+yt*width]) <
		     MAX(data[(*x2)+(*y2)*width],river[(*x2)+(*y2)*width]))
		    { /* new lowest exit */
		      *x2 = xt;
		      *y2 = yt;
		    }
		}
	      else /* not border */
		{
		  next(xt-1, yt);
		  next(xt+1, yt);
		  next(xt,   yt-1);
		  next(xt,   yt+1);

		  /* diagonalen */
		  next(xt-1, yt-1);
		  next(xt+1, yt-1);
		  next(xt-1, yt+1);
		  next(xt+1, yt+1);
		}
 	    } /* end if (Max(data,river)<level) */
	  else
	    { /* (Max(data,river)>=level) */
	      /* only check if point is lowest exit */
	      if(MAX(data[xt+yt*width],river[xt+yt*width]) <
		 MAX(data[(*x2)+(*y2)*width],river[(*x2)+(*y2)*width]))
		{
		  *x2 = xt;
		  *y2 = yt;
		}
	    } /* end else (Max(data,river)>=level) */
	} /* end next */

      level = MAX(data[x+y*width],river[x+y*width]);

      level += adding;

      next(x,y);

      return level;
    } /* end fill_lake */

  g_return_if_fail (center_x >= 0.0);
  g_return_if_fail (center_y >= 0.0);
  g_return_if_fail (center_x <= 1.0);
  g_return_if_fail (center_y <= 1.0);

  width = terrain->width;
  height = terrain->height;
  sealevel = terrain->sealevel;

  x = center_x * (terrain->width-1);
  y = center_y * (terrain->height-1);

  source.x = center_x;
  source.y = center_y;
  g_array_append_val (terrain->riversources, source);
  
  data = terrain->heightfield;
  river = g_new0 (gfloat, width*height);

  if (terrain->riverfield == NULL)
    terrain->riverfield = g_new0 (gfloat, width*height);
  else
    memcpy (river, terrain->riverfield, sizeof(gfloat)*width*height);

  for (pos=0; pos<width*height; ++pos)
    river[pos] = 0.0;
/*    if (river[pos]==0)
      river[pos] = data[pos] - RF_OFFSET;
*/


  last=0;
  end=FALSE;
  flooding=FALSE;
  do 
    {
    gint plast=0;

    pos = y * width + x;
    x2 = x;
    y2 = y;
    nlevel = 1.0; 
    tlevel = 0.0;

    /* right */
    if ( (x+1)<width &&
	 MAX(data[pos+1],river[pos+1])<nlevel &&
	 last != 2) 
      {
      tlevel = nlevel = MAX(data[pos+1],river[pos+1]);
      x2 = x+1;
      y2 = y;
      plast = 1;
      }

    /* left */
    if ( x>0 &&
	 MAX(data[pos-1],river[pos-1])<nlevel &&
	 last != 1) 
      {
      tlevel = nlevel = MAX(data[pos-1],river[pos-1]);
      x2 = x-1;
      y2 = y;
      plast = 2;
      }

    /* bottom */
    if ( (y+1)<height &&
	 MAX(data[pos+width],river[pos+width])<nlevel &&
	 last != 4) 
      {
      tlevel = nlevel = MAX(data[pos+width],river[pos+width]);
      x2 = x;
      y2 = y+1;
      plast = 3;
      }

    /* top */
    if ( y>0 &&
	 MAX(data[pos-width],river[pos-width])<nlevel &&
	 last != 3) 
      {
      tlevel = nlevel = MAX(data[pos-width],river[pos-width]);
      x2 = x;
      y2 = y-1;
      plast = 4;
      }

/*  // diagonalen
    //rechts oben
    if ( (x+1)<width && y>0 && 
	 MAX(data[pos+1-width],river[pos+1-width])<nlevel &&
	 last != 6) 
      {
      tlevel = nlevel = MAX(data[pos+1-width],river[pos+1-width]);
      x2 = x+1;
      y2 = y-1;
      plast = 5;
      }

    //links unten
    if ( x>0 && (y+1)<height &&
	 MAX(data[pos-1+width],river[pos-1+width])<nlevel &&
	 last != 5) 
      {
      tlevel = nlevel = MAX(data[pos-1+width],river[pos-1+width]);
      x2 = x-1;
      y2 = y+1;
      plast = 6;
      }

    //rechts unten
    if ( (x+1)<width && (y+1)<height &&
	 MAX(data[pos+1+width],river[pos+1+width])<nlevel &&
	 last != 8) 
      {
      tlevel = nlevel = MAX(data[pos+1+width],river[pos+1+width]);
      x2 = x+1;
      y2 = y+1;
      plast = 7;
      }

    //links oben
    if ( x>0 && y>0 &&
	 MAX(data[pos-1-width],river[pos-1-width])<nlevel &&
	 last != 7) 
      {
      tlevel = nlevel = MAX(data[pos-1-width],river[pos-1-width]);
      x2 = x-1;
      y2 = y-1;
      plast = 8;
      }
*/

    if(nlevel > MAX(data[pos],river[pos]))
      { /* have to fill this pit with a lake!!! */
	nlevel = fill_lake (x, y, &x2, &y2);
	plast=0;
      }
    else
      {
	if (river[pos] < data[pos])
	  river[pos] = data[pos] + RIVERDEPTH;//*(plast>4?2:1);

	/*if (river[pos] <= nlevel)
	  river[pos] += adding;*/

	end = (terrain->riverfield[pos] > terrain->heightfield[pos]);
      }

    x = x2;
    y = y2;
    last = plast;
    } 
  while (tlevel>sealevel && 
         x>0 && y>0 && 
         x<(width-1) && y<(height-1) && !end );

  for(x=0; x<width; x++)
    for(y=0; y<height; y++)
      if(river[y*width+x] != 0)
	{
	  /* Just use higer value (old riverfield or new river) */
	  nlevel = MAX(river[y*width+x],terrain->riverfield[y*width+x]);

	  terrain->riverfield[y*width+x]=nlevel;

          /*if(river[y*width+x]>=terrain->heightfield[y*width+x])
            {
            // lower land if river is on same level
            // don't lower if land is already lower than river (lakes)
            terrain->heightfield[y*width+x] -= RIVERDEPTH;
            } */

	}
      else /* river[y*width+x]==0 */
	{ /* no new river here */
	  nlevel=2.0;
	  { /* search lowest neighbor river */
	    for(x2 = MAX(0,x-1); x2<MIN(width,x+1); ++x2)
	      {
		if(y-1>=0 && 0<river[(y-1)*width+x2])
		  nlevel = MIN( nlevel, 
				MAX(river[(y-1)*width+x2],
				    terrain->riverfield[(y-1)*width+x2]) );
		if(y+1<height && 0<river[(y+1)*width+x2])
		  nlevel = MIN( nlevel, 
				MAX(river[(y+1)*width+x2],
				    terrain->riverfield[(y+1)*width+x2]) );
	      }
	    if(x-1>=0 && 0<river[y*width+x-1])
	      nlevel = MIN( nlevel, 
			    MAX(river[y*width+x-1],
				terrain->riverfield[y*width+x-1]) );
	    if(x+1<width && 0<river[y*width+x+1])
	      nlevel = MIN( nlevel, 
			    MAX(river[y*width+x+1],
				terrain->riverfield[y*width+x+1]) );
	  }
	  if(nlevel!=2.0)
	    terrain->riverfield[y*width+x]=nlevel;
	}

  g_free (river);
  t_terrain_set_modified (terrain, TRUE);
}


void t_terrain_remove_rivers  (TTerrain *terrain)
{
  if(terrain->riversources && terrain->riverfield)
    {
      /* printf("%d rivers to remove\n",terrain->riversources->len); */

      if(terrain->riversources->len > 0)
        {
	g_array_free (terrain->riversources, TRUE);
        terrain->riversources = 
	  g_array_new (FALSE, FALSE, sizeof (TTerrainRiverCoords));
        }

      g_free (terrain->riverfield);
      terrain->riverfield = NULL;
      t_terrain_set_modified (terrain, TRUE);
    }
}


void t_terrain_redraw_rivers  (TTerrain *terrain)
{
  if(terrain->riversources && terrain->riversources->len > 0)
    {
      GArray *org_rivers;
      gint i;
      TTerrainRiverCoords *source;

      /* printf("%d rivers to redraw\n",terrain->riversources->len); */
      
      org_rivers = terrain->riversources;     /* keep old list */

      terrain->riversources =                 /* get new empty list */
	g_array_new (FALSE, FALSE, sizeof (TTerrainRiverCoords));

      if(terrain->riverfield)
	{
	  g_free (terrain->riverfield);
	  terrain->riverfield = g_new0 (gfloat, terrain->width*terrain->height);
	}

      for(i=0;i<org_rivers->len;++i)
	{
	  source = &g_array_index( org_rivers, TTerrainRiverCoords, i);
	  t_terrain_river (terrain, 1.0*source->x, (1.0*source->y));
	}
      g_array_free( org_rivers, TRUE);        /* free old list */
      t_terrain_set_modified (terrain, TRUE);
    }
}


void
t_terrain_rotate (TTerrain *terrain,
                  gint      amount)
{
  if (amount == 0) /* 90 degrees clockwise */
    {
      gint    i, x, y;
      gint    width, height;
      gfloat *data, *data2;
      TTerrainRiverCoords *coords;

      width = terrain->width;
      height = terrain->height;

      data = terrain->heightfield;
      data2 = g_new (gfloat, terrain->width * terrain->height);
      for (x = 0; x < width; x++)
        for (y = 0; y < height; y++)
          data2[x * height + y] = data[(height - y - 1) * width + x];

      terrain->heightfield = data2;
      g_free (data);

      if (terrain->selection != NULL)
        {
          data = terrain->selection;
          data2 = g_new (gfloat, terrain->width * terrain->height);
          for (x = 0; x < width; x++)
            for (y = 0; y < height; y++)
              data2[x * height + y] = data[(height - y - 1) * width + x];

          terrain->selection = data2;
          g_free (data);
        }

      if (terrain->riverfield != NULL)
        {
          gfloat *river, *river2;

          river = terrain->riverfield;
          river2 = g_new (gfloat, terrain->width * terrain->height);
          for (x = 0; x < width; x++)
            for (y = 0; y < height; y++)
              river2[x * height + y] = river[(height - y - 1) * width + x];

          terrain->riverfield = river2;
          g_free (river);
	}

      if(terrain->riverfield!= NULL && terrain->riversources->len>0)
        for(i=0; i<terrain->riversources->len; i++)
          {
            gfloat tempx, tempy;
            coords = &g_array_index (terrain->riversources, TTerrainRiverCoords, i);
            tempx = coords->x;
            tempy = coords->y;
            coords->x = 1.0 - tempy;
            coords->y = tempx;
          }

      if (terrain->objects->len > 0)
        {
          gint i;
          gint size = terrain->objects->len;
	  gint xlim, ylim;

	  xlim = terrain->width-1;
	  ylim = terrain->height-1;

	  for (i=0; i<size; i++)
            {
	    TTerrainObject *object;
	    gfloat px, py;

            object = &g_array_index (terrain->objects, TTerrainObject, i);
            px = object->x;
            py = object->y;
	    object->x = (1-py); 
	    object->y = (1-(1-px));

            px = object->ox;
            py = object->oy;
	    object->ox = (xlim-py);
	    object->oy = (ylim-(ylim-px));
            }
        }

      terrain->width = height;
      terrain->height = width;
    }
  else if (amount == 1) /* 180 degrees */
    {
      gint    i, pos1, pos2, size;
      gfloat *data;
      TTerrainRiverCoords *coords;

      pos2 = terrain->width * terrain->height;
      size = pos2 >> 1;
      data = terrain->heightfield;
      for (pos1 = 0; pos1 < size; pos1++)
        {
          gfloat temp = data[--pos2];
          data[pos2] = data[pos1];
          data[pos1] = temp;
        }

      if (terrain->selection != NULL)
        {
          pos2 = terrain->width * terrain->height;
          data = terrain->selection;
          for (pos1 = 0; pos1 < size; pos1++)
            {
              gfloat temp = data[--pos2];
              data[pos2] = data[pos1];
              data[pos1] = temp;
            }
        }

      if (terrain->riverfield != NULL)
        {
          gfloat *river;

          pos2 = terrain->width * terrain->height;
          river = terrain->riverfield;
          for (pos1 = 0; pos1 < size; pos1++)
            {
              gboolean temp = river[--pos2];
              river[pos2] = river[pos1];
              river[pos1] = temp;
            }
        }

      if(terrain->riverfield!= NULL && terrain->riversources->len>0)
        for(i=0; i<terrain->riversources->len; i++)
          {
          gfloat tempx, tempy;
          coords = &g_array_index (terrain->riversources, TTerrainRiverCoords, i);
          tempx = coords->x;
          tempy = coords->y;
          coords->x = 1.0 - tempx;
          coords->y = 1.0 - tempy;
          }

      if (terrain->objects->len > 0)
        {
          gint i;
          gint size = terrain->objects->len;
	  gint xlim, ylim;

	  xlim = terrain->width-1;
	  ylim = terrain->height-1;

	  for (i=0; i<size; i++)
            {
	    TTerrainObject *object;
	    gfloat px, py;

            object = &g_array_index (terrain->objects, TTerrainObject, i);
            px = object->x;
            py = object->y;
	    object->x = (1-px);
	    object->y = (1-py);

            px = object->ox;
            py = object->oy;
	    object->ox = (xlim-px);
	    object->oy = (ylim-py);
            }
        }
    }
  else if (amount == 2) /* 270 degrees clockwise */
    {
      gint    i, x, y;
      gint    width, height;
      gfloat *data, *data2;
      TTerrainRiverCoords *coords;

      data = terrain->heightfield;
      width = terrain->width;
      height = terrain->height;

      data2 = g_new (gfloat, terrain->width * terrain->height);
      for (x = 0; x < width; x++)
        for (y = 0; y < height; y++)
          data2[x * height + y] = data[y * width + (width - x - 1)];

      terrain->heightfield = data2;
      g_free (data);

      if (terrain->selection != NULL)
        {
          data = terrain->selection;
          data2 = g_new (gfloat, terrain->width * terrain->height);
          for (x = 0; x < width; x++)
            for (y = 0; y < height; y++)
              data2[x * height + y] = data[y * width + (width - x - 1)];

          terrain->selection = data2;
          g_free (data);
        }

      if (terrain->riverfield != NULL)
        {
          gfloat *river, *river2;

          river = terrain->riverfield;
          river2 = g_new (gfloat, terrain->width * terrain->height);
          for (x = 0; x < width; x++)
            for (y = 0; y < height; y++)
              river2[x * height + y] = river[y * width + (width - x - 1)];

          terrain->riverfield = river2;
          g_free (river);
	}

      if(terrain->riverfield!= NULL && terrain->riversources->len>0)
        for(i=0; i<terrain->riversources->len; i++)
          {
          gfloat tempx, tempy;
          coords = &g_array_index (terrain->riversources, TTerrainRiverCoords, i);
          tempx = coords->x;
          tempy = coords->y;
          coords->x = tempy;
          coords->y = 1.0 - tempx;
          }

      if (terrain->objects->len > 0)
        {
          gint i;
          gint size = terrain->objects->len;
	  gint xlim, ylim;

	  xlim = terrain->width-1;
	  ylim = terrain->height-1;

	  for (i=0; i<size; i++)
            {
	    TTerrainObject *object;
	    gfloat px, py;

            object = &g_array_index (terrain->objects, TTerrainObject, i);
            px = object->x;
            py = object->y;
	    object->x = (1-(1-py)); 
	    object->y = (1-px);

            px = object->ox;
            py = object->oy;
	    object->ox = (xlim-(xlim-py)); 
	    object->oy = (ylim-px);
            }
        }

      terrain->width = height;
      terrain->height = width;
    }

  t_terrain_set_modified (terrain, TRUE);
}

void
t_terrain_terrace (TTerrain *terrain,
                   gint      level_count,
                   gfloat    factor,
                   gboolean  adjust_sealevel)
{
  gint   i, size;
  gint   level;
  gfloat range = 1.0, level_range, base, diff;

  /* increase level count by one so that the highest elevation (1.0) gets 
     the last level (which is 1 point high). */

  level_count++;
  factor = 1 - factor; /* convert tightness scale */

  level_range = range / level_count;

  size = terrain->width * terrain->height;
  for (i = 0; i < size; i++)
    {
      gfloat value;

      level = (gint) (terrain->heightfield[i] / level_range);
      base = level * level_range;
      diff = terrain->heightfield[i] - base;

      value = base + diff * factor;

      APPLY (terrain, i, value);
    }

  t_terrain_set_modified (terrain, TRUE);
}


static gfloat
threshold (gfloat x, gfloat power, gfloat midpoint)
{
  if (x < midpoint)
    return pow (x * (1.0 / midpoint), power) * 0.5;

  return 1.0 - pow ((1.0 - x) * (1.0 / (1.0 - midpoint)), power) * 0.5;
}

static gfloat
transform (gfloat x, 
           gfloat sea_threshold,
           gfloat sea_depth,
           gfloat sea_dropoff,
           gfloat above_power,
           gfloat below_power)
{
  if (x <= sea_threshold)
    {
      /* Apply a threshold function to terrain underwater. */

      return threshold (x / sea_threshold, below_power, sea_dropoff) *
             sea_depth;
    }

  /* Above the sea threshold */
  return pow ((x - sea_threshold) / (1.0 - sea_threshold), above_power) *
         (1.0 - sea_depth) + sea_depth;
}

void
t_terrain_draw_transform (TPixbuf  *pixbuf,
                          gfloat    sea_threshold,
                          gfloat    sea_depth,
                          gfloat    sea_dropoff,
                          gfloat    above_power,
                          gfloat    below_power)
{
  gint i;
  gint sea_line_1, sea_line_2, sea_line_3;

  /* TPixbuf should refer to a drawable of size: 250 x 102 */

  t_pixbuf_set_back (pixbuf, 255, 255, 255);
  t_pixbuf_clear (pixbuf);

  sea_line_1 = (gint) rint (sea_threshold * 100.0);
  sea_line_2 = (gint) rint (sea_depth * 100.0);
  sea_line_3 = (gint) rint (sea_threshold * sea_dropoff * 100.0);

  t_pixbuf_set_fore (pixbuf, 0, 0, 255);
  t_pixbuf_draw_line (pixbuf, 1, 100 - sea_line_1, 100, 100 - sea_line_1);
  t_pixbuf_draw_line (pixbuf, 149, 100 - sea_line_2, 248, 100 - sea_line_2);
  t_pixbuf_draw_arrow (pixbuf, 104, 100 - sea_line_1, 145, 100 - sea_line_2);

  t_pixbuf_set_fore (pixbuf, 192, 192, 192);
  t_pixbuf_draw_line (pixbuf, 149 + sea_line_3, 1, 149 + sea_line_3, 100);

  t_pixbuf_set_fore (pixbuf, 0, 0, 0);
  t_pixbuf_draw_rect (pixbuf, 0, 0, 101, 101);
  t_pixbuf_draw_rect (pixbuf, 148, 0, 249, 101);
  for (i = 0; i < 100; i++)
    t_pixbuf_draw_pel (pixbuf, i + 1, 100 - i);

  for (i = 0; i < 100; i++)
    {
      gfloat y;

      y = transform ((i + 0.5) / 100.0,
                     sea_threshold, sea_depth, sea_dropoff,
                     above_power, below_power) * 100.0;
      t_pixbuf_draw_pel (pixbuf, i + 150, 100 - ((gint) (y + 0.5)));
    }
}

void
t_terrain_transform (TTerrain *terrain,
                     gfloat    sea_threshold,
                     gfloat    sea_depth,
                     gfloat    sea_dropoff,
                     gfloat    above_power,
                     gfloat    below_power)
{
  gint i, size;

  size = terrain->width * terrain->height;
  for (i = 0; i < size; i++)
    {
      gfloat value;

      value =
        transform (terrain->heightfield[i],
                   sea_threshold, sea_depth, sea_dropoff,
                   above_power, below_power);

      APPLY (terrain, i, value);
    }

  //t_terrain_normalize (terrain, FALSE);
  t_terrain_set_modified (terrain, TRUE);
}

void
t_terrain_tiler (TTerrain *terrain,
                 gfloat    offset)
{
  gint    x, y;
  gint    width, height;
  gint    margin;
  gfloat  coeff, step;
  gfloat *data;

  width = terrain->width;
  height = terrain->height;
  data = terrain->heightfield;

  margin = (gint) (MIN (width, height) * offset);
  step = 1.0 / margin;

  for (x = 0; x < width; x++)
    for (y = 0, coeff = 0.0; y < margin; y++, coeff += step)
      {
      gfloat v = data[y * width + x] * (1.0 - coeff) +
                 data[(height - y - 1) * width + x] * coeff;
      APPLY (terrain, (height-y-1)*width+x, v);
      }

  for (y = 0; y < height; y++)
    for (x = 0, coeff = 0.0; x < margin; x++, coeff += step)
      {
      gfloat v = data[y * width + x] * (1.0 - coeff) +
                 data[y*width+(width-x-1)] * coeff;
      APPLY (terrain, y*width+(width-x-1), v);
      }

  t_terrain_set_modified (terrain, TRUE);
}

void t_terrain_tile (TTerrain *terrain,
                     gint      multiply)
{
  TTerrain *terrain_clone;
  gint      i, j, y, x, pos1, pos2;

  terrain_clone = t_terrain_clone (terrain);
  t_terrain_set_size (terrain, terrain->width * multiply,
                               terrain->height * multiply);
  pos1 = 0;
  for (i = 0; i < multiply; i++)
    for (y = 0; y < terrain_clone->height; y++)
      for (j = 0; j < multiply; j++)
        {
          pos2 = y * terrain_clone->width;

          for (x = 0; x < terrain_clone->width; x++)
            terrain->heightfield[pos1++] = terrain_clone->heightfield[pos2++];
        }

  t_terrain_unref (terrain_clone);

  t_terrain_set_modified (terrain, TRUE);
}

/* t_terrain_spherical: Convert the terrain into a form useable as
 * a POV-Ray spherical_map.
 */
void
t_terrain_spherical (TTerrain *terrain,
                     gfloat    offset)
{
  gint    x, y;
  gint    width;
  gint    height;
  gfloat *raster;

  g_return_if_fail (offset >= 0.0);
  g_return_if_fail (offset <= 1.0);

  width = terrain->width;
  height = terrain->height;
  raster = g_new (gfloat, width);

  for (y = 0; y < height; y++)
    {
      gfloat  *data;
      gdouble  scale;
      gdouble  from_x;
      gdouble  fract_x;
      int      int_x;

      scale = ((y * 2.0) / (height - 1)) - 1.0;
      scale = sqrt (1.0 - scale * scale);

      data = &terrain->heightfield[y * width];
      memcpy (raster, data, width * sizeof (gfloat));

      /* Stretch a row of the terrain outwards */
      for (x = 0; x < width; x++)
        {
          gdouble a, b;
          gdouble height;

          from_x = (x - width / 2.0) * scale + width / 2.0;
          int_x = (int) from_x;
          fract_x = from_x - int_x;

          if (int_x < 0)
            a = 0.0;
          else
            a = raster[int_x];

          if (int_x + 1 >= width)
            b = 0.0;
          else
            b = raster[int_x + 1];

          height = a * (1.0 - fract_x) + b * fract_x;
          data[x] = height;
        }

      from_x = (width * offset * 0.5 - width / 2.0) * scale + width / 2.0;
      int_x = (int) from_x;

      /* Make the terrain tilable in the X direction */
      for (x = 0; x < int_x; x++)
        {
          gdouble k;

          k = ((gdouble) x) / int_x;
          data[width - x - 1] = data[width - x - 1] * k + data[x] * (1.0 - k);
        }
    }

  g_free (raster);

  t_terrain_set_modified (terrain, TRUE);
}

#define NO_FLOW         -987654321  /* marker for 'undefined' */

/* find greatest common denominaor */
static gint 
greatest_common_denominator (gint a, gint b) 
{
  if (a%b==0) 
    return b; 
  else 
    return greatest_common_denominator (b, a%b);
}

/* 
 * return the sum of the neightbouring cells in a square size with 
 * sides 1+(size*2) long
 */
static gfloat 
grid_neighbour_sum_size  (TTerrain *terrain, 
                          gint      x, 
                          gint      y,
                          gint      size)
{
  gint   xx;
  gint   yy;
  gint   xoff = MAX (x-size, 0);
  gint   yoff = MAX (y-size, 0);
  gint   xlim = MIN (x+size, terrain->width-1);
  gint   ylim = MIN (y+size, terrain->height-1);
  gfloat sum = 0;

  g_return_val_if_fail (x>=0, -1);
  g_return_val_if_fail (y>=0, -1);
  g_return_val_if_fail (size>0, -1);

  for (xx=xoff; xx<=xlim; xx++)
    for (yy=yoff; yy<=ylim; yy++)
      if (xx!=x || yy!=y)
        sum += terrain->heightfield[yy*terrain->width+xx];

  return sum;
} 

static gfloat
grid_neighbour_average_size (TTerrain *terrain, 
                             gint      x, 
                             gint      y,
                             gint      size)
{
  gint   xoff = MAX (x-size, 0);
  gint   yoff = MAX (y-size, 0);
  gint   xlim = MIN (x+size, terrain->width-1);
  gint   ylim = MIN (y+size, terrain->height-1);

  gfloat num = (((xlim-xoff)+1) * ((ylim-yoff)+1)) - 1;
  gfloat sum;

  g_return_val_if_fail (num>0, -1);
  sum = grid_neighbour_sum_size (terrain, x, y, size);

  return sum/num;
}

static gfloat
grid_neighbour_average (TTerrain *terrain, 
                        gint      x, 
                        gint      y)
{
  return grid_neighbour_average_size (terrain, x, y, 1);
}

static gboolean
is_lowest_point_size (TTerrain *terrain, 
                      gint      x, 
                      gint      y,
                      gint      size)
{
  gint   xx;
  gint   yy;
  gint   xoff;
  gint   yoff;
  gint   xlim;
  gint   ylim;
  gfloat elv;
  gfloat diff;

  g_return_val_if_fail (x>=0, FALSE);
  g_return_val_if_fail (y>=0, FALSE);
  g_return_val_if_fail (size>0, FALSE);

  xoff = MAX (x-size, 0);
  yoff = MAX (y-size, 0);
  xlim = MIN (x+size, terrain->width-1);
  ylim = MIN (y+size, terrain->height-1);

  elv  = terrain->heightfield[y*terrain->width+x]; 

  for (xx=xoff; xx<=xlim; xx++)
    for (yy=yoff; yy<=ylim; yy++)
      {
      diff = terrain->heightfield[yy*terrain->width+xx] - elv;
      if (diff < 0)
        return FALSE;
      }

  return TRUE;
} 

static gboolean
is_lowest_point (TTerrain *terrain, 
                 gint      x, 
                 gint      y)
{
  return is_lowest_point_size (terrain, x, y, 1);
}

/*
 * t_terrain_fill_basins: fill the terrains' basins. 
 */
void
t_terrain_fill_basins (TTerrain *terrain, gint iterations, gboolean big_grid)
{
  gint xsize, ysize;
  gint gridsize = 1;
  gint x, y, i;
  gfloat *tmpdata;

  xsize = terrain->width;
  ysize = terrain->height;
  tmpdata = g_new0 (gfloat, xsize * ysize);
  memcpy (tmpdata, terrain->heightfield, sizeof(gfloat)*xsize*ysize);
  if (big_grid) 
    gridsize = 2;

  for(i=0; i<iterations; i++)
    {
    gint count = 0;

    memcpy (tmpdata, terrain->heightfield, sizeof(gfloat)*xsize*ysize);

    for(y=0; y<ysize; y++)
      for (x=0; x<xsize; x++)
        {
        gint   off = y*xsize+x;
	gfloat sel;

        if (terrain->selection == NULL)
          sel = 1.0;
        else
          sel = terrain->selection[off];

        if (sel>0.0 && is_lowest_point (terrain, x, y))  
          {
	  tmpdata[off] = (sel*grid_neighbour_average_size(terrain,x,y,gridsize) + 
			  (1.0-sel) * terrain->heightfield[off]);
	  count++;
          }
        else 
	  tmpdata[off] = terrain->heightfield[off];
        }

    if (count == 0)
      i = iterations;

    memcpy (terrain->heightfield, tmpdata, sizeof(gfloat)*xsize*ysize);
    }

  g_free (tmpdata);
} 

static void
t_terrain_iterate_flowmap (TTerrain *tflowmap)
{
  gint    i;
  gint    size; 
  gfloat *data = tflowmap->heightfield;

  size = tflowmap->width * tflowmap->height;
  for (i=0; i<size; i++)
    data[i] = pow (data[i], 1.15);

  t_terrain_normalize (tflowmap, FALSE);
}

/**
 *  upslope_sfd: find the number of single-flow-direction
 *      upslope cells for a given cell. Cache is used to
 *      to store computed SFD values.
 */
static gint
t_terrain_upslope_sfd (TTerrain *terrain,
                       gint      given_x,
                       gint      given_y,
                       gfloat   *cache)
{
  if (cache[given_y * terrain->width + given_x] == NO_FLOW)
    {
      gboolean found;
      gint     count;
      gint     upflow_x, upflow_y;
      gint     x_low, x_high;
      gint     y_low, y_high;
      gint     x, y;

      upflow_x = given_x;
      upflow_y = given_y;

      found = FALSE;
      x_low = MAX (upflow_x - 1, 0);
      x_high = MIN (upflow_x + 1, terrain->width - 1);
      y_low = MAX (upflow_y - 1, 0);
      y_high = MIN (upflow_y + 1, terrain->height - 1);

      /* find the highest downflow neighbour */
      for (x = x_low; x <= x_high; x++)
        for (y = y_low; y <= y_high; y++)
          {
            if (terrain->heightfield[y * terrain->width + x] >
                terrain->heightfield[upflow_y * terrain->width + upflow_x])
              {
                found = TRUE;
                upflow_x = x;
                upflow_y = y;
              }
          }

      /* downflow */
      if (found)
        count = t_terrain_upslope_sfd (terrain, upflow_x, upflow_y, cache) + 1;
      else
        count = 0;

      cache[given_y * terrain->width + given_x] = count;

      return count;
    }

  return cache[given_y * terrain->width + given_x];
}


/**
 *  upslope_mfd: find the number of multiple-flow-direction
 *      upslope cells for a given cell. hitcache should be
 *      re-initialized every time flowmap() calls this
 *      function since it's being used as an indicator
 *      that a cell has been counted in the current MFD
 *      pass.
 */
static gfloat
t_terrain_upslope_mfd (TTerrain *terrain,
                       gint      given_x,
                       gint      given_y,
                       gfloat   *cache,
                       gboolean *hitcache)
{
  gint   x, y;
  gint   x_low, x_high;
  gint   y_low, y_high;
  gfloat count;

  count = 0.0;

  x_low = MAX (given_x - 1, 0);
  x_high = MIN (given_x + 1, terrain->width - 1);
  y_low = MAX (given_y - 1, 0);
  y_high = MAX (given_y + 1, terrain->height - 1);

  for (x = x_low; x <= x_high; x++)
    for (y = y_low; y <= y_high; y++)
      {
	gint yoff            = y * terrain->width;
	gint pos             = yoff + x;
	gint gpos            = given_y * terrain->width + given_x;

        if (hitcache[pos] == FALSE &&
            terrain->heightfield[pos] > terrain->heightfield[gpos])
	    {
	    if (cache[pos] == NO_FLOW)
	      {
	      hitcache[pos] = TRUE;
              count += t_terrain_upslope_mfd (terrain, x, y, cache, hitcache) +
                                   ((x == given_x || y == given_y) ? 0.6 : 0.4);
              cache[pos] = count;
	      }
	    else
	      {
              count += cache[pos];
	      }
	    }
      }

  return count;
}


/**
 *  flowmap: calculate the flowmap for the height field as detailed in
 *    "Comparison of single and mulitple flow direction algorithms for
 *    computing topographic parameters in TOPMODEL", Wolock & McCabe Jr.,
 *    Water Resources Research, Vol. 31, No. 5, Pages 1315-1324, May 1995
 *
 * /verbatim
 *  The flowmap is essentially   ln(a/tan(b))   where
 *      a = A/C
 *      C = contour length
 *      A = (number of upslope cells + 1) * (grid cell area)
 *      tan(b) = ( change in elevation with neighboring cell /
 *                              cell center distance )
 *      ln = Napierian Log =
 *                                 log (n/10^7)
 *              ln (n) =     -  -------------------
 *                              log (10^7/(10^7-1))
 * /endverbatim
 *
 *  Interestingly enough, the paper uses the Napierian Log, which
 *  in my case gives huge numbers at the top of the scale and zero
 *  at the bottom. I find that using the natural log I get a
 *  range which closely resembles the ones shown in the paper.
 *  Does anybody out there (who has found this) know more about this?
 *  I've probably goofed up somewhere but don't know where.
 */
TTerrain *
t_terrain_flowmap (TTerrain *terrain,
                   gboolean  do_sfd,
                   gboolean  ignore_sealevel,
                   gfloat    max_elevation_erode)
{
  gfloat     elevation_rt = 0.33;
  gint       i, x, y, xx, yy;
  gint       offxy;
  gint       dfx, dfy;                       /* upflow x & y coordinates */
  gboolean   found;
  gint       x_low, x_high;
  gint       y_low, y_high;
  gint       size;
  gdouble    C = 10.0;                       /* contour length XXXXXXX */
  gdouble    gca = C * C;                    /* Grid Cell Area */
  gdouble    a, A;
  gfloat     fmin = -NO_FLOW;         /* -NO_FLOW is positive */
  gfloat    *cache;                   /* SFD/MFD cache */
  gfloat    *flowmap;                 /* flowmap */
  gboolean  *hitcache;                   /* MFD cache */
  TTerrain  *terrain_out;
  GtkObject *terrain_object;

  terrain_object = t_terrain_new (terrain->width, terrain->height);
  terrain_out = T_TERRAIN (terrain_object);

  flowmap = terrain_out->heightfield;
  size = terrain->width * terrain->height;

  cache = g_new (gfloat, size);
  for (i = 0; i < size; i++)
    cache[i] = fmin;

  if (do_sfd)
    hitcache = NULL;
  else
    hitcache = g_new (gboolean, size);

  for (y = 0; y < terrain->height; y++)
    for (x = 0; x < terrain->width; x++)
      if (ignore_sealevel ||
          (terrain->heightfield[y * terrain->width + x] > terrain->sealevel &&
           terrain->heightfield[y * terrain->width + x] < max_elevation_erode))
        {
          if (do_sfd)
            A = t_terrain_upslope_sfd (terrain, x, y, cache) * gca;
          else
            {
              for (i = 0; i < size; i++)
                hitcache[i] = FALSE;

              A = t_terrain_upslope_mfd (terrain, x, y, cache, hitcache) * gca;
            }
          a = A / C;

          /* find lowest downslope square */
          found = FALSE;
          dfx = x;
          dfy = y;
          x_low = MAX (dfx - 1, 0);
          x_high = MIN (dfx + 1, terrain->width - 1);
          y_low = MAX (dfy - 1, 0);
          y_high = MIN (dfy + 1, terrain->height - 1);

          for (xx = x_low; xx <= x_high; xx++)
            for (yy = y_low; yy <= y_high; yy++)
              {
                if (terrain->heightfield[yy * terrain->width + xx] <
                    terrain->heightfield[dfy * terrain->width + dfx])
                  {
                    found = TRUE;
                    dfx = xx;
                    dfy = yy;
                  }
              }

          /* there is a downflow */
          offxy = y * terrain->width + x;
          if (found && a != 0.0)
            {
              gdouble tanb;

              tanb = (terrain->heightfield[offxy] -
                      terrain->heightfield[dfy * terrain->width + dfx]) *
                      terrain->width * elevation_rt / C;

              if (fabs (tanb) >= 0.00001)
                {
                  flowmap[offxy] = log (a / tanb);

                  if (flowmap[offxy] < fmin)
                    fmin = flowmap[offxy];
                }
              else
               flowmap[offxy] = NO_FLOW;
            }
          /* no downflow */
          else
            flowmap[offxy] = NO_FLOW;
        }
      else
        flowmap[y * terrain->width + x] = NO_FLOW;

  /* make 0.0 lowest value so that NO_FLOW can equal 0.0 */
  for (i = 0; i < size; i++)
    if (flowmap[i] == NO_FLOW)
      flowmap[i] = 0.0;
    else
      flowmap[i] -= fmin;

  t_terrain_normalize (terrain_out, FALSE);
  terrain_out->sealevel = 0;

  g_free (cache);
  g_free (hitcache);

  t_terrain_set_modified (terrain_out, TRUE);

  return terrain_out;
}

/**
 * erode: erode the HF using the flowmap to determine the terrain delta
 */
gint
t_terrain_erode_flowmap (TTerrain *terrain,
                         TTerrain *flowmap,
                         gint      iterations,       // # of erosion iterations
                         gboolean  trim_local_peaks) // trim if (flowmap==0)
{
  gint      i, x, y;
  gdouble   erosion_rate = 0.01;

  for (i=0; i<iterations; i++)
    {
    gint pos = 0;

    t_terrain_fill_basins (terrain, 1, FALSE);

    for (y=0; y<terrain->height; y++)
      for (x=0; x<terrain->width; x++, pos++)
        {
        if (flowmap->heightfield[pos] > 0.0)
          {
          APPLY (terrain, pos, 
                   (terrain->heightfield[pos]-
                     (erosion_rate*flowmap->heightfield[pos])));
	  }
        else
          {
            /* average flowmap value with neighbors */
            if (trim_local_peaks)
              {
                gint   count;
                gfloat sum;

                count = 0;
                sum = 0;

                if (x > 0)
                  count++, sum += flowmap->heightfield[pos - 1];

                if (x < terrain->width - 1)
                  count++, sum += flowmap->heightfield[pos + 1];

                if (y > 0)
                  count++, sum +=
                    flowmap->heightfield[pos - terrain->width];

                if (y < terrain->height - 1)
                  count++, sum +=
                    flowmap->heightfield[pos + terrain->width];

                APPLY (terrain, pos, 
                       terrain->heightfield[pos]-(erosion_rate*sum/count));
              }
            else
              {
                APPLY (terrain, pos, 
                         (terrain->heightfield[pos]-
		           (erosion_rate*flowmap->heightfield[pos])));
              }
          }

        if (terrain->heightfield[pos] < 0.0)
          terrain->heightfield[pos] = 0.0;
        }

    t_terrain_roughen_smooth (terrain, FALSE, FALSE, 0.05);
    }

  return 0;
}

static gint
validate_erode_filenames (gchar **filenames)
{
  gint      i;

  /* filenames should be a sprintf format string and contain %s and %d */
  for (i=0; filenames[i]!=NULL; i++)
    {
    gchar *filename = filenames[i];

    if (filename != NULL)
      {
      /* Should only be more than 1 '%' sign in the filename */
      if (strchr (filename, '%') == strrchr (filename, '%'))
        return -1;

      /* Should have a '%s' in the filename */
      if (strstr (filename, "%s") == NULL)
        return -1;

      /* Should have a '%d' in the filename */
      if (strstr (filename, "%d") == NULL)
        return -1;
      }
    }

  return 0;
}

/**
 * erode: erode the HF using the flowmap to determine the terrain delta
 */
gint
t_terrain_erode (TTerrain *terrain,
                 gint      iterations,          // # of erosion iterations
                 gint      max_flow_age,        // new flowmap every n iters
                 gint      age_flow_times,      // how many times to age flowmap
                 gfloat    max_elevation_erode, // above this no erosion
                 gchar    *filename_anim,       // if != NULL save animation
                 gchar    *filename_flow,       // if != NULL save flowmap
                 gint      n_frames,            // # of frames in animation
                 gboolean  trim_local_peaks,    // trim if (flowmap==0)
                 gboolean  do_sfd,              // flowmap param
                 gboolean  ignore_sealevel,     // flowmap param
                 TPixbuf  *pixbuf)              // for screen refresh
{
  gint      i;
  gint      frame_lim = 0;
  gint      gcd; 
  gchar    *filename_array[] = { filename_anim, filename_flow, NULL };
  TTerrain *flowmap_terrain = NULL;

  /* sanity check */
  if (validate_erode_filenames (filename_array))
    return -1;

  /* force parameters to be sensical */
  if (max_flow_age > iterations)
    max_flow_age = iterations;

  frame_lim = iterations/n_frames;

  /* greatest common denominator */
  gcd = greatest_common_denominator (max_flow_age, frame_lim);

  for (i=0; i<iterations; i+=gcd)
    {
      gint     fmap_count = i/max_flow_age;
      gboolean do_new_flowmap = !(i%max_flow_age);
      gboolean do_age_flowmap = (age_flow_times<=0 ? FALSE : 
		                   !(fmap_count%(age_flow_times+1)));

      if (do_new_flowmap)
        {
        /* create a new flowmap or iterate/age it */
	if (!do_age_flowmap)
          {
          if (flowmap_terrain) 
            t_terrain_unref (flowmap_terrain);

          flowmap_terrain = t_terrain_flowmap (terrain, do_sfd, 
      		               ignore_sealevel, max_elevation_erode);
          }
	else
          {
          if (flowmap_terrain == NULL)
            flowmap_terrain = t_terrain_flowmap (terrain, do_sfd, 
      		               ignore_sealevel, max_elevation_erode);

          t_terrain_iterate_flowmap (flowmap_terrain);
          }

        /* do we have a filename to save the flowmap with? */
        if (filename_flow != NULL)
          {
          gchar *fnt;
          gchar *filename_current;

          fnt = filename_get_base_without_extension_and_dot (terrain->filename);
          filename_current = g_strdup_printf (filename_flow,fnt,fmap_count);
          t_terrain_normalize (flowmap_terrain, FALSE);
          t_terrain_save (flowmap_terrain, FILE_AUTO, filename_current);

          g_free (filename_current);
          g_free (fnt);
          }
        }

      /* erode for max_flow_age teerains */
      t_terrain_erode_flowmap (terrain, flowmap_terrain, 
		               gcd, trim_local_peaks);

      /* redraw screen */
      if (pixbuf != NULL)
        {
              /* FIXME, repaint pixbuf */
        }

      /* save eroded terrain */
      if (filename_anim)
        {
        if (!(i%frame_lim))
          {
          gchar *fnt;
          gchar *filename_current;
          gint   frame_count = i/frame_lim;

          fnt = filename_get_base_without_extension_and_dot (terrain->filename);
          filename_current = g_strdup_printf (filename_anim, fnt, frame_count);
          t_terrain_normalize (terrain, FALSE);
          t_terrain_save (terrain, FILE_AUTO, filename_current);

          g_free (filename_current);
          g_free (fnt);
          }
        }

      t_terrain_normalize (terrain, FALSE);
    }

  t_terrain_unref (flowmap_terrain);
  t_terrain_set_modified (terrain, TRUE);

  return 0;
}

void
t_terrain_roughen_smooth (TTerrain *terrain,
                          gboolean  roughen,
                          gboolean  big_grid,
                          gfloat    factor)
{
  gfloat *data;
  gint    width, height;
  gint    x, y;

  g_return_if_fail (factor >= 0.0);
  g_return_if_fail (factor <= 1.0);

  data = terrain->heightfield;
  width = terrain->width;
  height = terrain->height;

  for (y = 1; y < height - 1; y++)
    for (x = 1; x < width - 1; x++)
      {
        gint    pos = y * width + x;
        gint    size = (big_grid ? 2 : 1);
        gfloat  value, original, average;

        original = data[pos];
	average = grid_neighbour_average_size (terrain, x, y, size);

        if (roughen)
          value = original - factor * (average - original);
        else
          value = original + factor * (average - original);

        APPLY (terrain, pos, value);
      }

  t_terrain_set_modified (terrain, TRUE);
}

TTerrain *
t_terrain_join (TTerrain *terrain_1,
                TTerrain *terrain_2,
                gfloat    distance, 
		gboolean  reverse_dir,
		gboolean  x_axis)
{
  TTerrain *terrain;
  gint      x, y;
  gint      offset_1;
  gint      offset_2;
  gint      width, width_1, width_2;
  gint      height, height_1, height_2;
  gfloat    dist;
  gfloat   *data, *data_1, *data_2;

  GtkObject *terrain_object;

  if (!x_axis)
    {
    terrain_object = t_terrain_new (MIN(terrain_1->width, terrain_2->width),
                                    (terrain_1->height + terrain_2->height));
    terrain = T_TERRAIN (terrain_object);

    offset_1 = (terrain_1->width - terrain->width) >> 1;
    offset_2 = (terrain_2->width - terrain->width) >> 1;

    }
  else
    {
    terrain_object = t_terrain_new ((terrain_1->width + terrain_2->width),
                                    MIN(terrain_1->height, terrain_2->height));
    terrain = T_TERRAIN (terrain_object);

    offset_1 = (terrain_1->height - terrain->height) >> 1;
    offset_2 = (terrain_2->height - terrain->height) >> 1;
    }

  data = terrain->heightfield;
  data_1 = terrain_1->heightfield;
  data_2 = terrain_2->heightfield;

  width = terrain->width;
  width_1 = terrain_1->width;
  width_2 = terrain_2->width;
  height = terrain->height;
  height_1 = terrain_1->height;
  height_2 = terrain_2->height;

  //printf ("t: %d, %d\n", terrain->width, terrain->height);
  //printf ("t1: %d, %d\n", terrain_1->width, terrain_1->height);
  //printf ("t2: %d, %d\n\n", terrain_2->width, terrain_2->height);

  if (!x_axis)
    {
    dist = MIN(height_1, height_2)/2*distance;

    /* populate the terrain */
    for (y=0; y<height_1; y++)
      for (x=0; x<width; x++)
        data[y*width+x] = data_1[y*width_1+x+offset_1];

    for (y=0; y<height_2; y++)
      for (x=0; x<width; x++)
        data[(y+height_1)*width+x] = data_2[y*width_2+x+offset_2];

    /* now smooth the resulting join area */
    for (y=0; y<(gint)dist; y++)
      for (x=0; x<width; x++)
        {
        gint off1, off2;
        gfloat f, v1, v2;

        if (reverse_dir)
          {
          off1 = (height_1-dist+y)*width+x;
          off2 = (height_1+dist-y-1)*width+x;
          }
        else
          {
          off2 = (height_1-dist+y)*width+x;
          off1 = (height_1+dist-y-1)*width+x;
          }

        f = y/dist;
        v1 = data[off1];
        v2 = data[off2];

        data[off1] = (1-f)*v1 + (f*v2);
        }
    }
  else
    {
    dist = MIN(width_1, width_2)/2*distance;

    /* populate the terrain */
    for (x=0; x<width_1; x++)
      for (y=0; y<height; y++)
        data[y*width+x] = data_1[y*width_1+x+offset_1];

    for (x=0; x<width_2; x++)
      for (y=0; y<height; y++)
        data[y*terrain->width+x+width_1] = data_2[y*width_2+x+offset_2];

    /* now smooth the resulting join area */
    for (x=0; x<(gint)dist; x++)
      for (y=0; y<height; y++)
        {
        gint off1, off2;
        gfloat f, v1, v2;

        if (reverse_dir)
          {
          off1 = y*width+width_1-dist+(dist-x);
          off2 = y*width+width_1+dist-(dist-x);
          }
        else
          {
          off1 = y*width+(width_1+dist-x-1);
          off2 = y*width+(width_1-dist+x);
          }

        f = x/dist;

	if (reverse_dir)
          f = 1-f;

        v1 = data[off1];
        v2 = data[off2];

        data[off1] = (1-f)*v1 + f*v2;
        }
    }

  t_terrain_set_modified (terrain, TRUE);

  return terrain;
}

TTerrain *
t_terrain_merge (TTerrain *terrain_1,
                 TTerrain *terrain_2,
                 gfloat    weight_1,
                 gfloat    weight_2,
                 gint      operation)
{
  TTerrain *terrain;
  gint      x, y;
  gint      offset_x_1, offset_y_1;
  gint      offset_x_2, offset_y_2;
  gint      width_1, width_2;
  gfloat   *data, *data_1, *data_2;

  if (operation >= 0 && operation <= 5)
    {
      GtkObject *terrain_object;

      terrain_object =
        t_terrain_new (MIN (terrain_1->width, terrain_2->width),
                       MIN (terrain_1->height, terrain_2->height));
      terrain = T_TERRAIN (terrain_object);

      offset_x_1 = (terrain_1->width - terrain->width) >> 1;
      offset_y_1 = (terrain_1->height - terrain->height) >> 1;
      offset_x_2 = (terrain_2->width - terrain->width) >> 1;
      offset_y_2 = (terrain_2->height - terrain->height) >> 1;

      data = terrain->heightfield;
      data_1 = terrain_1->heightfield;
      data_2 = terrain_2->heightfield;

      width_1 = terrain_1->width;
      width_2 = terrain_2->width;
    }
  else
    return NULL;

  switch (operation)
    {
    case 1: /* Subtract */
      weight_2 = -weight_2;

    case 0: /* Addition */
      for (y = 0; y < terrain->height; y++)
        for (x = 0; x < terrain->width; x++)
          data[y * terrain->width + x] =
            data_1[(offset_y_1 + y) * width_1 + (offset_x_1 + x)] * weight_1 +
            data_2[(offset_y_2 + y) * width_2 + (offset_x_2 + x)] * weight_2;
      break;

    case 2: /* Multiplication */
      for (y = 0; y < terrain->height; y++)
        for (x = 0; x < terrain->width; x++)
          data[y * terrain->width + x] =
            (1.0 + data_1[(offset_y_1 + y) * width_1 + (offset_x_1 + x)] * weight_1) *
            (1.0 + data_2[(offset_y_2 + y) * width_2 + (offset_x_2 + x)] * weight_2);
      break;

    case 3: /* Division */
      for (y = 0; y < terrain->height; y++)
        for (x = 0; x < terrain->width; x++)
          data[y * terrain->width + x] =
            (1.0 + data_1[(offset_y_1 + y) * width_1 +
                          (offset_x_1 + x)] * weight_1) /
            (1.0 + data_2[(offset_y_2 + y) * width_2 +
                          (offset_x_2 + x)] * weight_2);
      break;

    case 4: /* Minimum */
      for (y = 0; y < terrain->height; y++)
        for (x = 0; x < terrain->width; x++)
          data[y * terrain->width + x] = MIN (
            data_1[(offset_y_1 + y) * width_1 + (offset_x_1 + x)] * weight_1,
            data_2[(offset_y_2 + y) * width_2 + (offset_x_2 + x)] * weight_2);
      break;

    case 5: /* Maximum */
      for (y = 0; y < terrain->height; y++)
        for (x = 0; x < terrain->width; x++)
          data[y * terrain->width + x] = MAX (
            data_1[(offset_y_1 + y) * width_1 + (offset_x_1 + x)] * weight_1,
            data_2[(offset_y_2 + y) * width_2 + (offset_x_2 + x)] * weight_2);
      break;
    }

  t_terrain_set_modified (terrain, TRUE);

  return terrain;
}

/*
 * t_terrain_half: half resolution by averaging 4 points into 1
 * This routine is adapted from John Beales's HF-Lab.
 */

void
t_terrain_half (TTerrain *terrain)
{
  TTerrain *clone;
  TTerrainRiverCoords *coords;
  gint      x, y, pos1, pos2;

  clone = t_terrain_clone (terrain);
  t_terrain_set_size (terrain, clone->width >> 1, clone->height >> 1);

  if (clone->selection != NULL)
    terrain->selection = g_new (gfloat, terrain->width * terrain->height);

  if (clone->riverfield != NULL)
    terrain->riverfield = g_new (gfloat, terrain->width * terrain->height);

  pos1 = 0;
  for (y = 0; y < terrain->height; y++)
    {
      pos2 = (y << 1) * clone->height;
      for (x = 0; x < terrain->width; x++)
        {
          terrain->heightfield[pos1] =
            (clone->heightfield[pos2] +
             clone->heightfield[pos2 + 1] +
             clone->heightfield[pos2 + clone->width] +
             clone->heightfield[pos2 + clone->width + 1]) * 0.25;

          if (terrain->selection != NULL)
            terrain->selection[pos1] = clone->selection[pos2];

	  if (terrain->riverfield != NULL)
            terrain->riverfield[pos1] = clone->riverfield[pos2];

          pos1++;
          pos2 += 2;
        }
    }

  if (clone->riversources->len > 0)
    for (x=0; x<clone->riversources->len; x++)
      {
      coords = &g_array_index (terrain->riversources, TTerrainRiverCoords, x);
      coords->x /= 2;
      coords->y /= 2;
      }

  t_terrain_unref (clone);

  t_terrain_set_modified (terrain, TRUE);
}

#define MOD(terrain,x,y) (terrain->heightfield[((x)%terrain->width) + ((y)%terrain->height) * terrain->width])
#define CLIP(terrain,x,y) (terrain->heightfield[MIN(MAX(x,0),terrain->width - 1) + MIN(MAX(y,0),terrain->height - 1) * terrain->width])

/*
 * t_terrain_double:
 *   Double resolution using modified-midpoint-displacement
 *   algorithm; different versions for tiling and non-tiling
 *   matrices. <local> is local fractal scaling, <global> is overall
 *   fractal scaling.
 *
 *   This routine is adapted from John Beales's HF-Lab.
 */

void
t_terrain_double (TTerrain *terrain,
                  gfloat    local,
                  gfloat    global)
{
  TTerrain *original;
  TRandom  *gauss;
  GArray   *sources;
  gint      x, y, pos1, pos2;
  gdouble   sfac;
  gdouble   a, b, c, d;
  gboolean  tile;

  gauss = t_random_new (new_seed ());
  tile = FALSE;

  original = t_terrain_clone (terrain);
  sources = terrain->riversources;
  terrain->riversources = g_array_new (FALSE, FALSE, sizeof (TTerrainRiverCoords));

  t_terrain_set_size (terrain, original->width << 1, original->height << 1);

  /* Normalize to matrix size. */
  global /= sqrt (original->width * original->height);

  pos2 = 0;
  for (y = 0; y < original->height; y++)
    {
      pos1 = (y << 1) * terrain->width;
      for (x = 0; x < original->width; x++)
        {
          terrain->heightfield[pos1] =
            original->heightfield[pos2];
          pos1 += 2;
          pos2++;
        }
    }

  /* Fill in edges. */
  if (!tile)
    {
      pos1 = terrain->width;
      pos2 = pos1 + terrain->width - 1;
      for (y = 1; y < terrain->height; y += 2)
        {
          terrain->heightfield[pos1] =
            terrain->heightfield[pos1 - terrain->width];

          terrain->heightfield[pos2] =
            terrain->heightfield[pos2 - terrain->width];

          pos1 += terrain->width * 2;
          pos2 += terrain->width * 2;
        }

      pos1 = 1;
      pos2 = pos1 + (terrain->height - 1) * terrain->width;
      for (x = 1; x < terrain->width; x += 2)
        {
          terrain->heightfield[pos1] =
            terrain->heightfield[pos1 - 1];

          terrain->heightfield[pos2] =
            terrain->heightfield[pos2 - 1];

          pos1 += 2;
          pos2 += 2;
        }
    }

  /* Tiling and non-tiling interpolation. */
  for (y = 1; y < terrain->height; y += 2)
    {
      for (x = 1; x < terrain->width; x += 2)
        {
          if (tile)
            {
              a = MOD (terrain, x + 1, y + 1);
              b = MOD (terrain, x - 1, y + 1);
              c = MOD (terrain, x + 1, y - 1);
              d = MOD (terrain, x - 1, y - 1);
            }
          else
            {
              a = CLIP (terrain, x + 1, y + 1);
              b = CLIP (terrain, x - 1, y + 1);
              c = CLIP (terrain, x + 1, y - 1);
              d = CLIP (terrain, x - 1, y - 1);
            }

          sfac = global + local *
                 (MAX (MAX (a, b), MAX (c, d)) - MIN (MIN (a, b), MIN(c, d)));
          terrain->heightfield[y * terrain->width + x] =
            (a + b + c + d) * 0.25 + sfac * (t_random_rnd1 (gauss) - 0.5);
        }
    }

  for (y = 0; y < terrain->height; y += 2)
    {
      for (x = 1; x < terrain->width; x += 2)
        {
          if (tile)
            {
              a = MOD (terrain, x + 1, y);
              b = MOD (terrain, x - 1, y);
              c = MOD (terrain, x, y - 1);
              d = MOD (terrain, x, y + 1);
            }
          else
            {
              a = CLIP (terrain, x + 1, y);
              b = CLIP (terrain, x - 1, y);
              c = CLIP (terrain, x, y - 1);
              d = CLIP (terrain, x, y + 1);
            }

          sfac = global + local *
                 (MAX (MAX (a, b), MAX (c, d)) - MIN (MIN (a, b), MIN(c, d)));
          terrain->heightfield[y * terrain->width + x] =
            (a + b + c + d) * 0.25 + sfac * (t_random_rnd1 (gauss) - 0.5);
        }
    }

  for (y = 1; y < terrain->height; y += 2)
    {
      for (x = 0; x < terrain->width; x += 2)
        {
          if (tile)
            {
              a = MOD (terrain, x + 1, y);
              b = MOD (terrain, x - 1, y);
              c = MOD (terrain, x, y - 1);
              d = MOD (terrain, x, y + 1);
            }
          else
            {
              a = CLIP (terrain, x + 1, y);
              b = CLIP (terrain, x - 1, y);
              c = CLIP (terrain, x, y - 1);
              d = CLIP (terrain, x, y + 1);
            }

          sfac = global + local *
                 (MAX (MAX (a, b), MAX (c, d)) - MIN (MIN (a, b), MIN(c, d)));
          terrain->heightfield[y * terrain->width + x] =
            (a + b + c + d) * 0.25 + sfac * (t_random_rnd1 (gauss) - 0.5);
        }
    }

  /* Backtrack and adjust original data */
  for (y = 0; y < terrain->height; y += 2)
    {
      for (x = 0; x < terrain->width; x += 2)
        {
          if (tile)
            {
              a = MOD (terrain, x + 1, y + 1);
              b = MOD (terrain, x - 1, y + 1);
              c = MOD (terrain, x + 1, y - 1);
              d = MOD (terrain, x - 1, y - 1);
            }
          else
            {
              a = CLIP (terrain, x + 1, y + 1);
              b = CLIP (terrain, x - 1, y + 1);
              c = CLIP (terrain, x + 1, y - 1);
              d = CLIP (terrain, x - 1, y - 1);
            }

          sfac = 0.5 * (global + local *
                 (MAX (MAX (a, b), MAX (c, d)) - MIN (MIN (a, b), MIN(c, d))));
          terrain->heightfield[y * terrain->width + x] =
            (a + b + c + d) * 0.25 + sfac * (t_random_rnd1 (gauss) - 0.5);
        }
    }

  /*
  if (terrain->riverfield!=NULL && original->riverfield!=NULL)
    {
      terrain->riverfield[pos1] = original->riverfield[pos2];
      terrain->riverfield[pos1+1] = original->riverfield[pos2]; 
      terrain->riverfield[pos1+terrain->width] = original->riverfield[pos2];
      terrain->riverfield[pos1+terrain->width+1] = original->riverfield[pos2];
    }
  */

  if (sources!=NULL && sources->len>0)
    {
    gint i;
    TTerrainRiverCoords *coords;

    for(i=0; i<sources->len; ++i)
      {
      coords = &g_array_index( sources, TTerrainRiverCoords, i);
      t_terrain_river (terrain, 1.0*coords->x, (1.0*coords->y));
      }

    g_array_free ( sources, TRUE);
    }

  t_terrain_unref (original);
  t_random_free (gauss);

  t_terrain_set_modified (terrain, TRUE);
}


void
t_terrain_move (TTerrain *terrain,
                gfloat    x_offset,
                gfloat    y_offset)
{
  TTerrain *clone;
  gint      pos1, pos1_offset, pos2;
  gint      i, x, y, x_start, y_start;

  clone = t_terrain_clone (terrain);
  x_start = (terrain->width - 1) * x_offset;
  y_start = (terrain->height - 1) * y_offset;

  pos2 = 0;
  for (y = 0; y < terrain->height; y++)
    {
      pos1 = ((y + y_start) % terrain->height) * terrain->width;
      pos1_offset = x_start;

      for (x = 0; x < terrain->width; x++)
        {
          clone->heightfield[pos1 + pos1_offset] = terrain->heightfield[pos2];

	  if (terrain->selection != NULL)
            clone->selection[pos1 + pos1_offset] = terrain->selection[pos2];

	  if (terrain->riverfield != NULL)
            clone->riverfield[pos1 + pos1_offset] = terrain->riverfield[pos2];

          pos1_offset++;
          if (pos1_offset >= terrain->width)
            pos1_offset = 0;

          pos2++;
        }
    }

  if(terrain->riverfield!= NULL && terrain->riversources->len>0)
    for(i=0; i<terrain->riversources->len; i++)
      {
        gfloat               tx, ty;
        TTerrainRiverCoords *coords;

        coords = &g_array_index (terrain->riversources, TTerrainRiverCoords, i);
	tx = coords->x + x_offset;
	ty = coords->y + y_offset;
        coords->x = (tx > 1.0 ? tx - 1.0 : tx);
        coords->y = (ty > 1.0 ? ty - 1.0 : ty);
      }

  if (terrain->objects->len > 0)
    {
      gint i;
      gint size = terrain->objects->len;

      for (i=0; i<size; i++)
        {
	gfloat          tx, ty;
	TTerrainObject *object;

        object = &g_array_index (terrain->objects, TTerrainObject, i);
        tx = object->x + x_offset;
        ty = object->y + y_offset;
        object->x = (tx > 1.0 ? tx - 1.0 : tx);
        object->y = (ty > 1.0 ? ty - 1.0 : ty);
        }
    }

  t_terrain_copy_heightfield (clone, terrain);
  t_terrain_unref (clone);

  t_terrain_set_modified (terrain, TRUE);
}

/* 
Here is the code. Just two functions, LevelConnector and SolveEquations.
There only one parameter (int maxiter) for this filter. One iteration
round increases the affected area 1-2 pixels in all directions from 
selection boundary.

Test images are in the win32 zip on the levcon page. I cleaned the
Leveller plug-in code for this so there may some errors.

BTW, I do have Linux boxes here at the office but not any decent X
server for a PC. Every other GUI explodes with with this MIX server. If
you can't get the code to work then point me to a decent X server and
I'll help out.

LevelConnector uses these pseudo functions to communicate with the
rest of the app.

SetHeight(x, y, hv);      
hv=GetHeight(x, y);
Sel=GetSelValue(x, y); // is pixel selected
PixV=HVToFloat(hv);    // convert from internal type to float
hv=FloatToHV(PixV);    // convert to internal type
*/

/* compare func used by bsearch */
typedef int (*compare_func)(const void*, const void*);

static int
level_compare (const int *a, const int *b)
{
  return *a - *b;
}

/**
 * The main calculations. Processes data in u[].
 * Solves this Laplace's 2nd order differential
 * equation using Gauss-Seidel iteration.
 *
 *    d2u   d2u
 *    --- + --- = 0 + (force, not used)
 *    dx2   dy2
 *
 * Can be solved by FFT also but this iterative method lets user control
 * the accuracy of the solution. If all pixels are selected then this works
 * like smooth with variable radius. Basically it just averages 4 adjacent
 * pixels into the middle pixel :) Note however that one iteration pass uses
 * uses 2 values per pixel already computed in that pass so it's a kind of
 * cumulative smooth.
 */
static void
level_solve_equations (int **s, float *h, float *u,
                       int node_count, int iteration_count)
{
  int     i, c, dir, node;
  float   sum;

  dir = 1;
  for (i = 0; i < iteration_count; i++)
    {
      for (node = (node_count - 1) * (1 - dir) / 2;
           dir * node <= (node_count - 1) * (1 + dir) / 2;
           node += dir)
        {
          sum = h[node];
          for (c = 1; c <= s[node][0]; c++)
            sum = sum + u[s[node][c]];
          u[node] = 0.25 * sum;
        }

      dir = -dir;  /* toggle direction */
    }
}


/**
 * t_terrain_connect: preapares data for LvLSolveEquations.  At this point
 * not selection-sensitive.
 */
void
t_terrain_connect (TTerrain *terrain,
                   gint      iteration_count)
{
  int     width, height;
  int     xo[4] = {1, -1, 0,  0};
  int     yo[4] = {0,  0, 1, -1};
  int     c, node_count, *node_ptr, node, pos;
  int     x, y, xt, yt, i, j, *position, **s;
  float   *h, *u, value, k;

  width = terrain->width;
  height = terrain->height;

  node_count = width * height;

  u = g_new (gfloat, node_count);        /* This is the actual height data  */
  h = g_new (gfloat, node_count);        /* aux data for equations          */
  position = g_new (int, node_count);    /* Stores pixel positions of nodes */
  s = g_new (int*, node_count);          /* aux data for equations          */

  /* clear arrays */
  for (node = 0; node < node_count; node++)
    {
      s[node] = g_new (int, 5);

      for (j = 0; j < 5; j++)
        s[node][j] = 0;

      u[node] = 0;
      h[node] = 0;
      position[node] = 0;
    }

  /* Fill position table */
  pos = 0;
  for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
        {
          position[pos] = pos;
          pos++;
        }
    }


  /* Make equations */
  node = 0;
  pos = 0;
  for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
        {
          if (TRUE)
            {
              k = 0;
              c = 0;
              for (i = 0; i < 4; i++)
                {
                  yt = y + yo[i];
                  xt = x + xo[i];
                  if (yt == -1)
                    yt = height - 1;
                  if (yt == height)
                    yt = 0;
                  if (xt == -1)
                    xt = width - 1;
                  if (xt == width)
                    xt = 0;

                  value = terrain->heightfield[yt * width + xt];

                  if (terrain->selection != NULL &&
                      terrain->selection[yt * width + xt] <= 0.5)
                    k = k + value;
                  else
                    {
                      c++;

                      /* find node number of adjacent pixel */
                      pos = yt * width + xt;
                      node_ptr = (int*)bsearch (&pos, position, node_count,
                                                sizeof (int),
                                                (compare_func) level_compare);
                      if (node_ptr == NULL)
                        {
                          perror ("error in bserch");
                          exit(1);
                        }

                      s[node][c] = node_ptr - position;
                    }
                }

              u[node] = terrain->heightfield[y * width + x];
              h[node] = k;
              s[node][0] = c;
              node++;
            }
        }
    }

  level_solve_equations (s, h, u, node_count, iteration_count);
  g_free (h);

  /* Write data in u[] back to hf */
  for (node = 0; node < node_count; node++)
    {
      g_free (s[node]);

      APPLY (terrain, position[node], u[node]);
    }

  g_free (s);
  g_free (position);
  g_free (u);

  t_terrain_set_modified (terrain, TRUE);
}

/* Digital Filter
 * Given the filter, the terrain and the elevation range, we 
 * calculate the sum of values (multiplied by the filter
 * scaler) for the particular tile being considered.
 * The sum is then divided by the number of tiles in the
 * passed filter.
 * see: http://www.cosc.brocku.ca/Project/info/moss/system/system32.html
 */
void 
t_terrain_digital_filter (TTerrain *terrain, 
		          gfloat *filterData,
			  gint filterSize,  /* filter is square */
                          gfloat elv_min,
                          gfloat elv_max)
{
  gint       x, y;
  gint       i, j;
  gint       width = terrain->width;
  gint       height = terrain->height;
  gfloat    *tmpdata;

  g_return_if_fail (filterSize>0);

  /* create a reference copy of HF data */
  tmpdata = g_new0 (gfloat, width*height); 
  memcpy (tmpdata, terrain->heightfield, sizeof(gfloat)*width*height);

  for(y=0; y<height; y++)
    {
    for(x=0; x<width; x++)
      {
      gint   off = y*width+x;
      gfloat elv = tmpdata[off];

      if (elv >= elv_min && elv <= elv_max)
        {
        gfloat count = 0;
        gfloat total = 0;
      
        for (j=0; j<filterSize; j++ ) 
          for (i=0; i<filterSize; i++ )
            if (x + (i-(filterSize/2)) <  width &&
                x + (i-(filterSize/2)) >= 0 &&
		y + (j-(filterSize/2)) <  height &&
                y + (j-(filterSize/2)) >= 0)
            { 
              gint pos = (y+(j-(filterSize/2)))*width + (x+(i-(filterSize/2)));

              total += terrain->heightfield[pos] * filterData[j*filterSize+i]; 
              count++;
            }

        APPLY (terrain, off, total/count);
        }
      }
    }

  g_free (tmpdata);
}

void
t_terrain_rasterize (TTerrain *terrain,
                     gint      x_size,
		     gint      y_size,
                     gfloat    tightness)
{
  gint width = terrain->width;
  gint height = terrain->height;

  gint x, y;

  g_return_if_fail (x_size>0);
  g_return_if_fail (y_size>0);
  g_return_if_fail (tightness >=0 && tightness<=1);

  for (x=0; x<width; x+=x_size)
    for (y=0; y<height; y+=y_size)
      {
      gfloat avg, sum=0;
      gint xx=0, yy =0;
      gint xlim = x+x_size;
      gint ylim = y+y_size;

      for (xx=x; xx<xlim && xx<width; xx++)
        for (yy=y; yy<ylim && yy<height; yy++)
          sum += terrain->heightfield[yy*width+xx];
      avg = sum/((xx-x)*(yy-y));

      for (yy=y; yy<ylim && yy<height; yy++)
        for (xx=x; xx<xlim && xx<width; xx++)
          {
          gint pos = yy*width+xx;
          gfloat elv = terrain->heightfield[pos];
          gfloat diff = (elv - avg)*(1-tightness);
          gfloat newval = avg+diff;

          APPLY (terrain, pos, newval);
          }
      }
}

/* adapted from David Beale's HF-lab */
TTerrain *t_terrain_warp (TTerrain *terrain, TTerrain *tdisplacement, 
		          gfloat center_x, gfloat center_y, 
			  gint op, gfloat factor)
{
  TTerrain  *terrain_out;
  GtkObject *terrain_object;

  int xsize, ysize;
  int x, y, xx, yy, xx1, yy1;
  double fx, fy, ax, ay, ay2;
  double r, theta;             /* point in polar coordinates */
  double tmp1, tmp2;           /* interpolation values */
  double xfrac, yfrac;

  g_return_val_if_fail ((factor>=0 && factor<=1), NULL);
  g_return_val_if_fail (terrain->width == tdisplacement->width, NULL);
  g_return_val_if_fail (terrain->height == tdisplacement->height, NULL);

  xsize = terrain->width;
  ysize = terrain->height;
  center_x *= xsize;
  center_y *= ysize;

  terrain_object = t_terrain_new (xsize, xsize);
  terrain_out = T_TERRAIN (terrain_object);

  /* 0==twist, 1==bloom */
  if (op!=0) 
    factor *= xsize;

  for (y=0;y<ysize;y++) 
    {
    ay = ((double)y-center_y);
    ay2 = ay*ay;

    for (x=0;x<xsize;x++) 
      {
      ax = ((double)x - center_x);
      r = sqrt(ax*ax + ay2);
      theta = ATAN2(ay,ax);

      if (op==0) 
        theta += factor * tdisplacement->heightfield[y*tdisplacement->width+x];
      else 
	r += factor * tdisplacement->heightfield[y*tdisplacement->width+x];

      fx = r * cos(theta);
      fy = r * sin(theta);

      xx = (int)(fx); 
      yy = (int)(fy); 
      xfrac = ABS(fx-xx);
      yfrac = ABS(fy-yy);

      if (fx>0) 
	xx1 = xx+1; 
      else 
	xx1 = xx-1; 

      if (fy>0) 
	yy1 = yy+1; 
      else 
	yy1 = yy-1;

      xx += center_x; 
      yy += center_y;

      xx1 += center_x; 
      yy1 += center_y;
    
      X_CLIP(xx); 
      Y_CLIP(yy); 
      X_CLIP(xx1); 
      Y_CLIP(yy1);

      tmp1 = INTERPOLATE(terrain->heightfield[yy*terrain->width+xx], 
		         terrain->heightfield[yy1*terrain->width+xx],
			 yfrac);
      tmp2 = INTERPOLATE(terrain->heightfield[yy*terrain->width+xx1], 
		         terrain->heightfield[yy1*terrain->width+xx1], 
			 yfrac);

      terrain_out->heightfield[y*terrain_out->width+x] = 
	                 INTERPOLATE (tmp1,tmp2,xfrac);
      } 
    } 

  return terrain_out;
} 

/* get_random: Return a random number given two gints used as seeds */
static gdouble
zoom_get_random (guint32 x, guint32 y)
{
  guint32 value;

  value = x ^ 0x2f93a473;
  value = (value + 219169373) * 31793417;
  value += y ^ 0xfca814ed;
  value = (value + 31793417) * 219169373;

  return value / 4294967295.0;
}


static gdouble
zoom_midpoint (gint    location_x,
               gint    location_y,
               gdouble value1,
               gdouble value2)
{
  gdouble rand;
  gdouble mean, deviation;

  mean = (value1 + value2) / 2.0;
  deviation = fabs (value1 - value2) * 0.9;

  rand = zoom_get_random (location_x, location_y) - 0.5;

  return mean + rand * deviation;
}


#define INVALID -9999.0


static void
zoom_plot (TTerrain *terrain,
           gint      x,
           gint      y,
           gdouble   height)
{
  gint pos = y * terrain->width + x;

  if (terrain->heightfield[pos] == INVALID)
    terrain->heightfield[pos] = height;
}


static void
t_terrain_zoom_b (TTerrain *terrain,
                  gint      x1,
                  gint      y1,
                  gint      x2,
                  gint      y2,
                  gdouble   a,
                  gdouble   b,
                  gdouble   c,
                  gdouble   d)
{
  gint    mid_x, mid_y;
  gdouble h, i, j, k, l;
  gdouble rand;
  gdouble x, x_sqr;
  gdouble mean, deviation;

  /* Plot values */
  zoom_plot (terrain, x1, y1, a);
  zoom_plot (terrain, x2, y1, b);
  zoom_plot (terrain, x1, y2, c);
  zoom_plot (terrain, x2, y2, d);

  /* Stop if finished */
  if (x2 - x1 <= 1 && y2 - y1 <= 1)
    return;

  /* Split square and recurse */
  mid_x = (x1 + x2) / 2;
  mid_y = (y1 + y2) / 2;

  x = a + b + c + d;
  x_sqr = a * a + b * b + c * c + d * d;
  mean = x / 4.0;
  deviation = sqrt ((x_sqr - x * x / 4.0) / 3.0);
  rand = zoom_get_random (mid_x, mid_y) - 0.5;

  /*
   * ahb
   * ijk
   * cld
   */

  h = zoom_midpoint (mid_x, y1, a, b);
  i = zoom_midpoint (x1, mid_y, a, c);
  j = mean + rand * deviation;
  k = zoom_midpoint (x2, mid_y, b, d);
  l = zoom_midpoint (mid_x, y2, c, d);

  t_terrain_zoom_b (terrain, x1, y1, mid_x, mid_y, a, h, i, j);
  t_terrain_zoom_b (terrain, mid_x, y1, x2, mid_y, h, b, j, k);
  t_terrain_zoom_b (terrain, x1, mid_y, mid_x, y2, i, j, c, l);
  t_terrain_zoom_b (terrain, mid_x, mid_y, x2, y2, j, k, l, d);
}


void
t_terrain_zoom (TTerrain *terrain,
                gint      x1,
                gint      y1,
                gint      x2,
                gint      y2)
{
  TTerrain *data;
  gint      i, x, y;

  g_return_if_fail (terrain != NULL);
  g_return_if_fail (x2 > x1);
  g_return_if_fail (y2 > y1);
  g_return_if_fail (x1 >= 0);
  g_return_if_fail (y1 >= 0);
  g_return_if_fail (x1 < terrain->width);
  g_return_if_fail (y1 < terrain->height);

  /* Copy data */
  data = t_terrain_crop_new (terrain, x1, y1, x2, y2);
  t_terrain_ref (data);

  /* Initialize new terrain */
  for (i = 0; i < terrain->width * terrain->height; i++)
    terrain->heightfield[i] = INVALID;

  for (y = y1; y < y2; y++)
    {
      for (x = x1; x < x2; x++)
        {
          gdouble from_x1, from_y1, from_x2, from_y2;
          gint    to_x1, to_y1, to_x2, to_y2;
          gdouble a, b, c, d;

          to_x1 = (terrain->width - 1) * (x - x1) / (x2 - x1);
          to_y1 = (terrain->height - 1) * (y - y1) / (y2 - y1);
          to_x2 = (terrain->width - 1) * (x + 1 - x1) / (x2 - x1);
          to_y2 = (terrain->height - 1) * (y + 1 - y1) / (y2 - y1);

          from_x1 = (x - x1 + 0.0) / (x2 - x1);
          from_y1 = (y - y1 + 0.0) / (y2 - y1);
          from_x2 = (x - x1 + 1.0) / (x2 - x1);
          from_y2 = (y - y1 + 1.0) / (y2 - y1);

          a = t_terrain_get_height (data, from_x1, from_y1);
          b = t_terrain_get_height (data, from_x2, from_y1);
          c = t_terrain_get_height (data, from_x1, from_y2);
          d = t_terrain_get_height (data, from_x2, from_y2);

          t_terrain_zoom_b (terrain, to_x1, to_y1, to_x2, to_y2,
                            a, b, c, d);
        }
    }

  t_terrain_unref (data);

  t_terrain_set_modified (terrain, TRUE);
}

