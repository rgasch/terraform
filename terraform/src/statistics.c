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


#include "statistics.h"


/* use the box-counting method to determine the 
 * terrain's fractal dimension where fractal 
 * dimension is defined as 
 *   log(N)/log(r)
 * where 
 *   N = number of box points in the set
 *   r = scale factor (avg side length);
 *
 * The implementation is generally correct, although 
 * the code suffers from the fact that terraform 
 * always scales the terrain between 0 .. 1 so that 
 * the computed fractal dimension will almost be 
 * somewhere between 2.25 and 2.75.
 * I need to come up with some bright idea here on 
 * how to deal with this!!
 */
gfloat 
t_terrain_calculate_dimension (TTerrain *terrain)
{
  gint width = terrain->width;
  gint height = terrain->height;
  gint cnt_x = 1;
  gint cnt_y = 1;
  gint cnt_z = 1;
  gfloat inc_x;
  gfloat inc_y;
  gfloat inc_z;
  gfloat off_x ;
  gfloat off_y ;
  gfloat off_z;
  gint x, y, z;
  gint i, j;
  gint count;
  gdouble dim;

  /* determine x box size */
  do 
    {
    cnt_x = cnt_x*2;
    inc_x = width/cnt_x;
    }
  while (inc_x>=4);

  /* determine z box size */
  do 
    {
    cnt_z = cnt_z*2;
    inc_z = height/cnt_z;
    }
  while (inc_z>=4);

  /* determine y box size */
  cnt_y = (cnt_x + cnt_z)/2;
  inc_y = 1.0/cnt_y;

  /* iterate through the boxes */
  count = 0;
  off_z = 0;
  for (z=0; z<cnt_z; z++)
    {
    off_x = 0;

    for (x=0; x<cnt_x; x++)
      {
      off_y = 0;

      for (y=0; y<cnt_y; y++)
        {
	/* iterate through the points in each box and 
	 * see if it contains part of the terrain 
	 */
        gboolean found = FALSE;
	gint     lim_z = (gint)(off_z+inc_z); 

	off_y += inc_y;
	for (j=(gint)off_z; j<lim_z && !found; j++)
          {
	  gint lim_x = (gint)(off_x+inc_x); 
	  for (i=(gint)off_x; i<lim_x && !found; i++)
            {
            if (terrain->heightfield[j*width+i] >= off_y)
              found = TRUE;
            }
	  }

	if (found)
          count++;
	else
          y = cnt_y; /* exit upwards search, boxes above this one are empty */
	    
        off_y += inc_y;
        }
      off_x += inc_x;
      }
    off_z += inc_z;
    }

  dim = log10 (count) / log10 ((cnt_z+cnt_x)/2);
  return dim;
}

gfloat 
t_terrain_calculate_average (TTerrain *terrain)
{
  gfloat *data = terrain->heightfield;
  gfloat  sum = 0; 
  gint    size = terrain->width*terrain->height;
  gint    i; 

  for (i=0; i<size; i++)
   sum += data[i];

  return sum/(gfloat)size;
}

gfloat 
t_terrain_calculate_variance (TTerrain *terrain, gfloat *return_average)
{
  gint    size = terrain->width*terrain->height;
  gfloat *data = terrain->heightfield;
  gfloat  sum = 0.0; 
  gfloat  ss  = 0.0; 
  gfloat  var = 0.0; 
  gfloat  avg;
  gint    i; 

  /* return result to avoid 'wasting' the average calculations */
  avg = t_terrain_calculate_average (terrain);
  if (return_average != NULL)
    *return_average = avg;

  var = 0.0;
  ss = 0.0;
  for (i=0; i<size; i++)
    {
    sum = data[i] - avg;
    var += sum*sum;
    ss += sum;
    }

  var = (var - ss * ss / size) / (size -  1);
  return var;
}

gfloat 
t_terrain_calculate_skewness (TTerrain *terrain, 
		              gfloat *return_average, 
			      gfloat *return_variance)
{
  gint    size = terrain->width*terrain->height;
  gfloat *data = terrain->heightfield;
  gfloat  skew = 0.0; 
  gfloat  var;
  gfloat  avg;
  gfloat  s;
  gdouble sqrtvar;
  gint    i; 

  var = t_terrain_calculate_variance (terrain, &avg);
  sqrtvar = sqrt(var);

  /* return result to avoid 'wasting' the average calculations */
  if (return_average != NULL)
    *return_average = avg;

  /* return result to avoid 'wasting' the variance calculations */
  if (return_variance != NULL)
    *return_variance = var;

  for (i=0; i<size; i++)
    {
    s = (data[i] - avg) / sqrtvar;
    skew += s * s * s;
    }

 return skew/(gfloat)size;
}

