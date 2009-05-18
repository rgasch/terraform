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

/*
 * The 4-layered Perlin noise generation code is taken/adapted from
 * John Ratcliff (jratcliff@verant.com) who placed his code under
 * the public domain.
 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <glib.h>
#include "trandom.h"
#include "tterrain.h"
#include "genperlin.h"

#define ZRECIP1 (1.0f / 64.0f)
#define ZRECIP2 (1.0f / 8.0f)

#define TABLESIZE 1024
#define TABLEMASK (TABLESIZE - 1)

#define PRECISION 16

#define RANDOM1(octave,i) ((octave)->random1[(i) & TABLEMASK])
#define RANDOM2(octave,i) ((octave)->random2[(i) & TABLEMASK])

#define CLIP(x,min,max) MIN(MAX(x,min),max)

/* ********************************************************************** */

typedef enum EquationType EquationType;
enum EquationType
{
  MF_NULL,
  MF_SQUARED,
  MF_SIN,
  MF_COS,
  MF_TAN,
  MF_SINH,
  MF_COSH,
  MF_TANH,
  MF_ASIN,
  MF_ACOS,
  MF_ATAN,
  MF_EXP,
  MF_LOG,
  MF_SQRT,
  MF_LAST
};

typedef enum EnvelopeType EnvelopeType;
enum EnvelopeType
{
  ET_NULL,      // 0 do nothing, leave it as is.
  ET_SQUARED1,  // 1
  ET_SQUARED2,  // 2
  ET_SQUARED3,  // 3
  ET_ATAN1,     // 4 (arc tangent 1)
  ET_ATAN2,     // 5 (arc tangent 2)
  ET_ATAN3,     // 6 (arc tangent 3)
  ET_ACOS1,     // 7 (arc cosine 1)
  ET_ACOS2,     // 8 (arc cosine 2)
  ET_ACOS3,     // 9 (arc cosine 3)
  ET_ACOS4,     // 10 (arc cosine 4)
  ET_COS1,      // 11
  ET_COS2,      // 12
  ET_EXP1,      // 13
  ET_EXP2,      // 14
  ET_EXP3,      // 15
  ET_EXP4,      // 16
  ET_EXP5,      // 17
  ET_LOG1,      // 18
  ET_LOG2,      // 19
  ET_LOG3,      // 20
  ET_TANH,      // 21 (hyperbolic tangent)
  ET_SQRT,      // 22
  ET_LAST
};

typedef struct Equation Equation;
struct Equation
{
  EquationType function;
  gfloat       constant;
  gfloat       scaler;
  gfloat       min;
  gfloat       max;
};

static Equation *equation_envelope_new (EnvelopeType  type);
static void      equation_free         (Equation     *equation);
static gfloat    equation_get          (Equation     *equation,
                                        gfloat        x);

typedef struct Octave Octave;
struct Octave
{
  gint    amplitude;
  gint    width;
  gint    height;
  gint    u;
  gint    v;
  gint    du;
  gint    dv;
  gint    scan_y;
  gint16 *scan_line;
  gint16 *random1;
  gint16 *random2;
};

static Octave *octave_new  (gint width,     gint height,
                            gint u,         gint v,
                            gint du,        gint dv,
                            gint amplitude, gint seed);
static void    octave_free (Octave *octave);
static gint    octave_get  (Octave *octave,
                            gint    x,
                            gint    y);

typedef struct Octaves Octaves;
struct Octaves
{
  Octave **octaves;
  gint     octave_count;
};

static Octaves *octaves_new  (gint octave_count, gint width, gint height,
                              gint u, gint v,
                              gfloat frequency, gint base, gint amplitude,
                              gfloat freq_decay, gfloat amp_decay, gint seed);
static void     octaves_free (Octaves *octaves);
static gint     octaves_get  (Octaves *octaves,
                              gint     x,
                              gint     y);

/**
 *  generate: create the Perlin noise and run it through an envelope.
 */
TTerrain *
t_terrain_generate_perlin (int width, int height, int seed,
                           gboolean b[4], float f[4], float a[4],
                           int o[4], int e[4])
{
  int        i, x, y, pos;
  TTerrain  *terrain;
  GtkObject *object;
  Octaves   *octaves[4];
  Equation  *envelope[4];

  if (seed == -1)
    seed = new_seed ();

  srand (seed);
  object = t_terrain_new (width, height);
  terrain = T_TERRAIN (object);

  for (i = 0; i < 4; i++)
    {
      if (b[i])
        {
          octaves[i] = octaves_new (o[i], width, height,
                                    rand (), rand (),
                                    f[i], 0.0F, a[i], 2.0F, 0.5F, seed + i);
          envelope[i] = equation_envelope_new (e[i]);
        }
    }

  /*
   * We have to call the get() in this order to avoid re-seeding 
   * scanlines at every get. See Octave::Get()
   */

  pos = 0;
  for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
        {
          gfloat sum;

          sum = 0.0;
          for (i = 0; i < 4; i++)
            {
              if (b[i])
                {
                  float temp;

                  temp = octaves_get (octaves[i], x, y) * ZRECIP1;
                  sum += equation_get (envelope[i], temp);
                }
            }

          terrain->heightfield[pos] = sum * ZRECIP2;
          pos++;
        }
    }

  terrain->modified = TRUE;

  for (i = 0; i < 4; i++)
    if (b[i])
      {
        octaves_free (octaves[i]);
        equation_free (envelope[i]);
      }

  t_terrain_normalize (terrain, FALSE);
  t_terrain_set_modified (terrain, TRUE);

  return terrain;
}


/* ********************************************************************** */

static Equation *
equation_new (EquationType function,
              gfloat constant, gfloat scaler, gfloat min, gfloat max)
{
  Equation *equ;

  equ = g_new (Equation, 1);

  equ->function = function;
  equ->constant = constant;
  equ->scaler = scaler;
  equ->min = min;
  equ->max = max;

  return equ;
}

static Equation *
equation_envelope_new (EnvelopeType type)
{
  gfloat ratio, scaler, amp;

  switch (type)
    {
    case ET_SQUARED1:
    case ET_SQUARED2:
    case ET_SQUARED3:
      ratio = (type - ET_NULL) / 3.0;
      scaler = ratio * 0.02f;
      return equation_new (MF_SQUARED, scaler, 1.0, -256.0, 256.0);

    case ET_ATAN1:
    case ET_ATAN2:
    case ET_ATAN3:
      ratio = (type - ET_SQUARED3) / 3.0;
      scaler = 90 * ratio;
      amp = 0.03f;
      return equation_new (MF_ATAN, amp, scaler, -256.0, 256.0);

    case ET_ACOS1:
    case ET_ACOS2:
    case ET_ACOS3:
    case ET_ACOS4:
      #define T_SCALE3 90
      #define T_AMP3 0.011f
      ratio = (type - ET_ATAN3) / 4.0;
      scaler = T_SCALE3 * ratio;
      amp = T_AMP3;
      return equation_new (MF_ACOS, amp, scaler, -256.0, 256.0);

    case ET_COS1:
      return equation_new (MF_COS, 0.015, 128.0, -256.0, 256.0);

    case ET_COS2:
      return equation_new (MF_COS, 0.02, 128.0, -256.0, 256.0);

    case ET_EXP1:
      return equation_new (MF_EXP, 0.025, 128.0, -256.0, 256.0);

    case ET_EXP2:
      return equation_new (MF_EXP, 0.010, 80.0, -256.0, 256.0);

    case ET_EXP3:
      return equation_new (MF_EXP, 0.013, 120.0, -256.0, 256.0);

    case ET_EXP4:
      return equation_new (MF_EXP, 0.018, 120.0, -256.0, 256.0);

    case ET_EXP5:
      return equation_new (MF_EXP, 0.055, 100.0, -256.0, 256.0);

    case ET_LOG1:
      return equation_new (MF_LOG, 0.12, 30.0, -256.0, 256.0);

    case ET_LOG2:
      return equation_new (MF_LOG, 0.03, 70.0, -256.0, 256.0);

    case ET_LOG3:
      return equation_new (MF_LOG, 0.1, 60.0, -256.0, 256.0);

    case ET_TANH:
      return equation_new (MF_TANH, 0.027, 64.0, -256.0, 256.0);

    case ET_SQRT:
      return equation_new (MF_TANH, 0.009, 256.0, 30.0, 256.0);

    case ET_NULL:
    case ET_LAST:
      return equation_new (MF_NULL, 1.0, 1.0, -256.0, 256.0);
    }

  return NULL;
}

static void
equation_free (Equation *equation)
{
  g_free (equation);
}

static gfloat
equation_get (Equation *equation,
              gfloat    x)
{
  switch (equation->function)
    {
    case MF_NULL:
      break;
    case MF_SQUARED:
      x = x * x * equation->constant;
      break;
    case MF_SIN:
      x = sin (x * equation->constant);
      break;
    case MF_COS:
      x = cos (x * equation->constant);
      break;
    case MF_TAN:
      x = tan (x * equation->constant);
      break;
    case MF_SINH:
      x = sinh (x * equation->constant);
      break;
    case MF_COSH:
      x = cosh (x * equation->constant);
      break;
    case MF_TANH:
      x = tanh (x * equation->constant);
      break;
    case MF_ASIN:
      x *= equation->constant;
      x = asin (CLIP (x, -1.0, 1.0));
      break;
    case MF_ACOS:
      x *= equation->constant;
      x = acos (CLIP (x, -1.0, 1.0));
      break;
    case MF_ATAN:
      x = atan (x * equation->constant);
      break;
    case MF_EXP:
      x = exp (x * equation->constant);
      break;
    case MF_LOG:
      x = log (fabs (x * equation->constant));
      break;
    case MF_SQRT:
      x = sqrt (fabs (x * equation->constant));
      break;
    case MF_LAST:
      break;
    }

  x *= equation->scaler;

  return CLIP (x, equation->min, equation->max);
}

/* ********************************************************************** */


static Octave *
octave_new (gint width, gint height,
            gint u, gint v,
            gint du, gint dv,
            gint amplitude, gint seed)
{
  Octave *octave;
  gint    i;

  octave = g_new (Octave, 1);
  octave->width = width;
  octave->height = height;
  octave->u = u;
  octave->v = v;
  octave->du = du;
  octave->dv = dv;
  octave->amplitude = amplitude;
  octave->scan_y = -1;
  octave->scan_line = g_new (gint16, width);

  octave->random1 = g_new (gint16, TABLESIZE);
  octave->random2 = g_new (gint16, TABLESIZE);
  srand (seed);
  for (i = 0; i < TABLESIZE; i++)
    {
      octave->random1[i] = rand();
      octave->random2[i] = rand();
    }

  return octave;
}

static void
octave_free (Octave *octave)
{
  g_free (octave->scan_line);
  g_free (octave->random1);
  g_free (octave->random2);
  g_free (octave);
}

/* 
 * Procedure octave_generate
 * Inputs: starting u and v parameters into the image.
 *      du, dv: the incremental change to u_init and v_init.
 *      ddu, ddv: the incremental change to du and dv.
 *      Num_Pix: number of pixels in the scan line to draw.
 *      screen_buffer: Pointer to the screen buffer.
 *      Output: 16 bit pixels are drawn to the screen.
 */

static void
octave_generate (Octave *octave,
                 gint    y)
{
  if (y != octave->scan_y)
    {
      gint    u, v, du, width;
      guint8  bx0, bx1, by0, by1;
      guint8  rx0, ry0;
      gint16 *scan_line;

      /*
       * FOR ALGORITHM BASED COMMENTS - PLEASE SEE ORIGINAL PROGRAM IN
       * SECTION 5.
       * Original Perlin's noise program used an array g[512][2].
       * This program replaces the array with real-time calculations.
       * The results are stored into variables of the form g_b##_#.
       */

      short g_b00_0; /* Used to replace array g[b00][0] */
      short g_b00_1; /* Used to replace array g[b00][1] */
      short g_b01_0; /* replaces array g[b01][0]        */
      short g_b01_1; /* replaces array g[b01][1]        */
      short g_b10_0; /* replaces array g[b10][0]        */
      short g_b10_1; /* replaces array g[b10][1]        */
      short g_b11_0; /* replaces array g[b11][0]        */
      short g_b11_1; /* replaces array g[b11][1]        */

      /* Used to replace array: b00 = p[p[bx0] + by0]; */
      short b00;
      short b01;
      short b10;
      short b11;

      unsigned short i;
      unsigned short u_16bit, v_16bit;
      signed   short rx1, ry1;
      gint32   sx, sy;
      signed   int u1, v1, u2, v2;
      gint32   a, b;

      octave->scan_y = y;
      u = octave->u;

      /*
       * dbartold: before multiplying, I truncate some bits off of dv
       * to prevent aliasing artifacts due to dv getting truncated later.
       */

      v = octave->v + (y * ((octave->dv >> 14) << 14));
      du = octave->du;
      width = octave->width;
      scan_line = octave->scan_line;

      /* Inner loop for the scan line.  "width" pixels will be drawn. */
      for (i = width; i; i--)
        {
          short color;

          /* Convert the u and v parameters from 10.22 to 8.8 format. */
          u_16bit = (short) (u >> 14);
          v_16bit = (short) (v >> 14);

          /*
           * Same as Perlin's original code except floating point
           * is converted over to fixed integer.
           */

          bx0 = u_16bit >> 8; /* Obtain integer part of the u coordinate */
          bx1 = bx0 + 1;
          rx0 = u_16bit & 255; /* Obtain fractional part of the u coordinate */
          rx1 = rx0 - 256;
          by0 = v_16bit >> 8;
          by1 = by0 + 1;
          ry0 = v_16bit & 255;
          ry1 = ry0 - 256;

          /*
           * Same as Perlin's original code except floating point
           * is converted over to fixed integer.  The ">> 1" is used to
           * avoid overflow when the values are multiplied together.
           * This is not a problem in "C" but will be in the MMX implementation.
           */

          /* dbartold: truncate by 12 not 16 bits, reduces zipper noise */
          sx = (((rx0 * rx0) >> 1) * ((1536 - (rx0 << 2)))) >> 12;
          sy = (((ry0 * ry0) >> 1) * ((1536 - (ry0 << 2)))) >> 12;

          /*
           * This is where the code differs from Perlins.
           * Rather than use arrays p[] and g[][], the values are
           * calculated real-time.  Here random1() replaces array p[].
           * Perlin's equivalent: b00 = p[p[bx0] + by0];
           */

          b00 = RANDOM1 (octave, (RANDOM1 (octave, bx0) + by0));
          b01 = RANDOM1 (octave, (RANDOM1 (octave, bx0) + by1));
          b10 = RANDOM1 (octave, (RANDOM1 (octave, bx1) + by0));
          b11 = RANDOM1 (octave, (RANDOM1 (octave, bx1) + by1));

          /* Here, random2() replaces array g[][]. */
          /* Perlin's equivalent: g_b00_0 = g[b00][0]; */

          g_b00_0 = (RANDOM2 (octave, b00) & 511) - 256;
          g_b01_0 = (RANDOM2 (octave, b01) & 511) - 256;
          g_b10_0 = (RANDOM2 (octave, b10) & 511) - 256;
          g_b11_0 = (RANDOM2 (octave, b11) & 511) - 256;
          g_b00_1 = (RANDOM2 (octave, b00 + 1) & 511) - 256;
          g_b01_1 = (RANDOM2 (octave, b01 + 1) & 511) - 256;
          g_b10_1 = (RANDOM2 (octave, b10 + 1) & 511) - 256;
          g_b11_1 = (RANDOM2 (octave, b11 + 1) & 511) - 256;


          /* Same as Perlin's original code */
          u1 = rx0 * g_b00_0 + ry0 * g_b00_1;
          v1 = rx1 * g_b10_0 + ry0 * g_b10_1;
          a = u1 + ((sx * (v1 - u1)) >> 12);
          /* Same as Perlin's original code */
          u2 = rx0 * g_b01_0 + ry1 * g_b01_1;
          v2 = rx1 * g_b11_0 + ry1 * g_b11_1;
          b = u2 + ((sx * (v2 - u2)) >> 12);

          /*
           * Same as Perlin's original code except the output is
           * converted from fixed point to regular integer.  Also
           * since the output ranges from -256 to +256, a 256 offest
           * is added to make the range from 0 to 511.  This offset
           * is the 65536 value.  Then the 0 to 511 is scaled down.
           * to a range of 0 to 255.
           */

          color = (short) ((a + 65536 + ((sy * (b - a)) >> 12)) >> 4);
          color -= (256 * PRECISION);
          *(scan_line) = color;


          /*
           * New u for calc the color of the next pixel
           *
           * dbartold:  I trim off 14 bits so that rx0 and rx1 are
           * always incremented by the same amount for each loop iteration.
           * That prevents aliasing noise.
           */
          u += ((du >> 14) << 14);

          scan_line++; /* Advance 1 word */
        } /* End of the scanline - repeat for next pixel */
    }
}

static gint
octave_get (Octave *octave,
            gint    x,
            gint    y)
{
  gint value;

  octave_generate (octave, y);
  value = octave->scan_line[x];

  return (value * octave->amplitude) >> 12;
}


/* ********************************************************************** */


static Octaves *
octaves_new (gint octave_count, gint width, gint height, gint u, gint v,
             gfloat frequency, gint base, gint amplitude,
             gfloat freq_decay, gfloat amp_decay, gint seed)
{
  Octaves *octaves;
  gint     i;
  gint     freq = (gint) (frequency * ((float) (1 << 20)));
  gint     ampl = amplitude;

  octaves = g_new (Octaves, 1);
  octaves->octaves = g_new (Octave*, octave_count);
  octaves->octave_count = octave_count;

  for (i = 0; i < octave_count; i++)
    {
      octaves->octaves[i] =
        octave_new (width, height, u * freq, v * freq,
                    freq, freq, ampl, seed + i);

      freq = (gint) ((gfloat) freq * freq_decay);
      ampl = (gint) ((gfloat) ampl * amp_decay);
    }

  return octaves;
}


static void
octaves_free (Octaves *octaves)
{
  gint i;

  for (i = 0; i < octaves->octave_count; i++)
    octave_free (octaves->octaves[i]);

  g_free (octaves->octaves);
  g_free (octaves);
}


static gint
octaves_get (Octaves *octaves,
             gint     x,
             gint     y)
{
  gint total;
  gint i;

  total = 0;
  for (i = 0; i < octaves->octave_count; i++)
    total += octave_get (octaves->octaves[i], x, y);

  return total;
}
