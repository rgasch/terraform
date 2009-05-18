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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "trandom.h"

TRandom *
t_random_new (gint seed)
{
  TRandom *random;

  random = g_new (TRandom, 1);

  if (seed == -1) /* Default */
    t_random_init (random, new_seed ());
  else
    t_random_init (random, seed);

  return random;
}

void
t_random_free (TRandom *random)
{
  g_free (random);
}

void
t_random_init (TRandom *random,
               gint     seed)
{
  gint i, j;

  /* fixed abs(); overflow cond. 12/16/95 jpb */
  i = abs (97 * seed) % 31329;
  j = abs (33 * seed) % 30082;

  t_random_init_sequence (random, i, j);
}

/* 
C This is the initialization routine for the random number generator RANMAR()
C NOTE: The seed variables can have values between:    0 <= IJ <= 31328
C                                                      0 <= KL <= 30081
C The random number sequences created by these two seeds are of sufficient 
C length to complete an entire calculation with. For example, if several
C different groups are working on different parts of the same calculation,
C each group could be assigned its own IJ seed. This would leave each group
C with 30000 choices for the second seed. That is to say, this random 
C number generator can create 900 million different subsequences -- with 
C each subsequence having a length of approximately 10^30.
C 
C Use IJ = 1802 & KL = 9373 to test the random number generator. The
C subroutine RANMAR should be used to generate 20000 random numbers.
C Then display the next six random numbers generated multiplied by 4096*4096
C If the random number generator is working properly, the random numbers
C should be:
C           6533892.0  14220222.0  7275067.0
C           6172232.0  8354498.0   10633180.0
*/

void
t_random_init_sequence (TRandom *random,
                        gint     ij,
                        gint     kl)
{
  int i, j, k, l, ii, jj, m;
  float s, t;
        
  if (ij < 0 || ij > 31328 || kl < 0 || kl > 30081) 
    {
      fprintf (stderr, "The first random number seed must have a value between 0 and 31328.\n");
      fprintf (stderr, "The second seed must have a value between 0 and 30081.\n");
      exit (1);

    }

  i = (ij / 177) % 177 + 2;
  j = ij % 177 + 2;
  k = (kl / 169) % 178 + 1;
  l = kl % 169;
        
  for (ii = 1; ii <= 97; ii++) 
    {
      s = 0.0;
      t = 0.5;
      for (jj = 1; jj <= 24; jj++) 
        {
          m = (((i * j) % 179) * k) % 179;
          i = j;
          j = k;
          k = m;
          l = (53 * l + 1) % 169;
          if ((l * m) % 64 >= 32) s += t;
          t *= 0.5;
        }
      random->u[ii] = s;
    }
        
  random->c = 362436.0 / 16777216.0;
  random->cd = 7654321.0 / 16777216.0;
  random->cm = 16777213.0 / 16777216.0;
        
  random->i97 = 97;
  random->j97 = 33;
}

gdouble
t_random_rnd (TRandom *random,
              gdouble  floor,
              gdouble  ceil)
{
  return floor + (ceil - floor) * t_random_rnd1 (random);
}

/*  return a single floating-point random number  -mod jpb 7/23/95 */
/*  the output is a number between 0.0 and 1.0 */

gfloat
t_random_rnd1 (TRandom *random)
{
  /* the random number itself */
  float uni;

  /* difference between two [0..1] #s */
  uni = random->u[random->i97] - random->u[random->j97];
  if (uni < 0.0)
    uni += 1.0;
  random->u[random->i97] = uni;

  /* i97 ptr decrements and wraps around */
  random->i97--;
  if (random->i97 == 0) 
    random->i97 = 97;

  /* j97 ptr decrements likewise */
  random->j97--;
  if (random->j97 == 0) 
    random->j97 = 97;

  /* finally, condition with c-decrement */
  random->c -= random->cd;
  if (random->c < 0.0) 
    random->c += random->cm; /* cm > cd we hope! (only way c always >0 */

  uni -= random->c;
  if (uni < 0.0) 
    uni += 1.0;

  return uni;
}

/** 
 *  gauss: return a gaussian random (-1 <= n <= 1)
 */

gdouble
t_random_gauss (TRandom *random)
{
  /* 
   *  initialize the gaussian random number generator
   *  taken from 'The Science of Fractal Images' by Peitgen & Saupe, 
   *  1988 (page 77)
   */ 

/*
  int    s_gaussNRand = 4;
  double s_maxRand = pow (2.0, 15.0) - 1.0;
  double s_gaussAdd = sqrt (3.0 * s_gaussNRand);
  double s_gaussFac = 2 * s_gaussAdd / (s_gaussNRand * s_maxRand);
*/

  const int    s_gaussNRand = 4;
  const double s_gaussAdd = 3.46410161514 / 3.45;
  const double s_gaussFac = 5.28596089837e-5 / 3.45;

  gdouble r;
  gfloat  sum;
  gint    i;

  sum = 0.0;
  for (i = 0; i < s_gaussNRand; i++)
    sum += t_random_rnd (random, 0.0, 1.0) * 0x7FFF;

  r = s_gaussFac * sum - s_gaussAdd;

  /* the most extreme rndGauss() values I have ever observed (even */
  /* when running through a few billion tries) was -3.42 and 3.42. */
  /* 3.45 should safely scale the number to the -1 ... 1 range     */

  return r;
}

gint
new_seed ()
{
  static gint seed = 0;

  if (seed == 0)
    seed = time (NULL);
  else
    seed++;

  return seed;
}

