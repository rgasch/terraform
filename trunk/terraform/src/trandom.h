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


#ifndef __T_RANDOM_H__
#define __T_RANDOM_H__

#include <glib.h>

/*
C This random number generator originally appeared in "Toward a Universal 
C Random Number Generator" by George Marsaglia and Arif Zaman. 
C Florida State University Report: FSU-SCRI-87-50 (1987)
C 
C It was later modified by F. James and published in "A Review of Pseudo-
C random Number Generators" 
C 
C THIS IS THE BEST KNOWN RANDOM NUMBER GENERATOR AVAILABLE.
C       (However, a newly discovered technique can yield 
C         a period of 10^600. But that is still in the development stage.)
C
C It passes ALL of the tests for random number generators and has a period 
C   of 2^144, is completely portable (gives bit identical results on all 
C   machines with at least 24-bit mantissas in the floating point 
C   representation). 
C 
C The algorithm is a combination of a Fibonacci sequence (with lags of 97
C   and 33, and operation "subtraction plus one, modulo one") and an 
C   "arithmetic sequence" (using subtraction).
C======================================================================== 
This C language version was written by Jim Butler, and was based on a
FORTRAN program posted by David LaSalle of Florida State University.
seed_ran1() and ran1() added by John Beale 7/23/95
*/


typedef struct TRandom TRandom;

struct TRandom
{
  gfloat u[98], c, cd, cm;
  gint   i97, j97;
};

TRandom *t_random_new           (gint     seed);
void     t_random_free          (TRandom *random);
void     t_random_init          (TRandom *random,
                                 gint     seed);
void     t_random_init_sequence (TRandom *random,
                                 gint     ij,
                                 gint     kl);
gdouble  t_random_rnd           (TRandom *random,
                                 gdouble  floor,
                                 gdouble  ceil);
gfloat   t_random_rnd1          (TRandom *random);
gdouble  t_random_gauss         (TRandom *random);

gint     new_seed               (void);

#endif /* __T_RANDOM_H__ */
