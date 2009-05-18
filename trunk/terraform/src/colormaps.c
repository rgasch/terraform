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
#include "colormaps.h"

static const gint colormap_land_data[15] =
{
  0x191970 /* MidnightBlue */,
  0x0000cd /* MediumBlue */,
  0x0000ff /* Blue */,
  0x1e90ff /* Dodger Blue */,
  0x00ee00 /* Green2 */,
  0x00cd00 /* Green3 */,
  0x00b800 /* Green4 */,
  0x6e8b3d /* DarkOliveGreen4 */,
  0xeedc82 /* LightGoldenrod2 */,
  0xcdbe70 /* LightGoldenrod3 */,
  0x8b814c /* LightGoldenrod4 */,
  0x8b8b7a /* LightYellow4 */,
  0x696969 /* DimGray */,
  0xbebebe /* Gray */,
  0xd3d3d3 /* LightGray */
};
static const gint colormap_land_count = 15;
static const gint colormap_land_boundary = 4;

static const gint colormap_desert_data[6] =
{
  0xcdbe70 /* LightGoldenrod3 */,
  0x8b814c /* LightGoldenrod4 */,
  0x696969 /* DimGray */,
  0x778899 /* LightSlateGray */,
  0xbebebe /* Gray */,
  0xd3d3d3 /* LightGray */
};
static const gint colormap_desert_count = 6;
static const gint colormap_desert_boundary = 1;

static const gint colormap_heat_data[12] =
{
  0xaf00af /* Purple */,
  0xff00ff /* Purple */,
  0xaf00af /* Purple */,
  0x0000ff /* Blue */,
  0x00ffff /* Cyan */,
  0x00afaf /* Cyan */,
  0x00ffff /* Cyan */,
  0x00ff00 /* Green */,
  0xafaf00 /* Yellow */,
  0xffff00 /* Yellow */,
  0xafaf00 /* Yellow */,
  0xff0000 /* Red */
};
static const gint colormap_heat_count = 12;

static const gint colormap_wasteland_data[33] =
  {
    0x006bef, /* Blue */
    0x00a0f0,
    0x00ccf0,
    0x00e0ef,
    0x00d0e0,
    0x00b0b3,
    0x009090,
    0x006f5f,
    0x3b7f60,
    0x5b9070,
    0x709070,
    0x8c9f70,
    0x9ca07f,
    0xa0af7f,
    0xa09f7f,
    0x9f9f6f,
    0x908f70,
    0x8f8f6f, /* Brown */
    0x7f7f63,
    0x73735f,
    0x707061,
    0x6f6f50,
    0x60604f,
    0x5f5f51,
    0x505040,
    0x433f30,
    0x313f1f,
    0x7bd040,
    0xbcd040,
    0xd0df50,
    0xe0e060,
    0xe0e052,
    0xeff0b5  /* Light Yellow */
  };
static const gint colormap_wasteland_count = 33;


static
void colormap_gradient (guchar *colormap,
                        gint    start,
                        gint    stop,
                        gint    r1,
                        gint    g1,
                        gint    b1,
                        gint    r2,
                        gint    g2,
                        gint    b2)
{
  gint i;
  gint delta;
  gfloat g_r1, g_g1, g_b1;
  gfloat g_r2, g_g2, g_b2;

  g_r1 = pow (r1 / 255.0, 1.6);
  g_g1 = pow (g1 / 255.0, 1.6);
  g_b1 = pow (b1 / 255.0, 1.6);
  g_r2 = pow (r2 / 255.0, 1.6);
  g_g2 = pow (g2 / 255.0, 1.6);
  g_b2 = pow (b2 / 255.0, 1.6);

  g_r1 = r1 / 255.0;
  g_g1 = g1 / 255.0;
  g_b1 = b1 / 255.0;
  g_r2 = r2 / 255.0;
  g_g2 = g2 / 255.0;
  g_b2 = b2 / 255.0;

  delta = stop - start;
  for (i = 0; i <= delta; i++)
    {
      gfloat r, g, b;

      r = g_r1 + i * (g_r2 - g_r1) / delta;
      g = g_g1 + i * (g_g2 - g_g1) / delta;
      b = g_b1 + i * (g_b2 - g_b1) / delta;

      r = pow (r, 1.6);
      g = pow (g, 1.6);
      b = pow (b, 1.6);

      colormap[(start + i) * 3 + 0] = (gint) (r * 255.0 + 0.5);
      colormap[(start + i) * 3 + 1] = (gint) (g * 255.0 + 0.5);
      colormap[(start + i) * 3 + 2] = (gint) (b * 255.0 + 0.5);
    }
}

static void
colormap_gradient_bands (guchar       *colormap,
                         gint          index_start,
                         gint          index_stop,
                         gint          num_points,
                         const gint32 *colors)
{
  gint i;
  gint num_divisions;

  num_divisions = num_points - 1;
  for (i = 0; i < num_divisions; i++)
    {
      gint start, stop;

      start = index_start + i * (index_stop - index_start + 1) / num_divisions;
      stop = index_start + (i + 1) *
             (index_stop - index_start + 1) / num_divisions - 1;

      colormap_gradient (colormap, start, stop,
                         (colors[i] >> 16) & 0xff,
                         (colors[i] >> 8) & 0xff,
                         colors[i] & 0xff,
                         (colors[i + 1] >> 16) & 0xff,
                         (colors[i + 1] >> 8) & 0xff,
                         colors[i + 1] & 0xff);
    }
}

static void
colormap_land_bands (guchar       *colormap,
                     gint          num_bands,
                     gfloat        water_level,
                     gint          land_level,
                     const gint32 *colors)
{
  gint middle_index;

  middle_index = (gint) (water_level * 256.0 + 0.5);

  if (middle_index > 0) 
  {
      colormap_gradient_bands (colormap, 0, middle_index - 1, land_level, colors);
  } 
  if (middle_index < 256) {
      colormap_gradient_bands (colormap, middle_index, 255, num_bands - land_level, &colors[land_level]);
  }
}

guchar *
colormap_new (TColormapType which_colormap,
              gfloat        water_level)
{
  guchar   *colormap;

  colormap = g_new (guchar, 256 * 3);
  switch (which_colormap)
    {
    case T_COLORMAP_LAND: 
      colormap_land_bands (colormap, colormap_land_count, water_level,
                           colormap_land_boundary, colormap_land_data);
      break;

    case T_COLORMAP_DESERT: 
      colormap_gradient_bands (colormap, 0, 255, colormap_desert_count,
                               colormap_desert_data);
      break;

    case T_COLORMAP_GRAYSCALE:
      colormap_gradient (colormap, 0, 255, 0, 0, 0, 255, 255, 255);
      break;

    case T_COLORMAP_HEAT:
      colormap_gradient_bands (colormap, 0, 255, colormap_heat_count,
                               colormap_heat_data);
      break;
    case T_COLORMAP_WASTELAND:
      colormap_gradient_bands (colormap, 0, 255, colormap_wasteland_count,
			       colormap_wasteland_data);
      break;

    default: /* T_COLORMAP_GRAYSCALE */
      colormap_gradient (colormap, 0, 255, 0, 0, 0, 255, 255, 255);
      break;
    }

  return colormap;
}
