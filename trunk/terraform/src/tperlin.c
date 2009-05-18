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
 * Code in this file is copyright (c) 2000-2002 David A. Bartold
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
  gfloat data[1024];
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
  for (i = 0; i < 1024; i++)
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
              gfloat   y)
{
  gint   int_x, int_y;
  gfloat frac_x, frac_y;
  gfloat point_1, point_2, point_3, point_4, point_5, point_6;

  x *= octave->frequency;
  y *= octave->frequency;

  int_x = (gint) x;
  int_y = (gint) y;
  frac_x = x - int_x;
  frac_y = y - int_y;
  int_x &= 31;
  int_y &= 31;

  point_1 = octave->data[int_y * 32 + int_x];
  point_2 = octave->data[int_y * 32 + ((int_x + 1) & 31)];
  point_3 = octave->data[((int_y + 1) & 31) * 32 + int_x];
  point_4 = octave->data[((int_y + 1) & 31) * 32 + ((int_x + 1) & 31)];

  point_5 = interpolate (point_1, point_2, frac_x);
  point_6 = interpolate (point_3, point_4, frac_x);

  return interpolate (point_5, point_6, frac_y);
}


typedef struct TPerlin TPerlin;
struct TPerlin
{
  gint      octave_count;
  TOctave **octaves;
};


TPerlin *t_perlin_new      (gint   count,
                            gfloat frequency,
                            gfloat persistence,
                            gint   seed);

TPerlin *t_perlin_new_full (gint    count,
                            gfloat  frequency,
                            gfloat *amplitudes,
                            gint    seed);

void     t_perlin_free     (TPerlin *perlin);

gfloat   t_perlin_get      (TPerlin *perlin,
                            gfloat   x,
                            gfloat   y);


TPerlin *
t_perlin_new (gint   count,
              gfloat frequency,
              gfloat persistence,
              gint   seed)
{
  TPerlin *perlin;
  gfloat   amplitude, max;
  gint     i;

  perlin = g_new (TPerlin, 1);
 
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

TPerlin *
t_perlin_new_full (gint    count,
                   gfloat  frequency,
                   gfloat *amplitudes,
                   gint    seed)
{
  TPerlin *perlin;
  gfloat   max;
  gint     i;

  perlin = g_new (TPerlin, 1);
 
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
t_perlin_free (TPerlin *perlin)
{
  gint i;

  for (i = 0; i < perlin->octave_count; i++)
    t_octave_free (perlin->octaves[i]);

  g_free (perlin->octaves);
  g_free (perlin);
}

gfloat
t_perlin_get (TPerlin *perlin,
              gfloat   x,
              gfloat   y)
{
  gint   i;
  gfloat sum;

  sum = 0.0;
  for (i = 0; i < perlin->octave_count; i++)
    sum += t_octave_get (perlin->octaves[i], x, y);

  return sum;
}
