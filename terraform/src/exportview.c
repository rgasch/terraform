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


#ifdef HAVE_LIBPNG
#include <png.h>
#endif /* HAVE_LIBPNG */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "tterrainview.h"
#include "tpixbuf.h"
#include "exportview.h"
#include "filenameops.h"
#include "clocale.h"

static gint t_terrain_view_export_png  (TTerrainView *view,
                                        const gchar  *filename);
static gint t_terrain_view_export_vrml (TTerrainView *view,
                                        const gchar  *filename);


gint
t_terrain_view_export_view (TTerrainView *view,
                            TFileType     type,
                            const gchar  *filename)
{
  if (type == FILE_AUTO)
    type = filename_determine_type (filename);

  if (type == FILE_PNG)
    return t_terrain_view_export_png (view, filename);
  else if (type == FILE_VRML)
    return t_terrain_view_export_vrml (view, filename);

  return -1;
}


static gint
t_terrain_view_export_png (TTerrainView *view,
                           const gchar  *filename)
{
#ifdef HAVE_LIBPNG
  TPixbuf     *pixbuf;
  FILE        *out;
  png_structp  png;
  png_infop    info;
  png_color_8  sig_bit;
  gint         y;
  gint         status;

  gint         width = view->terrain->width;
  gint         height = view->terrain->height;

  out = fopen (filename, "wb");
  if (out == NULL)
    return -1;

  pixbuf = t_pixbuf_new ();
  t_pixbuf_set_size (pixbuf, width, height);
  t_terrain_view_export (view, pixbuf);

  png = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png == NULL)
    {
      t_pixbuf_free (pixbuf);
      fclose (out);
      return -1;
    }

  info = png_create_info_struct (png);
  if (info == NULL)
    {
      t_pixbuf_free (pixbuf);
      png_destroy_write_struct (&png, (png_infopp) NULL);
      fclose (out);
      return -1;
    }

  png_init_io (png, out);

  /* Header fields */
  png_set_IHDR (png, info, width, height, 8,
                PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  /* Significant bits */
  sig_bit.red   = 8;
  sig_bit.green = 8;
  sig_bit.blue  = 8;
  png_set_sBIT (png, info, &sig_bit);

  png_write_info (png, info);
  png_set_shift (png, &sig_bit);
  png_set_packing (png);

  for (y = 0; y < height; y++)
    {
      png_bytep row = &pixbuf->data[y * width * 3];

      png_write_rows (png, &row, 1);
    }

  png_write_end (png, info);
  png_destroy_write_struct (&png, (png_infopp) NULL);

  t_pixbuf_free (pixbuf);

  status = ferror (out);
  fclose (out);

  return status;
#else /* HAVE_LIBPNG */
  return -1;
#endif/* HAVE_LIBPNG */
}

/*
 * t_terrain_view_export_vrml:
 *   Write the Height Field to VRML file.
 *
 * Written by Joerg Scheurich <rusmufti@helpdesk.rus.uni-stuttgart.de>
 * Modified by Robert Gasch
 */

static gint
t_terrain_view_export_vrml (TTerrainView *view,
                            const gchar  *filename)
{
  FILE   *out;
  int     x, y;
  int     status;
  guchar *colormap;

  colormap = colormap_new (view->colormap, view->terrain->sealevel);

  out = fopen (filename, "wb");
  if (out == NULL)
    return -1;

  /* *** Write out data to WRL format file *** */
  /* *** VRML Header *** */
  fprintf (out, "#VRML V2.0 utf8\n\n");
  fprintf (out, "Shape \n");
  fprintf (out, "   { \n");
  fprintf (out, "   appearance  Appearance \n");
  fprintf (out, "      { \n");
  fprintf (out, "      material  Material \n");
  fprintf (out, "         { \n");
  fprintf (out, "         ambientIntensity 1 \n");
  fprintf (out, "#         specularColor    1 1 1 \n");
  fprintf (out, "         specularColor    0.295918 0.096084 0.00204082 \n");
  fprintf (out, "#         emissiveColor   0 0 0 \n");
  fprintf (out, "         emissiveColor    0.795918 0.505869 0.164673 \n");
  fprintf (out, "         shininess        1 \n");
  fprintf (out, "         transparency     0 \n");
  fprintf (out, "         } # material \n");
  fprintf (out, "      } # appearance \n");
  fprintf (out, "   geometry ElevationGrid \n");
  fprintf (out, "      { \n");
  fprintf (out, "      xDimension %d \n", view->terrain->width);
  fprintf_C (out, "      xSpacing   %f \n", 1.0 / 10.0);
  fprintf (out, "      zDimension %d \n", view->terrain->height);
  fprintf_C (out, "      zSpacing   %f \n", 1.0 / 10.0);
  fprintf (out, "      height \n");
  fprintf (out, "         [ \n");

  for (y = 0; y < view->terrain->height; y++)
    for (x = 0; x < view->terrain->width; x++)
      fprintf_C (out, "         %f\n",
        view->terrain->heightfield[y * view->terrain->width + x]);

  fprintf (out, "         ] #height \n");
  fprintf (out, "      color Color \n");
  fprintf (out, "         { \n");
  fprintf (out, "         color \n");
  fprintf (out, "            [ \n");

  for (y = 0; y < view->terrain->height; y++)
    {
      gint hf_pos = y * view->terrain->width;

      for (x = 0; x < view->terrain->width; x++)
        {
          gint   off = (gint) (view->terrain->heightfield[hf_pos + x] * 255 * 3);
          gfloat red = (gfloat) colormap[off] / 255.0;
          gfloat green = (gfloat) colormap[++off] / 255.0;
          gfloat blue = (gfloat) colormap[++off] / 255.0;

          fprintf_C (out, "         %f, %f, %f\n", red, green, blue);
        }
    }

  fprintf (out, "            ] #color \n");
  fprintf (out, "         } #color Color \n");

  fprintf (out, "      } #geometry \n");
  fprintf (out, "   } # shape \n");

  status = ferror (out);
  fclose (out);

  g_free (colormap);

  return status;
}
