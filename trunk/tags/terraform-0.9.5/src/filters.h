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
#include "tpixbuf.h"

/* methods for all_rivers upslope */
#define OWN_COUNTING 1
#define UPSLOPE_SFD  2
#define UPSLOPE_MFD  3

void t_terrain_rough_map      (TTerrain *terrain);
void t_terrain_fill           (TTerrain *terrain,
                               gfloat    elevation,
                               gfloat    factor);
void t_terrain_fold           (TTerrain *terrain,
                               gint      margin);
void t_terrain_radial_scale   (TTerrain *terrain,
                               gfloat    center_x, 
                               gfloat    center_y, 
                               gfloat    scale_factor, 
                               gfloat    min_dist,
                               gfloat    max_dist, 
                               gfloat    smooth_factor,
			       gint      frequency);
void t_terrain_gaussian_hill  (TTerrain *terrain,
                               gfloat    center_x,
                               gfloat    center_y,
                               gfloat    radius,
                               gfloat    radius_factor,
                               gfloat    hscale,
                               gfloat    smooth_factor,
                               gfloat    delta_scale);
void t_terrain_mirror         (TTerrain *terrain,
                               gint      type);
void t_terrain_craters        (TTerrain *terrain,
                               gint      count,
                               gboolean  wrap,
                               gfloat    height_scale,
                               gfloat    radius_scale,
                               gfloat    crater_coverage,
                               gint      seed,
                               gfloat    center_x,
                               gfloat    center_y);
void t_terrain_all_rivers     (TTerrain *terrain,
                               gfloat    power, 
			       gint      method);
void t_terrain_river          (TTerrain *terrain,
                               gfloat    center_x,
                               gfloat    center_y);
void t_terrain_remove_rivers  (TTerrain *terrain);
void t_terrain_redraw_rivers  (TTerrain *terrain);
void t_terrain_rotate         (TTerrain *terrain,
                               gint      amount);
void t_terrain_roughen_smooth (TTerrain *terrain,
                               gboolean  roughen,
                               gboolean  big_grid,
                               gfloat    factor);
void t_terrain_terrace        (TTerrain *terrain,
                               gint      level_count,
                               gfloat    factor,
                               gboolean  adjust_sealevel);
void t_terrain_draw_transform (TPixbuf  *pixbuf,
                               gfloat    sea_threshold,
                               gfloat    sea_depth,
                               gfloat    sea_dropoff,
                               gfloat    above_power,
                               gfloat    below_power);
void t_terrain_transform      (TTerrain *terrain,
                               gfloat    sea_threshold,
                               gfloat    sea_depth,
                               gfloat    sea_dropoff,
                               gfloat    above_power,
                               gfloat    below_power);
void t_terrain_tiler          (TTerrain *terrain,
                               gfloat    offset);
void t_terrain_tile           (TTerrain *terrain,
                               gint      multiply);
void t_terrain_spherical      (TTerrain *terrain,
                               gfloat    offset);
void t_terrain_fill_basins    (TTerrain *terrain,
                               gint      iterations,
			       gboolean  big_grid);
TTerrain *t_terrain_flowmap   (TTerrain *terrain,
                               gboolean  do_sfd,
                               gboolean  ignore_sealevel,
                               gfloat    max_elevation_erode);
gint t_terrain_erode_flowmap  (TTerrain *terrain,
                               TTerrain *flowmap,
                               gint      iterations,
                               gboolean  trim_local_peaks);
gint t_terrain_erode          (TTerrain *terrain,
                               gint      iterations,
                               gint      max_flow_age,
                               gint      age_flowmap_times,
                               gfloat    max_elevation_erode,
                               gchar    *filename_anim,
                               gchar    *filename_flow,
                               gint      n_frames,
                               gboolean  trim_local_peaks,
                               gboolean  do_sfd,
                               gboolean  ignore_sealevel,
                               TPixbuf  *pixbuf);
TTerrain *t_terrain_join      (TTerrain *terrain_1,
                               TTerrain *terrain_2,
                               gfloat    distance, 
			       gboolean  reverse_dir,
			       gboolean  reverse_axis);
TTerrain *t_terrain_merge     (TTerrain *terrain_1,
                               TTerrain *terrain_2,
                               gfloat    weight_1,
                               gfloat    weight_2,
                               gint      operation);
void t_terrain_half           (TTerrain *terrain);
void t_terrain_double         (TTerrain *terrain,
                               gfloat    local,
                               gfloat    global);
void t_terrain_move           (TTerrain *terrain,
                               gfloat    x_offset,
                               gfloat    y_offset);
void t_terrain_connect        (TTerrain *terrain,
                               gint      iteration_count);
void t_terrain_digital_filter (TTerrain *terrain,
                               gfloat *filterData,
                               gint filterSize,  /* filter is square */
                               gfloat elv_min,
                               gfloat elv_max);
void t_terrain_rasterize      (TTerrain *terrain,
                               gint      x_size,
			       gint      y_size,
                               gfloat    tightness);
TTerrain *t_terrain_warp      (TTerrain *terrain,
                               TTerrain *tdisplacement,
                               gfloat    center_x,
                               gfloat    center_y, 
                               gint      op,
                               gfloat    factor);
void t_terrain_zoom           (TTerrain *terrain,
                               gint      x1,
                               gint      y1,
                               gint      x2,
                               gint      y2);

