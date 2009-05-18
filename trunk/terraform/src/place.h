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


#include "tterrain.h"

void t_terrain_place_objects          (TTerrain *terrain,
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
			               gboolean  uniform_scaling, 
                                       gboolean  decrease_density_bottom,
                                       gboolean  decrease_density_top,
                                       gboolean  vary_bottom_edge,
                                       gboolean  vary_top_edge,
                                       gint      seed);

void t_terrain_rescale_placed_objects (TTerrain *terrain,
                                       gfloat    scale_x,
                                       gfloat    scale_y,
                                       gfloat    scale_z,
                                       gfloat    variance_x,
                                       gfloat    variance_y,
                                       gfloat    variance_z,
			               gboolean  uniform_scaling, 
			               gint      seed);

void t_terrain_prune_placed_objects   (TTerrain *terrain,
                                       gfloat    p,
                                       gint      seed);

