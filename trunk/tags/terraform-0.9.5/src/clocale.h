/*  
 *  Copyright (c) 2000-2002 David A. Bartold
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
 * Code taken from Terraform.
 */


#ifndef __C_LOCALE__H__
#define __C_LOCALE__H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define __USE_ISOC9X 1

#include <stdio.h>
#include <glib.h>


int fscanf_C   (FILE *in,
                char *format,
                ...) G_GNUC_SCANF (2, 3);
int sscanf_C   (char *in,
                char *format,
                ...) G_GNUC_SCANF (2, 3);
void fprintf_C (FILE *out,
                char *format,
                ...) G_GNUC_PRINTF (2, 3);
void sprintf_C (char *out,
                char *format,
                ...) G_GNUC_PRINTF (2, 3);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __C_LOCALE__H__ */
