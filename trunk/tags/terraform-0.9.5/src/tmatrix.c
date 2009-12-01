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

#include <assert.h>
#include <math.h>
#include "tmatrix.h"

#ifndef G_PI
#define G_PI 3.14159265358979323846264
#endif

#define DEGRAD(x) ((x) * G_PI / 180.0)

void
t_vector_set (TVector vector,
              gdouble x,
              gdouble y,
              gdouble z)
{
  vector[0] = x;
  vector[1] = y;
  vector[2] = z;
}

void
t_vector_copy (TVector in,
               TVector out)
{
  out[0] = in[0];
  out[1] = in[1];
  out[2] = in[2];
}

gdouble
t_vector_magnitude_squared (TVector vector)
{
  return vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2];
}

gdouble
t_vector_magnitude (TVector vector)
{
  return sqrt (t_vector_magnitude_squared (vector));
}

void
t_vector_normalize (TVector inout)
{
  gdouble mag;

  mag = t_vector_magnitude (inout);
  inout[0] /= mag;
  inout[1] /= mag;
  inout[2] /= mag;
}

gdouble
t_vector_dot_product (TVector in1,
                      TVector in2)
{
  return in1[0] * in2[0] + in1[1] * in2[1] + in1[2] * in2[2];
}

void
t_vector_cross_product (TVector in1,
                        TVector in2,
                        TVector out)
{
  out[0] = in1[1] * in2[2] - in1[2] * in2[1];
  out[1] = in1[2] * in2[0] - in1[0] * in2[2];
  out[2] = in1[0] * in2[1] - in1[1] * in2[0];
}

/* Create an identity matrix. */
void
t_matrix_identity (TMatrix matrix)
{
  t_matrix_scale (matrix, 1.0);
}

/* Multiply two matrices together. */
void
t_matrix_multiply (TMatrix in1,
                   TMatrix in2,
                   TMatrix out)
{
  gint i, j, k;

  for (i = 0; i < 4; i++)
    for(j = 0; j < 4; j++)
      {
        gdouble sum;

        sum = 0.0;
        for (k = 0; k < 4; k++)
          sum += in1[i][k] * in2[k][j];

        out[i][j] = sum;
      }
}

/*
 * Create a rotation matrix.
 *
 * This rotation matrix code is adapted from the Rotate3D function on
 * page 28 of "Advanced Graphics Programming in C and C++" by
 * Roger T. Stevens and Christopher D. Watkins.
 * 
 * Axes:
 *   x is 1
 *   y is 2
 *   z is 3
 */

void
t_matrix_rotate (TMatrix matrix,
                 gdouble angle,
                 gint    axis)
{
  gint   m1, m2;
  gdouble c, s;

  t_matrix_zero (matrix);

  assert (axis >= 1 && axis <= 3);

  matrix[axis - 1][axis - 1] = 1.0;
  matrix[3][3] = 1.0;

  m1 = axis % 3;
  m2 = (m1 + 1) % 3;

  c = cos (DEGRAD (angle));
  s = sin (DEGRAD (angle));

  matrix[m1][m1] = c;
  matrix[m1][m2] = s;
  matrix[m2][m2] = c;
  matrix[m2][m1] = -s;
}

/* Create a translation matrix. */
void
t_matrix_translate (TMatrix matrix,
                    gdouble x,
                    gdouble y,
                    gdouble z)
{
  t_matrix_identity (matrix);

  matrix[0][3] = x;
  matrix[1][3] = y;
  matrix[2][3] = z;
}

/* Transform a vector by a matrix. */
void
t_matrix_transform (TMatrix matrix,
                    TVector in,
                    TVector out)
{
  out[0] = matrix[0][0] * in[0] +
           matrix[0][1] * in[1] +
           matrix[0][2] * in[2] +
           matrix[0][3];

  out[1] = matrix[1][0] * in[0] +
           matrix[1][1] * in[1] +
           matrix[1][2] * in[2] +
           matrix[1][3];

  out[2] = matrix[2][0] * in[0] +
           matrix[2][1] * in[1] +
           matrix[2][2] * in[2] +
           matrix[2][3];
}

/* Mathematically combine two matrices.
 * in1 is done before in2.
 */
void
t_matrix_combine (TMatrix in1,
                  TMatrix in2,
                  TMatrix out)
{
  gint i, j;

  t_matrix_zero (out);
  t_matrix_multiply (in2, in1, out);
  for (i = 0; i < 3; i++)
    {
      out[i][3] = in2[i][3];

      for (j = 0; j < 3; j++)
        out[i][3] += in1[j][3] * in2[i][j];
    }
}

/* Zero a matrix. */
void
t_matrix_zero (TMatrix matrix)
{
  gint i, j;

  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++)
      matrix[i][j] = 0.0;
}

/* Copy a matrix. */
void
t_matrix_assign (TMatrix in,
                 TMatrix out)
{
  gint i, j;

  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++)
      out[i][j] = in[i][j];
}

/* Create a scaling matrix. */
void
t_matrix_scale (TMatrix matrix,
                gdouble k)
{
  gint i;

  t_matrix_zero (matrix);

  for (i = 0; i < 3; i++)
    matrix[i][i] = k;
}
