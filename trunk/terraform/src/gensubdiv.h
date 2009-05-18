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


#include <glib.h>
#include "tterrain.h"
#include "trandom.h"

#define NUM_METHODS_SUBDIV 6

TTerrain *t_terrain_generate_subdiv            (gint   method,
                                                gint   size,
                                                gfloat scale_factor,
                                                gint   seed);
TTerrain *t_terrain_generate_subdiv_seed       (gint      method,
                                                gint      size,
                                                gfloat    scale_factor,
                                                gint      seed, 
						TTerrain *terrain, 
						gint      sel_x1, 
						gint      sel_y1, 
						gint      sel_w, 
						gint      sel_h);
TTerrain *t_terrain_generate_subdiv_seed_ns    (gint      method,
                                                gint      size_x,
                                                gint      size_y,
                                                gfloat    scale_factor,
                                                gint      seed, 
						TTerrain *terrain, 
						gint      sel_x1, 
						gint      sel_y1, 
						gint      sel_w, 
						gint      sel_h);
TTerrain *t_terrain_generate_recursive_square  (TTerrain *terrain,
		                                TRandom  *gauss, 
						gint      size,
                                                gfloat    scale_factor,
                                                gboolean  doinit);
TTerrain *t_terrain_generate_recursive_diamond (TTerrain *terrain, 
		                                TRandom  *gauss, 
						gint      size,
                                                gfloat    scale_factor,
                                                gboolean  doinit);
TTerrain *t_terrain_generate_offset            (TTerrain *terrain, 
		                                TRandom  *gauss,
						gint      size,
                                                gfloat    scale_factor);
TTerrain *t_terrain_generate_midpoint          (TTerrain *terrain,
		                                TRandom  *gauss,
						gint       size,
                                                gfloat     scale_factor);
TTerrain *t_terrain_generate_recursive_plasma  (TTerrain *terrain, 
		                                TRandom  *gauss,
						gint      size,
                                                gfloat    scale_factor,
                                                gboolean  doinit);
TTerrain *t_terrain_generate_diamond_square    (TTerrain *terrain, 
		                                TRandom  *gauss,
						gint      size,
                                                gfloat    scale_factor);
