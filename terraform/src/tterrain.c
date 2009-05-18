
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <gnome.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

#include "optionwindow.h"
#include "tterrain.h"
#include "tterrainview.h"
#include "tperlin.h"
#include "selection.h"
#include "support.h"
#include "support2.h"
#include "base64.h"
#include "xmlsupport.h"
#include "terrainwindow.h"
#include "filterwindow.h"
#include "contour.h"
#include "filenameops.h"

#define CLIP(x,m,M) MAX(MIN(x,M),m)
#define SQR(x) ((x)*(x))

guint heightfield_modified_signal;
guint title_modified_signal;
guint selection_modified_signal;
guint object_added_signal;
guint object_modified_signal;
guint object_deleted_signal;

void
t_terrain_object_clear (TTerrainObject *object)
{
  g_free (object->name);
  memset (object, 0, sizeof (TTerrainObject));
}

static gint
g_list_string_index (GList *list,
                     gchar *string)
{
  gint i;

  g_return_val_if_fail (list != NULL, -1);
  g_return_val_if_fail (string != NULL, -1);

  for (i = 0, list = g_list_first (list); list != NULL; i++, list = list->next)
    if (!strcmp (list->data, string))
      return i;

  return -1;
}

static void t_terrain_class_init    (TTerrainClass  *klass);
static void t_terrain_init          (TTerrain       *terrain);
static void t_terrain_destroy       (GtkObject      *object);
static void t_terrain_finalize      (GObject        *object);


GtkType
t_terrain_get_type (void)
{
  static GtkType terrain_type = 0;
  
  if (!terrain_type)
    {
      static const GtkTypeInfo terrain_info =
      {
        "TTerrain",
        sizeof (TTerrain),
        sizeof (TTerrainClass),
        (GtkClassInitFunc) t_terrain_class_init,
        (GtkObjectInitFunc) t_terrain_init,
        /* reserved_1 */ NULL,
        /* reserved_2 */ NULL,
        (GtkClassInitFunc) NULL
      };
      
      terrain_type = gtk_type_unique (GTK_TYPE_OBJECT, &terrain_info);
    }
  
  return terrain_type;
}


GtkObject *
t_terrain_new (gint width,
               gint height)
{
  TTerrain *terrain;

  terrain = gtk_type_new (T_TYPE_TERRAIN);
  terrain->width = width;
  terrain->height = height;
  terrain->heightfield = g_new0 (gfloat, width * height);

  return GTK_OBJECT (terrain);
}


void
t_terrain_set_filename (TTerrain    *terrain,
                        const gchar *filename)
{
  if (terrain->filename != filename)
    {
      g_free (terrain->filename);
      terrain->filename = g_strdup (filename);
      gtk_signal_emit (GTK_OBJECT (terrain), title_modified_signal);
    }
}


gchar *
t_terrain_get_title (TTerrain *terrain)
{
  return g_strdup_printf ("%s%s", terrain->filename,
                          terrain->modified ? _(" (not saved)") : "");
}


void
t_terrain_set_modified (TTerrain   *terrain,
                        gboolean    modified)
{
  if (terrain->modified != modified)
    {
      terrain->modified = modified;
      gtk_signal_emit (GTK_OBJECT (terrain), title_modified_signal);
    }
}


/* t_terrain_get_height: retrieve height of terrain at a point using
 * linear interpolation
 */
gdouble
t_terrain_get_height (TTerrain *terrain,
                      gdouble   x,
                      gdouble   y)
{
  gdouble point[4];
  int     index_x, index_y;
  int     index;

  index_x = (int) (x * (terrain->width - 1));
  index_x = CLIP (index_x, 0, terrain->width - 2);

  index_y = (int) (y * (terrain->height - 1));
  index_y = CLIP (index_y, 0, terrain->height - 2);

  index = index_y * terrain->width + index_x;
  point[0] = terrain->heightfield[index];
  point[1] = terrain->heightfield[index + 1];
  point[2] = terrain->heightfield[index + terrain->width];
  point[3] = terrain->heightfield[index + terrain->width + 1];

  x = x * (terrain->width - 1) - index_x;
  y = y * (terrain->height - 1) - index_y;

  return (point[0] * (1.0 - x) + point[1] * x) * (1.0 - y) +
         (point[2] * (1.0 - x) + point[3] * x) * y;
}


void
t_terrain_heightfield_modified (TTerrain *terrain)
{
  t_terrain_set_modified (terrain, TRUE);

  gtk_signal_emit (GTK_OBJECT (terrain), heightfield_modified_signal);
}


void
t_terrain_selection_modified (TTerrain *terrain)
{
  gtk_signal_emit (GTK_OBJECT (terrain), selection_modified_signal);
}


static GtkObjectClass *parent_class;

static void
t_terrain_class_init (TTerrainClass *klass)
{
  GObjectClass   *g_object_class = G_OBJECT_CLASS (klass);
  GtkObjectClass *object_class = GTK_OBJECT_CLASS (klass);
  GtkType         type;

  type = T_TYPE_TERRAIN;

  parent_class = gtk_type_class (GTK_TYPE_OBJECT);

  klass->heightfield_modified = NULL;
  klass->title_modified       = NULL;
  klass->selection_modified   = NULL;
  klass->object_added         = NULL;
  klass->object_modified      = NULL;
  klass->object_deleted       = NULL;

  object_class->destroy       = &t_terrain_destroy;
  g_object_class->finalize    = &t_terrain_finalize;

  heightfield_modified_signal =
    gtk_signal_new ("heightfield_modified",
                    GTK_RUN_FIRST,
                    type,
                    GTK_SIGNAL_OFFSET (TTerrainClass, heightfield_modified),
                    gtk_marshal_NONE__NONE,
                    GTK_TYPE_NONE, 0);

  title_modified_signal =
    gtk_signal_new ("title_modified",
                    GTK_RUN_FIRST,
                    type,
                    GTK_SIGNAL_OFFSET (TTerrainClass, title_modified),
                    gtk_marshal_NONE__NONE,
                    GTK_TYPE_NONE, 0);

  selection_modified_signal =
    gtk_signal_new ("selection_modified",
                    GTK_RUN_FIRST,
                    type,
                    GTK_SIGNAL_OFFSET (TTerrainClass, selection_modified),
                    gtk_marshal_NONE__NONE,
                    GTK_TYPE_NONE, 0);

  object_added_signal =
    gtk_signal_new ("object_added",
                    GTK_RUN_FIRST,
                    type,
                    GTK_SIGNAL_OFFSET (TTerrainClass, object_added),
                    gtk_marshal_NONE__INT,
                    GTK_TYPE_NONE, 1, GTK_TYPE_INT);

  object_modified_signal =
    gtk_signal_new ("object_modified",
                    GTK_RUN_FIRST,
                    type,
                    GTK_SIGNAL_OFFSET (TTerrainClass, object_modified),
                    gtk_marshal_NONE__INT,
                    GTK_TYPE_NONE, 1, GTK_TYPE_INT);

  object_deleted_signal =
    gtk_signal_new ("object_deleted",
                    GTK_RUN_FIRST,
                    type,
                    GTK_SIGNAL_OFFSET (TTerrainClass, object_deleted),
                    gtk_marshal_NONE__INT,
                    GTK_TYPE_NONE, 1, GTK_TYPE_INT);
}

static void
t_terrain_init (TTerrain *terrain)
{
  terrain->filename = g_strdup ("");
  terrain->heightfield = NULL;
  terrain->selection = NULL;
  terrain->width = 0;
  terrain->height = 0;
  terrain->modified = FALSE;
  terrain->autogenned = FALSE;

  terrain->objects = g_array_new (FALSE, FALSE, sizeof (TTerrainObject));
  terrain->riversources = g_array_new (FALSE, FALSE, 
                                       sizeof (TTerrainRiverCoords));
  terrain->riverfield = NULL;

  terrain->camera_height_factor = 0.33;
  terrain->contour_levels = 10;
  terrain->do_filled_sea = TRUE;
  terrain->sealevel = 0.33;
  terrain->wireframe_resolution = 10;
  terrain->y_scale_factor = 0.33;

  terrain->render_options.do_clouds = FALSE;
  terrain->render_options.do_fog = FALSE;
  terrain->render_options.do_ground_fog = FALSE;
  terrain->render_options.do_observe_sealevel = TRUE;
  terrain->render_options.do_rainbow = FALSE;
  terrain->render_options.do_trace_object_relativ = FALSE;
  terrain->render_options.do_trace_object_conserve = TRUE;
  terrain->render_options.do_trace_object_float = TRUE;
  terrain->render_options.do_camera_light = FALSE;
  terrain->render_options.do_stars = FALSE;  
  terrain->render_options.do_celest_objects = TRUE;  
  terrain->render_options.do_radiosity = FALSE;
  terrain->render_options.do_texture_constructor = FALSE;
  terrain->render_options.do_texture_stratum = FALSE;
  terrain->render_options.do_texture_grasstex = FALSE;
  terrain->render_options.do_texture_waterborder = FALSE;
  terrain->render_options.do_texture_waterborder_sand = TRUE;  
  terrain->render_options.do_texture_waterborder_ice = FALSE;
  terrain->render_options.do_water_apply_on_river = TRUE;

  terrain->render_options.camera_x = 0.5;
  terrain->render_options.camera_y = 2.5;
  terrain->render_options.camera_z = -1.25;
  terrain->render_options.lookat_x = 0.5;
  terrain->render_options.lookat_y = 0.0;
  terrain->render_options.lookat_z = 0.5;
  terrain->render_options.elevation_offset = 0.005;
  terrain->render_options.fog_alt = 0.40;
  terrain->render_options.fog_density = 0.25;
  terrain->render_options.fog_offset = 0.40;
  terrain->render_options.fog_turbulence = 0.00;
  terrain->render_options.time_of_day = 10.5;
  terrain->render_options.west_direction = 0.0;
  terrain->render_options.water_clarity = 0.30;
  terrain->render_options.trace_object_x = 0.5;  
  terrain->render_options.trace_object_y = 2.0;
  terrain->render_options.trace_object_z = 0.5;
  terrain->render_options.trace_object_scale = 10.0;
  terrain->render_options.ambient_light_luminosity = 1.0;
  terrain->render_options.camera_light_luminosity = 0.5;
  terrain->render_options.moon_time_offset = 12.0;
  terrain->render_options.ground_fog_density= 0.25;
  terrain->render_options.skycolor_red= 1.0; 
  terrain->render_options.skycolor_green= 1.0; 
  terrain->render_options.skycolor_blue= 1.0; 
  terrain->render_options.lights_luminosity= 1.00; 
  terrain->render_options.water_reflection= 0.30; 
  terrain->render_options.water_color_red= 0.60; 
  terrain->render_options.water_color_green= 0.75; 
  terrain->render_options.water_color_blue= 0.85; 
  terrain->render_options.water_frequency= 1300.00;  
  terrain->render_options.waves_height= 1.00;  
  terrain->render_options.texture_spread= 1.00;  
  terrain->render_options.terrain_turbulence= 0.01;  
  terrain->render_options.bgimage_scale= 1.00;  
  terrain->render_options.bgimage_offset= 0.30;  
  terrain->render_options.bgimage_enlightment= 0.20;  
  terrain->render_options.sun_size= 1.00;  
  terrain->render_options.sun_color_red= 1.00;  
  terrain->render_options.sun_color_green= 0.95;  
  terrain->render_options.sun_color_blue= 0.60;  
  terrain->render_options.moon_rot_Y= 0.00;  
  terrain->render_options.moon_rot_Z= 0.00;  
  terrain->render_options.moon_map_enlightment= 1.00;  
  terrain->render_options.moon_size= 1.00;  
  terrain->render_options.moon_color_red= 0.35;  
  terrain->render_options.moon_color_green= 0.35;  
  terrain->render_options.moon_color_blue= 0.35;  
  terrain->render_options.fog_blue= 0.30;
  terrain->render_options.texture_rock_amount = 0.33;
  terrain->render_options.texture_grass_amount = 0.33;
  terrain->render_options.texture_snow_amount = 0.33;
  terrain->render_options.texture_rock_red = 0.60;
  terrain->render_options.texture_rock_green = 0.60;
  terrain->render_options.texture_rock_blue = 0.60;
  terrain->render_options.texture_rock_grain = 1.00;
  terrain->render_options.texture_grass_red = 0.85;
  terrain->render_options.texture_grass_green = 0.99;
  terrain->render_options.texture_grass_blue = 0.70;
  terrain->render_options.texture_grass_grain = 1.50;
  terrain->render_options.texture_snow_red = 1.00;
  terrain->render_options.texture_snow_green = 1.00;
  terrain->render_options.texture_snow_blue = 1.00;
  terrain->render_options.texture_snow_grain = 1.00;
  terrain->render_options.texture_stratum_size = 1.00;
  terrain->render_options.texture_waterborder_size = 0.80;
  terrain->render_options.texture_waterborder_sand_red = 0.90;
  terrain->render_options.texture_waterborder_sand_green = 0.85;
  terrain->render_options.texture_waterborder_sand_blue = 0.70;
  terrain->render_options.texture_waterborder_sand_grain = 0.70;
  terrain->render_options.camera_zoom = 0.50;
  terrain->render_options.radiosity_quality = 0.0;

  terrain->render_options.sky_system= 0;  /*Regular sky*/
  terrain->render_options.light_system= 0;  /*Dynamic lights*/
  terrain->render_options.camera_type= 0; /*Perspective*/
  terrain->render_options.texture_method= 0;

  terrain->render_options.render_scale_x = 1000;
  terrain->render_options.render_scale_y = 1000*terrain->y_scale_factor;
  terrain->render_options.render_scale_z = 1000;

  terrain->render_options.atmosphere_file = g_strdup ("earth_fog.inc");
  terrain->render_options.cloud_file = g_strdup ("clouds_01.inc");
  terrain->render_options.image_size_string = g_strdup ("320x240");
  terrain->render_options.povray_filename =  g_strdup ("");
  terrain->render_options.star_file = g_strdup ("stars_01.inc");
  terrain->render_options.texture_file = g_strdup ("earth_green_landscape.inc");
  terrain->render_options.water_file = g_strdup ("earth_water.inc");
  terrain->render_options.light_file = g_strdup ("lights_studio.inc");
  terrain->render_options.scene_theme_file = g_strdup ("");
  terrain->render_options.skycolor_file = g_strdup ("skycolor_earth.inc");
  terrain->render_options.background_image_file = g_strdup ("sky_test_320x180.tga");
  terrain->render_options.moon_image_file = g_strdup ("moon_moon.png"); 

  terrain->render_options.do_output_file = TRUE;
  terrain->render_options.use_alpha = FALSE;
  terrain->render_options.do_aa = FALSE;
  terrain->render_options.do_custom_size = FALSE;
  terrain->render_options.do_jitter = FALSE;
  terrain->render_options.bits_per_colour = 8;
  terrain->render_options.filetype = 0;          /* 0 = png */
  terrain->render_options.image_width = 320;
  terrain->render_options.image_height = 240;
  terrain->render_options.render_quality = 9;
  terrain->render_options.do_partial_render = FALSE;
  //terrain->render_options.partial_render_as_percentage = FALSE;
  terrain->render_options.start_column = 0;
  terrain->render_options.end_column = 320;
  terrain->render_options.start_row = 0;
  terrain->render_options.end_row = 240;
  terrain->render_options.aa_depth = 1.0;
  terrain->render_options.aa_threshold = 0.3;
  terrain->render_options.aa_type = 0;
  terrain->render_options.jitter_amount = 0.5;
}

static void
t_terrain_free_render_options (TTerrainOptRender *ropt)
{
  g_free (ropt->povray_filename);
  g_free (ropt->texture_file);
  g_free (ropt->cloud_file);
  g_free (ropt->atmosphere_file);
  g_free (ropt->star_file);
  g_free (ropt->water_file);
  g_free (ropt->trace_object_file);  
  g_free (ropt->light_file);
  g_free (ropt->image_size_string);
  g_free (ropt->scene_theme_file);
  g_free (ropt->skycolor_file);
  g_free (ropt->background_image_file);
  g_free (ropt->moon_image_file);
}

static void
t_terrain_free (TTerrain *terrain)
{
  t_terrain_remove_objects (terrain);

  g_array_free (terrain->objects, TRUE);
  g_array_free (terrain->riversources, TRUE);

  g_free (terrain->heightfield);
  g_free (terrain->riverfield);
  g_free (terrain->selection);
  g_free (terrain->filename);

  t_terrain_free_render_options (&(terrain->render_options));
}

static void
t_terrain_destroy (GtkObject *object)
{
  TTerrain *terrain = T_TERRAIN (object);

  t_terrain_free (terrain);
  t_terrain_init (terrain);

  if (parent_class->destroy != NULL)
    parent_class->destroy (object);
}

static void
t_terrain_finalize (GObject *object)
{
  TTerrain *terrain = T_TERRAIN (object);

  t_terrain_free (terrain);

  if (G_OBJECT_CLASS (parent_class)->finalize != NULL)
    G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void 
t_terrain_copy_settings_render_options (TTerrainOptRender *new, 
                                        TTerrainOptRender *orig)
{
  /* render related options, assumed non-required for previews */
  g_free (new->texture_file);
  new->texture_file = g_strdup (orig->texture_file);

  g_free (new->cloud_file);
  new->cloud_file = g_strdup (orig->cloud_file);

  g_free (new->atmosphere_file);
  new->atmosphere_file = g_strdup (orig->atmosphere_file);

  g_free (new->star_file);
  new->star_file = g_strdup (orig->star_file);

  g_free (new->water_file);
  new->water_file = g_strdup (orig->water_file);

  g_free (new->trace_object_file);
  new->trace_object_file = g_strdup (orig->trace_object_file);

  g_free (new->light_file);
  new->light_file = g_strdup (orig->light_file);

  g_free (new->scene_theme_file);
  new->scene_theme_file = g_strdup (orig->scene_theme_file);

  g_free (new->skycolor_file);
  new->skycolor_file = g_strdup (orig->skycolor_file);

  g_free (new->background_image_file);
  new->background_image_file = g_strdup (orig->background_image_file);

  g_free (new->moon_image_file);
  new->moon_image_file = g_strdup (orig->moon_image_file);

  new->camera_x = orig->camera_x;
  new->camera_y = orig->camera_y;
  new->camera_z = orig->camera_z;
  new->lookat_x = orig->lookat_x;
  new->lookat_y = orig->lookat_y;
  new->lookat_z = orig->lookat_z;
  new->elevation_offset = orig->elevation_offset;
  new->do_clouds = orig->do_clouds;
  new->do_fog = orig->do_fog;
  new->do_ground_fog = orig->do_ground_fog;
  new->do_rainbow = orig->do_rainbow;
  new->do_trace_object_relativ = orig->do_trace_object_relativ;
  new->do_trace_object_conserve = orig->do_trace_object_conserve;
  new->do_trace_object_float = orig->do_trace_object_float;
  new->do_camera_light = orig->do_camera_light;
  new->do_stars = orig->do_stars;
  new->do_celest_objects = orig->do_celest_objects;
  new->do_radiosity= orig->do_radiosity;
  new->do_texture_constructor = orig->do_texture_constructor;
  new->do_texture_stratum = orig->do_texture_stratum;
  new->do_texture_grasstex = orig->do_texture_grasstex;
  new->do_texture_waterborder = orig->do_texture_waterborder;
  new->do_texture_waterborder_sand= orig->do_texture_waterborder_sand;
  new->do_texture_waterborder_ice = orig->do_texture_waterborder_ice;
  new->do_water_apply_on_river = orig->do_water_apply_on_river;

  new->fog_offset = orig->fog_offset;
  new->fog_alt = orig->fog_alt;
  new->fog_density = orig->fog_density;
  new->fog_turbulence = orig->fog_turbulence;
  new->time_of_day = orig->time_of_day;
  new->west_direction = orig->west_direction;
  new->water_clarity = orig->water_clarity;
  new->trace_object_x = orig->trace_object_x;
  new->trace_object_y = orig->trace_object_y;
  new->trace_object_z = orig->trace_object_z;
  new->ambient_light_luminosity  = orig->ambient_light_luminosity;
  new->camera_light_luminosity  = orig->camera_light_luminosity;
  new->moon_time_offset  = orig->moon_time_offset;
  new->ground_fog_density  = orig->ground_fog_density;
  new->skycolor_red  = orig->skycolor_red;
  new->skycolor_green  = orig->skycolor_green;
  new->skycolor_blue  = orig->skycolor_blue; 
  new->lights_luminosity = orig->lights_luminosity;
  new->water_reflection = orig->water_reflection;
  new->water_color_red = orig->water_color_red;
  new->water_color_green = orig->water_color_green;
  new->water_color_blue = orig->water_color_blue;
  new->water_frequency = orig->water_frequency;
  new->waves_height = orig->waves_height;
  new->texture_spread = orig->texture_spread;
  new->terrain_turbulence = orig->terrain_turbulence;
  new->bgimage_scale = orig->bgimage_scale;
  new->bgimage_offset = orig->bgimage_offset;
  new->bgimage_enlightment = orig->bgimage_enlightment;
  new->sun_size = orig->sun_size;
  new->sun_color_red = orig->sun_color_red;
  new->sun_color_green = orig->sun_color_green;
  new->sun_color_blue = orig->sun_color_blue;
  new->moon_rot_Y = orig->moon_rot_Y;
  new->moon_rot_Z = orig->moon_rot_Z;
  new->moon_map_enlightment = orig->moon_map_enlightment;
  new->moon_size = orig->moon_size;
  new->moon_color_red = orig->moon_color_red;
  new->moon_color_green = orig->moon_color_green;
  new->moon_color_blue = orig->moon_color_blue;
  new->fog_blue = orig->fog_blue;
  new->texture_rock_amount = orig->texture_rock_amount;
  new->texture_grass_amount = orig->texture_grass_amount;
  new->texture_snow_amount = orig->texture_snow_amount;
  new->texture_rock_red = orig->texture_rock_red;
  new->texture_rock_green = orig->texture_rock_green;
  new->texture_rock_blue = orig->texture_rock_blue;
  new->texture_rock_grain = orig->texture_rock_grain;
  new->texture_grass_red = orig->texture_grass_red;
  new->texture_grass_green = orig->texture_grass_green;
  new->texture_grass_blue = orig->texture_grass_blue;
  new->texture_grass_grain = orig->texture_grass_grain;
  new->texture_snow_red = orig->texture_snow_red;
  new->texture_snow_green = orig->texture_snow_green;
  new->texture_snow_blue = orig->texture_snow_blue;
  new->texture_snow_grain = orig->texture_snow_grain;
  new->texture_stratum_size = orig->texture_stratum_size;
  new->texture_waterborder_size = orig->texture_waterborder_size;
  new->texture_waterborder_sand_red = orig->texture_waterborder_sand_red;
  new->texture_waterborder_sand_green= orig->texture_waterborder_sand_green;
  new->texture_waterborder_sand_blue = orig->texture_waterborder_sand_blue;
  new->texture_waterborder_sand_grain = orig->texture_waterborder_sand_grain;
  new->camera_zoom = orig->camera_zoom;
  new->radiosity_quality = orig->radiosity_quality;

  new->sky_system = orig->sky_system;
  new->light_system= orig->light_system;
  new->camera_type= orig->camera_type;
  new->texture_method= orig->texture_method;
 
  new->render_scale_x = orig->render_scale_x;
  new->render_scale_y = orig->render_scale_y;
  new->render_scale_z = orig->render_scale_z;
  new->render_scale_z = orig->render_scale_z;
  new->do_observe_sealevel = orig->do_observe_sealevel;

  /* povray specific options */
  g_free (new->povray_filename);
  new->povray_filename = g_strdup (orig->povray_filename);

  g_free (new->image_size_string);
  new->image_size_string = g_strdup (orig->image_size_string);

  new->do_output_file = orig->do_output_file;
  new->use_alpha = orig->use_alpha;
  new->bits_per_colour = orig->bits_per_colour;
  new->render_quality = orig->render_quality;
  new->filetype = orig->filetype;
  new->do_custom_size = orig->do_custom_size;
  new->image_width = orig->image_width;
  new->image_height = orig->image_height;
  new->do_partial_render = orig->do_partial_render;
  //new->partial_render_as_percentage = orig->partial_render_as_percentage;
  new->start_column = orig->start_column;
  new->end_column = orig->end_column;
  new->start_row = orig->start_row;
  new->end_row = orig->end_row;
  new->do_aa = orig->do_aa;
  new->aa_threshold = orig->aa_threshold;
  new->aa_type = orig->aa_type;
  new->aa_depth = orig->aa_depth;
  new->do_jitter = orig->do_jitter;
  new->jitter_amount = orig->jitter_amount;
}

static void 
t_terrain_copy_settings (TTerrain *new, TTerrain *orig)
{
  g_free (new->filename);
  new->filename = g_strdup (orig->filename);

  new->camera_height_factor = orig->camera_height_factor;
  new->contour_levels = orig->contour_levels;
  new->do_filled_sea = orig->do_filled_sea;
  new->sealevel = orig->sealevel;
  new->wireframe_resolution = orig->wireframe_resolution;
  new->y_scale_factor = orig->y_scale_factor;

  t_terrain_copy_settings_render_options (&(new->render_options), 
                                          &(orig->render_options));
}

TTerrain *
t_terrain_clone (TTerrain *terrain)
{
  GtkObject *object;
  TTerrain  *out;
  gint       i;
  gint       memsize;
  gint       memsizeb;

  memsize = terrain->width * terrain->height * sizeof(gfloat);
  memsizeb = terrain->width * terrain->height * sizeof(gboolean);

  object = t_terrain_new (terrain->width, terrain->height);
  out = T_TERRAIN (object);

  t_terrain_copy_settings (out, terrain);

  memcpy (out->heightfield, terrain->heightfield, memsize);

  if (terrain->riverfield != NULL)
    out->riverfield = g_memdup (terrain->riverfield,
                                sizeof(gfloat)*terrain->width*terrain->height);

  if (terrain->selection != NULL)
    out->selection = g_memdup (terrain->selection,
                               sizeof(gfloat)*terrain->width*terrain->height);

  if (terrain->riversources != NULL)
    for (i=0; i<terrain->riversources->len; i++)
      g_array_append_val (out->riversources, 
              g_array_index (terrain->riversources, TTerrainRiverCoords, i));

  if (terrain->objects != NULL)
    for (i=0; i<terrain->objects->len; i++)
      g_array_append_val (out->objects, g_array_index (terrain->objects, TTerrainObject, i));

  return out;
}

TTerrain *
t_terrain_clone_resize (TTerrain *terrain,
                        gint      width,
                        gint      height, 
			gboolean  terrain_only)
{
  GtkObject  *object;
  TTerrain   *out;
  gfloat      fx = width/terrain->width;
  gfloat      fz = height/terrain->height;
  gfloat      fy = (fx+fz)/2;
  gint        y;
  gint        pos;

  object = t_terrain_new (width, height);
  out = T_TERRAIN (object);

  t_terrain_copy_settings (out, terrain);

  if (terrain->riverfield != NULL)
    out->riverfield = g_new (gfloat, out->width * out->height);

  if (terrain->selection != NULL)
    out->selection = g_new (gfloat, out->width * out->height);

  pos = 0;
  for (y = 0; y < out->height; y++)
    {
      gint x;
      gint count;
      gint prev_pos;

      prev_pos = y * terrain->height / out->height * terrain->width;
      count = 0;
      for (x = 0; x < out->width; x++)
        {
          out->heightfield[pos] = terrain->heightfield[prev_pos];

	  if (out->riverfield != NULL)
            out->riverfield[pos] = terrain->riverfield[prev_pos];

          if (out->selection != NULL)
            out->selection[pos] = terrain->selection[prev_pos];

          pos++;
          count += terrain->width;
          while (count >= out->width)
            {
              count -= out->width;
              prev_pos++;
            }
        }
    }

  /* clone_resize doesn't always need rivers (when it's used for preview) */
  if (!terrain_only)
    for (y=0; y<terrain->riversources->len; y++)
      {
      TTerrainRiverCoords  *obj;

      obj = &g_array_index (terrain->riversources, TTerrainRiverCoords, y);
      obj->x *= fx;
      obj->y *= fz;

      g_array_append_val (out->riversources, obj);
      }

  /* clone_resize doesn't always need objects (when it's used for preview) */
  if (!terrain_only)
    for (y = 0; y < terrain->objects->len; y++)
      {
        TTerrainObject obj;

        obj = g_array_index (terrain->objects, TTerrainObject, y);
        obj.x *= fx;
        obj.y *= fz;
        obj.scale_x *= fx;
        obj.scale_z *= fz;
        obj.scale_y *= fy;

        g_array_append_val (out->objects, obj);
      }

  return out;
}

void
t_terrain_set_size (TTerrain *terrain,
                    gint      width,
                    gint      height)
{
  if (terrain->width != width || terrain->height != height)
    {
      terrain->width = width;
      terrain->height = height;

      g_free (terrain->heightfield);
      g_free (terrain->selection);
      g_free (terrain->riverfield);

      terrain->heightfield = g_new0 (gfloat, width * height);
      terrain->selection = NULL;
      terrain->riverfield = NULL;
    }
}

TTerrain *
t_terrain_clone_preview (TTerrain *terrain)
{
  gint      maximum;
  TTerrain *clone;

  maximum = MAX (terrain->width, terrain->height);

  clone = t_terrain_clone_resize (terrain,
              MAX (1, MIN(terrain->width * 100 / maximum, terrain->width)),
              MAX (1, MIN(terrain->height * 100 / maximum, terrain->height)),
              TRUE);

  clone->wireframe_resolution = 5;

  return clone;
}

gint *
t_terrain_histogram (TTerrain *terrain, gint n, gint scale)
{
  gint        tsize = terrain->width * terrain->height;
  gint       *data;
  gint        max = 0;
  gint        i;

  data = g_new0 (gint, n);
  for (i = 0; i < tsize; i++)
    {
      gint off;
	    
      off = (gint) (terrain->heightfield[i] * n);
      off = (off == n ? off - 1 : off);
      data[off]++;
    }

  /* find max and adjust all data points to 1 .. 100 */
  for (i=0; i<n; i++)
    if (data[i] > max)
      max = data[i];

  for (i=0; i<n; i++)
    {
      gint old = data[i];

      data[i] = (gint)(((gfloat) data[i] / max) * scale);

      /* make sure small counts don't disappear */
      if (data[i] == 0 && old > 0)
        data[i] = 1;
    }

  return data;
}

/*
 * create a preview HF filled with histogram values 
 * since we already have code to paint HFs, this is 
 * a lazy way of getting a histogram displayed 
 */
TTerrain *
t_terrain_clone_histogram (TTerrain *terrain, gfloat display_yscale)
{
  GtkObject  *object;
  TTerrain   *thist;
  gint       *dhist;
  gint        size;   /* number of intervals */
  gint        width;
  gint        height;
  gint        i,j;
  gint        ypos;
  gfloat      f;
  gfloat      v;
  gfloat      lim;

  size = width = height = 100;
  f = 1.0/size;
  
  dhist = t_terrain_histogram (terrain, size, height);

  object = t_terrain_new (width, height);
  thist = T_TERRAIN (object);

  /* populate histogram data */
  for (i=0; i<width; i++)
    {
      v = i * f;
      lim = dhist[i] * display_yscale;

      for (j=0; j<lim; j++)
        {
          ypos = height-j-1;
          thist->heightfield[ypos*width+i] = v;
        }
    }

  g_free (dhist);

  return thist;
}

void
t_terrain_copy_heightfield (TTerrain *from,
                            TTerrain *to)
{
  gint memsize;

  g_return_if_fail (from->width == to->width &&
                    from->height == to->height);

  memsize = from->width*from->height*sizeof(gfloat);

  memcpy (to->heightfield, from->heightfield, memsize);
  to->sealevel = from->sealevel;
}

void
t_terrain_copy_heightfield_and_extras (TTerrain   *from,
                                       TTerrain   *to)
{
  gint memsize;
  gint i;

  g_return_if_fail (from->width == to->width &&
                    from->height == to->height);

  memsize = from->width*from->height*sizeof(gfloat);

  t_terrain_copy_heightfield (from, to);

  /* copy selection */
  if (from->selection == NULL)
    {
      g_free (to->selection);
      to->selection = NULL;
    }
  else
    {
      if (to->selection == NULL)
        to->selection = g_memdup (from->selection, memsize);
      else
        memcpy (to->selection, from->selection, memsize);
    }

  /* copy riverfield */
  if (from->riverfield == NULL)
    {
      g_free (to->riverfield);
      to->riverfield = NULL;
    }
  else
    {
      if (to->selection == NULL)
        to->selection = g_memdup (from->riverfield, memsize);
      else
        memcpy (to->riverfield, from->riverfield, memsize);
    }

  /* copy riversources */
  if (from->riversources != NULL)
    for (i=0; i<from->riversources->len; i++)
      g_array_append_val (to->riversources, 
              g_array_index (from->riversources, TTerrainRiverCoords, i));

  /* copy riversources */
  if (from->objects != NULL)
    for (i=0; i<from->objects->len; i++)
      g_array_append_val (to->objects, 
              g_array_index (from->objects, TTerrainObject, i));

}

void
t_terrain_normalize (TTerrain *terrain,
                     gboolean  never_grow)
{
  gint    i, last;
  gfloat  min, max;
  gfloat *data;

  data = terrain->heightfield;
  last = terrain->width * terrain->height;

  /*
   * When never_grow is true, normalize assumes that if the minimum
   * and maximum value range of the terrain is smaller than [0.0, 1.0],
   * the user wants it that way.
   */

  if (never_grow)
    min = 0.0, max = 1.0;
  else
    min = max = data[0];

  for (i = 0; i < last; i++)
    {
      if (data[i] < min) min = data[i];
      if (data[i] > max) max = data[i];
    }

  if (fabs (max - min) > 0.0001)
    {
      gfloat k;

      k = 1.0 / (max - min);
      for (i = 0; i < last; i++)
        data[i] = (data[i] - min) * k;

      if (never_grow)
        terrain->sealevel =
          MIN (MAX ((terrain->sealevel - min) * k, 0.0), 1.0);
    }
}

void
t_terrain_crop (TTerrain *terrain,
                gint      x1,
                gint      y1,
                gint      x2,
                gint      y2)
{ 
  gint from_x, from_y;
  gint to_x, to_y;

  g_return_if_fail (x1 >= 0 && y1 >= 0 &&
                    x2 < terrain->width && y2 < terrain->height);
  g_return_if_fail (x1 <= x2 && y1 <= y2);

  for (from_y = y1, to_y = 0; from_y <= y2; from_y++, to_y++)
    {
      gint from_pos, to_pos;

      from_pos = from_y * terrain->width + x1;
      to_pos = to_y * (x2 - x1 + 1);
      for (from_x = x1, to_x = 0; from_x <= x2; from_x++, to_x++)
        {
          terrain->heightfield[to_pos] = terrain->heightfield[from_pos];

          if (terrain->riverfield != NULL)
            terrain->riverfield[to_pos] = terrain->riverfield[from_pos];

          if (terrain->selection != NULL)
            terrain->selection[to_pos] = terrain->selection[from_pos];

          from_pos++;
          to_pos++;
        }
    }

  terrain->width = x2 - x1 + 1;
  terrain->height = y2 - y1 + 1;
  terrain->heightfield = g_renew (gfloat, terrain->heightfield,
                                  terrain->width * terrain->height);

  if (terrain->riverfield != NULL)
    terrain->riverfield = g_renew (gfloat, terrain->riverfield,
                                  terrain->width * terrain->height);
  if (terrain->selection != NULL)
    terrain->selection = g_renew (gfloat, terrain->selection,
                                  terrain->width * terrain->height);
}

TTerrain *
t_terrain_crop_new (TTerrain *terrain,
                    gint      x1,
                    gint      y1,
                    gint      x2,
                    gint      y2)
{ 
  gint       x, y;
  gint       width = x2 - x1 + 1;
  gint       height = y2 - y1 + 1;
  GtkObject *tnew_object;
  TTerrain  *tnew;

  g_return_val_if_fail (width > 10, NULL);
  g_return_val_if_fail (height > 10, NULL);

  /* fix larger-than-allowed size params */
  if (width > terrain->width)
    width = terrain->width;
  if (height > terrain->height)
    height = terrain->height;

  tnew_object = t_terrain_new (width, height);
  tnew = T_TERRAIN (tnew_object);

  if (terrain->riverfield != NULL)
    tnew->riverfield = g_new0 (gfloat, width * height);

  if (terrain->selection != NULL)
    tnew->selection = g_new0 (gfloat, width * height);

  for (y = 0; y < height; y++)
    for (x = 0; x < width; x++)
      {
        gint offdst = y * width + x;
        gint offsrc = (y1 + y) * terrain->width + (x1 + x);

        tnew->heightfield[offdst] = terrain->heightfield[offsrc];

	if (terrain->riverfield != NULL)
          tnew->riverfield[offdst] = terrain->riverfield[offsrc];

	if (terrain->selection != NULL)
          tnew->selection[offdst] = terrain->selection[offsrc];
      }

  t_terrain_normalize (terrain, TRUE);
  t_terrain_set_modified (tnew, TRUE);

  return tnew;
}

void
t_terrain_invert (TTerrain *terrain)
{
  gint i;
  gint size;

  size = terrain->width * terrain->height;
  for (i = 0; i < size; i++)
    terrain->heightfield[i] = 1.0 - terrain->heightfield[i];
}

void
t_terrain_pack_scene_options (TTerrain  *terrain,
                              GtkWidget *options)
{
  TTerrainOptRender * const ropt = &(terrain->render_options);
  GList *textures;
  GList *clouds;
  GList *atms;
  GList *stars;
  GList *waters;
  GList *lights;
  GList *scene_themes;
  GList *skycolors;
  GList *background_images;
  GList *moon_images;

  textures = gtk_object_get_data (GTK_OBJECT (options), "data_textures");
  clouds = gtk_object_get_data (GTK_OBJECT (options), "data_clouds");
  atms = gtk_object_get_data (GTK_OBJECT (options), "data_atmospheres");
  stars = gtk_object_get_data (GTK_OBJECT (options), "data_stars");
  waters = gtk_object_get_data (GTK_OBJECT (options), "data_water");
  lights = gtk_object_get_data (GTK_OBJECT (options), "data_lights");
  scene_themes = gtk_object_get_data (GTK_OBJECT (options), "data_scene_themes");
  skycolors = gtk_object_get_data (GTK_OBJECT (options), "data_skycolors");
  background_images = gtk_object_get_data (GTK_OBJECT (options), "data_background_images");
  moon_images = gtk_object_get_data (GTK_OBJECT (options), "data_moon_images");

  terrain->do_filled_sea = get_boolean (options, "render_filled_sea");
  terrain->sealevel = get_float (options, "render_sealevel");
  terrain->y_scale_factor = get_float (options, "render_y_scale_factor");

  ropt->camera_x = get_float (options, "render_camera_x");
  ropt->camera_y = get_float (options, "render_camera_y");
  ropt->camera_z = get_float (options, "render_camera_z");
  ropt->lookat_x = get_float (options, "render_lookat_x");
  ropt->lookat_y = get_float (options, "render_lookat_y");
  ropt->lookat_z = get_float (options, "render_lookat_z");
  ropt->do_observe_sealevel = get_boolean (options, "render_observe_sealevel");
  ropt->elevation_offset = get_float (options, "render_elevation");
  ropt->do_clouds = get_boolean (options, "render_clouds");
  ropt->do_fog = get_boolean (options, "render_fog");
  ropt->do_ground_fog = get_boolean (options, "render_ground_fog");
  ropt->do_rainbow = get_boolean (options, "render_rainbow");
  ropt->do_camera_light = get_boolean (options, "render_camera_light");
  ropt->do_stars = get_boolean (options, "render_stars");
  ropt->do_celest_objects = get_boolean (options, "render_celest_objects");
  ropt->do_radiosity = get_boolean (options, "render_radiosity");
  ropt->do_texture_constructor = get_boolean (options, "render_texture_constructor");
  ropt->do_texture_stratum = get_boolean (options, "render_texture_stratum");
  ropt->do_texture_grasstex = get_boolean (options, "render_texture_grasstex");
  ropt->do_texture_waterborder = get_boolean (options, "render_texture_waterborder");
  ropt->do_texture_waterborder_sand = get_boolean (options, "render_texture_waterborder_sand");
  ropt->do_texture_waterborder_ice = get_boolean (options, "render_texture_waterborder_ice");
  ropt->do_water_apply_on_river= get_boolean (options, "render_water_apply_on_river");

  ropt->fog_offset = get_float (options, "render_fog_offset");
  ropt->fog_alt = get_float (options, "render_fog_alt");
  ropt->fog_density = get_float (options, "render_fog_density");
  ropt->fog_turbulence = get_float (options, "render_fog_turbulence");
  ropt->time_of_day = get_float (options, "render_time_of_day");
  ropt->west_direction = get_float (options, "render_west_direction");
  ropt->water_clarity = get_float (options, "render_clarity");
  ropt->ambient_light_luminosity = get_float (options, "render_ambient_light_luminosity");
  ropt->camera_light_luminosity = get_float (options, "render_camera_light_luminosity");
  ropt->moon_time_offset = get_float (options, "render_moon_time_offset");
  ropt->ground_fog_density = get_float (options, "render_ground_fog_density");
  ropt->skycolor_red = get_float (options, "render_skycolor_red");
  ropt->skycolor_green = get_float (options, "render_skycolor_green");
  ropt->skycolor_blue = get_float (options, "render_skycolor_blue");
  ropt->lights_luminosity = get_float (options, "render_lights_luminosity");
  ropt->water_reflection = get_float (options, "render_water_reflection");
  ropt->water_color_red = get_float (options, "render_water_color_red");
  ropt->water_color_green = get_float (options, "render_water_color_green");
  ropt->water_color_blue = get_float (options, "render_water_color_blue");
  ropt->water_frequency = get_float (options, "render_water_frequency");
  ropt->waves_height = get_float (options, "render_waves_height");
  ropt->texture_spread = get_float (options, "render_texture_spread");
  ropt->terrain_turbulence = get_float (options, "render_texture_turbulence");
  ropt->bgimage_scale = get_float (options, "render_bgimage_scale");
  ropt->bgimage_offset = get_float (options, "render_bgimage_offset");
  ropt->bgimage_enlightment = get_float (options, "render_bgimage_enlightment");
  ropt->sun_size = get_float (options, "render_sun_size");
  ropt->sun_color_red = get_float (options, "render_sun_color_red");
  ropt->sun_color_green = get_float (options, "render_sun_color_green");
  ropt->sun_color_blue = get_float (options, "render_sun_color_blue");
  ropt->moon_rot_Y = get_float (options, "render_moon_rot_Y");
  ropt->moon_rot_Z = get_float (options, "render_moon_rot_Z");
  ropt->moon_map_enlightment = get_float (options, "render_moon_map_enlightment");
  ropt->moon_size = get_float (options, "render_moon_size");
  ropt->moon_color_red = get_float (options, "render_moon_color_red");
  ropt->moon_color_green = get_float (options, "render_moon_color_green");
  ropt->moon_color_blue = get_float (options, "render_moon_color_blue");
  ropt->fog_blue = get_float (options, "render_fog_blue");
  ropt->texture_rock_amount = get_float (options, "render_texture_rock_amount");
  ropt->texture_grass_amount = get_float (options, "render_texture_grass_amount");
  ropt->texture_snow_amount = get_float (options, "render_texture_snow_amount");
  ropt->texture_rock_red = get_float (options, "render_texture_rock_red");
  ropt->texture_rock_green = get_float (options, "render_texture_rock_green");
  ropt->texture_rock_blue = get_float (options, "render_texture_rock_blue");
  ropt->texture_rock_grain = get_float (options, "render_texture_rock_grain");
  ropt->texture_grass_red = get_float (options, "render_texture_grass_red");
  ropt->texture_grass_green = get_float (options, "render_texture_grass_green");
  ropt->texture_grass_blue = get_float (options, "render_texture_grass_blue");
  ropt->texture_grass_grain= get_float (options, "render_texture_grass_grain");
  ropt->texture_snow_red = get_float (options, "render_texture_snow_red");
  ropt->texture_snow_green = get_float (options, "render_texture_snow_green");
  ropt->texture_snow_blue = get_float (options, "render_texture_snow_blue");
  ropt->texture_snow_grain = get_float (options, "render_texture_snow_grain");
  ropt->texture_stratum_size = get_float (options, "render_texture_stratum_size");
  ropt->texture_waterborder_size = get_float (options, "render_texture_waterborder_size");
  ropt->texture_waterborder_sand_red = get_float (options, "render_texture_waterborder_sand_red");
  ropt->texture_waterborder_sand_green = get_float (options, "render_texture_waterborder_sand_green");
  ropt->texture_waterborder_sand_blue = get_float (options, "render_texture_waterborder_sand_blue");
  ropt->texture_waterborder_sand_grain = get_float (options, "render_texture_waterborder_sand_grain");
  ropt->camera_zoom = get_float (options, "render_camera_zoom");
  ropt->radiosity_quality = get_float (options, "render_radiosity_quality");

  ropt->sky_system = get_option (options, "render_sky_system");
  ropt->light_system = get_option (options, "render_light_system");
  ropt->camera_type = get_option (options, "render_camera_type");
  ropt->texture_method = get_option (options, "render_texture_method");

  ropt->render_scale_x = get_int (options, "render_scale_x");
  ropt->render_scale_y = (
      (ropt->render_scale_x + ropt->render_scale_y)/2)*terrain->y_scale_factor;
  ropt->render_scale_z = get_int (options, "render_scale_z");

  g_free (ropt->texture_file);
  ropt->texture_file = g_strdup (
    g_list_nth_data (textures, get_option (options, "render_texture_type")));

  g_free (ropt->cloud_file);
  ropt->cloud_file = g_strdup (
    g_list_nth_data (clouds, get_option (options, "render_cloud_type")));

  g_free (ropt->atmosphere_file);
  ropt->atmosphere_file = g_strdup (
    g_list_nth_data (atms, get_option (options, "render_atmosphere_type")));

  g_free (ropt->star_file);
  ropt->star_file = g_strdup (
    g_list_nth_data (stars, get_option (options, "render_star_type")));

  g_free (ropt->water_file);
  ropt->water_file = g_strdup (
    g_list_nth_data (waters, get_option (options, "render_water_type")));

  g_free (ropt->light_file);
  ropt->light_file = g_strdup (
    g_list_nth_data (lights, get_option (options, "render_light_type")));

  g_free (ropt->scene_theme_file);
  ropt->scene_theme_file = g_strdup (
    g_list_nth_data (scene_themes, get_option (options, "render_scene_theme")));

 g_free (ropt->skycolor_file);
  ropt->skycolor_file = g_strdup (
    g_list_nth_data (skycolors, get_option (options, "render_skycolor_type")));

 g_free (ropt->background_image_file);
  ropt->background_image_file = g_strdup (
    g_list_nth_data (background_images, get_option (options, "render_background_image_file")));

 g_free (ropt->moon_image_file);
  ropt->moon_image_file = g_strdup (
    g_list_nth_data (moon_images, get_option (options, "render_moon_image_file")));

  t_terrain_set_modified (terrain, TRUE);

}

void
t_terrain_pack_terrain_options (TTerrain  *terrain,
                        GtkWidget *options)
{
  /* TTerrainOptRender * const ropt = &(terrain->render_options); */

  terrain->contour_levels = get_int (options, "render_levels");
  terrain->camera_height_factor = get_float (options, "render_camera_height_factor");
  terrain->wireframe_resolution = get_int (options, "render_wireframe_resolution");

  t_terrain_set_modified (terrain, TRUE);
}

void
t_terrain_unpack_scene_options (TTerrain  *terrain,
                                GtkWidget *options)
{
  const TTerrainOptRender *ropt = &(terrain->render_options);
  GList *textures;
  GList *clouds;
  GList *atms;
  GList *stars;
  GList *waters;
  GList *lights;
  GList *scene_themes;
  GList *skycolors;  
  GList *background_images;  
  GList *moon_images;  
  gint   value;

  /* options = lookup_toplevel (options); */
  textures = gtk_object_get_data (GTK_OBJECT (options), "data_textures");
  clouds = gtk_object_get_data (GTK_OBJECT (options), "data_clouds");
  atms = gtk_object_get_data (GTK_OBJECT (options), "data_atmospheres");
  stars = gtk_object_get_data (GTK_OBJECT (options), "data_stars");
  waters = gtk_object_get_data (GTK_OBJECT (options), "data_water");
  lights = gtk_object_get_data (GTK_OBJECT (options), "data_lights");
  scene_themes = gtk_object_get_data (GTK_OBJECT (options), "data_scene_themes");
  skycolors = gtk_object_get_data (GTK_OBJECT (options), "data_skycolors");
  background_images = gtk_object_get_data (GTK_OBJECT (options), "data_background_images");
  moon_images = gtk_object_get_data (GTK_OBJECT (options), "data_moon_images");

  set_boolean (options, "render_filled_sea", terrain->do_filled_sea);
  set_float (options, "render_sealevel", terrain->sealevel);
  set_float (options, "render_y_scale_factor", terrain->y_scale_factor);

  set_float (options, "render_camera_x", ropt->camera_x);
  set_float (options, "render_camera_y", ropt->camera_y);
  set_float (options, "render_camera_z", ropt->camera_z);
  set_float (options, "render_lookat_x", ropt->lookat_x);
  set_float (options, "render_lookat_y", ropt->lookat_y);
  set_float (options, "render_lookat_z", ropt->lookat_z);
  /* set_boolean (options, "render_tile_terrain", terrain->do_tile); */
  set_boolean (options, "render_observe_sealevel", ropt->do_observe_sealevel);
  set_float (options, "render_elevation", ropt->elevation_offset);
  set_boolean (options, "render_clouds", ropt->do_clouds);
  set_boolean (options, "render_fog", ropt->do_fog);
  set_boolean (options, "render_ground_fog", ropt->do_ground_fog);
  set_boolean (options, "render_rainbow", ropt->do_rainbow);
  set_boolean (options, "render_camera_light", ropt->do_camera_light);
  set_boolean (options, "render_stars", ropt->do_stars);
  set_boolean (options, "render_celest_objects", ropt->do_celest_objects);
  set_boolean (options, "render_radiosity", ropt->do_radiosity);
  set_boolean (options, "render_texture_constructor", ropt->do_texture_constructor);
  set_boolean (options, "render_texture_stratum", ropt->do_texture_stratum);
  set_boolean (options, "render_texture_grasstex", ropt->do_texture_grasstex);
  set_boolean (options, "render_texture_waterborder", ropt->do_texture_waterborder);
  set_boolean (options, "render_texture_waterborder_sand", ropt->do_texture_waterborder_sand);
  set_boolean (options, "render_texture_waterborder_ice", ropt->do_texture_waterborder_ice);
  set_boolean (options, "render_water_apply_on_river", ropt->do_water_apply_on_river);

  set_float (options, "render_fog_offset", ropt->fog_offset);
  set_float (options, "render_fog_alt", ropt->fog_alt);
  set_float (options, "render_fog_density", ropt->fog_density);
  set_float (options, "render_fog_turbulence", ropt->fog_turbulence);
  set_float (options, "render_time_of_day", ropt->time_of_day);
  set_float (options, "render_west_direction", ropt->west_direction);
  set_float (options, "render_clarity", ropt->water_clarity);
  set_float (options, "render_ambient_light_luminosity", ropt->ambient_light_luminosity);
  set_float (options, "render_camera_light_luminosity", ropt->camera_light_luminosity);
  set_float (options, "render_moon_time_offset", ropt->moon_time_offset);
  set_float (options, "render_ground_fog_density", ropt->ground_fog_density);
  set_float (options, "render_skycolor_red", ropt->skycolor_red);
  set_float (options, "render_skycolor_green", ropt->skycolor_green);
  set_float (options, "render_skycolor_blue", ropt->skycolor_blue);
  set_float (options, "render_lights_luminosity", ropt->lights_luminosity);
  set_float (options, "render_water_reflection", ropt->water_reflection);
  set_float (options, "render_water_color_red", ropt->water_color_red);
  set_float (options, "render_water_color_green", ropt->water_color_green);
  set_float (options, "render_water_color_blue", ropt->water_color_blue);
  set_float (options, "render_water_frequency", ropt->water_frequency);
  set_float (options, "render_waves_height", ropt->waves_height);
  set_float (options, "render_texture_spread", ropt->texture_spread);
  set_float (options, "render_texture_turbulence", ropt->terrain_turbulence);
  set_float (options, "render_bgimage_scale", ropt->bgimage_scale);
  set_float (options, "render_bgimage_offset", ropt->bgimage_offset);
  set_float (options, "render_bgimage_enlightment", ropt->bgimage_enlightment);
  set_float (options, "render_sun_size", ropt->sun_size);
  set_float (options, "render_sun_color_red", ropt->sun_color_red);
  set_float (options, "render_sun_color_green", ropt->sun_color_green);
  set_float (options, "render_sun_color_blue", ropt->sun_color_blue);
  set_float (options, "render_moon_rot_Y", ropt->moon_rot_Y);
  set_float (options, "render_moon_rot_Z", ropt->moon_rot_Z);
  set_float (options, "render_moon_map_enlightment", ropt->moon_map_enlightment);
  set_float (options, "render_moon_size", ropt->moon_size);
  set_float (options, "render_moon_color_red", ropt->moon_color_red);
  set_float (options, "render_moon_color_green", ropt->moon_color_green);
  set_float (options, "render_moon_color_blue", ropt->moon_color_blue);
  set_float (options, "render_fog_blue", ropt->fog_blue);
  set_float (options, "render_texture_rock_amount", ropt->texture_rock_amount);
  set_float (options, "render_texture_grass_amount", ropt->texture_grass_amount);
  set_float (options, "render_texture_snow_amount", ropt->texture_snow_amount);
  set_float (options, "render_texture_rock_red", ropt->texture_rock_red);
  set_float (options, "render_texture_rock_green", ropt->texture_rock_green);
  set_float (options, "render_texture_rock_blue", ropt->texture_rock_blue);
  set_float (options, "render_texture_rock_grain", ropt->texture_rock_grain);
  set_float (options, "render_texture_grass_red", ropt->texture_grass_red);
  set_float (options, "render_texture_grass_green", ropt->texture_grass_green);
  set_float (options, "render_texture_grass_blue", ropt->texture_grass_blue);
  set_float (options, "render_texture_grass_grain", ropt->texture_grass_grain);
  set_float (options, "render_texture_snow_red", ropt->texture_snow_red);
  set_float (options, "render_texture_snow_green", ropt->texture_snow_green);
  set_float (options, "render_texture_snow_blue", ropt->texture_snow_blue);
  set_float (options, "render_texture_snow_grain", ropt->texture_snow_grain);
  set_float (options, "render_texture_stratum_size", ropt->texture_stratum_size);
  set_float (options, "render_texture_waterborder_size", ropt->texture_waterborder_size);
  set_float (options, "render_texture_waterborder_sand_red", ropt->texture_waterborder_sand_red);
  set_float (options, "render_texture_waterborder_sand_green", ropt->texture_waterborder_sand_green);
  set_float (options, "render_texture_waterborder_sand_blue", ropt->texture_waterborder_sand_blue);
  set_float (options, "render_texture_waterborder_sand_grain", ropt->texture_waterborder_sand_grain);
  set_float (options, "render_camera_zoom", ropt->camera_zoom);
  set_float (options, "render_radiosity_quality", ropt->radiosity_quality);

  set_option (options, "render_sky_system", ropt->sky_system);
  set_option (options, "render_light_system", ropt->light_system);
  set_option (options, "render_camera_type", ropt->camera_type);
  set_option (options, "render_texture_method", ropt->texture_method);

  set_int (options, "render_scale_x", ropt->render_scale_x);
  set_int (options, "render_scale_z", ropt->render_scale_z);
  set_int (options, "spinbutton_render_scale_x", ropt->render_scale_x);
  set_int (options, "spinbutton_render_scale_z", ropt->render_scale_z);
  set_float (options, "spinbutton_render_y_scale_factor", terrain->y_scale_factor);

  value = g_list_string_index (textures, ropt->texture_file);
  set_option (options, "render_texture_type", value >= 0 ? value : 0);

  value = g_list_string_index (clouds, ropt->cloud_file);
  set_option (options, "render_cloud_type", value >= 0 ? value : 0);

  value = g_list_string_index (atms, ropt->atmosphere_file);
  set_option (options, "render_atmosphere_type", value >= 0 ? value : 0);

  value = g_list_string_index (stars, ropt->star_file);
  set_option (options, "render_star_type", value >= 0 ? value : 0);

  value = g_list_string_index (waters, ropt->water_file);
  set_option (options, "render_water_type", value >= 0 ? value : 0);

  value = g_list_string_index (lights, ropt->light_file);
  set_option (options, "render_light_type", value >= 0 ? value : 0);

  value = g_list_string_index (scene_themes, ropt->scene_theme_file);
  set_option (options, "render_scene_theme", value >= 0 ? value : 0);

  value = g_list_string_index (skycolors, ropt->skycolor_file);
  set_option (options, "render_skycolor_type", value >= 0 ? value : 0);

  value = g_list_string_index (background_images, ropt->background_image_file);
  set_option (options, "render_background_image_file", value >= 0 ? value : 0);

  value = g_list_string_index (moon_images, ropt->moon_image_file);
  set_option (options, "render_moon_image_file", value >= 0 ? value : 0);

  /* doesn't have a GUI counterpart -> derived */ 
  /* set_int (options, "render_scale_y", ((ropt->render_scale_x + ropt->render_scale_y))/2*terrain->y_scale_factor); */
} 

void
t_terrain_unpack_terrain_options (TTerrain  *terrain,
                          GtkWidget *options)
{
  /* const TTerrainOptRender *ropt = &(terrain->render_options); */

  set_float (options, "render_camera_height_factor", terrain->camera_height_factor);
  set_int (options, "render_levels", terrain->contour_levels);
  set_int (options, "render_wireframe_resolution", terrain->wireframe_resolution);
} 

void 
t_terrain_pack_povray_options (TTerrain *terrain,
			       GtkWidget *options)
{
  GtkWidget *fileentry;
  GList     *render_sizes;
  TTerrainOptRender * const ropt = &(terrain->render_options);

  t_terrain_set_modified (terrain, TRUE);

  /* first tab: filename and filetype */
  ropt->do_output_file = get_boolean (options, "checkbutton_file");
  fileentry = lookup_widget (GTK_WIDGET (options), "fileentry_povray_filename");
  ropt->povray_filename = gnome_file_entry_get_full_path (GNOME_FILE_ENTRY (fileentry), FALSE);

  ropt->filetype = get_option (options, "optionmenu_filetype");
  ropt->use_alpha = get_boolean (options, "checkbutton_use_alpha");
  ropt->bits_per_colour = get_int (options, "spinbutton_bits_per_colour");

  /* get image size */
  render_sizes = gtk_object_get_data (GTK_OBJECT (options), "data_render_size");

  g_free (ropt->image_size_string);
  ropt->image_size_string = g_strdup (
    g_list_nth_data (render_sizes, get_option (options, "optionmenu_render_size")));

  ropt->do_custom_size = get_boolean (options, "custom_size");
  if(ropt->do_custom_size)
    {
      ropt->image_width = get_int(options, "image_width");
      ropt->image_height = get_int(options, "image_height");
    }
  else
    {
      ropt->image_width = atoi(g_string_get_image_size(ropt->image_size_string, TRUE));
      ropt->image_height = atoi(g_string_get_image_size(ropt->image_size_string, FALSE));
    }

  /* partial render */
  ropt->do_partial_render = get_boolean (options, "checkbutton_partial_render");
  //ropt->partial_render_as_percentage = get_boolean (options, "checkbutton_percentage");
  ropt->start_column = get_float (options, "spinbutton_start_col");
  ropt->end_column = get_float (options, "spinbutton_end_col");
  ropt->start_row = get_float (options, "spinbutton_start_row");
  ropt->end_row = get_float (options, "spinbutton_end_row");

  /* get quality settings */
  ropt->render_quality = get_option (options, "optionmenu_quality");

  /* get antialiasing options */
  ropt->do_aa = get_boolean (options, "use_antialiasing");
  ropt->aa_threshold = get_float (options, "aa_threshold");
  ropt->aa_type = get_option (options, "optionmenu_aa_type");
  ropt->aa_depth = get_float (options, "aa_depth");    
  ropt->do_jitter = get_boolean (options, "use_jitter");
  ropt->jitter_amount = get_float (options, "jitter_amount");
  
}

void 
t_terrain_unpack_povray_options (TTerrain  *terrain,
				 GtkWidget *options)
{
  GtkWidget *fileentry;
  GList     *render_sizes;
  gint       value;
  const gchar *filename;
  const TTerrainOptRender *ropt = &(terrain->render_options);

  /* first tab: set filename and filetype settings*/
  set_boolean (options, "checkbutton_file", ropt->do_output_file);
  fileentry = lookup_widget (options, "fileentry_povray_filename");

  if ( /* ropt->povray_filename == NULL || */ strlen(ropt->povray_filename) < 1 )
    filename = filename_get_without_extension_and_dot (terrain->filename);
  else
    filename = g_strdup (ropt->povray_filename);

  gtk_entry_set_text (GTK_ENTRY (gnome_file_entry_gtk_entry (GNOME_FILE_ENTRY(fileentry))), filename);

  set_option (options, "optionmenu_filetype", ropt->filetype);
  if (ropt->filetype == 0)
    {
      gtk_widget_set_sensitive (lookup_widget (options, "checkbutton_use_alpha"), TRUE);
      gtk_widget_set_sensitive (lookup_widget (options, "label_bits_per_colour"), TRUE);
      gtk_widget_set_sensitive (lookup_widget (options, "spinbutton_bits_per_colour"), TRUE);
    }
  else if (ropt->filetype == 2 || ropt->filetype == 3)
    gtk_widget_set_sensitive (lookup_widget (options, "checkbutton_use_alpha"), TRUE);
  
  set_boolean (options, "checkbutton_use_alpha", ropt->use_alpha);
  set_int (options, "spinbutton_bits_per_colour", ropt->bits_per_colour);

  /* set image size */
  set_boolean (options, "custom_size", ropt->do_custom_size);
  render_sizes = gtk_object_get_data (GTK_OBJECT (options), "data_render_size");
  value = g_list_string_index (render_sizes, ropt->image_size_string);
  set_option (options, "optionmenu_render_size", value >= 0 ? value : 0);
  set_int (options, "image_width", ropt->image_width);
  set_int (options, "image_height", ropt->image_height);

  /* partial render */
  set_boolean (options, "checkbutton_partial_render",ropt->do_partial_render);
  //set_boolean (options, "checkbutton_percentage", ropt->partial_render_as_percentage);
  set_float (options, "spinbutton_start_col", ropt->start_column);
  set_float (options, "spinbutton_end_col", ropt->end_column);
  set_float (options, "spinbutton_start_row", ropt->start_row);
  set_float (options, "spinbutton_end_row", ropt->end_row);

  gtk_widget_set_sensitive (lookup_widget (options, "spinbutton_start_col"), ropt->do_partial_render);
  gtk_widget_set_sensitive (lookup_widget (options, "label_start_col"), ropt->do_partial_render);
  gtk_widget_set_sensitive (lookup_widget (options, "spinbutton_end_col"), ropt->do_partial_render);
  gtk_widget_set_sensitive (lookup_widget (options, "label_end_col"), ropt->do_partial_render);
  gtk_widget_set_sensitive (lookup_widget (options, "spinbutton_start_row"), ropt->do_partial_render);
  gtk_widget_set_sensitive (lookup_widget (options, "label_start_row"), ropt->do_partial_render);
  gtk_widget_set_sensitive (lookup_widget (options, "spinbutton_end_row"), ropt->do_partial_render);
  gtk_widget_set_sensitive (lookup_widget (options, "label_end_row"), ropt->do_partial_render);

  /* set quality settings */
  set_option (options, "optionmenu_quality", ropt->render_quality);

  /* set antialiasing options */
  set_boolean (options, "use_antialiasing", ropt->do_aa);
  set_float (options, "aa_threshold", ropt->aa_threshold);
  set_option (options, "optionmenu_aa_type", ropt->aa_type); 
  set_float (options, "aa_depth", ropt->aa_depth);
  set_boolean (options, "use_jitter", ropt->do_jitter);
  set_float (options, "jitter_amount", ropt->jitter_amount);
}

void
t_terrain_print_contour_map (TTerrain  *terrain,
                             GnomePrintContext *context)
{
  GList   *list;
  gdouble  scale;
  gdouble  translate_x, translate_y;
  gdouble  page_width, page_height;
  gdouble  usable_width, usable_height;
  GList   *contour_lines;

  page_width = 8.5 * 72.0;
  page_height = 11.0 * 72.0;

  usable_width = 7.0 * 72.0;
  usable_height = 9.0 * 72.0;

  scale = MIN (usable_width / terrain->width, usable_height / terrain->height);

  translate_x = (page_width - scale * terrain->width) * 0.5;
  translate_y = page_height - (page_height - scale * terrain->height) * 0.5;

  gnome_print_translate (context, translate_x, translate_y);
  gnome_print_scale (context, scale, -scale);

  gnome_print_newpath (context);
  gnome_print_moveto (context, 0.0, 0.0);
  gnome_print_lineto (context, terrain->width, 0.0);
  gnome_print_lineto (context, terrain->width, terrain->height);
  gnome_print_lineto (context, 0.0, terrain->height);
  gnome_print_closepath (context);
  gnome_print_stroke (context);

  contour_lines = t_terrain_contour_lines (terrain, terrain->contour_levels, 5);
  list = g_list_first (contour_lines);
  while (list != NULL)
    {
      GArray  *array;
      gdouble  x, y;
      gint     i;

      array = list->data;
      x = g_array_index (array, gfloat, 1);
      y = g_array_index (array, gfloat, 2);

      gnome_print_newpath (context);
      gnome_print_moveto (context, x, y);
      for (i = 1; i < array->len; i += 2)
        {
          x = g_array_index (array, gfloat, i + 0);
          y = g_array_index (array, gfloat, i + 1);
          gnome_print_lineto (context, x, y);
        }
      gnome_print_stroke (context);

      list = list->next;
    }
  t_terrain_contour_list_free (contour_lines);
}

void
t_terrain_ref (TTerrain *terrain)
{
  gtk_object_ref (GTK_OBJECT (terrain));
  gtk_object_sink (GTK_OBJECT (terrain));
}

void
t_terrain_unref (TTerrain *terrain)
{
  gtk_object_unref (GTK_OBJECT (terrain));
}



TTerrain *
t_terrain_import_heightfield (gchar *filename)
{
  return NULL;
}

void
t_terrain_export_heightfield (TTerrain *terrain,
                              gchar    *filename)
{
}

static void
raster_set (TTerrain   *terrain,
            gint        x1,
            gint        y,
            gint        x2,
            TComposeOp  op)
{
  gint pos;

  if (y < 0 || y >= terrain->height ||
      x2 < 0 || x1 >= terrain->width ||
      x2 < x1)
    return;

  x1 = MAX (x1, 0);
  x2 = MIN (x2, terrain->width - 1);

  pos = y * terrain->width + x1;

  switch (op)
    {
    case T_COMPOSE_REPLACE:
    case T_COMPOSE_ADD:
      for (; x1 <= x2; x1++)
        terrain->selection[pos++] = 1.0;
      break;

    case T_COMPOSE_SUBTRACT:
      for (; x1 <= x2; x1++)
        terrain->selection[pos++] = 0.0;
      break;

    default:
      break;
    }
}

void
t_terrain_select_by_height (TTerrain  *terrain,
                            gfloat     floor,
                            gfloat     ceil,
                            TComposeOp op)
{
  gint i = 0;
  gint x, y; 
  gint size = terrain->width * terrain->height;

  if (op == T_COMPOSE_NONE)
    return;

  if (terrain->selection == NULL)
    terrain->selection = g_new (gfloat, size);

  if (op == T_COMPOSE_REPLACE)
    memset (terrain->selection, 0, sizeof (gfloat)*size);

  for (y=0; y<terrain->height; y++)
    for (x=0; x<terrain->width; x++, i++)
      {
      if (terrain->heightfield[i] >= floor && terrain->heightfield[i] <= ceil)
        raster_set (terrain, x, y, x, op);
      }
}

void
t_terrain_select (TTerrain       *terrain,
                  gfloat          x1,
                  gfloat          y1,
                  gfloat          x2,
                  gfloat          y2,
                  TSelectionType  type,
                  TComposeOp      op)
{
  gint y, _x1, _y1, _x2, _y2;

  if (op == T_COMPOSE_NONE)
    return;

  _x1 = x1 * (terrain->width - 1);
  _y1 = y1 * (terrain->height - 1);
  _x2 = x2 * (terrain->width - 1);
  _y2 = y2 * (terrain->height - 1);

  if (type == T_SELECTION_NONE)
    {
      if (op == T_COMPOSE_REPLACE)
        {
          t_terrain_select_destroy (terrain);
        }

      return;
    }

  if (terrain->selection == NULL)
    terrain->selection = g_new0 (gfloat, terrain->width * terrain->height);
  else if (op == T_COMPOSE_REPLACE)
    memset (terrain->selection, 0,
            sizeof (gfloat) * terrain->width * terrain->height);

  switch (type)
    {
    case T_SELECTION_RECTANGLE:
      for (y = _y1; y <= _y2; y++)
        raster_set (terrain, _x1, y, _x2, op);
      break;

    case T_SELECTION_ELLIPSE:
      for (y = _y1; y <= _y2; y++)
        {
          gfloat value;
          gfloat x_radius;
          gint   x_start, x_stop;
          gfloat y_value;

          if (y < ((_y1 + _y2) >> 1))
            value = y - _y1;
          else
            value = _y2 - y;

          y_value = 1.0 - 2.0 * value / (_y2 - _y1 + 1);
          x_radius =
            sqrt (1.0 - y_value * y_value) * (_x2 - _x1 + 1) * 0.5;
          x_start = (_x1 + _x2) * 0.5 - x_radius;
          x_stop = (_x1 + _x2) * 0.5 + x_radius;
          raster_set (terrain, x_start, y, x_stop, op);
        }
      break;

      default:
        break;
    }

  t_terrain_selection_modified (terrain);
}

void
t_terrain_select_destroy (TTerrain   *terrain)
{
  if (terrain->selection != NULL)
    {
      g_free (terrain->selection);
      t_terrain_selection_modified (terrain);
    }

  terrain->selection = NULL;
}

gint
t_terrain_add_object (TTerrain   *terrain,
                      gint        ox,
                      gint        oy,
                      gfloat      x,
                      gfloat      y,
                      gfloat      angle,
                      gfloat      scale_x,
                      gfloat      scale_y,
                      gfloat      scale_z,
                      gchar      *object_name)
{
  TTerrainObject object;

  object.name = g_strdup (object_name);
  object.ox = ox;
  object.oy = oy;
  object.x = x;
  object.y = y;
  object.angle = angle;
  object.scale_x = scale_x;
  object.scale_y = scale_y;
  object.scale_z = scale_z;

  g_array_append_val (terrain->objects, object);

  gtk_signal_emit (GTK_OBJECT (terrain), object_added_signal,
                   terrain->objects->len - 1);

  return terrain->objects->len - 1;
}

void
t_terrain_move_object (TTerrain   *terrain,
                       gint        item,
                       gfloat      x,
                       gfloat      y)
{
  TTerrainObject *object;

  object = &g_array_index (terrain->objects, TTerrainObject, item);
  object->x = CLIP (x, 0.0, 1.0);
  object->y = CLIP (y, 0.0, 1.0);

  gtk_signal_emit (GTK_OBJECT (terrain), object_modified_signal, item);
}

void
t_terrain_rotate_object (TTerrain   *terrain,
                         gint        item,
                         gfloat      angle)
{
  g_array_index (terrain->objects, TTerrainObject, item).angle = angle;

  gtk_signal_emit (GTK_OBJECT (terrain), object_modified_signal, item);
}

void
t_terrain_remove_object (TTerrain   *terrain,
                         gint        item)
{
  t_terrain_object_clear (&g_array_index (terrain->objects,
                          TTerrainObject, item));

  g_array_remove_index_fast (terrain->objects, item);
}

void
t_terrain_remove_objects (TTerrain   *terrain)
{
  gint i, size;
  GArray *objects = terrain->objects;

  size = objects->len;
  for (i = 0; i < size; i++)
    t_terrain_object_clear (&g_array_index (objects, TTerrainObject, i));

  for (i = size - 1; i >= 0; i--)
    gtk_signal_emit (GTK_OBJECT (terrain), object_deleted_signal, i);

  g_array_free (objects, TRUE);
  terrain->objects = g_array_new (FALSE, FALSE, sizeof (TTerrainObject));
}

/**
 * t_terrain_seed: seed the specified HF with the specified data. 
 * Returns a partially filled HF!
 */
gint
t_terrain_seed_data (TTerrain *terrain, 
		     gfloat   *data, 
		     gint      width, 
		     gint      height)
{
  gint      x, y;
  gfloat    incx, incy; 
  gfloat    offx=0, offy=0;
  gfloat   *heightfield = terrain->heightfield;

  g_return_val_if_fail (data != NULL, -1);
  g_return_val_if_fail (width > 10, -1);
  g_return_val_if_fail (height > 10, -1);
  g_return_val_if_fail (terrain->width > width, -1);
  g_return_val_if_fail (terrain->height > height, -1);

  incx = terrain->width / width;
  incy = terrain->height / height;

  memset (heightfield, 0, (terrain->width * terrain->height) * sizeof (gfloat));

  for (y = 0; y < height; y++, offy += incy, offx = 0)
    for (x = 0, offx = 0; x < width; x++, offx += incx)
      {
        /* printf ("%f, %f\n", offx, offy);fflush (stdout); */
        heightfield[(gint)(((gint) offy) * terrain->width + offx)] =
          data[y * width + x];
      }

  t_terrain_set_modified (terrain, TRUE);

  return 0;
}

/**
 * t_terrain_seed: re-seed the specified terrain with it's data from 
 * the specified region. Optionally resizes the terrain to 
 * new_width * new_height
 */
gint
t_terrain_seed (TTerrain *terrain, 
		gint      new_width, 
		gint      new_height, 
		gint      sel_x, 
		gint      sel_y, 
		gint      sel_width, 
		gint      sel_height)
{
  gint x, y;
  gint rc;
  gint sel_size = sel_width*sel_height;
  gfloat *heightfield = terrain->heightfield;
  gfloat *data_extract = g_new (gfloat, sel_size); 

  if (sel_x+sel_width > terrain->width)
    sel_width = terrain->width - sel_x;
  if (sel_y+sel_height > terrain->height)
    sel_height = terrain->height - sel_y;

  for (y=0; y<sel_height; y++)
    for (x=0; x<sel_width; x++)
      {
        gint offdst = y * sel_width + x;
        gint offsrc = (sel_y + y) * terrain->width + (sel_x + x);

        data_extract[offdst] = heightfield[offsrc];
      }

  if (new_width != -1 && new_height != -1)
    if (new_width != terrain->width && new_height != terrain->height)
      {
        terrain->heightfield = g_renew (gfloat, terrain->heightfield, new_width * new_height);
        terrain->width = new_width;
        terrain->height = new_height;
      }

  rc = t_terrain_seed_data (terrain, data_extract, sel_width, sel_height);

  if (rc == 0)
    t_terrain_select_none (terrain);

  g_free (data_extract);

  return rc;
}

TTerrain *
t_terrain_tile_new (TTerrain *terrain, 
                    gint      num_x, 
                    gint      num_y)
{
  GtkObject *object;
  TTerrain  *tnew;
  gint       lim_x = terrain->width * num_x;
  gint       lim_y = terrain->height * num_y;
  gint       x;
  gint       y;
  gchar      *ext;
  gchar      *base;
  gchar      buf[256];

  object = t_terrain_new (lim_x, lim_y);
  tnew = T_TERRAIN (object);

  if (terrain->riverfield != NULL)
    tnew->riverfield = g_new (gfloat, lim_x * lim_y);

  if (terrain->selection != NULL)
    tnew->selection = g_new (gfloat, lim_x * lim_y);

  for (y=0; y<lim_y; y++)
    {
      gint yofforg = y % terrain->height;

      for (x=0; x<lim_x; x++)
        {
          gint pnew = y * lim_x + x;
          gint porg = yofforg*terrain->width + x % terrain->width;

          tnew->heightfield[pnew] = terrain->heightfield[porg];

          if (terrain->riverfield != NULL)
            tnew->riverfield[pnew] = terrain->riverfield[porg];

          if (terrain->selection != NULL)
            tnew->selection[pnew] = terrain->selection[porg];
        }
    }

  base = filename_get_base_without_extension (terrain->filename);
  ext = filename_get_extension (terrain->filename);
  snprintf (buf, 256, "%s_tiled_%dx%d.%s", base, num_x, num_y, ext);
  g_free (base);
  g_free (ext);
  tnew->filename = g_strdup (buf);

  return tnew;
}

/*
 * This part added for Trace Objects by Raymond Ostertag
 */

void 
t_terrain_pack_povray_trace (TTerrain *terrain,
			       GtkWidget *trace_data)
{
  GList *trace_objects; 
  TTerrainOptRender * const ropt = &(terrain->render_options);

  t_terrain_set_modified (terrain, TRUE);

  trace_objects = gtk_object_get_data (GTK_OBJECT (trace_data), "data_trace_objects");
  g_free (ropt->trace_object_file);
  ropt->trace_object_file = g_strdup (
    g_list_nth_data (trace_objects, get_option (trace_data, "trace_object_name")));
 
  ropt->trace_object_x = get_float (trace_data, "trace_object_x");
  ropt->trace_object_y = get_float (trace_data, "trace_object_y");
  ropt->trace_object_z = get_float (trace_data, "trace_object_z");
  ropt->do_trace_object_relativ = get_boolean (trace_data, "trace_object_relativ");
  ropt->do_trace_object_conserve = get_boolean (trace_data, "trace_object_conserve");
  ropt->do_trace_object_float = get_boolean (trace_data, "trace_object_float");
  ropt->trace_object_scale = get_float (trace_data, "trace_object_scale");
}


void
t_terrain_remove_trace_objects (TTerrain *terrain)
{
  TTerrainOptRender * const ropt = &(terrain->render_options);

  g_free (ropt->trace_object_file);
  ropt->trace_object_file = (gchar *) NULL;
}

/*
 * End of Trace Objects
 */
