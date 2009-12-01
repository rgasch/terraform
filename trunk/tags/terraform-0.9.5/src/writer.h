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

gint     t_terrain_save            (TTerrain    *terrain,
                                    TFileType    type,
                                    const gchar *filename);
gboolean is_saving_as_native       (TFileType    type,
                                    const gchar *filename);
void     t_terrain_get_povray_size (TTerrain *terrain,
                                    gfloat   *x,
                                    gfloat   *y,
                                    gfloat   *z);
