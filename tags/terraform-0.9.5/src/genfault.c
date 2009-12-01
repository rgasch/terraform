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

/* RNG: 
 * This code is based on the Faulting algorithm descriptions given at 
 * http://www.lighthouse3d.com/opengl/terrain/index.php3?fault
 * by António Ramires Fernandes
 */
 
 
#include <math.h>
#include <stdlib.h>
#include "genfault.h"
#include "trandom.h"


static void t_terrain_iterate_line          (TTerrain *terrain, TRandom *gauss, 
		                             int method, int nIterations, 
				             float factor);
static void t_terrain_iterate_circles       (TTerrain *terrain, TRandom *gauss, 
		                             int nIterations, float factor); 
static void t_terrain_iterate_squares       (TTerrain *terrain, TRandom *gauss, 
		                             int nIterations, float factor,
					     int useConstantSize); 
static void t_terrain_iterate_particle_deposition (TTerrain *terrain, 
		                            TRandom *gauss, int nCycles, 
				            int nIterations, float factor);



TTerrain * 
t_terrain_generate_fault (int method, 
                          int size, 
                          int nIterations, 
                          int seed, 
			  float factor, 
			  int cycles, 
			  int useConstantSize) 
{
  TRandom   *gauss;
  GtkObject *terrain_object;
  TTerrain  *terrain;
  gfloat    *data;

  //printf ("Generating Fault: %d, %d, %d, %d\n", method, size, nIterations, seed); 

  gauss = t_random_new (seed);
  terrain_object = t_terrain_new (size, size);
  terrain = T_TERRAIN (terrain_object);
  data = terrain->heightfield;

  if (method==0 || method==1 || method==2)
    t_terrain_iterate_line (terrain, gauss, method, nIterations, factor);
  else
  if (method==3)
    t_terrain_iterate_circles (terrain, gauss, nIterations, factor);
  else
  if (method==4)
    t_terrain_iterate_squares (terrain, gauss, nIterations*cycles, 
		               factor, useConstantSize);
  if (method==5)
    t_terrain_iterate_particle_deposition (terrain, gauss, cycles, 
		                           nIterations*100, factor);

  t_random_free (gauss);
  t_terrain_normalize (terrain, FALSE);
  t_terrain_set_modified (terrain, TRUE);

  return terrain;
}


static void 
t_terrain_iterate_line (TTerrain *terrain, 
                        TRandom *gauss, 
                        int method, 
                        int nIterations,
			float factor)
{
  int halfX, halfZ;
  int i, j, n;
  float a, b, c, w, d;
  float deltaY=0, pd;
  float disp;
  float terrainWaveSize = 3.0;  
  float *data = terrain->heightfield;	

  halfX = terrain->width / 2;
  halfZ = terrain->height / 2;

  for (n=0; n<nIterations; n++) 
    {
    d = sqrt (halfX * halfX + halfZ * halfZ);
    w = rand ();		// random direction
    a = cos (w);
    b = sin (w);
    //c = t_random_gauss (gauss) * 2*d - d;  // makes concentric structures
    c = ((float)rand() / RAND_MAX) * 2*d - d; 

    disp = nIterations - n;
		
    for (j = 0; j<terrain->height; j++) 
      for (i = 0; i<terrain->width; i++)
        {
	switch(method)
	  {
	   case 0: // step: solve distance from line 
	     pd = (i-halfZ) * a + (j-halfX) * b + c;
	     if (pd > 0)
	        deltaY = disp;
	     else
	        deltaY = -disp;
	     break;

	   case 1: // sine
	     pd = ((i-halfZ) * a + (j-halfX) * b + c)/terrainWaveSize * factor;
	     if (pd > 1.57) 
	       pd = 1.57;
	     else if (pd < 0) 
	       pd = 0;
	     deltaY = -disp/2 + sin(pd)*disp;
	     break;

	   case 2: // cosine
	     pd = ((i-halfZ) * a + (j-halfX) * b + c)/terrainWaveSize;
	     if (pd > 3.14) 
	       pd = 3.14;
	     else if (pd < -3.14) 
	       pd = -3.14;
	     deltaY =  disp-(terrainWaveSize/(terrain->width+0.0)) + cos(pd)*disp * factor;
	     break;
	  }
        data[j*terrain->width+i] += deltaY;
	}
    }
}


static void
t_terrain_iterate_circles (TTerrain *terrain, 
			   TRandom *gauss,
                           int nIterations, 
			   float factor) 
{
  int i, j, n, x, z;
  int halfX, halfZ, dispSign;
  float deltaY;
  float disp;
  float terrainCircleSize = terrain->height;
  float r, pd;
  float *data = terrain->heightfield;

  halfX = terrain->width / 2;
  halfZ = terrain->height / 2;

  for (n=0; n<nIterations; n++) 
    {
    z = ((float)rand() / RAND_MAX) * terrain->height;
    x = ((float)rand() / RAND_MAX) * terrain->width;
 
    disp = nIterations - n;
    r = t_random_gauss (gauss);

    if (r > 0.5)
      dispSign = 1;
    else
      dispSign = -1;

    for (j = 0; j<terrain->height; j++) 
      for (i = 0; i<terrain->width; i++)
        {
        pd = sqrt(((i-x)*(i-x) + (j-z)*(j-z))/terrainCircleSize)*5*factor;
        if (pd > 1) 
          deltaY = 0.0;
        else 
        if (pd < -1) 
          deltaY = 0.0;
        else
	  deltaY =  disp/2*dispSign + cos(pd*3.14)*disp/2 * dispSign;

        data[j*terrain->width+i] += deltaY;
        }
    }
}


// written by RNG
static void
t_terrain_iterate_squares (TTerrain *terrain, 
			   TRandom *gauss,
                           int nIterations, 
			   float factor,
			   int useConstantSize) 
{
#define CLIP(x,min,max) MAX (MIN (x, max), min)
  int i, j, n, x, y;
  int xstart, xlim; 
  int ystart, ylim;
  int iter = nIterations*10;
  float *data = terrain->heightfield;

  /*
  x = t_random_gauss(gauss) * terrain->width;
  y = t_random_gauss(gauss) * terrain->height;

  if (x>= terrain->width)  x = x - terrain->width;
  if (x< 0)                x = terrain->width + x;
  if (y>= terrain->height) y = y - terrain->height;
  if (y< 0)                y = terrain->height + y;
  */
  x = ((float)rand() / RAND_MAX) * terrain->width;
  y = ((float)rand() / RAND_MAX) * terrain->height;

  for (n=0; n<iter; n++) 
    {
    //int size = 100.0/iter*(iter-n);
    int size = 10;
    int deltaY = iter - n;
    int dx, dy;
    float f;

    if (useConstantSize)
      size = 10;
    else
      size = 100.0/iter*(iter-n);

    xstart = CLIP (x-size/2, 0, terrain->width);
    xlim   = CLIP (x+size/2, 0, terrain->width);
    ystart = CLIP (y-size/2, 0, terrain->height);
    ylim   = CLIP (y+size/2, 0, terrain->height);

    for (j = ystart; j<ylim; j++) 
      for (i = xstart; i<xlim; i++)
        data[j*terrain->width+i] += deltaY;

    dx = (int)(((float)rand() / RAND_MAX) * size * factor);
    f = ((float)rand() / RAND_MAX);
    x = x + (f<0.5 ? dx : -dx);

    dy = (int)(((float)rand() / RAND_MAX) * size * factor);
    f = ((float)rand() / RAND_MAX);
    y = y + (f<0.5 ? dy : -dy);

    if (useConstantSize)
      {
      if (x>= terrain->width)  x = x - terrain->width;
      if (x< 0)                x = terrain->width + x;
      if (y>= terrain->height) y = y - terrain->height;
      if (y< 0)                y = terrain->height + y;
      }
    }
}


static void
t_terrain_iterate_particle_deposition (TTerrain *terrain, 
                                       TRandom *gauss,
                                       int nCycles, 
                                       int nIterations,
				       float factor)
{
  int    x, z;
  int    disp, dir, i, n;
  float *data = terrain->heightfield;

  for (i=0; i<nCycles; i++)
    {
    x = t_random_gauss(gauss) * terrain->width;
    z = t_random_gauss(gauss) * terrain->height;

    for (n=0; n<nIterations; n++) 
      {
      disp = nIterations - n;

      //dir = t_random_gauss(gauss) * 4;  // produces 'Fuzzy/striped' HFs
      dir = ((float)rand() / RAND_MAX) * 4;
      switch (dir)
        {
          case 0: x++; break;
          case 1: x--; break;
          case 2: z++; break;
          case 3: z--; break;
          default: z--; break;
        }

      if (x>= terrain->width)  x = x - terrain->width;
      if (x< 0)                x = terrain->width + x;
      if (z>= terrain->height) z = z - terrain->height;
      if (z< 0)                z = terrain->height + z;

      data[z*terrain->width+x] += disp;
      }
    }
}

