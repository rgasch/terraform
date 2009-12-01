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
#include <time.h>
#include "genfault.h"
#include "genperlin.h"
#include "gensubdiv.h"
#include "genspectral.h"
#include "genrandom.h"
#include "trandom.h"


static int fix_gen_bits (int gen_bits);


TTerrain *
t_terrain_generate_random (int size, float factor, int gen_bits)
{
  gint     i;
  gint     r;
  gboolean enabled[4];
  gint     octaves[4];
  gfloat   frequency[4];
  gfloat   amplitude[4];
  gint     filter[4];
  TTerrain *terrain = NULL;

  if (gen_bits != GEN_TYPE_NONE)
    gen_bits = fix_gen_bits (gen_bits);

  if (gen_bits == GEN_TYPE_NONE)
    gen_bits = GEN_TYPE_ALL;

  srand (new_seed ());

  while (terrain == NULL)
    {
    r = rand () % 11;
    //printf ("%d, %d\n", r, gen_bits); fflush (stdout);

    switch (r)
      {
        case 0:
        case 1:
        case 2:
        case 3: 
        case 4: 
          if (gen_bits & GEN_TYPE_1)
	    terrain = t_terrain_generate_fault (rand()%NUM_METHODS_FAULTING, size, 1000, -1, factor, rand()%20+1, rand()%2);
	  break;

        case 5: 
          if (gen_bits & GEN_TYPE_2)
            {
            for (i = 0; i < 4; i++)
              {
                enabled[i] = rand() & 1;
                frequency[i] = (rand() % 100) / 100 + 0.017;
                amplitude[i] = (rand() % 4096) + 4096;
                octaves[i] = (rand() % 10) + 1;
                filter[i] = rand() % 23;
              }

            enabled[0] = TRUE;
            terrain = t_terrain_generate_perlin (size, size, -1, enabled, frequency,
                                    amplitude, octaves, filter);
	    }
	  break;

        case 6:
          if (gen_bits & GEN_TYPE_3)
	    terrain = t_terrain_generate_spectral (size, factor, -1, FALSE);
	  break;

        case 7: 
        case 8: 
        case 9: 
        case 10: 
          if (gen_bits & GEN_TYPE_4)
	    terrain = t_terrain_generate_subdiv (rand () % NUM_METHODS_SUBDIV, size, factor, -1);
	  break;
      }
    }

  return terrain;

}


/* make sure we only have GEN_TYPE_BITS_USED bits set */
static int fix_gen_bits (int gen_bits)
{
  int i;
  int mask = 0;

  for (i=0; i<GEN_TYPE_BITS_USED; i++)
    mask += pow (2, i);

  gen_bits = gen_bits & mask;

  return gen_bits;
}
