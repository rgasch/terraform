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


#include <locale.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include "tterrainview.h"
#include "writer.h"
#include "reader.h"        /* defines BT file header */
#include "filenameops.h"
#include "filters.h"
#include "base64.h"
#include "xmlsupport.h"
#include "clocale.h"
#include "support2.h"


static gint     t_terrain_save_native      (TTerrain    *terrain);
static gint     t_terrain_export_tga_field (TTerrain    *terrain,
                                            const gchar *filename, 
                                            gfloat      *field);
static gint     t_terrain_export_tga       (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_pov       (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_ini       (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_bmp       (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_bmp_bw    (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_pgm       (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_pg8       (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_ppm       (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_mat       (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_oct       (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_ac        (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_terragen  (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_grd       (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_xyz       (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_dxf       (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_bna       (TTerrain    *terrain,
                                            const gchar *filename);
static gint     t_terrain_export_bt        (TTerrain    *terrain,
                                            const gchar *filename);


gint
t_terrain_save (TTerrain    *terrain,
                TFileType    type,
                const gchar *filename)
{
  if (type == FILE_AUTO)
    type = filename_determine_type (filename);

  if (type == FILE_NATIVE)
    {
      if (filename != NULL)
        t_terrain_set_filename (terrain, filename);

      return t_terrain_save_native (terrain);
    }
  else if (type == FILE_TGA)
    return t_terrain_export_tga (terrain, filename);
  else if (type == FILE_POV)
    return t_terrain_export_pov (terrain, filename);
  else if (type == FILE_INI)
    return t_terrain_export_ini (terrain, filename);
  else if (type == FILE_BMP)
    return t_terrain_export_bmp (terrain, filename);
  else if (type == FILE_BMP_BW)
    return t_terrain_export_bmp_bw (terrain, filename);
  else if (type == FILE_PGM)
    return t_terrain_export_pgm (terrain, filename);
  else if (type == FILE_PG8)
    return t_terrain_export_pg8 (terrain, filename);
  else if (type == FILE_PPM)
    return t_terrain_export_ppm (terrain, filename);
  else if (type == FILE_MAT)
    return t_terrain_export_mat (terrain, filename);
  else if (type == FILE_OCT)
    return t_terrain_export_oct (terrain, filename);
  else if (type == FILE_AC)
    return t_terrain_export_ac (terrain, filename);
  else if (type == FILE_TERRAGEN)
    return t_terrain_export_terragen (terrain, filename);
  else if (type == FILE_GRD)
    return t_terrain_export_grd (terrain, filename);
  else if (type == FILE_XYZ)
    return t_terrain_export_xyz (terrain, filename);
  else if (type == FILE_DXF)
    return t_terrain_export_dxf (terrain, filename);
  else if (type == FILE_BNA)
    return t_terrain_export_bna (terrain, filename);
  else if (type == FILE_BT)
    return t_terrain_export_bt (terrain, filename);

  return -1;
}

gboolean
is_saving_as_native (TFileType    type,
                     const gchar *filename)
{
  if (type == FILE_AUTO)
    type = filename_determine_type (filename);

  return type == FILE_NATIVE;
}

void
t_terrain_get_povray_size (TTerrain *terrain,
                           gfloat   *x,
                           gfloat   *y,
                           gfloat   *z)
{
  gfloat       max_size;

  max_size = MAX (terrain->width, terrain->height);

  *x = terrain->render_options.render_scale_x * terrain->width / max_size;
  *z = terrain->render_options.render_scale_z * terrain->height / max_size;
  *y = (*x + *z)/2 * terrain->y_scale_factor;
}

static void 
heightfield_save_data (xmlNodePtr node,
                       gfloat    *heightfield, 
                       gint       width, 
                       gint       height)
{
  gchar   *encoded;
  guint32 *decoded;
  gint     x, y;

  g_return_if_fail (sizeof (gfloat) == sizeof (guint32));
  g_return_if_fail (sizeof (gfloat) == 4);
  g_return_if_fail (heightfield != NULL);

  xml_pack_prop_int (node, "width", width);
  xml_pack_prop_int (node, "height", height);

  decoded = g_new (guint32, width);
  encoded = g_new (guchar, base64_encoded_length (width * 4));

  for (y = 0; y < height; y++)
    {
      guint32 *data;

      data = (guint32*) &heightfield[y * width];
      for (x = 0; x < width; x++)
        decoded[x] = GUINT32_TO_LE (data[x]);

      base64_encode ((gchar*) decoded, width * 4, encoded);
      xmlNewChild (node, NULL, "row", encoded);
    }

  g_free (encoded);
  g_free (decoded);
}

static void
heightfield_save (TTerrain   *terrain,
                  xmlNodePtr  node)
{
  g_return_if_fail (sizeof (gfloat) == sizeof (guint32));
  g_return_if_fail (sizeof (gfloat) == 4);

  heightfield_save_data (node, terrain->heightfield, 
                         terrain->width, terrain->height);

  t_terrain_set_modified (terrain, FALSE);
}

static void
riverfield_save (TTerrain   *terrain,
                 xmlNodePtr  node)
{
  g_return_if_fail (sizeof (gfloat) == sizeof (guint32));
  g_return_if_fail (sizeof (gfloat) == 4);

  heightfield_save_data (node, terrain->riverfield, 
                         terrain->width, terrain->height);

  t_terrain_set_modified (terrain, FALSE);
}

static void
selection_save (TTerrain   *terrain,
                xmlNodePtr  node)
{
  g_return_if_fail (sizeof (gfloat) == sizeof (guint32));
  g_return_if_fail (sizeof (gfloat) == 4);

  heightfield_save_data (node, terrain->selection, 
                         terrain->width, terrain->height);

  t_terrain_set_modified (terrain, FALSE);
}

static void
options_save (TTerrain   *terrain,
              xmlNodePtr  node)
{
  TTerrainOptRender * const ropt = &(terrain->render_options);

  xml_pack_int (node, "contour_levels", terrain->contour_levels);
  xml_pack_boolean (node, "filled_sea", terrain->do_filled_sea);
  xml_pack_float (node, "sealevel", terrain->sealevel);
  xml_pack_int (node, "wireframe_resolution", terrain->wireframe_resolution);
  xml_pack_float (node, "y_scale_factor", terrain->y_scale_factor);

  xml_pack_boolean (node, "clouds", ropt->do_clouds);
  xml_pack_boolean (node, "fog", ropt->do_fog);
  xml_pack_boolean (node, "ground_fog", ropt->do_ground_fog);
  xml_pack_boolean (node, "observe_sealevel", ropt->do_observe_sealevel);
  xml_pack_boolean (node, "rainbow", ropt->do_rainbow);
  xml_pack_boolean (node, "camera_light", ropt->do_camera_light);
  xml_pack_boolean (node, "stars", ropt->do_stars);
  xml_pack_boolean (node, "celest_objects", ropt->do_celest_objects);
  xml_pack_boolean (node, "radiosity", ropt->do_radiosity);
  xml_pack_boolean (node, "texture_constructor", ropt->do_texture_constructor);
  xml_pack_boolean (node, "texture_stratum", ropt->do_texture_stratum);
  xml_pack_boolean (node, "texture_grasstex", ropt->do_texture_grasstex);
  xml_pack_boolean (node, "texture_waterborder", ropt->do_texture_waterborder);
  xml_pack_boolean (node, "texture_waterborder_sand", ropt->do_texture_waterborder_sand);
  xml_pack_boolean (node, "texture_waterborder_ice", ropt->do_texture_waterborder_ice);
  xml_pack_boolean (node, "water_apply_on_river", ropt->do_water_apply_on_river);

  xml_pack_float (node, "camera_x", ropt->camera_x);
  xml_pack_float (node, "camera_y", ropt->camera_y);
  xml_pack_float (node, "camera_z", ropt->camera_z);
  xml_pack_float (node, "lookat_x", ropt->lookat_x);
  xml_pack_float (node, "lookat_y", ropt->lookat_y);
  xml_pack_float (node, "lookat_z", ropt->lookat_z);
  xml_pack_float (node, "elevation_offset", ropt->elevation_offset);
  xml_pack_float (node, "fog_alt", ropt->fog_alt);
  xml_pack_float (node, "fog_density", ropt->fog_density);
  xml_pack_float (node, "fog_offset", ropt->fog_offset);
  xml_pack_float (node, "fog_turbulence", ropt->fog_turbulence);
  xml_pack_float (node, "west_direction", ropt->west_direction);
  xml_pack_float (node, "time_of_day", ropt->time_of_day);
  xml_pack_float (node, "water_clarity", ropt->water_clarity);
  xml_pack_float (node, "ambient_light_luminosity", ropt->ambient_light_luminosity);
  xml_pack_float (node, "camera_light_luminosity", ropt->camera_light_luminosity);
  xml_pack_float (node, "moon_time_offset", ropt->moon_time_offset);
  xml_pack_float (node, "ground_fog_density", ropt->ground_fog_density);
  xml_pack_float (node, "skycolor_red", ropt->skycolor_red);
  xml_pack_float (node, "skycolor_green", ropt->skycolor_green);
  xml_pack_float (node, "skycolor_blue", ropt->skycolor_blue);
  xml_pack_float (node, "lights_luminosity", ropt->lights_luminosity);
  xml_pack_float (node, "water_reflection", ropt->water_reflection);
  xml_pack_float (node, "water_color_red", ropt->water_color_red);
  xml_pack_float (node, "water_color_green", ropt->water_color_green);
  xml_pack_float (node, "water_color_blue", ropt->water_color_blue);
  xml_pack_float (node, "water_frequency", ropt->water_frequency);
  xml_pack_float (node, "waves_height", ropt->waves_height);
  xml_pack_float (node, "texture_spread", ropt->texture_spread);
  xml_pack_float (node, "terrain_turbulence", ropt->terrain_turbulence);
  xml_pack_float (node, "bgimage_scale", ropt->bgimage_scale);
  xml_pack_float (node, "bgimage_offset", ropt->bgimage_offset);
  xml_pack_float (node, "bgimage_enlightment", ropt->bgimage_enlightment);
  xml_pack_float (node, "sun_size", ropt->sun_size);
  xml_pack_float (node, "sun_color_red", ropt->sun_color_red);
  xml_pack_float (node, "sun_color_green", ropt->sun_color_green);
  xml_pack_float (node, "sun_color_blue", ropt->sun_color_blue);
  xml_pack_float (node, "moon_rot_Y", ropt->moon_rot_Y);
  xml_pack_float (node, "moon_rot_Z", ropt->moon_rot_Z);
  xml_pack_float (node, "moon_map_enlightment", ropt->moon_map_enlightment);
  xml_pack_float (node, "moon_size", ropt->moon_size);
  xml_pack_float (node, "moon_color_red", ropt->moon_color_red);
  xml_pack_float (node, "moon_color_green", ropt->moon_color_green);
  xml_pack_float (node, "moon_color_blue", ropt->moon_color_blue);
  xml_pack_float (node, "fog_blue", ropt->fog_blue);
  xml_pack_float (node, "texture_rock_amount", ropt->texture_rock_amount);
  xml_pack_float (node, "texture_grass_amount", ropt->texture_grass_amount);
  xml_pack_float (node, "texture_snow_amount", ropt->texture_snow_amount);
  xml_pack_float (node, "texture_rock_red", ropt->texture_rock_red);
  xml_pack_float (node, "texture_rock_green", ropt->texture_rock_green);
  xml_pack_float (node, "texture_rock_blue", ropt->texture_rock_blue);
  xml_pack_float (node, "texture_rock_grain", ropt->texture_rock_grain);
  xml_pack_float (node, "texture_grass_red", ropt->texture_grass_red);
  xml_pack_float (node, "texture_grass_green", ropt->texture_grass_green);
  xml_pack_float (node, "texture_grass_blue", ropt->texture_grass_blue);
  xml_pack_float (node, "texture_grass_grain", ropt->texture_grass_grain);
  xml_pack_float (node, "texture_snow_red", ropt->texture_snow_red);
  xml_pack_float (node, "texture_snow_green", ropt->texture_snow_green);
  xml_pack_float (node, "texture_snow_blue", ropt->texture_snow_blue);
  xml_pack_float (node, "texture_snow_grain", ropt->texture_snow_grain);
  xml_pack_float (node, "texture_stratum_size", ropt->texture_stratum_size);
  xml_pack_float (node, "texture_waterborder_size", ropt->texture_waterborder_size);
  xml_pack_float (node, "texture_waterborder_sand_red", ropt->texture_waterborder_sand_red);
  xml_pack_float (node, "texture_waterborder_sand_green", ropt->texture_waterborder_sand_green);
  xml_pack_float (node, "texture_waterborder_sand_blue", ropt->texture_waterborder_sand_blue);
  xml_pack_float (node, "texture_waterborder_sand_grain", ropt->texture_waterborder_sand_grain);
  xml_pack_float (node, "camera_zoom", ropt->camera_zoom);
  xml_pack_float (node, "radiosity_quality", ropt->radiosity_quality);

  xml_pack_int (node, "sky_system", ropt->sky_system);
  xml_pack_int (node, "light_system", ropt->light_system);
  xml_pack_int (node, "camera_type", ropt->camera_type);
  xml_pack_int (node, "texture_method", ropt->texture_method);

  xml_pack_int (node, "render_scale_x", ropt->render_scale_x);
  xml_pack_int (node, "render_scale_y", ropt->render_scale_y);
  xml_pack_int (node, "render_scale_z", ropt->render_scale_z);

  xml_pack_string (node, "atmosphere_file", ropt->atmosphere_file); 
  xml_pack_string (node, "cloud_file", ropt->cloud_file); 
  xml_pack_string (node, "star_file", ropt->star_file); 
  xml_pack_string (node, "texture_file", ropt->texture_file); 
  xml_pack_string (node, "water_file", ropt->water_file); 
  xml_pack_string (node, "light_file", ropt->light_file); 
  //xml_pack_string (node, "scene_theme_file", ropt->scene_theme_file); 
  xml_pack_string (node, "skycolor_file", ropt->skycolor_file); 
  xml_pack_string (node, "background_image_file", ropt->background_image_file); 
  xml_pack_string (node, "moon_image_file", ropt->moon_image_file); 

  xml_pack_boolean (node, "do_output_file", ropt->do_output_file);
  xml_pack_boolean (node, "use_alpha", ropt->use_alpha);
  xml_pack_boolean (node, "do_partial_render", ropt->do_partial_render);
  //xml_pack_boolean (node, "partial_render_as_percentage", ropt->partial_render_as_percentage);
  xml_pack_boolean (node, "aa", ropt->do_aa);
  xml_pack_boolean (node, "custom_size", ropt->do_custom_size);
  xml_pack_boolean (node, "jitter", ropt->do_jitter);
  xml_pack_string (node, "image_size_string", ropt->image_size_string); 
  xml_pack_string (node, "povray_filename", ropt->povray_filename);
  xml_pack_int (node, "filetype", ropt->filetype);
  xml_pack_int (node, "bits_per_colour", ropt->bits_per_colour);
  xml_pack_int (node, "image_width", ropt->image_width);
  xml_pack_int (node, "image_height", ropt->image_height);
  xml_pack_int (node, "render_quality", ropt->render_quality);
  xml_pack_float (node, "aa_threshold", ropt->aa_threshold);
  xml_pack_int (node, "aa_type", ropt->aa_type);
  xml_pack_float (node, "start_column", ropt->start_column);
  xml_pack_float (node, "end_column", ropt->end_column);
  xml_pack_float (node, "start_row", ropt->start_row);
  xml_pack_float (node, "end_row", ropt->end_row);
  xml_pack_float (node, "aa_threshold", ropt->aa_type);
  xml_pack_float (node, "aa_depth", ropt->aa_depth);
  xml_pack_float (node, "jitter_amount", ropt->jitter_amount);

/*
 * This part added for Trace Objects by Raymond Ostertag
 */
  if (ropt->trace_object_file)
  {
    xml_pack_string (node, "trace_object_file", ropt->trace_object_file); 
    xml_pack_float (node, "trace_object_x", ropt->trace_object_x);
    xml_pack_float (node, "trace_object_y", ropt->trace_object_y);
    xml_pack_float (node, "trace_object_z", ropt->trace_object_z);
    xml_pack_boolean (node, "do_trace_object_relativ", ropt->do_trace_object_relativ);
    xml_pack_boolean (node, "do_trace_object_conserve", ropt->do_trace_object_conserve);
    xml_pack_boolean (node, "do_trace_object_float", ropt->do_trace_object_float);
    xml_pack_float (node, "trace_object_scale", ropt->trace_object_scale);
  }
/*
 * End of Trace Objects
 */

}

static void
objects_save (TTerrain   *terrain,
              xmlNodePtr  node)
{
  GArray *object_array;
  gint    i;

  object_array = terrain->objects;
  for (i = 0; i < object_array->len; i++)
    {
      TTerrainObject *object;
      xmlNodePtr      xml_object;

      object = &g_array_index (object_array, TTerrainObject, i);
      xml_object = xmlNewChild (node, NULL, "object", NULL);

      xmlSetProp (xml_object, "name", object->name);
      xml_pack_prop_float (xml_object, "ox", object->ox);
      xml_pack_prop_float (xml_object, "oy", object->oy);
      xml_pack_prop_float (xml_object, "x", object->x);
      xml_pack_prop_float (xml_object, "y", object->y);
      xml_pack_prop_float (xml_object, "angle", object->angle);
      xml_pack_prop_float (xml_object, "scale_x", object->scale_x);
      xml_pack_prop_float (xml_object, "scale_y", object->scale_y);
      xml_pack_prop_float (xml_object, "scale_z", object->scale_z);
    }
}

static void
riversources_save (TTerrain   *terrain,
		   xmlNodePtr  node)
{
  GArray *source_array;
  gint    i;

  source_array = terrain->riversources;
  for (i = 0; i < source_array->len; i++)
    {
      TTerrainRiverCoords *coords;
      xmlNodePtr      xml_object;

      coords = &g_array_index (source_array, TTerrainRiverCoords, i);
      xml_object = xmlNewChild (node, NULL, "riversource", NULL);

      xmlSetProp (xml_object, "name", "river");
      xml_pack_prop_float (xml_object, "center_x", coords->x);
      xml_pack_prop_float (xml_object, "center_y", coords->y);
    }
}

static gint
t_terrain_save_native (TTerrain *terrain)
{
  xmlDocPtr  doc;
  xmlNodePtr node;

  doc = xmlNewDoc ("1.0");
  doc->children = xmlNewDocNode (doc, NULL, "Terrain", NULL);
  node = xmlNewChild (doc->children, NULL, "Options", NULL);
  options_save (terrain, node);
  node = xmlNewChild (doc->children, NULL, "Heightfield", NULL);
  heightfield_save (terrain, node);

  if (terrain->objects->len)
    {
    node = xmlNewChild (doc->children, NULL, "Objects", NULL);
    objects_save (terrain, node);
    }

  if (terrain->selection)
    {
    node = xmlNewChild (doc->children, NULL, "Selection", NULL);
    selection_save (terrain, node);
    }

  if (terrain->riverfield)
    {
    node = xmlNewChild (doc->children, NULL, "Riverfield", NULL);
    riverfield_save (terrain, node);
    }

  if(terrain->riversources->len > 0)
    {
    node = xmlNewChild (doc->children, NULL, "Rivers", NULL);
    riversources_save (terrain, node);
    }

  xmlSaveFile (terrain->filename, doc);
  xmlFreeDoc (doc);

  return 0;
}

static gint
t_terrain_export_tga_field (TTerrain    *terrain,
                            const gchar *filename, 
                            gfloat      *field)
{
  FILE  *out;
  gchar  buf[3];
  gint   i, size;
  gint   status = 0;

  out = fopen (filename, "wb");
  if (!out)
    return -1;

  fputc (0, out); /* Id Length */
  fputc (0, out); /* CoMapType (0 = no map) */
  fputc (2, out); /* Image type */
  fputc (0, out); /* Index of first color entry */
  fputc (0, out);
  fputc (0, out); /* Number of entries in colormap */
  fputc (0, out);
  fputc (0, out); /* Size of colormap entry 15/16/24/32 */
  fputc (0, out); /* X origin */
  fputc (0, out);
  fputc (0, out); /* Y origin */
  fputc (0, out);
  fputc (terrain->width, out);        /* Width */
  fputc (terrain->width >> 8, out);
  fputc (terrain->height, out);       /* Height */
  fputc (terrain->height >> 8, out);
  fputc (24, out);   /* Depth */
  fputc (0x20, out); /* Descriptor */

  size = terrain->width * terrain->height;
  buf[0] = 0; /* Blue */
  for (i = 0; i < size; i++)
    {
      gint value;

      value =
        (gint) MIN (MAX (MAX_16_BIT * field[i], 0.0), MAX_16_BIT);

      buf[1] = value;      /* Green */
      buf[2] = value >> 8; /* Red   */

      fwrite (buf, 3, 1, out);
    }

  status = ferror (out);
  fclose (out);

  return status;
}

static gint
t_terrain_export_tga (TTerrain    *terrain,
                      const gchar *filename) 
{
  return t_terrain_export_tga_field (terrain, filename, terrain->heightfield);

}

static gint
t_terrain_export_pov (TTerrain    *terrain,
                      const gchar *filename)
{
  FILE   *out;
  time_t  now;
  gchar  *tga;
  gchar  *tmp;
  gchar  *river;
  gfloat  size_x, size_y, size_z;
  gint    status = 0;
  gint    i;
  GArray *object_array;
  GTree  *includes;
  TTerrainOptRender * const ropt = &(terrain->render_options);
  char   *previous_locale;

  out = fopen (filename, "wb");
  if (out == NULL)
    return -1;

  /* Save the current locale so that the povray files are written 
   * with the correct format
   */
  previous_locale = setlocale (LC_NUMERIC, "POSIX"); 

  now = time (NULL);

  fprintf (out, "// File created by Terraform on %s\n", ctime (&now));
  tmp = filename_get_base_without_extension_and_dot (filename);
  tga = g_strdup_printf ("%s_hf.tga", tmp);
  g_free (tmp);

  fprintf (out, "#declare TF_HEIGHT_FIELD = height_field { tga \"%s\" smooth }\n",
           tga);
  g_free (tga);
  fprintf (out, "#declare TF_CAMERA_TYPE = %i;\n", ropt->camera_type);
  fprintf_C(out, "#declare TF_CAMERA_ZOOM  = %f;\n", ropt->camera_zoom);

  fprintf (out, "//Atmosphere\n");
  fprintf (out, "#declare TF_HAVE_FOG = %s;\n", ropt->do_fog ? "true" : "false");
  fprintf (out, "#declare TF_HAVE_GROUND_FOG = %s;\n", ropt->do_ground_fog ? "true" : "false");
  fprintf (out, "#declare TF_HAVE_RAINBOW = %s;\n", ropt->do_rainbow ? "true" : "false");
  fprintf (out, "#declare TF_FOG_OFFSET = %f;\n", ropt->fog_offset);
  fprintf (out, "#declare TF_FOG_ALT = %f;\n", ropt->fog_alt);
  fprintf (out, "#declare TF_FOG_TURBULENCE = %f;\n", ropt->fog_turbulence);
  fprintf (out, "#declare TF_FOG_DENSITY = %f;\n", ropt->fog_density);
  fprintf (out, "#declare TF_GROUND_FOG_DENSITY = %f;\n", ropt->ground_fog_density);
  fprintf (out, "#declare TF_FOG_BLUE = %f;\n", ropt->fog_blue);

  fprintf (out, "//Sky\n");
  fprintf_C (out, "#declare TF_SKY_SYSTEM = %i;\n", ropt->sky_system);
  fprintf (out, "#declare TF_SKY_COLORDESCRIPTION = \"%s\"\n", ropt->skycolor_file);
  fprintf_C(out, "#declare TF_SKY_COLORATION_RED = %f;\n", ropt->skycolor_red);
  fprintf_C(out, "#declare TF_SKY_COLORATION_GREEN = %f;\n", ropt->skycolor_green);
  fprintf_C(out, "#declare TF_SKY_COLORATION_BLUE = %f;\n", ropt->skycolor_blue);
  fprintf (out, "#declare TF_HAVE_CLOUDS = %s;\n", ropt->do_clouds ? "true" : "false");
  fprintf (out, "#declare TF_CLOUDS_TEXTURE = \"%s\"\n", ropt->cloud_file);
  fprintf (out, "#declare TF_HAVE_STARS = %s;\n", ropt->do_stars ? "true" : "false");
  fprintf (out, "#declare TF_STARS_TEXTURE = \"%s\"\n", ropt->star_file);
  fprintf (out, "#declare TF_HAVE_CELEST_OBJECTS = %s;\n", ropt->do_celest_objects ? "true" : "false");
  fprintf (out, "#declare TF_SKY_IMAGE = \"%s\"\n", ropt->background_image_file);
  fprintf_C(out, "#declare TF_SKY_IMAGE_SCALE = %f;\n", ropt->bgimage_scale);
  fprintf_C(out, "#declare TF_SKY_IMAGE_ELEVATION_OFFSET= %f;\n", ropt->bgimage_offset);
  fprintf_C(out, "#declare TF_SKY_IMAGE_LUMINOSITY= %f;\n", ropt->bgimage_enlightment);
  fprintf (out, "#declare TF_HAVE_CELEST_OBJECTS = %s;\n", ropt->do_celest_objects ? "true" : "false");
  fprintf_C(out, "#declare TF_SUN_APPARENT_SIZE= %f;\n", ropt->sun_size);
  fprintf_C(out, "#declare TF_SUN_LIGHT_COLOR_RED= %f;\n", ropt->sun_color_red);
  fprintf_C(out, "#declare TF_SUN_LIGHT_COLOR_GREEN= %f;\n", ropt->sun_color_green);
  fprintf_C(out, "#declare TF_SUN_LIGHT_COLOR_BLUE= %f;\n", ropt->sun_color_blue);
  fprintf (out, "#declare TF_MOON_IMAGE = \"%s\"\n", ropt->moon_image_file);
  fprintf_C(out, "#declare TF_MOON_Y_ROT= %f;\n", ropt->moon_rot_Y);
  fprintf_C(out, "#declare TF_MOON_Z_ROT= %f;\n", ropt->moon_rot_Z);
  fprintf_C(out, "#declare TF_MOON_IMAGE_LUMINOSITY= %f;\n", ropt->moon_map_enlightment);
  fprintf_C(out, "#declare TF_MOON_APPARENT_SIZE= %f;\n", ropt->moon_size);
  fprintf_C(out, "#declare TF_MOON_LIGHT_COLOR_RED= %f;\n", ropt->moon_color_red);
  fprintf_C(out, "#declare TF_MOON_LIGHT_COLOR_GREEN= %f;\n", ropt->moon_color_green);
  fprintf_C(out, "#declare TF_MOON_LIGHT_COLOR_BLUE= %f;\n", ropt->moon_color_blue);

  fprintf (out, "//Lights\n");
  fprintf_C (out, "#declare TF_LIGHTS_SYSTEM = %i;\n", ropt->light_system);
  fprintf_C (out, "#declare TF_LIGHTS_LUMINOSITY = %f;\n", ropt->lights_luminosity);
  fprintf_C (out, "#declare TF_AMBIENT_LIGHT_LUMINOSITY = %f;\n", ropt->ambient_light_luminosity);
  fprintf (out, "#declare TF_RADIOSITY = %s;\n", ropt->do_radiosity? "true" : "false");
  fprintf_C (out, "#declare TF_RADIOSITY_QUALITY = %f;\n", ropt->radiosity_quality);
  fprintf_C (out, "#declare TF_TIME_OF_DAY = %f;\n", ropt->time_of_day);
  fprintf_C (out, "#declare TF_MOON_TIME_OFFSET= %f;\n", ropt->moon_time_offset);
  fprintf_C (out, "#declare TF_WEST_DIR = %f;\n", ropt->west_direction);
  fprintf (out, "#declare TF_LIGHTS_DESCRIPTION = \"%s\"\n", ropt->light_file);
  fprintf (out, "#declare TF_HAVE_CAMERA_LIGHT = %s;\n", ropt->do_camera_light ? "true" : "false");
  fprintf_C (out, "#declare TF_CAMERA_LIGHT_LUMINOSITY = %f;\n", ropt->camera_light_luminosity);

  fprintf (out, "//Sea\n");
  fprintf (out, "#declare TF_HAVE_WATER = %s;\n", terrain->do_filled_sea ? "true" : "false");
  fprintf (out, "#declare TF_WATER_APPLY_ON_RIVER = %s;\n", ropt->do_water_apply_on_river ? "true" : "false");
  fprintf_C (out, "#declare TF_WATER_LEVEL = %f;\n", terrain->sealevel);
  fprintf_C (out, "#declare TF_WATER_CLARITY = %f;\n", ropt->water_clarity);
  fprintf_C (out, "#declare TF_WATER_REFLECTION = %f;\n", ropt->water_reflection);
  fprintf_C (out, "#declare TF_WATER_COLOR_RED = %f;\n", ropt->water_color_red);
  fprintf_C (out, "#declare TF_WATER_COLOR_GREEN = %f;\n", ropt->water_color_green);
  fprintf_C (out, "#declare TF_WATER_COLOR_BLUE = %f;\n", ropt->water_color_blue);
  fprintf_C (out, "#declare TF_WATER_FREQUENCY = %f;\n", ropt->water_frequency);
  fprintf_C (out, "#declare TF_WATER_HEIGHT = %f;\n", ropt->waves_height);

  fprintf (out, "//Terrain\n");
  fprintf_C (out, "#declare TF_LANDSCAPE_DISTRIBUTION = %f;\n", ropt->texture_spread);
  fprintf_C (out, "#declare TF_LANDSCAPE_TURBULENCE = %f;\n", ropt->terrain_turbulence);
  fprintf_C (out, "#declare TF_TEXTURE_METHOD = %f;\n", ropt->texture_method);
  fprintf (out, "#declare TF_ROCK_STRATUM  = %s;\n", ropt->do_texture_stratum? "true" : "false");
  fprintf (out, "#declare TF_GRASSTEX = %s;\n", ropt->do_texture_grasstex? "true" : "false");
  fprintf (out, "#declare TF_LANDSCAPE_WATER_TRANSITION = %s;\n", ropt->do_texture_waterborder? "true" : "false");
  fprintf (out, "#declare TF_LANDSCAPE_WATER_TRANSITION_SAND= %s;\n", ropt->do_texture_waterborder_sand? "true" : "false");
  fprintf (out, "#declare TF_LANDSCAPE_WATER_TRANSITION_ICE = %s;\n", ropt->do_texture_waterborder_ice? "true" : "false");
  fprintf_C (out, "#declare TF_ROCK_AMOUNT = %f;\n", ropt->texture_rock_amount);
  fprintf_C (out, "#declare TF_GRASS_AMOUNT = %f;\n", ropt->texture_grass_amount);
  fprintf_C (out, "#declare TF_SNOW_AMOUNT = %f;\n", ropt->texture_snow_amount);
  fprintf_C (out, "#declare BTEX_ROCK_COLOR_RED = %f;\n", ropt->texture_rock_red);
  fprintf_C (out, "#declare BTEX_ROCK_COLOR_GREEN = %f;\n", ropt->texture_rock_green);
  fprintf_C (out, "#declare BTEX_ROCK_COLOR_BLUE = %f;\n", ropt->texture_rock_blue);
  fprintf_C (out, "#declare BTEX_ROCK_GRAIN = %f;\n", ropt->texture_rock_grain);
  fprintf_C (out, "#declare BTEX_GRASS_COLOR_RED = %f;\n", ropt->texture_grass_red);
  fprintf_C (out, "#declare BTEX_GRASS_COLOR_GREEN = %f;\n", ropt->texture_grass_green);
  fprintf_C (out, "#declare BTEX_GRASS_COLOR_BLUE = %f;\n", ropt->texture_grass_blue);
  fprintf_C (out, "#declare BTEX_GRASS_GRAIN = %f;\n", ropt->texture_grass_grain);
  fprintf_C (out, "#declare BTEX_SNOW_COLOR_RED = %f;\n", ropt->texture_snow_red);
  fprintf_C (out, "#declare BTEX_SNOW_COLOR_GREEN = %f;\n", ropt->texture_snow_green);
  fprintf_C (out, "#declare BTEX_SNOW_COLOR_BLUE = %f;\n", ropt->texture_snow_blue);
  fprintf_C (out, "#declare BTEX_SNOW_GRAIN = %f;\n", ropt->texture_snow_grain);
  fprintf_C (out, "#declare BTEX_STRATUM_SIZE = %f;\n", ropt->texture_stratum_size);;
  fprintf_C (out, "#declare TF_TRANSITION_VALUE = %f;\n", ropt->texture_waterborder_size);
  fprintf_C (out, "#declare BTEX_SAND_COLOR_RED = %f;\n", ropt->texture_waterborder_sand_red);
  fprintf_C (out, "#declare BTEX_SAND_COLOR_GREEN = %f;\n", ropt->texture_waterborder_sand_green);
  fprintf_C (out, "#declare BTEX_SAND_COLOR_BLUE = %f;\n", ropt->texture_waterborder_sand_blue);
  fprintf_C (out, "#declare BTEX_SAND_GRAIN = %f;\n", ropt->texture_waterborder_sand_grain);

  //fprintf (out, "#declare TF_TILE_TERRAIN = %s;\n", terrain->do_tile ? "true" : "false");
  fprintf (out, "#declare TF_TILE_TERRAIN = %s;\n", "false");
 
/*
  * This part added for Trace Objects by Raymond Ostertag
  */
if (ropt->trace_object_file)
{
    fprintf (out, "#declare TF_TRACE_OBJECT_1 = \"%s\"\n", ropt->trace_object_file);
    fprintf (out, "#declare TF_TRACE_OBJECT_X = %f;\n", ropt->trace_object_x);
    fprintf (out, "#declare TF_TRACE_OBJECT_Y = %f;\n", ropt->trace_object_y);
    fprintf (out, "#declare TF_TRACE_OBJECT_Z = %f;\n", ropt->trace_object_z);
    fprintf (out, "#declare TF_TRACE_OBJECT_RELATIV = %s;\n", ropt->do_trace_object_relativ ? "true" : "false");
    fprintf (out, "#declare TF_TRACE_OBJECT_CONSERVE = %s;\n", ropt->do_trace_object_conserve ? "true" : "false");
    fprintf (out, "#declare TF_TRACE_OBJECT_FLOAT = %s;\n", ropt->do_trace_object_float ? "true" : "false");
    fprintf (out, "#declare TF_TRACE_OBJECT_SCALE = %f;\n", ropt->trace_object_scale);
}
/*
 * End of Trace Objects
 */

  /* write any objects which are to be placed to the POV file */
  fprintf (out, "\n\n");

  fprintf_C (out, "#declare TF_Y_SCALE_FACTOR = %f;\n", terrain->y_scale_factor);

  t_terrain_get_povray_size (terrain, &size_x, &size_y, &size_z);

  fprintf_C (out, "#declare TF_X_SCALE = %f;\n", size_x);
  // handled by defaults.inc 
  //fprintf_C (out, "#declare TF_Y_SCALE = %f;\n", size_y); 
  fprintf_C (out, "#declare TF_Z_SCALE = %f;\n", size_z);

  fprintf_C (out, "#declare TF_CAMERA_LOCATION = <%f, %f, %f>;\n", 
             ropt->camera_x * size_x,
             ropt->camera_y * size_y,
             ropt->camera_z * size_z);

  fprintf_C (out, "#declare TF_CAMERA_LOOK_AT = <%f, %f, %f>;\n", 
             ropt->lookat_x * size_x,
             ropt->lookat_y * size_y,
             ropt->lookat_z * size_z);

  fprintf (out, "\n\n");

//  fprintf (out, "#include \"colors.inc\"\n"); 
  fprintf (out, "#include \"defaults.inc\"\n");

  fprintf (out, "#include \"%s\"\n", ropt->atmosphere_file);
  fprintf (out, "#include \"sky_system.inc\"\n");
  if (ropt->do_texture_constructor)
  {
     if (ropt->texture_method == 0)
     {
       fprintf (out, "#include \"texture_constructor.inc\"\n");
     }
     if (ropt->texture_method == 1)
     {
       fprintf (out, "#include \"texture_constructor2.inc\"\n");
     }
  }
  else 
  {
     fprintf (out, "#include \"%s\"\n", ropt->texture_file);
  }
  fprintf (out, "#include \"%s\"\n", ropt->water_file);

  fprintf (out, "#include \"generic_land.inc\"\n");

  fprintf (out, "\n");

  object_array = terrain->objects;

  /* Write out all the #includes */
  includes = g_tree_new ((GCompareFunc) strcmp);
  for (i = 0; i < object_array->len; i++)
    {
      TTerrainObject *object;

      object = &g_array_index (object_array, TTerrainObject, i);
      if (g_tree_lookup (includes, object->name) == NULL)
        {
          g_tree_insert (includes, object->name, object->name);
          fprintf (out, "#include \"%s\"\n", object->name);
        }
    }
  g_tree_destroy (includes);

  fprintf (out, "\n");

  /* Write out all the placed objects */
  for (i = 0; i < object_array->len; i++)
    {
      TTerrainObject *object;
      gint            hf_x, hf_y;
      gfloat          pov_x, pov_y, pov_z;
      gchar          *filename, *name;
      gboolean        macro;

      object = &g_array_index (object_array, TTerrainObject, i);

      hf_x = object->x * (terrain->width - 1);
      hf_y = object->y * (terrain->height - 1);
      pov_x = object->x * size_x;
      // write the raw elevation value and let POVRay do the scaling later
      //pov_y = terrain->heightfield[hf_y * terrain->width + hf_x] * size_y;
      pov_y = terrain->heightfield[hf_y * terrain->width + hf_x];
      pov_z = (1.0 - object->y) * size_z;

      filename = filename_get_base (object->name);
      name = filename_new_extension (filename, NULL);
      g_free (filename);

      macro = FALSE;
      /* Does the object name end with "_macro"? */
      if (strlen (name) > 6 && !strcmp (&name[strlen (name) - 6], "_macro"))
        macro = TRUE;

      /* FIXME: Handle macros */
      fprintf_C (out, "object { %s%s() scale <%f, %f, %f> rotate y * %f translate <%f, TF_Y_SCALE*%f, %f> }\n",
        name, macro ? "()" : "",
        object->scale_x, object->scale_y, object->scale_z,
        object->angle, pov_x, pov_y, pov_z);

      g_free (name);
    }

  /* handle placed rivers */
  if (terrain->riverfield != NULL)
    {
      /* River heightfield name */
      tmp = filename_get_base_without_extension_and_dot (filename);
      river = g_strdup_printf ("%s_river.tga", tmp);
      g_free (tmp);

      if (status != -1)
        {
        fprintf (out, 
          "#declare RIVER_HEIGHT_FIELD = height_field { tga \"%s\" smooth }\n",
          river);
        fprintf (out, "#include \"river_texture.inc\"\n");
        fprintf (out, "#include \"generic_river.inc\"\n");
	}

      g_free (river);
    } // end river

  status = ferror (out);
  fclose (out);

  /* restore the original locale */
  setlocale (LC_NUMERIC, previous_locale); 

  /* Write heightfield */
  tmp = filename_get_without_extension_and_dot (filename);
  tga = g_strdup_printf ("%s_hf.tga", tmp);
  if (t_terrain_export_tga (terrain, tga) != 0)
    status = -1;
  g_free (tga);

  /* Write river heightfield */
  if (terrain->riverfield != NULL)
    {
      river = g_strdup_printf ("%s_river.tga", tmp);
      if (t_terrain_export_tga_field (terrain, river, terrain->riverfield) != 0)
        status = -1;
      g_free (river);
    }

  g_free (tmp);

  return status;
}

/* write a povray ini file to drive a POV rendering */
static gint
t_terrain_export_ini (TTerrain    *terrain,
                      const gchar *filename)
{
  FILE  *ini_file;
  gint   status;
  gchar *home_dir;
  gchar *pov;
  gchar *out;
  time_t now;
  TTerrainOptRender * const ropt = &(terrain->render_options);

  /* save the current locale so that the povray files are written 
   * with the correct format
   */
  char * previous_locale = setlocale (LC_NUMERIC, "POSIX"); 

  if (ropt->povray_filename == NULL ||
      strlen (ropt->povray_filename) == 0)
    {
      g_free (ropt->povray_filename);
      ropt->povray_filename =
        filename_get_without_extension_and_dot (terrain->filename);
    }

  status = 0;

  ini_file = fopen (filename, "w");
  if (ini_file == NULL)
    return -1;

  pov = filename_new_extension (filename, "pov");

  time (&now);
  fprintf (ini_file, ";; This ini file generated by Terraform on %s\n\n",
           ctime (&now));

  /* filename specific options */
  fprintf (ini_file, "Input_File_Name=%s\n", pov);
  out = g_strdup ("CantHappen");
  if (ropt->do_output_file)
    {
      fprintf (ini_file, "Output_To_File=on\n");

      if (ropt->filetype == 0) /* PNG */
	{
          out = filename_new_extension (ropt->povray_filename, "png");

	  /* I hope this is the best solution for the "out" variable */
          fprintf (ini_file, "Output_File_Type=N\n");
          fprintf (ini_file, "Bits_Per_Color=%d\n", ropt->bits_per_colour);
	  if (ropt->use_alpha)
            fprintf (ini_file, "Output_Alpha=on\n");
	}
      else if (ropt->filetype == 1) /* PPM */
        {
	  out = filename_new_extension (ropt->povray_filename, "ppm");
          fprintf (ini_file, "Output_File_Type=P\n");
        }
      else if (ropt->filetype == 2) /* TGA */
        {
	  out = filename_new_extension (ropt->povray_filename, "tga");

          fprintf (ini_file, "Output_File_Type=T\n");
          if (ropt->use_alpha) 
            fprintf (ini_file, "Output_Alpha=on\n");
        }
      else if (ropt->filetype == 3) /* TGA (compressed) */
        {
	  out = filename_new_extension (ropt->povray_filename, "tga");

          fprintf (ini_file, "Output_File_Type=C\n");
          if (ropt->use_alpha) 
            fprintf (ini_file, "Output_Alpha=on\n");
        }

      fprintf (ini_file, "Output_File_Name=%s\n", out);
    }
  else
    fprintf (ini_file, "Output_To_File=off\n");

  /* width and height of the image */
  fprintf (ini_file, "Width=%d\n", ropt->image_width);
  fprintf (ini_file, "Height=%d\n", ropt->image_height);
  /* partial render */
  if (ropt->do_partial_render)
    {
      fprintf (ini_file, "Start_Column=%f\n", ropt->start_column);
      fprintf (ini_file, "Start_Row=%f\n", ropt->start_row);
      fprintf (ini_file, "End_Column=%f\n", ropt->end_column);
      fprintf (ini_file, "End_Row=%f\n", ropt->end_row);
      //need to find good solution for percentage option
    }

  fprintf (ini_file, "Quality=%d\n", ropt->render_quality);

  if (ropt->do_aa)
    {
      fprintf (ini_file, "Antialias=on\n");
      fprintf (ini_file, "Antialias_Threshold=%f\n", ropt->aa_threshold);

      if (ropt->do_jitter)
        {
          fprintf (ini_file, "Jitter=on\n");
          fprintf (ini_file, "Jitter_Amount=%f\n", ropt->jitter_amount);
	}

      if (ropt->aa_type == 1) /* 0 = type 1, 1 = type 2 */
	{
	  fprintf (ini_file, "Sampling_Method=2\n");
	  fprintf (ini_file, "Antialias_Depth=%f\n", ropt->aa_depth);
	}
    }

  fprintf (ini_file, "Display=on\n");
  fprintf (ini_file, "Pause_When_Done=on\n\n");

  /* POV-Ray files are installed under the $HOME dir */
  home_dir = getenv ("HOME");
  if (home_dir != NULL)
    {
      gchar       *terraform_dir;
      struct stat  statdir;
      gint         rc;

      terraform_dir = g_strdup_printf ("%s/.terraform", home_dir);
      rc = stat (terraform_dir, &statdir);

      if (rc != -1 && S_ISDIR (statdir.st_mode))
        {
          gchar *td = terraform_dir; /* shorthand notation */

          fprintf (ini_file, "Library_Path=%s/include\n", td);
          fprintf (ini_file, "Library_Path=%s/objects\n", td);
          fprintf (ini_file, "Library_Path=%s/include/atmospheres\n", td);
          fprintf (ini_file, "Library_Path=%s/include/earth_textures\n", td);
          fprintf (ini_file, "Library_Path=%s/include/skies\n", td);
          fprintf (ini_file, "Library_Path=%s/include/skies/include\n", td);
          fprintf (ini_file, "Library_Path=%s/include/water\n", td);
          fprintf (ini_file, "Library_Path=%s/image_maps\n", td);
        }

      g_free (terraform_dir);
    }

  /* Global installed povray files */
  fprintf (ini_file, "Library_Path=%s/include\n",
           TERRAFORM_DATA_DIR);
  fprintf (ini_file, "Library_Path=%s/objects\n",
           TERRAFORM_DATA_DIR);
  fprintf (ini_file, "Library_Path=%s/include/atmospheres\n",
           TERRAFORM_DATA_DIR);
  fprintf (ini_file, "Library_Path=%s/include/earth_textures\n",
           TERRAFORM_DATA_DIR);
  fprintf (ini_file, "Library_Path=%s/include/skies\n",
           TERRAFORM_DATA_DIR);
  fprintf (ini_file, "Library_Path=%s/include/skies/include\n",
           TERRAFORM_DATA_DIR);
  fprintf (ini_file, "Library_Path=%s/include/water\n",
           TERRAFORM_DATA_DIR);
  fprintf (ini_file, "Library_Path=%s/image_maps\n",
           TERRAFORM_DATA_DIR);

  /* Close ini file */
  status = ferror (ini_file);
  fclose (ini_file);

  /* Write POV file */
  if (t_terrain_export_pov (terrain, pov) != 0)
    status = -1;

  g_free (out);
  g_free (pov);

  /* Restore the original locale */
  setlocale (LC_NUMERIC, previous_locale); 

  return status;
}

typedef struct BmpHeader BmpHeader;

struct BmpHeader
{
  guint32 bfSize;
  guint32 bfReserved;
  guint32 bfOffBits;
  guint32 biSize;
  guint32 biWidth;
  guint32 biHeight;
  guint16 biPlanes;
  guint16 biBitCount;
  guint32 biCompression;
  guint32 biSizeImage;
  guint32 biXPelsPerMeter;
  guint32 biYPelsPerMeter;
  guint32 biClrUsed;
  guint32 biClrImportant;
};

static gint
t_terrain_export_bmp (TTerrain    *terrain,
                      const gchar *filename)
{
  FILE      *out;
  gint       row_size, row_width;
  BmpHeader  header;
  gint       x, y;
  gint       status;

  out = fopen (filename, "wb");
  if (!out)
    return -1;

  row_size = terrain->width * 3;
  row_width = (row_size + 3) & 0xfffc;

  header.bfSize = GUINT32_TO_LE (54 + row_width * terrain->height);
  header.bfReserved = 0;
  header.bfOffBits = GUINT32_TO_LE (54);
  header.biSize = GUINT32_TO_LE (40);
  header.biWidth = GUINT32_TO_LE (terrain->width);
  header.biHeight = GUINT32_TO_LE (terrain->height);
  header.biPlanes = GUINT16_TO_LE (1);
  header.biBitCount = GUINT16_TO_LE (24);
  header.biCompression = 0;
  header.biSizeImage = GUINT32_TO_LE (row_width * terrain->height);
  header.biXPelsPerMeter = GUINT32_TO_LE (2925);
  header.biYPelsPerMeter = GUINT32_TO_LE (2925);
  header.biClrUsed = 0;
  header.biClrImportant = 0;

  fwrite ("BM", 2, 1, out);
  fwrite (&header, sizeof (header), 1, out);

  for (y = 0; y < terrain->height; y++)
    {
      gfloat *pos;

      pos = &terrain->heightfield[(terrain->height - y - 1) * terrain->width];
      for (x = 0; x < terrain->width; x++)
        {
          gint32 value;

          value = MAX (MIN (*pos * MAX_16_BIT, MAX_16_BIT), 0.0);
          fputc (0, out);
          fputc (value, out);
          fputc (value >> 8, out);

          pos++;
        }

      x *= 3;
      for (; x < row_width; x++)
        fputc ('\0', out);
    }

  status = ferror (out);
  fclose (out);

  return status;
}

static gint
t_terrain_export_bmp_bw (TTerrain    *terrain,
                         const gchar *filename)
{
  FILE      *out;
  gint       row_size, row_width;
  BmpHeader  header;
  gint       x, y;
  gint       status;

  out = fopen (filename, "wb");
  if (!out)
    return -1;

  row_size = terrain->width;
  row_width = (row_size + 3) & 0xfffc;

  header.bfSize = GUINT32_TO_LE (54 + 256 * 4 + row_width * terrain->height);
  header.bfReserved = 0;
  header.bfOffBits = GUINT32_TO_LE (54 + 256 * 4);
  header.biSize = GUINT32_TO_LE (40);
  header.biWidth = GUINT32_TO_LE (terrain->width);
  header.biHeight = GUINT32_TO_LE (terrain->height);
  header.biPlanes = GUINT16_TO_LE (1);
  header.biBitCount = GUINT16_TO_LE (8);
  header.biCompression = 0;
  header.biSizeImage = GUINT32_TO_LE (row_width * terrain->height);
  header.biXPelsPerMeter = GUINT32_TO_LE (2925);
  header.biYPelsPerMeter = GUINT32_TO_LE (2925);
  header.biClrUsed = 256;
  header.biClrImportant = 256;

  fwrite ("BM", 2, 1, out);
  fwrite (&header, sizeof (header), 1, out);

  for (y = 0; y < 256; y++)
    {
      for (x = 0; x < 3; x++)
        fputc (y, out);

      fputc ('\0', out);
    }

  for (y = 0; y < terrain->height; y++)
    {
      gfloat *pos;

      pos = &terrain->heightfield[(terrain->height - y - 1) * terrain->width];
      for (x = 0; x < terrain->width; x++)
        {
          gint32 value;

          value = (gint32) (MAX (MIN (*pos * 255.0, 255.0), 0.0) + 0.5);
          fputc (value, out);

          pos++;
        }

      for (; x < row_width; x++)
        fputc ('\0', out);
    }

  status = ferror (out);
  fclose (out);

  return status;
}

static gint
t_terrain_export_pgm (TTerrain    *terrain,
                      const gchar *filename)
{
  int        cnt;               /* count of #s printed */
  int        x, y;
  int        status;
  gint32     idx;
  FILE      *out;

  /* *** open PGM file *** */
  out = fopen (filename, "wb");
  if (!out)
    return -1;

  fprintf (out, "P2\n");
  fprintf (out, "%d %d\n", terrain->width, terrain->height);
  fprintf (out, "%d\n", (int)MAX_16_BIT);

  idx = 0;
  cnt = 0;
  for (y = 0; y < terrain->height; y++) 
    {
      for (x = 0; x < terrain->height; x++) 
        {
          fprintf (out, "%d ", (int) (MAX_16_BIT * terrain->heightfield[idx++]));
          if (++cnt > 10) 
            { 
              fprintf (out, "\n"); 
              cnt = 0; 
            }
        }

      if (cnt != 0) 
        { 
          fprintf (out, "\n"); 
          cnt = 0;
        }
    }

  status = ferror (out);
  fclose (out);

  return status;
}

static gint
t_terrain_export_pg8 (TTerrain    *terrain,
                      const gchar *filename)
{
  int        x, y;
  gint32     idx;
  FILE      *out;
  const int  maxheight = 255; 
  int        status;

  /* *** open PG8 file *** */
  out = fopen (filename, "wb");
  if (!out)
    return -1;

  fprintf (out, "P5\n");
  fprintf (out, "%d %d\n", terrain->width, terrain->height);
  fprintf (out, "%d\n", maxheight);

  idx = 0;
  for (y = 0; y < terrain->height; y++) 
    for (x = 0; x < terrain->width; x++) 
      fputc (maxheight * terrain->heightfield[idx++], out);

  status = ferror (out);
  fclose (out);

  return status;
}

static gint
t_terrain_export_ppm (TTerrain    *terrain,
                      const gchar *filename)
{
  int        x, y;
  gint32     idx;
  FILE      *out;
  int        status;
  gchar      buf[3];

  /* *** open PPM file *** */
  out = fopen (filename, "wb");
  if (!out)
    return -1;

  fprintf (out, "P6\n"); /* binary PPM image */
  fprintf (out, "# Image exported by Terraform\n");
  fprintf (out, "%d %d\n", terrain->width, terrain->height);
  fprintf (out, "%d\n", MAX_1ST_BYTE);

  idx = 0;
  buf[0] = 0;
  for (y = 0; y < terrain->height; y++) 
    for (x = 0; x < terrain->width; x++) 
      {
      gint value = 
        (gint) MIN (MAX (MAX_16_BIT*terrain->heightfield[idx],0.0), MAX_16_BIT);

      buf[0] = value;
      buf[1] = value >> 8;
      buf[2] = value >> 16;

      fwrite (buf, 3, 1, out);
      ++idx;
      }

  status = ferror (out);
  fclose (out);

  return status;
}

/* defs for MATLAB file format */
#if G_BYTE_ORDER == G_BIG_ENDIAN
#  define NUM_FMT 1 /* 1 IEEE Big Endian (SPARC, SGI, Motorola, etc.) */
#else
#  define NUM_FMT 0 /* 0 IEEE Little Endian (80x86, DEC Risc) */
#endif

#define PREC      1 /* storing Matlab file as floats rather than longs */
typedef struct {    /* Matlab Level 1.0 format data file header        */
  gint32 type;      /* type                         */
  gint32 mrows;     /* row dimension                */
  gint32 ncols;     /* column dimension             */
  gint32 imagf;     /* flag indicating imag part    */
  gint32 namelen;   /* name length (including NULL) */ 
} Fmatrix;


static gint
t_terrain_export_mat (TTerrain    *terrain,
                      const gchar *filename)
{
  FILE       *out;
  Fmatrix     fhdr;
  int         x, y;
  const char *pname = "topo";
  int         status;

  /* *** open MAT file *** */
  out = fopen (filename, "wb");
  if (!out)
    return -1;

  fhdr.type = NUM_FMT * 1000 + PREC * 10;
  fhdr.ncols = terrain->width;
  fhdr.mrows = terrain->height;
  fhdr.imagf = FALSE;
  fhdr.namelen = strlen (pname) + 1;

  /* write header */
  fwrite (&fhdr, sizeof (Fmatrix), (size_t) 1, out); 

  /* write mx name */
  fwrite (pname, sizeof (char), (size_t) fhdr.namelen, out);   

  for (x = 0; x < terrain->width; x++)
    for (y = 0; y < terrain->height; y++)
      {   
        gfloat datum;

        datum = terrain->heightfield[y * terrain->width + x];
        fwrite (&datum, sizeof (datum), 1, out);
      } 

  status = ferror (out);
  fclose (out);

  return status;
}

static gint
t_terrain_export_oct (TTerrain    *terrain,
                      const gchar *filename)
{
  int     x, y;
  gint32  idx;
  int     status;
  FILE   *out;
  char   *mname = "topo";

  /* *** open OCT file *** */
  out = fopen (filename, "wb");
  if (!out)
    return -1;

  fprintf (out, "# name: %s\n", mname);
  fprintf (out, "# type: matrix\n");
  fprintf (out, "# rows: %d\n", terrain->height);
  fprintf (out, "# columns: %d\n", terrain->width);
  
  idx = 0;
  for (y = 0; y < terrain->height; y++) 
    for (x = 0; x < terrain->width; x++) 
      fprintf_C (out, "%e\n", terrain->heightfield[idx++]);

  status = ferror (out);
  fclose (out);
  return status;
}

/*
 * t_terrain_export_ac:
 *   Write the Height Field to an AC file.
 *   The strategy is to create a HeightFieldModel of the field and then to 
 *   save the 3D Data to the file.... 
 *
 * Function created by Roland Reckel (reckel@stud.montefiore.ulg.ac.be)
 * Modified by David A. Bartold
 */

static gint
t_terrain_export_ac (TTerrain    *terrain,
                     const gchar *filename)
{
  FILE   *out;
  int     x, y;
  int     vert_x, vert_y;
  int     res;
  int     status;

  out = fopen (filename, "wb");
  if (!out)
    return -1;

  res = terrain->wireframe_resolution;
  if (MIN (terrain->width, terrain->height) < 40)
    res = 1;

  vert_x = (terrain->width - 1) / res + 1;
  vert_y = (terrain->height - 1) / res + 1;

  fprintf (out, "AC3Db\n");
  /* TODO: set good materials */
  fprintf (out, "MATERIAL \"\" rgb 1 1 1  amb 0.2 0.2 0.2  emis 0 0 0 spec 0.5 0.5 0.5  shi 10  trans 0\n");
  fprintf (out, "kids 1\n");
  fprintf (out, "OBJECT poly\n");
  fprintf (out, "name \"terrain\"\n");
  fprintf (out, "numvert %d\n", vert_x * vert_y);

  for (y = 0; y < terrain->height; y += res)
    for (x = 0; x < terrain->width; x += res)
    fprintf_C (out, "%f %f %f\n",
      x / ((gfloat) terrain->width) - 0.5,
      terrain->heightfield[y * terrain->width + x] * terrain->y_scale_factor,
      y / ((gfloat) terrain->height) - 0.5);

  fprintf (out, "numsurf %d\n", (vert_y - 1) * (vert_x - 1) * 2);
  for (y = 0; y < vert_y - 1; y++)
    for (x = 0; x < vert_x - 1; x++)
      {
        int t1, t2, t3;

        t1 = y * vert_x + x;
        t2 = t1 + 1;
        t3 = t1 + vert_x;

        fprintf (out, "SURF 0x20\n");
        fprintf (out, "mat 0\n");
        fprintf (out, "refs 3\n");
        fprintf (out, "%d 0 0\n", t1);
        fprintf (out, "%d 0 0\n", t2);
        fprintf (out, "%d 0 0\n", t3);

        t1 = t2;
        t2 = t3 + 1;
        fprintf (out, "SURF 0x20\n");
        fprintf (out, "mat 0\n");
        fprintf (out, "refs 3\n");
        fprintf (out, "%d 0 0\n", t1);
        fprintf (out, "%d 0 0\n", t2);
        fprintf (out, "%d 0 0\n", t3);
      }
  fprintf (out, "kids 0\n");

  status = ferror (out);
  fclose (out);

  return status;
}


/*
 * This code is derived from the information gathered from the bmp2ter 
 * utililty, * originally written by Alistair Milne. I also found a 
 * description of the file format at 
 *   http://www.planetside.co.uk/terragen/dev/tgterrain.html
 * which made it possible to implement the complete spec.
 */

/* complete header */
typedef struct terragen_header terragen_header;
struct terragen_header
{
  gchar      magic[16];
  gchar      size_marker[4];
  guint16    size;
  guint16    pad1;
  gchar      xpts_marker[4];
  guint16    xpts;
  guint16    pad2;
  gchar      ypts_marker[4];
  guint16    ypts;
  guint16    pad3;
  gchar      scal_marker[4];
  gfloat     scal[3];
  gchar      crad_marker[4];
  gfloat     crad;
  gchar      crvm_marker[4];
  guint32    crvm;
  gchar      altw_marker[4];
  gint16     heightscale;
  gint16     baseheight;
};

static gint
t_terrain_export_terragen (TTerrain    *terrain,
                           const gchar *filename)
{
  int     i, size;
  int     bwritten=0;
  int     to_pad;
  int     paddata = 0;
  int     status;
  gint16  value;
  FILE   *out;
  terragen_header theader;

  out = fopen (filename, "wb");
  if (!out)
    return -1;

  strncpy (theader.magic, "TERRAGENTERRAIN ", 16);
  strncpy (theader.size_marker, "SIZE", 4);
  strncpy (theader.xpts_marker, "XPTS", 4);
  strncpy (theader.ypts_marker, "YPTS", 4);
  strncpy (theader.scal_marker, "SCAL", 4);
  strncpy (theader.crad_marker, "CRAD", 4);
  strncpy (theader.crvm_marker, "CRVM", 4);
  strncpy (theader.altw_marker, "ALTW", 4);
  theader.size = MIN (terrain->width, terrain->height) - 1;
  theader.pad1 = 0;
  theader.xpts = terrain->width;
  theader.pad2 = 0;
  theader.ypts = terrain->height;
  theader.pad3 = 0;
  theader.scal[0] = 30;
  theader.scal[1] = 30;
  theader.scal[2] = 30;
  theader.crad = 6370;
  theader.crvm = 0;
  theader.heightscale = 20;
  theader.baseheight = 0;

  fwrite (&theader, sizeof (theader), 1, out);

  /* mirror to adjust to terragen coordinate system */
  t_terrain_mirror (terrain, 1); 

  size = terrain->width * terrain->height; 
  for (i=0; i<size; i++) 
      {
      value = (gint16)((terrain->heightfield[i]*2-1) * theader.heightscale);
      fwrite (&value, sizeof (value), 1, out);
      bwritten += 2;
      }

  /* padding to 4 byte boundry derived from original source */
  to_pad = bwritten % 4;
  if (to_pad != 0)
    fwrite (&paddata, sizeof (paddata), 4-to_pad, out);

  /* reconstruct original terrain */
  t_terrain_mirror (terrain, 1); 

  status = ferror (out);
  fclose (out);
  return status;
}

static gint
t_terrain_export_grd (TTerrain    *terrain,
                      const gchar *filename)
{
  int        x, y;
  int        off;
  FILE      *out;
  int        status;

  /* *** open file *** */
  out = fopen (filename, "w");
  if (!out)
    return -1;

  fprintf (out, "0 1 %d\n", terrain->width);
  fprintf (out, "0 1 %d\n", terrain->height);

  off = 0;
  for (y = 0; y < terrain->height; y++) 
    {
    for (x = 0; x < terrain->width; x++) 
      {
      fprintf (out, "%f ", terrain->heightfield[off++]);
      }
    fprintf (out, "\n");
    }

  status = ferror (out);
  fclose (out);

  return status;
}

/*
 * export to a standard XYZ file:
 * 3 columns per line (x, y, elevation)
 */
static gint
t_terrain_export_xyz (TTerrain    *terrain,
                      const gchar *filename)
{
  gint  x, y;
  gint  off;
  FILE *out;
  gint  status;

  /* *** open file *** */
  out = fopen (filename, "w");
  if (!out)
    return -1;

  off = 0;
  for (y = 0; y < terrain->height; y++)
    {
    for (x = 0; x < terrain->width; x++)
      {
      fprintf (out, "%d %d %f\n", 
	             x, y, (terrain->heightfield[off++]*2-1) * MAX_16_BIT);
      }
    }

  status = ferror (out);
  fclose (out);

  return status;
}

/*
 * export to a DXF 
 * adapted from the code in dem2dxf, written by Sol Katz, May 27 1998
 */
static gint
t_terrain_export_dxf (TTerrain    *terrain,
                      const gchar *filename)
{
  gint  x, y;
  gint  off;
  FILE *out;
  gint  status;

  /* *** open file *** */
  out = fopen (filename, "w");
  if (!out)
    return -1;

  fprintf (out, "0\n" );
  fprintf (out, "SECTION\n" );
  fprintf (out, "2\n" );
  fprintf (out, "ENTITIES\n" );

  off = 0;
  for (y = 0; y < terrain->height; y++)
    {
    for (x = 0; x < terrain->width; x++)
      {
      fprintf (out,"0\n");
      fprintf (out,"POINT\n");
      fprintf (out,"8\n");
      fprintf (out,"0\n");
      fprintf (out," 10\n");
      fprintf (out,"%d\n", x);
      fprintf (out," 20\n");
      fprintf (out,"%d\n", y);
      fprintf (out," 30\n");
      fprintf (out,"%lf\n", (terrain->heightfield[off++]*2-1) * MAX_16_BIT);
      }
    }

  fprintf (out, "0\n" );
  fprintf (out, "ENDSEC\n" );
  fprintf (out, "0\n" );
  fprintf (out, "EOF\n" );

  status = ferror (out);
  fclose (out);

  return status;
}

static gint
t_terrain_export_bna (TTerrain    *terrain,
                      const gchar *filename)
{
  int        x, y;
  int        off;
  FILE      *out;
  int        status;

  /* *** open file *** */
  out = fopen (filename, "w");
  if (!out)
    return -1;

  fprintf (out, "\"ELEVATION\", \"X_OFFSET\", \"Y_OFFSET\", 3\n");

  off = 0;
  for (y = 0; y < terrain->height; y++) 
    {
    for (x = 0; x < terrain->width; x++) 
      {
      fprintf (out, "%f %d %d\n", terrain->heightfield[off++], x, y);
      }
    fprintf (out, "\n");
    }

  status = ferror (out);
  fclose (out);

  return status;
}

/* binary terrain format as desribed @ 
 * http://vterrain.org/Implementation/BT.html
 * written by RNG
 */
static gint 
t_terrain_export_bt (TTerrain    *terrain, 
		     const gchar *filename)
{
  FILE           *out;
  gint            x, y;
  gint            off = 0;
  gint            status;
  bt_header       btheader;

  /* *** open file *** */
  out = fopen (filename, "wb");
  if (!out)
    return -1;

  strncpy (btheader.marker, "binterr1.2", 10);
  btheader.width = terrain->width;
  btheader.width_b2 = 0;
  btheader.height = terrain->height;
  btheader.height_b2 = 0;
  btheader.data_size = 4;
  btheader.floating_point = 1;
  btheader.projection = 0;
  btheader.utm_zone = 1;
  btheader.datum = -2;
  btheader.left_extent = 1;
  btheader.right_extent = 1;
  btheader.bottom_extent = 1;
  btheader.top_extent = 1;
  btheader.external_projection = 0;
  for (x=0; x<194; x++)
  	btheader.pad[x] = ' ';

  if (fwrite (&btheader, sizeof (btheader), 1, out) != 1)
    {
    fclose (out);
    return -1;
    }

  for (y=0; y<terrain->height; y++)
    for (x=0; x<terrain->width; x++)
    {
    gfloat 	  fl;

    fl = terrain->heightfield[off++] * MAX_16_BIT;
    fwrite (&fl, sizeof(gfloat), 1, out);
    }

  status = ferror (out);
  fclose (out);

  return status;
}
