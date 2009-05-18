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
 * Code in this file is copyright (c) 2000 David A. Bartold
 *
 * Algorithm is implemented per description written by Hugo Elias, a
 * co-creater of Terragen.  Information about Perlin Noise is available 
 * online at: http://freespace.virgin.net/hugo.elias/
 */


#include <glib.h>
#include <math.h>
#include "trandom.h"

typedef struct TOctave TOctave;
struct TOctave
{
  gfloat data[32768];
  gfloat frequency;
};

static TOctave *
t_octave_new (gint   seed,
              gfloat frequency,
              gfloat amplitude)
{
  TOctave *octave;
  TRandom *random;
  gint     i;

  octave = g_new (TOctave, 1);
  random = t_random_new (seed);
  for (i = 0; i < 32768; i++)
    octave->data[i] = t_random_rnd1 (random) * amplitude;
  octave->frequency = frequency;
  t_random_free (random);

  return octave;
}

static void
t_octave_free (TOctave *octave)
{
  g_free (octave);
}

static gfloat
interpolate (gfloat a,
             gfloat b,
             gfloat weight)
{
  weight = 0.5 + cos (weight * M_PI) * 0.5;

  return a * weight + b * (1.0 - weight);
}

static gfloat
t_octave_get (TOctave *octave,
              gfloat   x,
              gfloat   y,
              gfloat   z)
{
  gint   int_x, int_y, int_z;
  gfloat frac_x, frac_y, frac_z;
  gfloat point_1, point_2, point_3, point_4;
  gfloat point_a, point_b, point_c, point_d;
  gfloat point_w, point_x, point_y, point_z;
  gfloat point_A, point_B;

  x *= octave->frequency;
  y *= octave->frequency;

  int_x = (gint) x;
  int_y = (gint) y;
  int_z = (gint) z;

  frac_x = x - int_x;
  frac_y = y - int_y;
  frac_z = z - int_z;

  int_x &= 31;
  int_y &= 31;
  int_z &= 31;

  point_1 = octave->data[int_y * 1024 + int_x * 32 + int_z];
  point_2 = octave->data[int_y * 1024 + ((int_x + 1) & 31) * 32 + int_z];
  point_3 = octave->data[((int_y + 1) & 31) * 1024 + int_x * 32 + int_z];
  point_4 = octave->data[((int_y + 1) & 31) * 1024 + ((int_x + 1) & 31) * 32 + int_z];

  point_a = octave->data[int_y * 1024 + int_x * 32 + ((int_z + 1) & 31)];
  point_b = octave->data[int_y * 1024 + ((int_x + 1) & 31) * 32 + ((int_z + 1) & 31)];
  point_c = octave->data[((int_y + 1) & 31) * 1024 + int_x * 32 + ((int_z + 1) & 31)];
  point_d = octave->data[((int_y + 1) & 31) * 1024 + ((int_x + 1) & 31) * 32 + ((int_z + 1) & 31)];

  point_w = interpolate (point_1, point_2, frac_x);
  point_x = interpolate (point_3, point_4, frac_x);

  point_y = interpolate (point_a, point_b, frac_x);
  point_z = interpolate (point_c, point_d, frac_x);

  point_A = interpolate (point_w, point_x, frac_y);
  point_B = interpolate (point_y, point_z, frac_y);

  return interpolate (point_A, point_B, frac_z);
}


typedef struct TPerlin3D TPerlin3D;
struct TPerlin3D
{
  gint      octave_count;
  TOctave **octaves;
};

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

TPerlin3D *
t_perlin3d_new (gint   count,
                gfloat frequency,
                gfloat persistence,
                gint   seed)
{
  TPerlin3D *perlin;
  gfloat     amplitude, max;
  gint       i;

  perlin = g_new (TPerlin3D, 1);
 
  perlin->octave_count = count;
  perlin->octaves = g_new (TOctave*, count);

  max = 0.0;
  amplitude = 1.0;
  for (i = 0; i < count; i++)
    {
      max += amplitude;
      amplitude *= persistence;
    }   

  amplitude = 1.0 / max;
  for (i = 0; i < count; i++)
    {
      perlin->octaves[i] = t_octave_new (seed + i, frequency, amplitude);

      frequency *= 2.0;
      amplitude *= persistence;
    }

  return perlin;
}

TPerlin3D *
t_perlin3d_new_full (gint    count,
                     gfloat  frequency,
                     gfloat *amplitudes,
                     gint    seed)
{
  TPerlin3D *perlin;
  gfloat     max;
  gint       i;

  perlin = g_new (TPerlin3D, 1);
 
  perlin->octave_count = count;
  perlin->octaves = g_new (TOctave*, count);

  max = 0.0;
  for (i = 0; i < count; i++)
    max += amplitudes[i];

  for (i = 0; i < count; i++)
    {
      perlin->octaves[i] =
        t_octave_new (seed + i, frequency, amplitudes[i] / max);

      frequency *= 2.0;
    }

  return perlin;
}

void
t_perlin3d_free (TPerlin3D *perlin)
{
  gint i;

  for (i = 0; i < perlin->octave_count; i++)
    t_octave_free (perlin->octaves[i]);

  g_free (perlin->octaves);
  g_free (perlin);
}

gfloat
t_perlin3d_get (TPerlin3D *perlin,
                gfloat     x,
                gfloat     y,
                gfloat     z)
{
  gint   i;
  gfloat sum;

  sum = 0.0;
  for (i = 0; i < perlin->octave_count; i++)
    sum += t_octave_get (perlin->octaves[i], x, y, z);

  return sum;
}
