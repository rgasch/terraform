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


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>
#include "base64.h"
#include "reader.h"
#include "xmlsupport.h"
#include "filters.h"
#include "clocale.h"

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef HAVE_GDKPIXBUF
#include "gdk-pixbuf/gdk-pixbuf-loader.h"
#include "gdk-pixbuf/gdk-pixbuf.h"
#endif


static TTerrain *t_terrain_load_native       (const gchar *filename);
static TTerrain *t_terrain_import_bmp        (const gchar *filename);
static TTerrain *t_terrain_import_gtopo      (const gchar *filename,
                                              gint         scale);
static TTerrain *t_terrain_import_mat        (const gchar *filename);
static TTerrain *t_terrain_import_oct        (const gchar *filename);
static TTerrain *t_terrain_import_tga        (const gchar *filename);
static TTerrain *t_terrain_import_pgm        (const gchar *filename);
static TTerrain *t_terrain_import_ppm        (const gchar *filename);
static TTerrain *t_terrain_import_gdk_pixbuf (const gchar *filename);
static TTerrain *t_terrain_import_terragen   (const gchar *filename);
static TTerrain *t_terrain_import_grd        (const gchar *filename);
static TTerrain *t_terrain_import_xyz        (const gchar *filename);
static TTerrain *t_terrain_import_dxf        (const gchar *filename);
static TTerrain *t_terrain_import_bna        (const gchar *filename);
static TTerrain *t_terrain_import_bt         (const gchar *filename);
static TTerrain *t_terrain_import_dted       (const gchar *filename);
static TTerrain *t_terrain_import_e00grid    (const gchar *filename);

TTerrain *
t_terrain_load (const gchar *filename,
                TFileType    type)
{
  if (type == FILE_AUTO)
    type = filename_determine_type (filename);

  if (type == FILE_NATIVE)
    return t_terrain_load_native (filename);
  else if (type == FILE_BMP || type == FILE_BMP_BW)
    return t_terrain_import_bmp (filename);
  else if (type == FILE_TGA)
    return t_terrain_import_tga (filename);
  else if (type == FILE_GTOPO)
    return t_terrain_import_gtopo (filename, 10);
  else if (type == FILE_GRD)
    return t_terrain_import_grd (filename);
  else if (type == FILE_MAT)
    return t_terrain_import_mat (filename);
  else if (type == FILE_OCT)
    return t_terrain_import_oct (filename);
  else if (type == FILE_PGM || type == FILE_PG8)
    return t_terrain_import_pgm (filename);
  else if (type == FILE_PPM)
    return t_terrain_import_ppm (filename);
  else if (type == FILE_GIF || type == FILE_ICO || type == FILE_JPG ||
           type == FILE_PNG || type == FILE_RAS || type == FILE_TIF ||
           type == FILE_XBM || type == FILE_XPM)
    return t_terrain_import_gdk_pixbuf (filename);
  else if (type == FILE_TERRAGEN)
    return t_terrain_import_terragen (filename);
  else if (type == FILE_XYZ)
    return t_terrain_import_xyz (filename);
  else if (type == FILE_DXF)
    return t_terrain_import_dxf (filename);
  else if (type == FILE_BNA)
    return t_terrain_import_bna (filename);
  else if (type == FILE_BT)
    return t_terrain_import_bt (filename);
  else if (type == FILE_DTED)
    return t_terrain_import_dted (filename);
  else if (type == FILE_E00)
    return t_terrain_import_e00grid (filename);

  return NULL;
}

static void
options_load (TTerrain   *terrain,
              xmlNodePtr  node)
{
  TTerrainOptRender *ropt = &(terrain->render_options);

  xml_unpack_int (node, "contour_levels", &terrain->contour_levels);
  xml_unpack_boolean (node, "filled_sea", &terrain->do_filled_sea);
  xml_unpack_float (node, "sealevel", &terrain->sealevel);
  xml_unpack_int (node, "wireframe_resolution", &terrain->wireframe_resolution);
  xml_unpack_float (node, "y_scale_factor", &terrain->y_scale_factor);

  xml_unpack_boolean (node, "clouds", &ropt->do_clouds);
  xml_unpack_boolean (node, "fog", &ropt->do_fog);
  xml_unpack_boolean (node, "ground_fog", &ropt->do_ground_fog);
  xml_unpack_boolean (node, "observe_sealevel", &ropt->do_observe_sealevel);
  xml_unpack_boolean (node, "rainbow", &ropt->do_rainbow);
  xml_unpack_boolean (node, "camera_light", &ropt->do_camera_light);
  xml_unpack_boolean (node, "stars", &ropt->do_stars);
  xml_unpack_boolean (node, "celest_objects", &ropt->do_celest_objects);
  xml_unpack_boolean (node, "radiosity", &ropt->do_radiosity);
  xml_unpack_boolean (node, "texture_constructor", &ropt->do_texture_constructor);
  xml_unpack_boolean (node, "texture_stratum", &ropt->do_texture_stratum);
  xml_unpack_boolean (node, "texture_grasstex", &ropt->do_texture_grasstex);
  xml_unpack_boolean (node, "texture_waterborder", &ropt->do_texture_waterborder);
  xml_unpack_boolean (node, "texture_waterborder_sand", &ropt->do_texture_waterborder_sand);
  xml_unpack_boolean (node, "texture_waterborder_ice", &ropt->do_texture_waterborder_ice);
  xml_unpack_boolean (node, "water_apply_on_river", &ropt->do_water_apply_on_river);

  xml_unpack_float (node, "camera_x", &ropt->camera_x);
  xml_unpack_float (node, "camera_y", &ropt->camera_y);
  xml_unpack_float (node, "camera_z", &ropt->camera_z);
  xml_unpack_float (node, "lookat_x", &ropt->lookat_x);
  xml_unpack_float (node, "lookat_y", &ropt->lookat_y);
  xml_unpack_float (node, "lookat_z", &ropt->lookat_z);
  xml_unpack_float (node, "elevation_offset", &ropt->elevation_offset);
  xml_unpack_float (node, "fog_alt", &ropt->fog_alt);
  xml_unpack_float (node, "fog_density", &ropt->fog_density);
  xml_unpack_float (node, "fog_offset", &ropt->fog_offset);
  xml_unpack_float (node, "fog_turbulence", &ropt->fog_turbulence);
  xml_unpack_float (node, "west_direction", &ropt->west_direction);
  xml_unpack_float (node, "time_of_day", &ropt->time_of_day);
  xml_unpack_float (node, "water_clarity", &ropt->water_clarity);
  xml_unpack_float (node, "ambient_light_luminosity", &ropt->ambient_light_luminosity);
  xml_unpack_float (node, "camera_light_luminosity", &ropt->camera_light_luminosity);
  xml_unpack_float (node, "moon_time_offset", &ropt->moon_time_offset);
  xml_unpack_float (node, "ground_fog_density", &ropt->ground_fog_density);
  xml_unpack_float (node, "skycolor_red", &ropt->skycolor_red);
  xml_unpack_float (node, "skycolor_green", &ropt->skycolor_green);
  xml_unpack_float (node, "skycolor_blue", &ropt->skycolor_blue);
  xml_unpack_float (node, "lights_luminosity", &ropt->lights_luminosity);
  xml_unpack_float (node, "water_reflection", &ropt->water_reflection);
  xml_unpack_float (node, "water_color_red", &ropt->water_color_red);
  xml_unpack_float (node, "water_color_green", &ropt->water_color_green);
  xml_unpack_float (node, "water_color_blue", &ropt->water_color_blue);
  xml_unpack_float (node, " water_frequency", &ropt-> water_frequency);
  xml_unpack_float (node, "waves_height", &ropt->waves_height);
  xml_unpack_float (node, "texture_spread", &ropt->texture_spread);
  xml_unpack_float (node, "terrain_turbulence", &ropt->terrain_turbulence);
  xml_unpack_float (node, "bgimage_scale", &ropt->bgimage_scale);
  xml_unpack_float (node, "bgimage_offset", &ropt->bgimage_offset); 
  xml_unpack_float (node, "bgimage_enlightment", &ropt->bgimage_enlightment);
  xml_unpack_float (node, "sun_size", &ropt->sun_size);
  xml_unpack_float (node, "sun_color_red", &ropt->sun_color_red);
  xml_unpack_float (node, "sun_color_green", &ropt->sun_color_green);
  xml_unpack_float (node, "sun_color_blue", &ropt->sun_color_blue);
  xml_unpack_float (node, "moon_rot_Y", &ropt->moon_rot_Y);
  xml_unpack_float (node, "moon_rot_Z", &ropt->moon_rot_Z);
  xml_unpack_float (node, "moon_map_enlightment", &ropt->moon_map_enlightment);
  xml_unpack_float (node, "moon_size", &ropt->moon_size);
  xml_unpack_float (node, "moon_color_red", &ropt->moon_color_red);
  xml_unpack_float (node, "moon_color_green", &ropt->moon_color_green);
  xml_unpack_float (node, "moon_color_blue", &ropt->moon_color_blue);
  xml_unpack_float (node, "fog_blue", &ropt->fog_blue);
  xml_unpack_float (node, "texture_rock_amount", &ropt->texture_rock_amount);
  xml_unpack_float (node, "texture_grass_amount", &ropt->texture_grass_amount);
  xml_unpack_float (node, "texture_snow_amount", &ropt->texture_snow_amount);
  xml_unpack_float (node, "texture_rock_red", &ropt->texture_rock_red);
  xml_unpack_float (node, "texture_rock_green", &ropt->texture_rock_green);
  xml_unpack_float (node, "texture_rock_blue", &ropt->texture_rock_blue);
  xml_unpack_float (node, "texture_rock_grain", &ropt->texture_rock_grain);
  xml_unpack_float (node, "texture_grass_red", &ropt->texture_grass_red);
  xml_unpack_float (node, "texture_grass_green", &ropt->texture_grass_green);
  xml_unpack_float (node, "texture_grass_blue", &ropt->texture_grass_blue);
  xml_unpack_float (node, "texture_grass_grain", &ropt->texture_grass_grain);
  xml_unpack_float (node, "texture_snow_red", &ropt->texture_snow_red);
  xml_unpack_float (node, "texture_snow_green", &ropt->texture_snow_green);
  xml_unpack_float (node, "texture_snow_blue", &ropt->texture_snow_blue);
  xml_unpack_float (node, "texture_snow_grain", &ropt->texture_snow_grain);
  xml_unpack_float (node, "texture_stratum_size", &ropt->texture_stratum_size);
  xml_unpack_float (node, "texture_waterborder_size", &ropt->texture_waterborder_size);
  xml_unpack_float (node, "texture_waterborder_sand_red", &ropt->texture_waterborder_sand_red);
  xml_unpack_float (node, "texture_waterborder_sand_green", &ropt->texture_waterborder_sand_green);
  xml_unpack_float (node, "texture_waterborder_sand_blue", &ropt->texture_waterborder_sand_blue);
  xml_unpack_float (node, "texture_waterborder_sand_grain", &ropt->texture_waterborder_sand_grain);
  xml_unpack_float (node, "camera_zoom", &ropt->camera_zoom);
  xml_unpack_float (node, "radiosity_quality", &ropt->radiosity_quality);

  xml_unpack_int (node, "sky_system", &ropt->sky_system);
  xml_unpack_int (node, "light_system", &ropt->light_system);
  xml_unpack_int (node, "camera_type", &ropt->camera_type);
  xml_unpack_int (node, "texture_method", &ropt->texture_method);

  xml_unpack_int (node, "render_scale_x", &ropt->render_scale_x);
  xml_unpack_int (node, "render_scale_y", &ropt->render_scale_y);
  xml_unpack_int (node, "render_scale_z", &ropt->render_scale_z);

  xml_unpack_string (node, "atmosphere_file", &ropt->atmosphere_file);
  xml_unpack_string (node, "cloud_file", &ropt->cloud_file);
  xml_unpack_string (node, "star_file", &ropt->star_file);
  xml_unpack_string (node, "texture_file", &ropt->texture_file);
  xml_unpack_string (node, "water_file", &ropt->water_file);
  xml_unpack_string (node, "light_file", &ropt->light_file); 
//  xml_unpack_string (node, "scene_theme_file", &ropt->scene_theme_file); 
  xml_unpack_string (node, "skycolor_file", &ropt->skycolor_file); 
  xml_unpack_string (node, "background_image_file", &ropt->background_image_file);
  xml_unpack_string (node, "moon_image_file", &ropt->moon_image_file); 

  xml_unpack_boolean (node, "do_output_file", &ropt->do_output_file);
  xml_unpack_boolean (node, "use_alpha", &ropt->use_alpha);
  xml_unpack_boolean (node, "do_partial_render", &ropt->do_partial_render);
//xml_unpack_boolean (node, "partial_render_as_percentage", &ropt->partial_render_as_percentage);
  xml_unpack_boolean (node, "aa", &ropt->do_aa);
  xml_unpack_boolean (node, "custom_size", &ropt->do_custom_size);
  xml_unpack_boolean (node, "jitter", &ropt->do_jitter);
  xml_unpack_string (node, "image_size_string", &ropt->image_size_string);
  xml_unpack_string (node, "povray_filename", &ropt->povray_filename);
  xml_unpack_int (node, "filetype", &ropt->filetype);
  xml_unpack_int (node, "bits_per_colour", &ropt->bits_per_colour);
  xml_unpack_int (node, "image_width", &ropt->image_width);
  xml_unpack_int (node, "image_height", &ropt->image_height);
  xml_unpack_int (node, "render_quality", &ropt->render_quality);
  xml_unpack_int (node, "aa_type", &ropt->aa_type);
  xml_unpack_float (node, "start_column", &ropt->start_column);
  xml_unpack_float (node, "end_column", &ropt->end_column);
  xml_unpack_float (node, "start_row", &ropt->start_row);
  xml_unpack_float (node, "end_row", &ropt->end_row);
  xml_unpack_float (node, "aa_threshold", &ropt->aa_threshold);
  xml_unpack_float (node, "aa_depth", &ropt->aa_depth);
  xml_unpack_float (node, "jitter_amount", &ropt->jitter_amount);

/*
 * This part added for Trace Objects by Raymond Ostertag
 */
  xml_unpack_string (node, "trace_object_file", &ropt->trace_object_file); 
  if (ropt->trace_object_file)
  {
    xml_unpack_float (node, "trace_object_x", &ropt->trace_object_x);
    xml_unpack_float (node, "trace_object_y", &ropt->trace_object_y);
    xml_unpack_float (node, "trace_object_z", &ropt->trace_object_z);
    xml_unpack_boolean (node, "do_trace_object_relativ", &ropt->do_trace_object_relativ);
    xml_unpack_boolean (node, "do_trace_object_conserve", &ropt->do_trace_object_conserve);
    xml_unpack_boolean (node, "do_trace_object_float", &ropt->do_trace_object_float);
    xml_unpack_float (node, "trace_object_scale", &ropt->trace_object_scale);
  }
/*
 * End of Trace Objects
 */

}

static gfloat*
heightfield_load_data (xmlNodePtr node, gfloat *heightfield,
                       gint width, gint height)
{
  xmlNodePtr child;
  gint       x, y;

  g_return_val_if_fail (sizeof (gfloat) == sizeof (guint32), NULL);
  g_return_val_if_fail (sizeof (gfloat) == 4, NULL);

  child = node->children;
  for (y = 0; y < height; y++)
    {
      guchar  *data;
      gint     length;
      gchar   *content;

      g_return_val_if_fail (child != NULL, NULL);
      g_return_val_if_fail (strcmp (child->name, "row") == 0, NULL);

      content = xmlNodeGetContent (child);
      g_return_val_if_fail (content != NULL, NULL);

      data = (guchar*) &heightfield[y * width];
      length = base64_decode (content, data, width * 4);
      free (content);
      g_return_val_if_fail (length == width * 4, NULL);

      for (x = 0; x < width; x++)
        data[x] = GUINT32_FROM_LE (data[x]);

      child = child->next;
    }

  return heightfield;
}

static TTerrain *
heightfield_load (xmlNodePtr node)
{
  GtkObject  *terrain_object;
  TTerrain   *terrain;
  gint        width, height;

  g_return_val_if_fail (sizeof (gfloat) == sizeof (guint32), NULL);
  g_return_val_if_fail (sizeof (gfloat) == 4, NULL);

  width = -1; height = -1;
  xml_unpack_prop_int (node, "width", &width);
  xml_unpack_prop_int (node, "height", &height);
  g_return_val_if_fail (width >= 0 && height >= 0, NULL);

  terrain_object = t_terrain_new (width, height);
  terrain = T_TERRAIN (terrain_object);

  heightfield_load_data (node, terrain->heightfield, width, height);

  return terrain;
}

static gfloat*
selection_load (TTerrain *terrain, xmlNodePtr node)
{
  gint        width, height;

  g_return_val_if_fail (sizeof (gfloat) == sizeof (guint32), NULL);
  g_return_val_if_fail (sizeof (gfloat) == 4, NULL);

  width = -1; height = -1;
  xml_unpack_prop_int (node, "width", &width);
  xml_unpack_prop_int (node, "height", &height);
  g_return_val_if_fail (width >= 0 && height >= 0, NULL);

  if (terrain->selection == NULL)
    terrain->selection = g_new (gfloat, width*height);

  heightfield_load_data (node, terrain->selection, width, height);

  return terrain->selection;
}

static void
objects_load (TTerrain   *terrain,
              xmlNodePtr  node)
{
  xmlNode *child;

  child = node->children;
  while (child != NULL)
    {
      TTerrainObject  object;

      object.name = g_strdup (xmlGetProp (child, "name"));
      xml_unpack_prop_int (child, "ox", &object.ox);
      xml_unpack_prop_int (child, "oy", &object.oy);
      xml_unpack_prop_float (child, "x", &object.x);
      xml_unpack_prop_float (child, "y", &object.y);
      xml_unpack_prop_float (child, "angle", &object.angle);
      xml_unpack_prop_float (child, "scale_x", &object.scale_x);
      xml_unpack_prop_float (child, "scale_y", &object.scale_y);
      xml_unpack_prop_float (child, "scale_z", &object.scale_z);

      t_terrain_add_object (terrain, object.ox, object.oy, 
                            object.x, object.y, object.angle,
                            object.scale_x, object.scale_y, object.scale_z,
                            object.name);

      g_free (object.name);
      child = child->next;
    }
}

static gfloat*
riverfield_load (TTerrain *terrain, xmlNodePtr node)
{
  gint        width, height;

  g_return_val_if_fail (sizeof (gfloat) == sizeof (guint32), NULL);
  g_return_val_if_fail (sizeof (gfloat) == 4, NULL);

  width = -1; height = -1;
  xml_unpack_prop_int (node, "width", &width);
  xml_unpack_prop_int (node, "height", &height);
  g_return_val_if_fail (width == terrain->width && height == terrain->height, NULL);

  if (terrain->riverfield == NULL)
    terrain->riverfield = g_new (gfloat, width*height);

  heightfield_load_data (node, terrain->riverfield, width, height);

  return terrain->riverfield;
}

static void
riversources_load (TTerrain   *terrain,
		   xmlNodePtr  node)
{
  xmlNode *child;

  child = node->children;
  while (child != NULL)
    {
      TTerrainRiverCoords coords;

      //object.name = g_strdup (xmlGetProp (child, "name"));
      xml_unpack_prop_float (child, "center_x", &coords.x);
      xml_unpack_prop_float (child, "center_y", &coords.y);

      g_array_append_val (terrain->riversources, coords);

      child = child->next;
    }
}

static TTerrain *
t_terrain_load_native (const gchar *filename)
{
  xmlDocPtr  doc;
  xmlNodePtr node;
  TTerrain  *terrain;

  doc = xmlParseFile (filename);
  if (doc == NULL)
    return NULL;

  if (strcmp (doc->children->name, "Terrain") != 0)
    {
      xmlFreeDoc (doc);
      return NULL;
    }

  node = xmlFindChild (doc->children, "Heightfield");

  terrain = NULL;
  if (node != NULL)
    terrain = heightfield_load (node);

  if (terrain != NULL)
    {
      node = xmlFindChild (doc->children, "Options");
      if (node != NULL)
        options_load (terrain, node);
      node = xmlFindChild (doc->children, "Objects");
      if (node != NULL)
        objects_load (terrain, node);
      node = xmlFindChild (doc->children, "Selection");
      if (node != NULL)
        selection_load (terrain, node);
      node = xmlFindChild (doc->children, "Riverfield");
      if (node != NULL)
        riverfield_load (terrain, node);
      node = xmlFindChild (doc->children, "Rivers");
      if (node != NULL)
        riversources_load (terrain, node);

      g_free (terrain->filename);
      terrain->filename = g_strdup (filename);
    }

  xmlFreeDoc (doc);

  return terrain;
}

/* ********************************************** */
/* **** Based on the bmp code from the GIMP  **** */
/* ********************************************** */
/* bmp.c                                          */
/* Version 0.44                                   */
/* This is a File input and output filter for     */
/* Gimp. It loads and saves images in windows(TM) */
/* bitmap format.                                 */
/* Some Parts that deal with the interaction with */
/* the Gimp are taken from the GIF plugin by      */
/* Peter Mattis & Spencer Kimball and from the    */
/* PCX plugin by Francisco Bustamante.            */
/*                                                */
/* Alexander.Schulz@stud.uni-karlsruhe.de         */

/* Changes:   28.11.1997 Noninteractive operation */
/*            16.03.1998 Endian-independent!!     */
/*            21.03.1998 Little Bug-fix           */
/*            06.04.1998 Bugfix in Padding        */
/*            11.04.1998 Arch. cleanup (-Wall)    */
/*                       Parses gtkrc             */
/*            14.04.1998 Another Bug in Padding   */
/* ********************************************** */

typedef struct BmpFileHead BmpFileHead;
struct BmpFileHead
{
  guint32 bfSize;         /* 02 */
  guint32 reserved;       /* 06 */
  guint32 bfOffs;         /* 0A */
  guint32 biSize;         /* 0E */
};

typedef struct BmpHead BmpHead;
struct BmpHead
{
  guint32 biWidth;        /* 12 */
  guint32 biHeight;       /* 16 */
  guint16 biPlanes;       /* 1A */
  guint16 biBitCnt;       /* 1C */
  guint32 biCompr;        /* 1E */
  guint32 biSizeIm;       /* 22 */
  guint32 biXPels;        /* 26 */
  guint32 biYPels;        /* 2A */
  guint32 biClrUsed;      /* 2E */
  guint32 biClrImp;       /* 32 */
                          /* 36 */
};

typedef struct BmpOS2Head BmpOS2Head;
struct BmpOS2Head
{
  guint16 bcWidth;        /* 12 */
  guint16 bcHeight;       /* 14 */
  guint16 bcPlanes;       /* 16 */
  guint16 bcBitCnt;       /* 18 */
};

static gint
bmp_read_palette (FILE   *in,
                  guchar  buffer[256][3],
                  gint    count,
                  gint    size)
{
  gint   i;
  guchar rgb[4];

  for (i = 0; i < count; i++)
    {
      if (fread (&rgb, size, 1, in) != 1)
        {
          /* Failure. */
          return -1;
        }

      /* Bitmap save the colors in another order! */
      /* But change only once! */
      if (size == 4)
        {
          buffer[i][0] = rgb[2];
          buffer[i][1] = rgb[1];
          buffer[i][2] = rgb[0];
        }
      else
        {
          /* this one is for old os2 Bitmaps, but it dosn't work well */
          buffer[i][0] = rgb[1];
          buffer[i][1] = rgb[0];
          buffer[i][2] = rgb[2];
        }
    }

  return 0;
}

static TTerrain *
bmp_read_image (FILE   *in,
                gint    width,
                gint    height,
                guchar  palette[256][3],
                gint    color_count,
                gint    bpp,
                gint    compression,
                gint    stride)
{
  gint       i, j, x, y, size;
  guchar    *image;
  guchar     buf[3];
  gint       shift_right, mask;
  GtkObject *terrain_object;
  TTerrain  *terrain;
  guchar    *pos;

  if (bpp == 24)
    {
      image = g_new (guchar, width * height * 3);

      for (y = 0; y < height; y++)
        {
          pos = &image[(height - y - 1) * width * 3];
          for (x = 0; x < width; x++)
            {
              fread (buf, 3, 1, in);
              pos[0] = buf[2];
              pos[1] = buf[1];
              pos[2] = buf[0];
              pos += 3;
            }

          for (x = 0; x < stride - width * 3; x++)
            fgetc (in);
        }
    }
  else
    {
      image = g_new0 (guchar, width * height);
      shift_right = 8 - bpp;
      mask = (1 << bpp) - 1;

      switch (compression)
        {
        case 0: /* Uncompressed. */
          for (y = 0; y < height; y++)
            {
              pos = &image[(height - y - 1) * width];
              for (x = 0; x < width;)
                {
                  guint32 c;

                  c = fgetc (in);
                  for (i = 0; i < 8 / bpp && x < width; i++, x++)
                    {
                      *pos = (c >> shift_right) & mask;
                      pos++;
                      c <<= bpp;
                    }
                }

              for (i = 0; i < (stride - width) / (8 / bpp); i++)
                fgetc (in);
            }
          break;

        default: /* Compressed. */
          for (y = 0; y < height; y++)
            {
              pos = &image[(height - y - 1) * width];
              for (x = 0; x < width;)
                {
                  fread (buf, 2, 1, in);
                  if (ferror (in))
                    {
                      g_free (image);
                      return NULL;
                    }

                  if (buf[0] != 0) /* Count + Color record. */
                    {
                      for (i = 0; i < buf[0] && x < width; i++)
                        {
                          gint32 c;

                          c = buf[1];
                          for (j = 0; j < (8 / bpp) && x < width; j++, x++)
                            {
                              *pos = (c >> shift_right) & mask;
                              pos++;
                              c <<= bpp;
                            }
                        }
                    }
                  else if (buf[1] > 2) /* Uncompressed record. */
                    {
                      for (i = 0; i < buf[1] && x < width; i++)
                        {
                          gint32 c;

                          c = fgetc (in);
                          for (j = 0; j < (8 / bpp) && x < width; j++, x++)
                            {
                              *pos = (c >> shift_right) & mask;
                              pos++;
                              c <<= bpp;
                            }
                        }

                      if (((buf[1] / (8 / bpp)) & 1) != 0)
                        fgetc (in);
                    }

                  /* Row end */
                  if (buf[0] == 0 && buf[1] == 0)
                    y++, x = 0;

                  /* Bitmap end */
                  if (buf[0] == 0 && buf[1] == 1)
                    {
                      y = height;
                      x = width;
                      break;
                    }

                  /* Delta record */
                  if (buf[0] == 0 && buf[1] == 2)
                    {
                      x += fgetc (in);
                      y += fgetc (in);
                    }
                }
            }
        }
    }

  if (ferror (in))
    {
      g_free (image);

      return NULL;
    }

  terrain_object = t_terrain_new (width, height);
  terrain = T_TERRAIN (terrain_object);

  size = width * height;
  if (bpp == 24)
    {
      for (i = 0; i < size; i++)
        terrain->heightfield[i] =
          (image[i * 3 + 0] * 256.0 + image[i * 3 + 1]) / MAX_16_BIT;
    }
  else
    {
      for (i = 0; i < size; i++)
        terrain->heightfield[i] =
          ((gint) palette[image[i]][0] +
                  palette[image[i]][1] +
                  palette[image[i]][2]) / 3.0 / 256.0;
    }

  g_free (image);

  return terrain;
}

static TTerrain *
t_terrain_import_bmp (const gchar *filename)
{
  FILE        *in;
  gchar        buf[5];
  gint         palette_size, stride, maps;
  guchar       palette[256][3];
  BmpFileHead  bmp_file_head;
  BmpHead      bmp_head;
  BmpOS2Head   bmp_os2_head;
  TTerrain    *terrain;

  in = fopen (filename, "rb");
  if (!in)
    return NULL;

  /* Now is it a Bitmap? */
  if (fread (buf, 2, 1, in) != 1 || strncmp (buf, "BM", 2) != 0)
    {
      fclose (in);
      return NULL;
    }

  /* How long is the Header? */
  if (fread (&bmp_file_head, sizeof (bmp_file_head), 1, in) != 1)
    {
      fclose (in);
      return NULL;
    }

  /* bring them to the right byteorder. Not too nice, but it should work */
  bmp_file_head.bfSize = GUINT32_FROM_LE (bmp_file_head.bfSize);
  bmp_file_head.reserved = GUINT32_FROM_LE (bmp_file_head.reserved);
  bmp_file_head.bfOffs = GUINT32_FROM_LE (bmp_file_head.bfOffs);
  bmp_file_head.biSize = GUINT32_FROM_LE (bmp_file_head.biSize);

  /* Is it a Windows (tm) Bitmap or not */
  if (bmp_file_head.biSize != 40)
    {
      /* OS/2 not supported, but load anyway. */
      if (fread (&bmp_os2_head, sizeof (bmp_os2_head), 1, in) != 1)
        {
          fclose (in);
          return NULL;
        }

      bmp_os2_head.bcWidth = GUINT16_FROM_LE (bmp_os2_head.bcWidth);
      bmp_os2_head.bcHeight = GUINT16_FROM_LE (bmp_os2_head.bcHeight);
      bmp_os2_head.bcPlanes = GUINT16_FROM_LE (bmp_os2_head.bcPlanes);
      bmp_os2_head.bcBitCnt = GUINT16_FROM_LE (bmp_os2_head.bcBitCnt);
      bmp_file_head.bfSize = (bmp_file_head.bfSize * 4) -
                              bmp_file_head.bfOffs * 3;

      bmp_head.biHeight = bmp_os2_head.bcHeight;
      bmp_head.biWidth = bmp_os2_head.bcWidth;
      bmp_head.biClrUsed = 0;
      bmp_head.biCompr = 0;
      maps = 3;
    }
  else
    {
      if (fread (&bmp_head, sizeof (bmp_head), 1, in) != 1)
        {
          fclose (in);
          return NULL;
        }

      bmp_head.biWidth = GUINT32_FROM_LE (bmp_head.biWidth);
      bmp_head.biHeight = GUINT32_FROM_LE (bmp_head.biHeight);
      bmp_head.biPlanes = GUINT16_FROM_LE (bmp_head.biPlanes);
      bmp_head.biBitCnt = GUINT16_FROM_LE (bmp_head.biBitCnt);
      bmp_head.biCompr = GUINT32_FROM_LE (bmp_head.biCompr);
      bmp_head.biSizeIm = GUINT32_FROM_LE (bmp_head.biSizeIm);
      bmp_head.biXPels = GUINT32_FROM_LE (bmp_head.biXPels);
      bmp_head.biYPels = GUINT32_FROM_LE (bmp_head.biYPels);
      bmp_head.biClrUsed = GUINT32_FROM_LE (bmp_head.biClrUsed);
      bmp_head.biClrImp = GUINT32_FROM_LE (bmp_head.biClrImp);

      maps = 4;
    }

  /* This means wrong file Format. I test this because it could crash */
  if (bmp_head.biBitCnt > 24)
    {
      fclose (in);
      return NULL;
    }

  /* There should be some colors used! */
  palette_size = (bmp_file_head.bfOffs - bmp_file_head.biSize - 14) / maps;
  if (bmp_head.biClrUsed == 0 && bmp_head.biBitCnt < 24)
    bmp_head.biClrUsed = palette_size;

  if (bmp_head.biBitCnt == 24)
    stride = (bmp_file_head.bfSize -
               bmp_file_head.bfOffs) / bmp_head.biHeight;
  else
    stride = (bmp_file_head.bfSize -
               bmp_file_head.bfOffs) / bmp_head.biHeight *
              (8 / bmp_head.biBitCnt);

  /* Get the Colormap */
  if (bmp_read_palette (in, palette, palette_size, maps) != 0)
    {
      fclose (in);
      return NULL;
    }

  /* Get the Image and return the terrain. */
  terrain = bmp_read_image (in, bmp_head.biWidth, bmp_head.biHeight,
                            palette,
                            bmp_head.biClrUsed, bmp_head.biBitCnt,
                            bmp_head.biCompr, stride);

  fclose (in);

  return terrain;
}

static TTerrain *
t_terrain_import_gtopo (const gchar *filename,
                        gint         scale)
{
  GtkObject *terrain_object;
  TTerrain  *terrain;
  gint       i, x, y, size;
  gint16     word;
  gchar      buf[1024];
  gchar      key[1024];
  gchar      value[1024];
  gchar     *hdr, *dem;
  FILE      *in;
  gint       width, height;
  gboolean   motorola;

  g_return_val_if_fail (scale >= 1, NULL);

  hdr = filename_new_extension (filename, "HDR");
  in = fopen (hdr, "rb");
  g_free (hdr);
  if (!in)
    return NULL;

  width = 0;
  height = 0;
  motorola = FALSE;

  /* Read Header file. */
  fgets (buf, 1024, in);
  while (!feof (in))
    {
      if (sscanf (buf, "%s %s", key, value) == 2)
        {
          if (!strcmp (key, "NROWS"))
            height = strtol (value, NULL, 10);
          else if (!strcmp (key, "NCOLS"))
            width = strtol (value, NULL, 10);
          else if (!strcmp (key, "BYTEORDER"))
            motorola = value[0] == 'M';
        }

      fgets (buf, 1024, in);
    }
  fclose (in);

  dem = filename_new_extension (filename, "DEM");
  in = fopen (dem, "rb");
  g_free (dem);
  if (!in)
    return NULL;

  terrain_object = t_terrain_new (width / scale, height / scale);
  terrain = T_TERRAIN (terrain_object);

  if (scale > 1)
    {
      i = 0;
      for (y = 0; y < height; y += scale)
        {
          for (x = 0; x < width; x += scale)
            {
              fseek (in, (y * width + x) * 2, SEEK_SET);

              fread (&word, sizeof (gint16), 1, in);
              if (motorola)
                word = GINT16_FROM_BE (word);
              else
                word = GINT16_FROM_LE (word);

              if (word == -9999) word = 0;

              terrain->heightfield[i] = word;
              i++;
            }
        }
    }
  else
    {
      size = width * height;
      for (i = 0; i < size; i++)
        {
          fread (&word, sizeof (gint16), 1, in);
          if (motorola)
            word = GINT16_FROM_BE (word);
          else
            word = GINT16_FROM_LE (word);

          if (word == -9999) word = 0;

          terrain->heightfield[i] = word;
        }
    }

#if 0 /* DON'T Perform this step because -9999 is used for the oceans. */
  /* Replace missing points with the average of their neighbors */
  i = 0;
  for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
        {
          /* Missing points have a value of -9999.0 */
          if (terrain->heightfield[i] == -9999.0)
            {
              gfloat value;
              gfloat average;
              gint   count;

              average = 0.0;
              count = 0;

              if (x > 0)
                {
                  value = terrain->heightfield[i - 1];
                  if (value != -9999.0)
                    {
                      average += value;
                      count++;
                    }
                }

              if (x < width - 1)
                {
                  value = terrain->heightfield[i + 1];
                  if (value != -9999.0)
                    {
                      average += value;
                      count++;
                    }
                }

              if (y > 0)
                {
                  value = terrain->heightfield[i - width];
                  if (value != -9999.0)
                    {
                      average += value;
                      count++;
                    }
                }

              if (y < height - 1)
                {
                  value = terrain->heightfield[i + width];
                  if (value != -9999.0)
                    {
                      average += value;
                      count++;
                    }
                }

              if (count != 0)
                terrain->heightfield[i] = average / count;
              else
                terrain->heightfield[i] = 0.0;
            }

          i++;
        }
    }
#endif

  fclose (in);

  t_terrain_normalize (terrain, FALSE);

  return terrain;
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

/* 
 * t_terrain_import_mat:
 *   read in data matrix in Matlab Level 1.0 format 
 *   and put data in header "x" and data array "hf"
 */

static TTerrain *
t_terrain_import_mat (const gchar *filename)
{
  GtkObject *terrain_object;
  TTerrain  *terrain;
  gint       cflag;                  /* complex type flag 0=real, 1=complex */
  gchar      pname[80] = "dat1\x00"; /* data matrix name, null-terminated   */
  Fmatrix    hd;                     /* Level 1.0 MAT-file header           */

  gint       ret, x, y;
  gint       etype;
  gint       mtype, prec, rem, numtype;
  FILE      *in;

  /* --------- read in Matlab-format (Level 1.0) file ------- */
  etype = (NUM_FMT * 1000) + (PREC * 10); /* expected data type flag */

  in = fopen (filename, "rb");
  if (!in)
    return NULL;

  /* read header */
  ret = fread (&hd, sizeof (Fmatrix), (size_t) 1, in);
  if (ret != 1) 
    {
      fclose (in);
      return NULL;
    }

  numtype = (hd.type % 10000) / 1000;
  prec = (hd.type % 100) / 10;
  mtype = (hd.type % 10);
  rem = (hd.type - (1000*numtype + 10*prec + mtype));
  cflag = hd.imagf;

  if (prec != 0 && prec != 1)
    {
      fclose (in);
      return NULL;
    }

  /* read mx name */
  ret = fread (pname, sizeof (char), (size_t) hd.namelen, in);
  if (ret != hd.namelen)
    {
      fclose (in);
      return NULL;
    }

  terrain_object = t_terrain_new (hd.ncols, hd.mrows);
  terrain = T_TERRAIN (terrain_object);

  for (x = 0; x < terrain->width; x++)
    {
      for (y = 0; y < terrain->height; y++)
        {
          double tdbl;
          float  tflt;

          if (prec == 0)
            {
              ret = fread (&tdbl, sizeof (double), 1, in);
              tflt = tdbl;
            }
          else
            ret = fread (&tflt, sizeof (float), 1, in);

          if (ret == 1) 
            terrain->heightfield[y * terrain->width + x] = tflt;
          else
            {
              x = terrain->width;
              y = terrain->height;
            }
        }
    }

  fclose (in);

  return terrain;
}

#define OCT_NAME        "name:"
#define OCT_TYPE        "type:"
#define OCT_HEIGHT      "rows:"
#define OCT_WIDTH       "columns:"

#define NUM_LINES_READ  4
#define META_DATA_CHARACTER '#'

typedef enum OctInputFlags
{
  OCT_None        = 0,
  OCT_Name        = 1,
  OCT_Type        = 2,
  OCT_Rows        = 4,
  OCT_Columns     = 8
} OCT_OIF;

#define ARRAY_SIZE 16384

static TTerrain *
t_terrain_import_oct (const gchar *filename)
{
  GtkObject *terrain_object;
  TTerrain  *terrain;
  gint       i, width, height;
  unsigned   long   points_read;
  long int   file_pos, file_pos_new;
  FILE      *in;
  gint       oct_flag;
  gfloat     junk;
  gchar      in_str[ARRAY_SIZE];
  gchar     *str_ptr;
  gfloat    *hf;

  in = fopen (filename, "rb");
  if (!in)
    return NULL;

  /* Initilize our data */
  width = height = 0;
  points_read = 0;
  file_pos = file_pos_new = 0;             
  oct_flag = OCT_None;         /* received no info about OCT file yet */
  hf = NULL;
  
  while (TRUE)
    {
      file_pos = ftell (in);

      /* Read in a line */
      if (!fgets (in_str, ARRAY_SIZE, in))
        break; /* EOF */

      file_pos_new = ftell (in);

      /* find the first non-whitespace */
      str_ptr = in_str;
      while (isspace (*str_ptr) && *str_ptr)
        str_ptr++;
    
      /* check to see if the file has image meta-data */
      if (*str_ptr == META_DATA_CHARACTER)
        {
          /* This line contains information about the image */
          str_ptr++;
          while (isspace(*str_ptr) && *str_ptr)
            str_ptr++;

          if (!strncmp (str_ptr, OCT_HEIGHT, strlen (OCT_HEIGHT)))
            {
              /* this has information about the number of height */
              str_ptr += strlen (OCT_HEIGHT);
              sscanf (str_ptr, "%d", &height);
              oct_flag |= OCT_Rows;
            }
          else if (!strncmp (str_ptr, OCT_WIDTH, strlen (OCT_WIDTH)))
            {
              /* this has information about the number of width */
              str_ptr += strlen (OCT_WIDTH);
              sscanf (str_ptr, "%d", &width);
              oct_flag |= OCT_Columns;
            }

          continue;
        }

      /* is our first character part of a number? */
      if (isdigit (*str_ptr) || *str_ptr == '+' || *str_ptr == '-')
        {
          /* we're dealing with a number of some sort  */
          /* do we know the dimensions of this image ? */
          if ((oct_flag & OCT_Rows) && (oct_flag & OCT_Columns))
            {
              /* Yep, we have all that we need to start. */
              hf = g_new0 (gfloat, width * height);

              /* assume that the rest of the information in the file is image data */
              /* go back to the start of the line and start reading */
              fseek (in, file_pos, SEEK_SET);
              for (i = points_read; i < width * height; i++)
                {
                  if (fscanf_C (in, "%e", &hf[i]) != 1)
                    {
                      g_free (hf);
                      return NULL;
                    }
                }
              break;
            }
          else if (oct_flag & OCT_Columns)
            {
              /* we only know the number of width, read another row */
              if (!height)
                {
                  /* haven't read in any data yet-- init hf */
                  height = 1;
                  hf = g_new0 (gfloat, width * height);
                }
              else
                {
                  gfloat *h;

                  /* have already read in some image data, */
                  /* reallocate memory for one more row */

                  height++;
                  h = g_renew (gfloat, hf, width * height);
                  if (h == NULL)
                    {
                      g_free (hf);
                      return NULL;
                    }
                }

              /* go back to the start of the line and start reading */
              fseek (in, file_pos, SEEK_SET);

              /* read in the next row */
              for (i = 0; i < width; i++)
                if (fscanf_C (in, "%e", &hf[i + points_read]) != 1)
                  {
                    g_free (hf);
                    return NULL;
                  }

              /* keep track of the number of elements read */
              points_read += width;

              /* done reading this line -- continue parsing the image file */
              /* go to the next line */
              fseek (in, file_pos_new, SEEK_SET);
              continue;
            }
          else if (oct_flag & OCT_Rows)
            {
              /* we know the number of width but not the number of height */
              /* I don't know what type of situation would use this...    */
        
              /* conditions leading to this section of code: */
              /*   o  have not read any image data yet       */
              /*   o  width = 0                              */
              /*   o  points_read = 0                        */
          
              while (sscanf_C (str_ptr, "%e", &junk) == 1)
                {
                  width++;  /* count the number of elements in this row */
                  /* skip to next number in the string */
                  while (!isspace (*(++str_ptr))) ;
                  while (isspace (*(++str_ptr))) ;
                }

              if (width == 0)
                return NULL;

              oct_flag |= OCT_Columns;

              /* Seek back to the line we just read */
              fseek (in, file_pos, SEEK_SET);
              continue;
            }
          else
            break;
        }

      /* If we get here, we got a line we can't understand, so ignore it. */
    } 

  fclose (in);

  if (hf == NULL)
    return NULL;

  terrain_object = t_terrain_new (width, height);
  terrain = T_TERRAIN (terrain_object);

  memcpy (terrain->heightfield, hf, sizeof (gfloat) * width * height);
  g_free (hf);

  return terrain;
}

/*
 * The TGA reading code has been adapted from the plug-ins/common/tga.c
 * file shipped with the GIMP (1.1.17).
 *
 * Release 1.2, 1997-09-24, Gordon Matzigkeit <gord@gnu.ai.mit.edu>:
 *   - Bug fixes and source cleanups.
 */

#define RLE_PACKETSIZE 0x80

struct tga_header
{
  guint8   id_length;
  guint8   color_map_type;

  /* The image type. */
#define TGA_TYPE_MAPPED      1
#define TGA_TYPE_COLOR       2
#define TGA_TYPE_GRAY        3
#define TGA_TYPE_MAPPED_RLE  9
#define TGA_TYPE_COLOR_RLE  10
#define TGA_TYPE_GRAY_RLE   11
  guint8   image_type;

  /*
   * Color Map Specification. We need to separately specify
   * High and Low bytes to avoid endianness and alignment problems.
   */

  guint8   color_map_index_lo;
  guint8   color_map_index_hi;
  guint8   color_map_length_lo;
  guint8   color_map_length_hi;
  guint8   color_map_size;

  /* Image Specification. */
  guint8   x_origin_lo;
  guint8   x_origin_hi;
  guint8   y_origin_lo;
  guint8   y_origin_hi;

  guint8   width_lo;
  guint8   width_hi;
  guint8   height_lo;
  guint8   height_hi;

  guint8 bpp;
  /* Image descriptor.
    3-0: attribute bpp
    4:   left-to-right ordering
    5:   top-to-bottom ordering
    7-6: zero
  */
#define TGA_DESC_ABITS      0x0f
#define TGA_DESC_HORIZONTAL 0x10
#define TGA_DESC_VERTICAL   0x20
  guint8  descriptor;
};


#define TGA_SIGNATURE "TRUEVISION-XFILE"
static struct
{
  guint32 extensionAreaOffset;
  guint32 developerDirectoryOffset;
  gchar   signature[16];
  gchar   dot;
  gchar   null;
} tga_footer;


/*
 *  tga_standard_fread: read in a bufferful of file
 */

static gint
tga_standard_fread (guchar *buf, gint datasize, gint nelems, FILE *in)
{
  return fread (buf, datasize, nelems, in);
}


/*
 *  tga_rle_fread: decode a bufferful of file
 */

static gint
tga_rle_fread (guchar *buf,
               gint    datasize,
               gint    nelems,
               FILE   *in)
{
  guchar *statebuf = NULL;
  gint    statelen = 0;
  gint    laststate = 0;
  gint    j, k;
  gint    buflen, count, bytes;
  guchar *p;

  /* Scale the buffer length. */
  buflen = nelems * datasize;

  j = 0;
  while (j < buflen)
    {
      if (laststate < statelen)
        {
          /* Copy bytes from our previously decoded buffer. */
          bytes = MIN (buflen - j, statelen - laststate);
          memcpy (buf + j, statebuf + laststate, bytes);
          j += bytes;
          laststate += bytes;

          /* If we used up all of our state bytes, then reset them. */
          if (laststate >= statelen)
            {
              laststate = 0;
              statelen = 0;
            }

          /* If we filled the buffer, then exit the loop. */
          if (j >= buflen)
            break;
        }

      /* Decode the next packet. */
      count = fgetc (in);
      if (count == EOF)
        return j / datasize; /* No next packet. */

      /* Scale the byte length to the size of the data. */
      bytes = ((count & ~RLE_PACKETSIZE) + 1) * datasize;

      if (j + bytes <= buflen)
        {
          /* We can copy directly into the image buffer. */
          p = buf + j;
        }
      else
        {
          /* Allocate the state buffer if we haven't already. */
          if (!statebuf)
            statebuf = (guchar *) g_malloc (RLE_PACKETSIZE * datasize);
          p = statebuf;
        }

      if (count & RLE_PACKETSIZE)
        {
          /* Fill the buffer with the next value. */
          if (fread (p, datasize, 1, in) != 1)
            return j / datasize;

          /* Optimized case for single-byte encoded data. */
          if (datasize == 1)
            memset (p + 1, *p, bytes - 1);
          else
            for (k = datasize; k < bytes; k += datasize)
              memcpy (p + k, p, datasize);
        }
      else
        {
          /* Read in the buffer. */
          if (fread (p, bytes, 1, in) != 1)
            return j / datasize;
        }

      /* We may need to copy bytes from the state buffer. */
      if (p == statebuf)
        statelen = bytes;
      else
        j += bytes;
    }

  return nelems;
}


static TTerrain *
tga_read_image (FILE *in, struct tga_header *hdr)
{
  GtkObject *terrain_object;
  TTerrain  *terrain;
  guchar    *data;
  int        width, height, bpp, pbpp;
  int        i, j;
  int        pelbytes=3, tileheight, wbytes, bsize, npels, pels;
  int        rle, badread;

  gint (*myfread)(guchar *, gint, gint, FILE *);

  /*
   * Find out whether the image is horizontally or vertically reversed
   * We like things left-to-right, top-to-bottom.
   */
  gchar horzrev = hdr->descriptor & TGA_DESC_HORIZONTAL;
  gchar vertrev = !(hdr->descriptor & TGA_DESC_VERTICAL);

  /* Byteswap header values. */
  width = hdr->width_lo | (hdr->width_hi << 8);
  height = hdr->height_lo | (hdr->height_hi << 8);
  bpp = hdr->bpp;

  if ((hdr->descriptor & TGA_DESC_ABITS) != 0)
    {
      g_print ("Hello: %i\n", hdr->descriptor);
      printf ("Sorry, Terraform cannot load TGA files with alpha channels.\n");
      return NULL;
    }

  if (hdr->image_type == TGA_TYPE_COLOR ||
      hdr->image_type == TGA_TYPE_COLOR_RLE)
    pbpp = MIN (bpp / 3, 8) * 3;
  else
    pbpp = bpp;

  rle = 0;
  switch (hdr->image_type)
    {
    case TGA_TYPE_MAPPED_RLE:
      rle = 1;

    case TGA_TYPE_MAPPED:
      printf ("Sorry, Terraform cannot load paletted TGA images.\n");
      return NULL;
      break;

    case TGA_TYPE_GRAY_RLE:
      rle = 1;

    case TGA_TYPE_GRAY:
      break;

    case TGA_TYPE_COLOR_RLE:
      rle = 1;

    case TGA_TYPE_COLOR:
      break;

    default:
      printf ("TGA: unrecognized image type %d\n", hdr->image_type);
      return NULL;
  }

  if (hdr->color_map_type != 0)
    {
      printf ("TGA: non-indexed image has invalid color map type %d\n",
              hdr->color_map_type);
      return NULL;
    }

  /* Calculate TGA bytes per pixel. */
  bpp = (pbpp + 7) / 8;

  /* Allocate the data. */
  tileheight = height;
  data = (guchar *) g_malloc (width * tileheight * pelbytes);

  if (rle)
    myfread = tga_rle_fread;
  else
    myfread = tga_standard_fread;

  wbytes = width * pelbytes;
  badread = 0;
  for (i = 0; i < height; i += tileheight)
    {
      tileheight = MIN (tileheight, height - i);

      npels = width * tileheight;
      bsize = wbytes * tileheight;

      /* Suck in the data one tileheight at a time */
      if (badread)
        pels = 0;
      else
        pels = (*myfread) (data, bpp, npels, in);

      if (pels != npels)
        {
          if (!badread)
            {
              /* Probably premature end of file. */
              printf ("TGA: error reading (ftell == %ld)\n", ftell (in));
              badread = 1;
            }

          /* Fill the rest of this tile with zeros. */
          memset (data + (pels * bpp), 0, ((npels - pels) * bpp));
        }

      if (pelbytes >= 3)
        {
          /* Rearrange the colors from BGR to RGB. */
          for (j = 0; j < bsize; j += pelbytes)
            {
              gint tmp;

              tmp = data[j];
              data[j] = data[j + 2];
              data[j + 2] = tmp;
            }
        }
    }

  if (fgetc (in) != EOF)
    printf ("TGA: too much input data, ignoring extra...\n");

  /* Allocate new HF */
  terrain_object = t_terrain_new (width, height);
  terrain = T_TERRAIN (terrain_object);

  /* Convert tga data to HF data. */
  if (bpp == 3)
    {
      gint pos, size;

      pos = 0;
      size = width * height;
      for (i = 0; i < size; i++)
        {
          short red;
          short green;
          short blue;

          red = data[pos + 0];
          green = data[pos + 1];
          blue = data[pos + 1];

          terrain->heightfield[i] = (red * 256.0 + green) / MAX_16_BIT;
          pos += 3;
        }
    }
  else if (bpp == 1)
    {
      gint i, size;

      size = width * height;
      for (i = 0; i < size; i++)
        terrain->heightfield[i] = data[i] / 256.0;
    }
  else
    g_assert_not_reached ();

  if (vertrev && horzrev)
    t_terrain_rotate (terrain, 1);
  else if (vertrev)
    t_terrain_mirror (terrain, 1);
  else if (horzrev)
    t_terrain_mirror (terrain, 0);

  g_free (data);

  return terrain;
}


static TTerrain *
t_terrain_import_tga (const gchar *filename)
{
  struct tga_header hdr;
  FILE     *in;
  TTerrain *terrain;

  in = fopen (filename, "rb");
  if (!in)
    return NULL;

  /* Check the footer */
  if (fseek (in, 0L - (sizeof (tga_footer)), SEEK_END)
      || fread (&tga_footer, sizeof (tga_footer), 1, in) != 1)
    {
      /* Couldn't read footer. */
      fclose (in);
      return NULL;
    }

  /* Load the header */
  if (fseek (in, 0, SEEK_SET) || fread (&hdr, sizeof (hdr), 1, in) != 1)
    {
      fclose (in);
      return NULL;
    }

  /* Skip the image ID field. */
  if (hdr.id_length && fseek (in, hdr.id_length, SEEK_CUR))
    {
      fclose (in);
      return NULL;
    }

  terrain = tga_read_image (in, &hdr);

  fclose (in);

  return terrain;
}

static TTerrain *
t_terrain_import_pgm (const gchar *filename)
{
  GtkObject *terrain_object;
  TTerrain  *terrain = NULL;
  FILE      *in;
  gint32     npoints = 0;     /* # points that should be in file */
  gint32     points_read = 0; /* # points read from hf datafile  */
  char       sbuf[512];       /* buffer for file input           */
  gint       val;
  gint       fbin;            /* PGM binary format flag */
  guint8     oneb;            /* one byte from file     */
  gint       width, height;

  in = fopen (filename, "rb");
  if (!in)
    return NULL;

  /* *** check read *** */
  if (fread (sbuf, sizeof (char), 2, in) != 2)
    return NULL;
  else
    sbuf[2] = 0x00;

  fbin = 0;                         /* assume PGM ascii */
  if (strcmp (sbuf, "P5") == 0) 
    fbin = 1;                       /* PGM binary format */

  if (strcmp (sbuf, "P5") == 0 || strcmp (sbuf, "P2") == 0) 
    { 
      /* *** read PGM ASCII or Binary file *** */

      /* *** read header *** */
      while (fscanf (in, "%s", sbuf) != EOF && sbuf[0] == '#') 
        {
          fscanf (in, "%[^\n]", sbuf); /* read comment beyond '#' */
          fscanf (in, "%[\n]", sbuf);  /* read newline            */
        }

      width = atoi (sbuf);           /* store xsize of array */
      fscanf (in, "%s", sbuf);       /* read ysize of array  */
      height = atoi (sbuf);          /* store ysize of array */
      fscanf (in, "%s\n", sbuf);     /* read maxval of array */
      terrain_object = t_terrain_new (width, height);
      terrain = T_TERRAIN (terrain_object);

      /* # of points to read in */
      npoints = width * height;

      if (fbin) 
        {
          /* *** read PGM binary *** */
          while (fread (&oneb, sizeof (oneb), 1, in) == 1 &&
                 points_read < npoints)
            {
              terrain->heightfield[points_read++] = oneb / 255.0;
            }
        }
      else 
        {
          /* *** read PGM ascii *** */
          while (fscanf (in, "%s", sbuf) != EOF && points_read < npoints) 
            {
              val = strtod (sbuf, NULL);
              terrain->heightfield[points_read++] = val;
            }
        } 
    }

  fclose (in);

  t_terrain_normalize (terrain, FALSE);
  return terrain;
}

static TTerrain *
t_terrain_import_ppm (const gchar *filename)
{
  GtkObject *terrain_object;
  TTerrain  *terrain = NULL;
  FILE      *in;
  gint32     npoints = 0;     /* # points that should be in file */
  gint32     points_read = 0; /* # points read from hf datafile  */
  char       sbuf[512];       /* buffer for file input           */
  gboolean   fbin = FALSE;
  gint       width, height;
  gint       maxval;

  in = fopen (filename, "rb");
  if (!in)
    return NULL;

  /* *** check read *** */
  if (fread (sbuf, sizeof (char), 2, in) != 2)
    return NULL;
  else
    sbuf[2] = '\0';

  if (strcmp (sbuf, "P6") == 0)     /* verify file type */
    fbin = TRUE;
  else
  if (strcmp (sbuf, "P3") == 0)     /* verify file type */
    fbin = FALSE;
  else
    {
      fclose (in);
      return NULL;
    }

  /* *** read header *** */
  while (fscanf (in, "%s", sbuf) != EOF && sbuf[0] == '#') 
    {
      fscanf (in, "%[^\n]", sbuf); /* read comment beyond '#' */
      fscanf (in, "%[\n]", sbuf);  /* read newline            */
    }

  width = atoi (sbuf);           /* store xsize of array */
  fscanf (in, "%s", sbuf);       /* read ysize of array  */
  height = atoi (sbuf);          /* store ysize of array */
  fscanf (in, "%s\n", sbuf);     /* read maxval of array */
  maxval = atoi (sbuf);          /* store maxval */
  terrain_object = t_terrain_new (width, height);
  terrain = T_TERRAIN (terrain_object);

  /* # of points to read in */
  npoints = width * height;

  if (fbin)
    {
      guchar bytes[4];

      while (fread (bytes, sizeof(guchar)*3, 1, in) == 1 && 
             points_read < npoints) 
        {
          bytes[4] = 0x0;
          terrain->heightfield[points_read++] = bytes[0] + 
                                    bytes[1]*MAX_2ND_BYTE/MAX_8_BIT + 
                                    bytes[2]*MAX_3RD_BYTE/MAX_16_BIT;
        }
    }
  else
    {
      while (fscanf (in, "%s", sbuf) != EOF && points_read < npoints) 
        {
          gint v1, v2, v3; 

          v1 = strtod (sbuf, NULL);
          fscanf (in, "%s", sbuf);
          v2 = strtod (sbuf, NULL);
          fscanf (in, "%s", sbuf);
          v3 = strtod (sbuf, NULL);

          terrain->heightfield[points_read++] = v1/maxval*MAX_1ST_BYTE + 
                                       v2/maxval*MAX_2ND_BYTE+MAX_8_BIT + 
                                       v3/maxval*MAX_3RD_BYTE+MAX_16_BIT;
        }
    }

  fclose (in);

  t_terrain_normalize (terrain, FALSE);
  return terrain;
}

static TTerrain *
t_terrain_import_gdk_pixbuf (const gchar *filename)
{
  TTerrain        *terrain = NULL;

#ifdef HAVE_GDKPIXBUF
  GtkObject       *terrain_object;
  GdkPixbuf       *pixbuf;
  guint8          *pels;
  gint             width, height;
  gint             x, y;
  gint             hf_index;
  gint             pb_index;
  gint             rowstride;
  gint             bpp;
  gint             value;
  gint             channels;

  pixbuf = gdk_pixbuf_new_from_file (filename);
  if (pixbuf == NULL)
    return NULL;

  gdk_pixbuf_ref (pixbuf);

  width = gdk_pixbuf_get_width (pixbuf);
  height = gdk_pixbuf_get_height (pixbuf);
  pels = gdk_pixbuf_get_pixels (pixbuf);
  rowstride = gdk_pixbuf_get_rowstride (pixbuf);
  bpp = gdk_pixbuf_get_bits_per_sample (pixbuf);
  channels = gdk_pixbuf_get_n_channels (pixbuf);

  if (bpp == 8)
    {
      if (channels == 3)
        {
          terrain_object = t_terrain_new (width, height);
          terrain = T_TERRAIN (terrain_object);

          hf_index = 0;
          for (y = 0; y < height; y++)
            {
              pb_index = y * rowstride;

              for (x = 0; x < width; x++)
                {
                  value = pels[pb_index] + pels[pb_index + 1] + pels[pb_index + 2];
                  terrain->heightfield[hf_index] = value / (3.0 * 255.0);

                  hf_index++;
                  pb_index += 3;
                }
            }
        }
      else if (channels == 1)
        {
          terrain_object = t_terrain_new (width, height);
          terrain = T_TERRAIN (terrain_object);

          hf_index = 0;
          for (y = 0; y < height; y++)
            {
              pb_index = y * rowstride;

              for (x = 0; x < width; x++)
                {
                  value = pels[pb_index];
                  terrain->heightfield[hf_index] = value / 255.0;
                  pb_index++;
                  hf_index++;
                }
            }
        }
    }

  gdk_pixbuf_unref (pixbuf);
#endif

  return terrain;
}




/* 
 * This code is based on the bmp2ter utililty, originally written by 
 * Alistair Milne. I also found a description of the file format at 
 *   http://www.planetside.co.uk/terragen/dev/tgterrain.html
 * which made it possible to implement the complete spec. 
 */

/* required header fields */
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
};

static TTerrain *
t_terrain_import_terragen (const gchar *filename)
{
  GtkObject      *terrain_object;
  TTerrain       *terrain = NULL;

  FILE           *in;
  gint            height;
  gint            width;
  gint            size;
  gint16 	  word;
  gint            i;

  /* data fields to read; mostly unused */
  gchar           marker[4];
  gfloat          scale[3];
  gfloat          radius;
  gboolean        found_data;
  gint32          curvature_mode;
  gint16          heightscale=-1;
  gint16          baseheight=-1;
  terragen_header theader;

  in = fopen (filename, "rb");
  if (!in)
    return NULL;

  /* *** check read *** */
  if (fread (&theader, sizeof (terragen_header), 1, in) != 1)
    {
    fclose (in);
    return NULL;
    }

  width = theader.xpts;
  height = theader.ypts;
  size = theader.size;

  found_data = FALSE;
  while (!found_data)
    {
    if (fread (marker, 4, 1, in) != 1)
      {
      fclose (in);
      return NULL;
      }

    if (!strncmp (marker, "SCAL", 4))
      fread (&scale, sizeof(gfloat), 3, in);  /* x, y, z */
    else
    if (!strncmp (marker, "CRAD", 4))
      fread (&radius, sizeof(gfloat), 1, in); 
    else
    if (!strncmp (marker, "CRVM", 4))
      fread (&curvature_mode, sizeof(gfloat), 1, in); 
    else
    if (!strncmp (marker, "ALTW", 4))
      {
      fread (&heightscale, sizeof(gint16), 1, in); 
      fread (&baseheight, sizeof(gint16), 1, in); 
      found_data = TRUE;
      }

    }

  /*
  printf ("height = %d\n", height);
  printf ("width = %d\n", width);
  printf ("size = %d\n", size);
  printf ("scale = %f, %f, %f\n", scale[0], scale[1], scale[2]);
  printf ("heightscale = %d\n", heightscale);
  printf ("baseheight = %d\n", baseheight);
  */
 
  terrain_object = t_terrain_new (width, height);
  terrain = T_TERRAIN (terrain_object);

  size = height*width;
  for (i=0; i<size; i++)
    {
    fread (&word, sizeof (word), 1, in);
    terrain->heightfield[i] = (gfloat) word;
    }

  t_terrain_normalize (terrain, TRUE);

  /* terragen has a different coordinate system. Mirroring the HF 
   * creates a display identical to the terragen terrain window */
  t_terrain_mirror (terrain, 1);

  fclose (in);
  return terrain;
}

/* gridded terrain format as desribed @ 
 * http://element.ess.ucla.edu/guide/grd_format.htm
 */
static TTerrain *
t_terrain_import_grd (const gchar *filename)
{
  GtkObject      *terrain_object;
  TTerrain       *terrain = NULL;

  FILE           *in;
  gfloat          x_off, x_inc, x_end;
  gfloat          y_off, y_inc, y_end;
  gfloat          val;
  gint            height;
  gint            width;
  gint            size;
  gint            i;

  in = fopen (filename, "rb");
  if (!in)
    return NULL;

  /* *** check read *** */
  fscanf (in, "%f", &x_off);
  fscanf (in, "%f", &x_inc);
  fscanf (in, "%f", &x_end);
  fscanf (in, "%f", &y_off);
  fscanf (in, "%f", &y_inc);
  fscanf (in, "%f", &y_end);

  width = (x_end - x_off)/x_inc;
  height = (y_end - y_off)/y_inc;

  /*
  printf ("height = %d\n", height);
  printf ("width = %d\n", width);
  */
 
  terrain_object = t_terrain_new (width, height);
  terrain = T_TERRAIN (terrain_object);

  size = height*width;
  for (i=0; i<size; i++)
    {
    fscanf (in, "%f", &val);
    terrain->heightfield[i] = val;
    }

  t_terrain_normalize (terrain, TRUE);

  /* terragen has a different coordinate system. Mirroring the HF 
   * creates a display identical to the terragen terrain window */

  fclose (in);
  return terrain;
}

/*
 * xyz: xpos, ypos, elevation
 */
static TTerrain *
t_terrain_import_xyz (const gchar *filename)
{
  GtkObject      *terrain_object;
  TTerrain       *terrain = NULL;

  FILE           *in;
  GArray         *lines = NULL;
  gfloat         *data = NULL;
  gfloat          elv;
  gfloat          ystart;
  gfloat          xval;
  gfloat          yval;
  gint            height=0;
  gint            width=0;
  gint            i;

  in = fopen (filename, "rb");
  if (!in)
    return NULL;

  fscanf (in, "%f", &ystart); /* don't need xstart */
  fscanf (in, "%f", &ystart);
  fscanf (in, "%f", &elv);

  /* measure first row */
  do 
    {
    fscanf (in, "%f", &xval);
    fscanf (in, "%f", &yval);
    fscanf (in, "%f", &elv);
    width++;
    }
  while (yval == ystart);
  fseek(in, 0L, SEEK_SET);

  /* read all rows from start */
  lines = g_array_new (FALSE, TRUE, sizeof(gfloat*));
  while (!feof(in))
    {
    gint    off = 0;
    gfloat *row = g_new0 (gfloat, width);

    for (i=0; i<width && !feof(in); i++)
      {
      fscanf (in, "%f", &xval);
      fscanf (in, "%f", &yval);
      fscanf (in, "%f", &elv);
      row[off++] = elv;
      }

    g_array_append_val (lines, row);
    height++;
    }

  terrain_object = t_terrain_new (width, height);
  terrain = T_TERRAIN (terrain_object);

  /* rebuild height field row by row */
  for (i=0; i<height; i++)
    {
    gfloat *row = g_array_index (lines, gfloat*, i);
    memcpy (terrain->heightfield+(i*width), row, sizeof(gfloat)*width);
    }

  t_terrain_normalize (terrain, TRUE);
  g_free (data);

  fclose (in);
  return terrain;
}

/*
 * dxf: Autocad V14 file; parser a subset of the file spec (point array)
 * see: http://www.programmersheaven.com/zone10/cat454/15090.htm
 * This code reads all points from the following file format until eof. 
 *
 * 999
 * <some string>
 * 0
 * SECTION
 * 2
 * ENTITIES
 * 0
 * POINT
 * ...
 * ENDSEC
 * 0
 * EOF
 *
 * DFX constructs other than a point array as above will not work 
 * correctly; you need to dump the terrain to a separate file. 
 *
 *
 */
static TTerrain *
t_terrain_import_dxf (const gchar *filename)
{
  GtkObject      *terrain_object;
  TTerrain       *terrain = NULL;

  FILE           *in;
  GArray         *lines = NULL;
  gchar           buf[80];
  gfloat         *data = NULL;
  gfloat          elv;
  gfloat          ystart=-1;
  gfloat          xval;
  gfloat          yval;
  gint            height=0;
  gint            width=0;
  gint            oldoff=0;
  gint            i;
  gboolean        done = FALSE; 

  in = fopen (filename, "rb");
  if (!in)
    return NULL;

  /* read up to the first point */
  do 
    {
    oldoff = ftell (in);
    fscanf (in, "%s", buf); 
    }
  while (strcmp(buf, "POINT"));
  fseek(in, oldoff-2, SEEK_SET);  /* reset pos to number before "POINT" */

  /* read the first row */
  do 
    {
    fscanf (in, "%s", buf);
    fscanf (in, "%s", buf);
    fscanf (in, "%s", buf);
    fscanf (in, "%s", buf);
    fscanf (in, "%s", buf);
    fscanf (in, "%f", &xval);
    fscanf (in, "%s", buf);
    fscanf (in, "%f", &yval);
    fscanf (in, "%s", buf);
    fscanf (in, "%f", &elv);

    if (ystart <= -1)
      ystart = yval;

    width++;
    }
  while (yval == ystart);
  width--;                        /* we overcount 1 in the do .. while loop */
  fseek(in, oldoff-2, SEEK_SET);  /* reset pos to number before "POINT" */

  //printf ("WIDTH=%d\n", width); fflush(stdout);

  /* read all rows from start */
  lines = g_array_new (FALSE, TRUE, sizeof(gfloat*));
  while (!feof(in) && !done)
    {
    gint    off = 0;
    gfloat *row = g_new0 (gfloat, width);

    for (i=0; i<width && !feof(in); i++)
      {
      fscanf (in, "%s", buf);
      fscanf (in, "%s", buf);
      if (!strcmp (buf, "ENDSEC"))
        {
        done = TRUE;
	break;
        }
      fscanf (in, "%s", buf);
      fscanf (in, "%s", buf);
      fscanf (in, "%s", buf);
      fscanf (in, "%f", &xval);
      fscanf (in, "%s", buf);
      fscanf (in, "%f", &yval);
      fscanf (in, "%s", buf);
      fscanf (in, "%f", &elv);
      row[off++] = elv;
      }

    g_array_append_val (lines, row);
    height++;
    }

  terrain_object = t_terrain_new (width, height);
  terrain = T_TERRAIN (terrain_object);

  /* rebuild height field row by row */
  for (i=0; i<height; i++)
    {
    gfloat *row = g_array_index (lines, gfloat*, i);
    memcpy (terrain->heightfield+(i*width), row, sizeof(gfloat)*width);
    }

  t_terrain_normalize (terrain, TRUE);
  g_free (data);

  fclose (in);
  return terrain;
}

static TTerrain *
t_terrain_import_bna (const gchar *filename)
{
  GtkObject      *terrain_object;
  TTerrain       *terrain = NULL;

  FILE           *in;
  GArray         *lines = NULL;
  gfloat         *data = NULL;
  gfloat          elv;
  gfloat          ystart;
  gfloat          xval;
  gfloat          yval;
  gint            height=0;
  gint            width=0;
  gint            i=3;
  gint            off;
  gchar           buf[80];

  in = fopen (filename, "rb");
  if (!in)
    return NULL;

  fscanf (in, "%s", buf); 
  fscanf (in, "%s", buf);
  fscanf (in, "%s", buf);
  fscanf (in, "%d", &i); 
  if (i != 3)
    {
    fclose (in);
    fprintf (stderr, "Error: dimension read %d instead of 3\n", i);
    return NULL;
    }
  off = ftell (in);

  fscanf (in, "%f", &elv);
  fscanf (in, "%f", &xval);
  fscanf (in, "%f", &ystart);
  /* measure first row */
  do 
    {
    fscanf (in, "%f", &elv);
    fscanf (in, "%f", &xval);
    fscanf (in, "%f", &yval);
    width++;
    }
  while (yval == ystart);

  /* read all rows from start */
  fseek(in, 0L, SEEK_SET);
  fscanf (in, "%s", buf); 
  fscanf (in, "%s", buf);
  fscanf (in, "%s", buf);
  fscanf (in, "%d", &i); 
  lines = g_array_new (FALSE, TRUE, sizeof(gfloat*));
  while (!feof(in))
    {
    gint    off = 0;
    gfloat *row = g_new0 (gfloat, width);

    for (i=0; i<width && !feof(in); i++)
      {
      fscanf (in, "%f", &elv);
      fscanf (in, "%f", &xval);
      fscanf (in, "%f", &yval);
      row[off++] = elv;
      }

    g_array_append_val (lines, row);
    height++;
    }

  terrain_object = t_terrain_new (width, height);
  terrain = T_TERRAIN (terrain_object);

  /* rebuild height field row by row */
  for (i=0; i<height; i++)
    {
    gfloat *row = g_array_index (lines, gfloat*, i);
    memcpy (terrain->heightfield+(i*width), row, sizeof(gfloat)*width);
    }

  t_terrain_normalize (terrain, TRUE);
  g_free (data);

  fclose (in);
  return terrain;
}

/* binary terrain format as desribed @ 
 * http://vterrain.org/Implementation/BT.html
 * written by RNG
 */
static TTerrain *
t_terrain_import_bt (const gchar *filename)
{
  GtkObject      *terrain_object;
  TTerrain       *terrain = NULL;

  FILE           *in;
  gint            height;
  gint            width;
  gint            x, y;
  gint            off;
  gboolean	  floating_point;
  bt_header       btheader;

  in = fopen (filename, "rb");
  if (!in)
    return NULL;

  /* *** check read *** */
  if (fread (&btheader, sizeof (btheader), 1, in) != 1)
    {
    fclose (in);
    return NULL;
    }

  width = btheader.width + MAX_16_BIT*btheader.width_b2;
  height = btheader.height + MAX_16_BIT*btheader.height_b2;
  floating_point = (btheader.floating_point == 0 ? 0 : 1);

  /*
  printf ("size= %d\n", sizeof(btheader));
  printf ("marker = %s\n", btheader.marker);
  printf ("width = %d\n", width);
  printf ("w_b2= %d\n", btheader.width_b2);
  printf ("height = %d\n", height);
  printf ("h_b2= %d\n", btheader.height_b2);
  printf ("datasize = %d\n", btheader.data_size);
  */

  terrain_object = t_terrain_new (width, height);
  terrain = T_TERRAIN (terrain_object);

  off = 0;
  for (y=0; y<height; y++)
    for (x=0; x<width; x++)
    {
    gint16 	  word16;
    gint32 	  word32;
    gfloat 	  fl=0;

    if (btheader.data_size == 2)
      {
      if (btheader.floating_point)
        {
      	fread (&fl, 2, 1, in);
        terrain->heightfield[off] = fl;
        }
      else
        {
      	fread (&word16, sizeof (word16), 1, in);
        terrain->heightfield[off] = word16;
        }
      }
    else
      {
      if (btheader.floating_point)
        {
      	fread (&fl, 4, 1, in);
        terrain->heightfield[off] = fl;
        }
      else
        {
      	fread (&word32, sizeof (word32), 1, in);
        terrain->heightfield[off] = word32;
        }
      }
    
    off++;
    }

  t_terrain_normalize (terrain, TRUE);

  fclose (in);
  return terrain;
}

/*
 *  Adapted from 
 *  hellodtd - DTD file reader by Aaron Gong (jajubear@hotmail.com)
 */

/* DTED level 1 Map, file header format */
typedef struct {
  guchar Pad0[361];     /* padding, unused */
  guchar sLatLines[4];  /* range 1..n (integer string) */
  guchar sLonLines[4];  /* range 1..n (integer string) */
  guchar Pad3[3059];    /* padding, unused */
} dted_header;

const glong DTED_ALT_FRONT_PAD = 8;
const glong DTED_ALT_REAR_PAD = 4;

static TTerrain * t_terrain_import_dted (const gchar *filename)
{
  GtkObject      *terrain_object;
  TTerrain       *terrain = NULL;

  FILE           *in;
  gint            height;
  gint            width;
  gint            x, y;
  const gint      tbufsize = 80;
  gchar           tbuf[tbufsize];

  dted_header     dted_hdr;
  const glong     dted_front_pad = 8;
  const glong     dted_rear_pad = 4;

  in = fopen (filename, "rb");
  if (!in)
    return NULL;

  /* *** check read *** */
  if (fread (&dted_hdr, sizeof(dted_header), 1, in) != 1);
    {
    fclose (in);
    return NULL;
    }

  // Find the number of longitude and latitude lines
  memset (tbuf, 0, tbufsize);
  memcpy (tbuf, dted_hdr.sLatLines, 4);
  width = atoi(tbuf);
  memset (tbuf, 0, tbufsize);
  memcpy (tbuf, dted_hdr.sLatLines, 4);
  height = atoi (tbuf);

  terrain_object = t_terrain_new (width, height);
  terrain = T_TERRAIN (terrain_object);

  // read and covert data 
  fseek (in, sizeof(dted_header), SEEK_SET);
  for (y=0; y<height; y++ )
    {
    fread (tbuf, dted_front_pad, 1, in);

    for (x=0; x<width; x++ )
      {
      gfloat fl=0;

      fread (&fl, sizeof(gint16), 1, in);
      terrain->heightfield[y*width+x] = fl;
      }

    fread (tbuf, dted_rear_pad, 1, in);
    }

  t_terrain_normalize (terrain, TRUE);

  fclose (in);
  return terrain;
}


static TTerrain *
t_terrain_import_e00grid (const gchar *filename)
{
  /* Based on t_terrain_import_grd
     e00 file have a different header
     the rest of the file is the same.
     Maybe parts of this function could be
     merged with t_terrain_import_grd.
     e00 files are created by writing 
     an ascii file from an Arc/Info grid
  */

  GtkObject      *terrain_object;
  TTerrain       *terrain = NULL;

  FILE           *in;
  gfloat          val;
  gint            height, width;
  gint            size;
  gint            i;
  gint            header_int;
  gfloat          header_float;
  char            header_string[13]; /* this should be long enough */

  in = fopen (filename, "rb");
  if (!in)
    return NULL;

  /* header */
  fscanf (in, "%s %d", header_string, &width);
  fscanf (in, "%s %d", header_string, &height);
  fscanf (in, "%s %f", header_string, &header_float); /* xllcorner */
  fscanf (in, "%s %f", header_string, &header_float); /* yllcorner */
  fscanf (in, "%s %d", header_string, &header_int);   /* cellsize */
  fscanf (in, "%s %d", header_string, &header_int);   /* NODATA_value */
 
  terrain_object = t_terrain_new (width, height);
  terrain = T_TERRAIN (terrain_object);

  size = height*width;
  for (i=0; i<size; i++)
    {
    fscanf (in, "%f", &val);
    terrain->heightfield[i] = val;
    }

  t_terrain_normalize (terrain, TRUE);

  /* terragen has a different coordinate system. Mirroring the HF 
   * creates a display identical to the terragen terrain window */

  fclose (in);
  return terrain;
}
