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

#ifndef __T_MATRIX_H__
#define __T_MATRIX_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef gdouble TVector[3];

void    t_vector_set               (TVector vector,
                                    gdouble x,
                                    gdouble y,
                                    gdouble z);
void    t_vector_copy              (TVector in,
                                    TVector out);
gdouble t_vector_magnitude_squared (TVector vector);
gdouble t_vector_magnitude         (TVector vector);
void    t_vector_normalize         (TVector inout);
gdouble t_vector_dot_product       (TVector in1,
                                    TVector in2);
void    t_vector_cross_product     (TVector in1,
                                    TVector in2,
                                    TVector out);

/*
 * matrices are used to store operations
 * to perform on vectors
 *
 *       out
 *      x y z
 *      | | |
 *   x- m m m m
 * i y- m m m m  m = normal matrix
 * n z- m m m m  t = transpose
 *      t t t x  x = for multiply
 */

typedef gdouble TMatrix[4][4];

void t_matrix_identity  (TMatrix matrix);
void t_matrix_multiply  (TMatrix in1,
                         TMatrix in2,
                         TMatrix out);
void t_matrix_rotate    (TMatrix matrix,
                         gdouble angle,
                         gint    axis);
void t_matrix_translate (TMatrix matrix,
                         gdouble x,
                         gdouble y,
                         gdouble z);
void t_matrix_transform (TMatrix matrix,
                         TVector in,
                         TVector out);
void t_matrix_combine   (TMatrix in1,
                         TMatrix in2,
                         TMatrix out);
void t_matrix_zero      (TMatrix matrix);
void t_matrix_assign    (TMatrix in,
                         TMatrix out);
void t_matrix_scale     (TMatrix matrix,
                         gdouble k);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __T_MATRIX_H__ */
