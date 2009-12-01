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


#ifndef __TTERRAIN_H__
#define __TTERRAIN_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <gtk/gtk.h>
#include <libgnomeprint/gnome-print.h>
#include "colormaps.h"
#include "tcanvas.h"

typedef enum TComposeOp TComposeOp;
enum TComposeOp
{
  T_COMPOSE_NONE,
  T_COMPOSE_REPLACE,
  T_COMPOSE_ADD,
  T_COMPOSE_SUBTRACT
};

typedef struct TTerrainRiverCoords TTerrainRiverCoords;
struct TTerrainRiverCoords
{
  gfloat    x, y;
};

typedef struct TTerrainObject TTerrainObject;
struct TTerrainObject
{
  gint      ox, oy;
  gfloat    x, y;
  gfloat    angle;
  gfloat    scale_x, scale_y, scale_z;
  gchar    *name;
};

typedef struct TTerrainOptRender TTerrainOptRender;
struct TTerrainOptRender
{
  gboolean       do_clouds;
  gboolean       do_fog;
  gboolean       do_ground_fog;
  gboolean       do_observe_sealevel;
  gboolean       do_rainbow;
   /* Added by Raymond Ostertag */ 
  gboolean       do_trace_object;
  gboolean       do_trace_object_relativ;
  gboolean       do_trace_object_conserve;
  gboolean       do_trace_object_float;
  gboolean       do_camera_light;
  gboolean       do_stars;
  gboolean       do_celest_objects;
  gboolean       do_radiosity;
  gboolean       do_texture_constructor;
  gboolean       do_texture_stratum;
  gboolean       do_texture_grasstex;
  gboolean       do_texture_waterborder;
  gboolean       do_texture_waterborder_sand;
  gboolean       do_texture_waterborder_ice; 
  gboolean       do_water_apply_on_river;
  /* End by Raymond Ostertag */ 

  gfloat         camera_x, camera_y, camera_z;
  gfloat         lookat_x, lookat_y, lookat_z;
  gfloat         elevation_offset;
  gfloat         fog_alt;
  gfloat         fog_density;
  gfloat         fog_offset;
  gfloat         fog_turbulence;
  gfloat         west_direction;
  gfloat         time_of_day;
  gfloat         water_clarity;
  /* Added by Raymond Ostertag */ 
  gfloat         trace_object_x, trace_object_y, trace_object_z;
  gfloat         trace_object_scale;
  gfloat         ambient_light_luminosity;
  gfloat         camera_light_luminosity;
  gfloat         moon_time_offset;
  gfloat         ground_fog_density;
  gfloat         skycolor_red;
  gfloat         skycolor_green;
  gfloat         skycolor_blue;
  gfloat         lights_luminosity;
  gfloat         water_reflection;
  gfloat         water_color_red;
  gfloat         water_color_green;
  gfloat         water_color_blue;
  gfloat         water_frequency;
  gfloat         waves_height;
  gfloat         texture_spread;
  gfloat         terrain_turbulence;
  gfloat         bgimage_scale;
  gfloat         bgimage_offset;
  gfloat         bgimage_enlightment;
  gfloat         sun_size;
  gfloat         sun_color_red;
  gfloat         sun_color_green;
  gfloat         sun_color_blue;
  gfloat         moon_rot_Y;
  gfloat         moon_rot_Z;
  gfloat         moon_map_enlightment;
  gfloat         moon_size;
  gfloat         moon_color_red;
  gfloat         moon_color_green;
  gfloat         moon_color_blue;
  gfloat         fog_blue;
  gfloat         texture_rock_amount;
  gfloat         texture_grass_amount;
  gfloat         texture_snow_amount;
  gfloat         texture_rock_red;
  gfloat         texture_rock_green;
  gfloat         texture_rock_blue;
  gfloat         texture_rock_grain;
  gfloat         texture_grass_red;
  gfloat         texture_grass_green;
  gfloat         texture_grass_blue;
  gfloat         texture_grass_grain;
  gfloat         texture_snow_red;
  gfloat         texture_snow_green;
  gfloat         texture_snow_blue;
  gfloat         texture_snow_grain;
  gfloat         texture_stratum_size;
  gfloat         texture_waterborder_size;
  gfloat         texture_waterborder_sand_red;
  gfloat         texture_waterborder_sand_green;
  gfloat         texture_waterborder_sand_blue;
  gfloat         texture_waterborder_sand_grain;
  gfloat         camera_zoom;
  gfloat         radiosity_quality;
  /* End by Raymond Ostertag */ 

  gint           render_scale_x;
  gint           render_scale_y;       /* is derived from ysf and (x,y) */
  gint           render_scale_z;
  /* Added by Raymond Ostertag */ 
  gint           sky_system;
  gint           light_system;
  gint           camera_type;
  gint           texture_method;
  /* End by Raymond Ostertag */ 

  /* POVRay specific options */
  gchar         *atmosphere_file;
  gchar         *cloud_file;
  gchar         *image_size_string;
  gchar         *povray_filename;
  gchar         *star_file;
  gchar         *texture_file;
  gchar         *water_file;
  /* Added by Raymond Ostertag */ 
  gchar         *trace_object_file;
  gchar         *light_file;
  gchar         *scene_theme_file;
  gchar         *skycolor_file;
  gchar         *background_image_file;
  gchar         *moon_image_file;
  /* End by Raymond Ostertag */ 

  gboolean       do_output_file;       /* if FALSE no picture will be saved */
  gboolean       use_alpha;
  gboolean       do_custom_size;
  gboolean       do_partial_render;
  //gboolean       partial_render_as_percentage;
  gboolean       do_jitter;            /* aa specific */
  gboolean       do_aa;
  gint           bits_per_colour;
  gint           filetype;
  gint           image_width;
  gint           image_height;
  gint           render_quality;
  gint           aa_type;              /* 0 = type 1, 1 = type 2 */
  gfloat         start_column;
  gfloat         end_column;
  gfloat         start_row;
  gfloat         end_row;
  gfloat         aa_threshold;
  gfloat         aa_depth;
  gfloat         jitter_amount;
};

void t_terrain_object_clear (TTerrainObject *object);

#define T_TYPE_TERRAIN            (t_terrain_get_type ())
#define T_TERRAIN(obj)            (GTK_CHECK_CAST ((obj), T_TYPE_TERRAIN, TTerrain))
#define T_TERRAIN_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), T_TYPE_TERRAIN, TTerrain))
#define T_IS_TERRAIN(obj)         (GTK_CHECK_TYPE ((obj), T_TYPE_TERRAIN))
#define T_IS_TERRAIN_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), T_TYPE_TERRAIN))


typedef struct TTerrainClass TTerrainClass;
typedef struct TTerrain      TTerrain;

struct TTerrainClass
{
  GtkObjectClass object_class;

  /* Signals */
  void           (*heightfield_modified) (GtkObject *object);
  void           (*title_modified)       (GtkObject *object);
  void           (*selection_modified)   (GtkObject *object);
  void           (*object_added)         (GtkObject *object,
                                          gint       item);
  void           (*object_modified)      (GtkObject *object,
                                          gint       item);
  void           (*object_deleted)       (GtkObject *object,
                                          gint       item);
};

struct TTerrain
{
  GtkObject         object;

  gchar            *filename;
  gboolean          modified;
  gboolean          autogenned; /* Set true if filename is auto generated. */

  gint              width, height;
  gfloat           *heightfield;
  gfloat           *selection;

  gboolean          do_filled_sea;
  gint              contour_levels;
  gint              wireframe_resolution;
  gfloat            camera_height_factor;
  gfloat            sealevel;
  gfloat            y_scale_factor;

  /* objects is a GArray of TTerrainObjects */
  GArray           *objects;

  /* river data */
  gfloat           *riverfield;
  GArray           *riversources;

  /* option structs */
  TTerrainOptRender render_options;
};

extern guint title_modified_signal;
extern guint heightfield_modified_signal;
extern guint selection_modified_signal;
extern guint object_added_signal;
extern guint object_modified_signal;
extern guint object_deleted_signal;

GtkType    t_terrain_get_type             (void);
GtkObject *t_terrain_new                  (gint         width,
                                           gint         height);

void       t_terrain_set_filename         (TTerrain    *terrain,
                                           const gchar *filename);
gchar     *t_terrain_get_title            (TTerrain    *terrain);
void       t_terrain_set_modified         (TTerrain    *terrain,
                                           gboolean     modified);

gdouble    t_terrain_get_height           (TTerrain    *terrain,
                                           gdouble      x,
                                           gdouble      y);
void       t_terrain_heightfield_modified (TTerrain    *terrain);
void       t_terrain_selection_modified   (TTerrain    *terrain);
TTerrain  *t_terrain_clone                (TTerrain    *terrain);
TTerrain  *t_terrain_clone_resize         (TTerrain    *terrain,
                                           gint         width,
                                           gint         height,
					   gboolean     terrain_only);
void       t_terrain_set_size             (TTerrain    *terrain,
                                           gint         width,
                                           gint         height);
TTerrain  *t_terrain_clone_preview        (TTerrain    *terrain);
gint      *t_terrain_histogram            (TTerrain    *terrain, gint n_intervals, gint scale);
TTerrain  *t_terrain_clone_histogram      (TTerrain    *terrain, gfloat display_yscale);
void       t_terrain_copy_heightfield     (TTerrain    *from,
                                           TTerrain    *to);
void       t_terrain_copy_heightfield_and_extras (TTerrain   *from,
                                                  TTerrain   *to);
void       t_terrain_normalize            (TTerrain    *terrain,
                                           gboolean     never_grow);
void       t_terrain_crop                 (TTerrain    *terrain,
                                           gint         x1,
                                           gint         y1,
                                           gint         x2,
                                           gint         y2);
TTerrain  *t_terrain_crop_new             (TTerrain    *terrain,
                                           gint         x1,
                                           gint         y1,
                                           gint         x2,
                                           gint         y2);
void       t_terrain_invert               (TTerrain    *terrain);

void       t_terrain_pack_terrain_options (TTerrain    *terrain,
                                           GtkWidget   *options);
void       t_terrain_unpack_terrain_options(TTerrain   *terrain,
                                           GtkWidget   *options);
void       t_terrain_pack_scene_options   (TTerrain    *terrain,
                                           GtkWidget   *options);
void       t_terrain_unpack_scene_options (TTerrain    *terrain,
                                           GtkWidget   *options);
void       t_terrain_pack_povray_options  (TTerrain    *terrain,
					   GtkWidget   *options);
void       t_terrain_unpack_povray_options(TTerrain    *terrain,
					   GtkWidget   *options);
void       t_terrain_print_contour_map    (TTerrain    *terrain,
                                           GnomePrintContext *context);

void       t_terrain_ref                  (TTerrain    *terrain);
void       t_terrain_unref                (TTerrain    *terrain);

TTerrain  *t_terrain_import_heightfield   (gchar       *filename);
void       t_terrain_export_heightfield   (TTerrain    *terrain,
                                           gchar       *filename);
void       t_terrain_select_by_height     (TTerrain   *terrain,
                                           gfloat      floor,
                                           gfloat      ceil,
                                           TComposeOp  op);
void       t_terrain_select               (TTerrain    *terrain,
                                           gfloat       x1,
                                           gfloat       y1,
                                           gfloat       x2,
                                           gfloat       y2,
                                           TSelectionType type,
                                           TComposeOp   op);
void       t_terrain_select_destroy       (TTerrain    *terrain);
gint       t_terrain_add_object           (TTerrain    *terrain,
                                           gint         ox,
                                           gint         oy,
                                           gfloat       x,
                                           gfloat       y,
                                           gfloat       angle,
                                           gfloat       scale_x,
                                           gfloat       scale_y,
                                           gfloat       scale_z,
                                           gchar       *object_name);
void       t_terrain_move_object          (TTerrain    *terrain,
                                           gint         item,
                                           gfloat       x,
                                           gfloat       y);
void       t_terrain_rotate_object        (TTerrain    *terrain,
                                           gint         item,
                                           gfloat       angle);
void       t_terrain_remove_object        (TTerrain    *terrain,
                                           gint         item);
void       t_terrain_remove_objects       (TTerrain    *terrain);
gint       t_terrain_seed_data            (TTerrain    *terrain, 
                                           gfloat      *data,
					   gint         width, 
					   gint         height);
gint       t_terrain_seed                 (TTerrain    *terrain,
                                           gint         new_width, 
                                           gint         new_height, 
			                   gint         sel_x,
			                   gint         sel_y, 
					   gint         sel_width, 
					   gint         sel_height);
TTerrain  *t_terrain_tile_new             (TTerrain    *terrain,
                                           gint         num_x, 
                                           gint         num_y); 

void       t_terrain_pack_povray_trace    (TTerrain    *terrain,
					   GtkWidget   *trace_data);

void       t_terrain_remove_trace_objects  (TTerrain   *terrain);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TTERRAIN_H__ */
