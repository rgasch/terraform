/*  Terraform - (C) 1997-2002 Robert Gasch (r.gasch@chello.nl)
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
 *  This file Copyright (c) 2000-2002 David A. Bartold
 */


#include <locale.h>
#include <stdio.h>
#include <stdarg.h>
#include "clocale.h"

/* Note that these functions aren't thread safe... */

int fscanf_C (FILE *in,
              char *format,
              ...)
{
  gchar   *old_locale = g_strdup (setlocale (LC_NUMERIC, NULL));
  va_list  args;
  int      result;

  setlocale (LC_NUMERIC, "C");  

  va_start (args, format);
#ifndef HAVE_LIB_TRIO
  result = vfscanf (in, format, args);
#else
  result = trio_vfscanf (in, format, args);
#endif
  va_end (args);

  setlocale (LC_NUMERIC, old_locale);
  g_free (old_locale);

  return result;
}

int sscanf_C (char *in,
              char *format,
              ...)
{
  gchar   *old_locale = g_strdup (setlocale (LC_NUMERIC, NULL));
  va_list  args;
  int      result;

  setlocale (LC_NUMERIC, "C");  

  va_start (args, format);
#ifndef HAVE_LIB_TRIO
  result = vsscanf (in, format, args);
#else
  result = trio_vsscanf (in, format, args);
#endif
  va_end (args);

  setlocale (LC_NUMERIC, old_locale);
  g_free (old_locale);

  return result;
}

void fprintf_C (FILE *out,
                char *format,
                ...)
{
  gchar   *old_locale = g_strdup (setlocale (LC_NUMERIC, NULL));
  va_list  args;

  setlocale (LC_NUMERIC, "C");  

  va_start (args, format);
  vfprintf (out, format, args);
  va_end (args);

  setlocale (LC_NUMERIC, old_locale);
  g_free (old_locale);
}

void sprintf_C (char *out,
                char *format,
                ...)
{
  gchar   *old_locale = g_strdup (setlocale (LC_NUMERIC, NULL));
  va_list  args;

  setlocale (LC_NUMERIC, "C");  

  va_start (args, format);
  vsprintf (out, format, args);
  va_end (args);

  setlocale (LC_NUMERIC, old_locale);
  g_free (old_locale);
}
