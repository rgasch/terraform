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


#include <stdio.h>
#include <math.h>
#include "trandom.h"
#include "genspectral.h"
#include "fftn.h"



/**
 *  spectralSynthesisFM2D 
 *  generate: spectralSynthesisFM2D create a landscape through fractal motion 
 *  in 2 dimensions. Taken from 'The Science of Fractal Images' by Peitgen & 
 *  Saupe, 1988 (page 108)
 */
TTerrain *
t_terrain_generate_spectral (gint      size,
                             gfloat    h,
                             gint      seed,
                             gboolean  invert)
{
  gint       x, y, x0, y0;
  gint       dim[2], lim;
  gfloat    *ar, *ai;
  TRandom   *gauss;
  TTerrain  *terrain;
  GtkObject *terrain_object;

  g_return_val_if_fail (size != 0, NULL);
  g_return_val_if_fail (h > 0.0, NULL);
  g_return_val_if_fail (h < 3.0, NULL);

  terrain_object = t_terrain_new (size, size);
  terrain = T_TERRAIN (terrain_object);

  gauss = t_random_new (seed);

  /* Real array */
  ar = terrain->heightfield;

  /* Imaginary array */
  ai = g_new0 (gfloat, size * size);

  lim = size / 2;
  for (x = 0; x < lim; x++)
    for (y = 0; y < lim; y++)
      {
        gdouble phase, rad; 

        phase = 2.0 * M_PI * t_random_rnd (gauss, 0.0, 1.0);

        if (x != 0 || y != 0)
          rad = pow (x * x + y * y, -(h + 1.0) / 2.0) *
                t_random_gauss (gauss);
        else
          rad = 0.0;

        if (invert)
          {
            ar[y * size + x] = -rad * cos (phase);
            ai[y * size + x] = -rad * sin (phase);
          }
        else
          {
            ar[y * size + x] = rad * cos (phase);
            ai[y * size + x] = rad * sin (phase);
          }

        x0 = (x == 0) ? 0 : size - x;
        y0 = (y == 0) ? 0 : size - y;

        if (invert)
          {
            ar[y0 * size + x0] = -rad * cos (phase);
            ai[y0 * size + x0] = rad * sin (phase);
          }
        else
          {
            ar[y0 * size + x0] = rad * cos (phase);
            ai[y0 * size + x0] = -rad * sin (phase);
          }
      }

  ai[size / 2] = 0;
  ai[(size / 2) * size] = 0;
  ai[(size / 2) * size + (size / 2)] = 0;

  lim = size - lim - 1;
  for (x = 1; x < lim; x++)
    for (y = 1; y < lim; y++)
      {
        double phase, rad; 

        phase = 2.0 * M_PI + t_random_rnd (gauss, 0.0, 1.0);
        rad = pow (x * x + y * y, -(h + 1.0) / 2.0) *
              t_random_gauss (gauss);

        if (invert)
          {
            ar[(size - y) * size + x] = rad * cos (phase);
            ai[(size - y) * size + x] = rad * sin (phase);
            ar[y * size + size - x] = rad * cos (phase);
            ai[y * size + size - x] = -rad * sin (phase);
          }
        else
          {
            ar[(size - y) * size + x] = -rad * cos (phase);
            ai[(size - y) * size + x] = -rad * sin (phase);
            ar[y * size + size - x] = -rad * cos (phase);
            ai[y * size + size - x] = rad * sin (phase);
          }
      }

  dim[0] = size;
  dim[1] = size;

  fftnf (2, dim, ar, ai, -1, 1.0);

  g_free (ai);
  t_random_free (gauss);

  t_terrain_normalize (terrain, FALSE);
  t_terrain_set_modified (terrain, TRUE);

  return terrain;
}
