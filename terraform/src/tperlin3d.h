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


#include "trandom.h"

typedef struct TPerlin3D TPerlin3D;

TPerlin3D *t_perlin3d_new      (gint       count,
                                gfloat     frequency,
                                gfloat     persistence,
                                gint       seed);

TPerlin3D *t_perlin3d_new_full (gint       count,
                                gfloat     frequency,
                                gfloat    *amplitudes,
                                gint       seed);

void       t_perlin3d_free     (TPerlin3D *perlin);

gfloat     t_perlin3d_get      (TPerlin3D *perlin,
                                gfloat   x,
                                gfloat   y,
                                gfloat   z);

