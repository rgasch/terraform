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
 *  This file copyright (c) 2001 David A. Bartold
 */


#include "tmatrix.h"
#include "tpixbuf.h"
#include "tterrain.h"

#ifndef __T_NATIVE3D_H__
#define __T_NATIVE3D_H__

#ifndef G_PI
#define G_PI 3.14159265358979323846264
#endif

#define SQR(x) ((x)*(x))
#define RINT(x) (rint(x))

#define TOSCREEN(x) pow(x, 1.0 / (gamma))
#define FROMSCREEN(x) pow(x, gamma)

#define EPSILON 0.0001

typedef void ObjectFunction (gpointer data, gdouble *uv, gdouble *xyz);
typedef void ProjectionFunction (gpointer data, gdouble *xyz, gdouble *xy);
typedef void TransformFunction (gpointer data, gdouble *xyz1, gdouble *xyz2);
typedef void ShaderFunction (gpointer data, gdouble *normal, gdouble *xyz, gdouble *color);

typedef struct _Region Region;

struct _Region
{
  guint8 *data;
  gfloat *zbuffer;
  gint    bpp;
  gint    rowstride;
  gint    x;
  gint    y;
  gint    w;
  gint    h;
};

typedef struct _NativePaint NativePaint;

struct _NativePaint
{
  gdouble             gamma;
  ProjectionFunction *projection;
  TransformFunction  *object_transform;
  TransformFunction  *shader_transform;
  ShaderFunction     *shader;
  gpointer            projection_data;
  gpointer            object_transform_data;
  gpointer            shader_transform_data;
  gpointer            shader_data;
};

typedef struct _NativeObject NativeObject;

struct _NativeObject
{
  NativePaint     paint;
  ObjectFunction *object;
  gpointer        object_data;
};

void paint_object           (Region       *region,
                             NativeObject *object);
void paint_triangle         (NativePaint  *state,
                             Region       *region,
                             gdouble       triangle[3][3]);
void sphere                 (gpointer      data,
                             gdouble      *uv,
                             gdouble      *xyz);
void plane                  (gpointer      data,
                             gdouble      *uv,
                             gdouble      *xyz);
void isomorphic_projection  (gpointer      data,
                             gdouble      *xyz,
                             gdouble      *xy);
void perspective_projection (gpointer      data,
                             gdouble      *xyz,
                             gdouble      *xy);
void affine_transform       (gpointer      data,
                             gdouble      *xyz1,
                             gdouble      *xyz2);
void solid_shader           (gpointer      data,
                             gdouble      *normal,
                             gdouble      *xyz,
                             gdouble      *color);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __T_NATIVE3D_H__ */
