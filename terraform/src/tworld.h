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
 *
 *  This file copyright (c) 2000-2001 David A. Bartold
 */


#include "tmatrix.h"
#include "tpixbuf.h"
#include "tterrain.h"

#ifndef __T_WORLD_H__
#define __T_WORLD_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _SkyValues SkyValues;

typedef struct _GimpRGB GimpRGB;
struct _GimpRGB
{
  gdouble r;
  gdouble g;
  gdouble b;
  gdouble a;
};

struct _SkyValues
{
  gfloat   gamma;
  gfloat   tilt;
  gfloat   rotation;
  gint     seed;
  gboolean sun_show;
  gfloat   sun_x;
  gfloat   sun_y;
  gfloat   time;
  GimpRGB  horizon_color;
  GimpRGB  sky_color;
  GimpRGB  sun_color;
  GimpRGB  cloud_color;
  GimpRGB  shadow_color;
};

void sky_values_init (SkyValues    *values,
                      gfloat        gamma);

void sky_render      (TTerrain     *terrain,
                      TPixbuf      *pixbuf,
                      SkyValues    *values);

void sky_preview     (TTerrain     *terrain,
                      guint8       *data,
                      guint         width,
                      guint         height,
                      SkyValues    *values);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __T_WORLD_H__ */
