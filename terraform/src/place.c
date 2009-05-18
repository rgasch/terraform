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

#include <stdlib.h>
#include <time.h>

#include "place.h"
#include "trandom.h"
#include "writer.h"

void
t_terrain_place_objects (TTerrain *terrain,
                         gchar    *name,
                         gfloat    elevation_min,
                         gfloat    elevation_max,
                         gfloat    density,
                         gfloat    variance,
                         gfloat    scale_x,
                         gfloat    scale_y,
                         gfloat    scale_z,
                         gfloat    variance_x,
                         gfloat    variance_y,
                         gfloat    variance_z,
                         gboolean  vary_direction,
                         gboolean  symmetrical,
                         gboolean  decrease_density_bottom,
                         gboolean  decrease_density_top,
                         gboolean  vary_bottom_edge,
                         gboolean  vary_top_edge,
                         gint      seed)
{
  gfloat x, y;
  gfloat cell_width, cell_height;
  gfloat pov_size_x, pov_size_y, pov_size_z;
  gfloat midelv;
  TRandom *rnd;

  g_return_if_fail (terrain != NULL);
  g_return_if_fail (elevation_min >= 0 && elevation_max <=1);
  g_return_if_fail (density >= 0 && density <= 1);
  g_return_if_fail (variance >= 0 && variance <=1);
  g_return_if_fail (scale_x > 0);
  g_return_if_fail (scale_y > 0);
  g_return_if_fail (scale_z > 0);
  g_return_if_fail (variance_x >= 0);
  g_return_if_fail (variance_y >= 0);
  g_return_if_fail (variance_z >= 0);

  if (seed == -1)
    seed = new_seed ();

  if (name == NULL)
    name = "placed_object";

  rnd = t_random_new (seed);

  /* Determine terrain size in POV-Ray space */
  t_terrain_get_povray_size (terrain, &pov_size_x, &pov_size_y, &pov_size_z);

  /* calculate cell size */
  cell_width = 1.0 / (pov_size_x / scale_x * density);
  cell_height = 1.0 / (pov_size_z / scale_z * density); 
  midelv = elevation_min + (elevation_max-elevation_min)/2;

  /* loop through the squares on Pov and Hf offsets */
  for (y = cell_height * 0.5; y <= 1.0; y += cell_height)
    for (x = cell_width * 0.5; x <= 1.0; x += cell_width)
      {
        gfloat angle;
        gfloat position_x, position_z;
        gfloat size_x, size_y, size_z;

        /* calculate the object position in the scene */
        position_x =
          x + cell_width * variance * (t_random_rnd1 (rnd) - 0.5);
        position_z =
          y + cell_height * variance * (t_random_rnd1 (rnd) - 0.5);

        /* calculate the object scaling factors */
        if (symmetrical)
          {
            gfloat f;

            f = t_random_rnd1 (rnd) - 0.5;
            size_x = scale_x * (1.0 + variance_x * f);
            size_y = scale_y * (1.0 + variance_y * f);
            size_z = scale_z * (1.0 + variance_z * f);
          }
        else
          {
            size_x =
              scale_x * (1.0 + variance_x * (t_random_rnd1 (rnd) * 2.0 - 1.0));
            size_y =
              scale_y * (1.0 + variance_y * (t_random_rnd1 (rnd) * 2.0 - 1.0));
            size_z =
              scale_z * (1.0 + variance_z * (t_random_rnd1 (rnd) * 2.0 - 1.0));
          }

        if (position_x >= 0.0 && position_x <= 1.0 &&
            position_z >= 0.0 && position_z <= 1.0)
          {
            gfloat   height;
            gfloat   elvdiff;
            gfloat   elvdiffabs;
            gint     hf_x, hf_y, pos;
	    gboolean do_place = TRUE;

            hf_x = position_x * (terrain->width - 1);
            hf_y = position_z * (terrain->height - 1);
            pos = hf_y * terrain->width + hf_x;
            height = terrain->heightfield[pos];
            elvdiff = midelv - height;
            elvdiffabs = ABS(midelv - (ABS(elvdiff)))/midelv;

	    if (elvdiff<=0 && decrease_density_bottom &&
                elvdiffabs < t_random_rnd1(rnd))
              do_place = FALSE;
              
	    if (elvdiff>0 && decrease_density_top && 
                elvdiffabs < t_random_rnd1(rnd))
              do_place = FALSE;

	    if (do_place)
              {
              gfloat elvmin = elevation_min;
	      gfloat elvmax = elevation_max;

              if (vary_direction)
                angle = rand () % 360;
              else
                angle = 0.0;

              if (vary_bottom_edge)
                {
                elvmin += (0.15)*(t_random_rnd1(rnd)-0.5);
		if (elvmin < 0) 
                  elvmin = 0;
                }

              if (vary_top_edge)
                {
                elvmax += (0.15)*(t_random_rnd1(rnd)-0.5);
		if (elvmax > 1) 
                  elvmax = 1;
                }

              if (height>=elvmin && height<=elvmax && 
                  (terrain->riverfield == NULL || 
                  terrain->heightfield[pos]>terrain->riverfield[pos]))
                if (terrain->selection == NULL ||
                    terrain->selection[pos] > 0.5)
                  t_terrain_add_object (terrain, hf_x, hf_y,
                                        position_x, position_z,
                                        angle,
                                        size_x, size_y, size_z, name);
              }
          }
      }

  t_random_free (rnd);
}

void
t_terrain_rescale_placed_objects (TTerrain *terrain,
                                  gfloat    scale_x,
                                  gfloat    scale_y,
                                  gfloat    scale_z,
                                  gfloat    variance_x,
                                  gfloat    variance_y,
                                  gfloat    variance_z,
                                  gboolean  symetrical, 
				  gint      seed)
{
  gint i;
  TTerrainObject *obj; 
  TRandom *rnd;

  g_return_if_fail (terrain != NULL);
  g_return_if_fail (scale_x > 0);
  g_return_if_fail (scale_y > 0);
  g_return_if_fail (scale_z > 0);
  g_return_if_fail (variance_x > 0);
  g_return_if_fail (variance_y > 0);
  g_return_if_fail (variance_z > 0);

  if (seed == -1)
    seed = new_seed ();

  rnd = t_random_new (seed);

  for (i=0; i<terrain->objects->len; i++)
    {
    obj = &g_array_index (terrain->objects,TTerrainObject,i);

    if (terrain->selection == NULL || 
        terrain->selection[obj->oy*terrain->width+obj->ox] > 0.0)
      {
      if (symetrical)
        {
        gfloat f;

        f = t_random_rnd1 (rnd) - 0.5;
        obj->scale_x *= (scale_x + variance_x * f);
        obj->scale_y *= (scale_y + variance_y * f);
        obj->scale_z *= (scale_z + variance_z * f);
        }
      else
        {
        obj->scale_x *= (scale_x + variance_x * (t_random_rnd1(rnd) * 2.0 - 1.0));
        obj->scale_y *= (scale_y + variance_y * (t_random_rnd1(rnd) * 2.0 - 1.0));
        obj->scale_z *= (scale_z + variance_z * (t_random_rnd1(rnd) * 2.0 - 1.0));
        }
      }

    }

  t_random_free (rnd);
}

void
t_terrain_prune_placed_objects (TTerrain *terrain,
                                gfloat    p, 
				gint      seed)
{
  gint i;
  TTerrainObject *obj; 
  TRandom *rnd;

  g_return_if_fail (terrain != NULL);
  g_return_if_fail (p>=0.0 && p<=1);

  if (seed == -1)
    seed = new_seed ();

  rnd = t_random_new (seed);

  for (i=0; i<terrain->objects->len; i++)
    {
    obj = &g_array_index (terrain->objects, TTerrainObject, i);

    if (obj)
      {
      gint pos = obj->oy*terrain->width+obj->ox;

      if (terrain->selection == NULL || terrain->selection[pos] > 0.0)
        if (t_random_rnd1(rnd) < p)
          g_array_remove_index_fast (terrain->objects,i);
      }
    }

  t_random_free (rnd);
}

