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


/* bit flags for the different generation methods */
#define GEN_TYPE_1         1
#define GEN_TYPE_2         2
#define GEN_TYPE_3         4
#define GEN_TYPE_4         8
#define GEN_TYPE_ALL       15
#define GEN_TYPE_NONE      0
#define GEN_TYPE_BITS_USED 4


TTerrain *t_terrain_generate_random (int size, float factor, int gen_bits);
