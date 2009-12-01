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
#include "filenameops.h"

TTerrain *t_terrain_load         (const gchar *filename,
                                  TFileType    type);


/* 
 * Defintiions used by both reader.c and writer.c
 */

#define MAX_8_BIT  0xFF
#define MAX_16_BIT 0xFFFF
#define MAX_24_BIT 0xFFFFFF
#define MAX_32_BIT 0xFFFFFFFF

#define MAX_1ST_BYTE 0xFF
#define MAX_2ND_BYTE 0xFF00
#define MAX_3RD_BYTE 0xFF0000

/* binary terrain format as desribed @ 
 * http://vterrain.org/Implementation/BT.html
 */
typedef struct bt_header bt_header;
struct bt_header
{
  gchar   marker[10];
  gshort  width;
  gshort  width_b2;
  gshort  height;
  gshort  height_b2;
  gshort  data_size;       /* in bytes; either 2 or 4 */
  gshort  floating_point;  /* 0: Integers; 1: floating point; anything else: Integer */
  gshort  projection;      /* 0: Geographic; 1: Universal Transverse Mercator (UTM */
  gshort  utm_zone;        /* 1-60: negative zone numbers for southern hemisphere */
  gshort  datum;           /* datum values from lookup table */
  gdouble left_extent;
  gdouble right_extent;
  gdouble bottom_extent;
  gdouble top_extent;
  gshort  external_projection;
  gchar   pad[194];        /* padding */
};

