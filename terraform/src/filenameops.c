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


#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <glib.h>
#include "filenameops.h"

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif




/**
 * filename_age: parse a filename and attempt to either update an existing 
 * 	versioning scheme or insert a new one into the string. All memory is 
 * 	allocated using malloc and the returned pointer should be disposed of 
 * 	using free(), not the C++ delete.
 * 
 * The Naming convention used is as follows: 
 * 	name-version.extension
 * where something like "filename-01.tga" becomes "filename-02.tga" and 
 * something like "filename.tga" becomes "filename-01.tga" 
 *
 * This is actually plain C code (regardless of the *.cc file it's in)
 */ 
gchar *
filename_age (const gchar *name)
{
	char 	*lMinus,				/* location of Minus sign */
		*lDot,					/* location of Dot */
		*dst,					/* destination string */
		*s; 
	char	version[80];				/* actual version part of string */
	int	val;
	int	insert=FALSE;

	if (!name) 
		return NULL;

	lMinus = strchr (name, '-');
	lDot = strchr (name, '.');

	if (!lDot) 					/* should at least be able to find the dot */
		return NULL;

	if (!lMinus) 					/* no minus -> insert a new one */
		insert = TRUE;

	dst = (char *)g_malloc(strlen(name)+16);
	if (!dst)
		return NULL;

	if (insert)					/* insert a new version */
		{
		strncpy (dst, name, lDot-name);
		dst[lDot-name] = '\0';
		strcat (dst, "-01");
		strcat (dst, lDot);
		}
	else						/* update existing version */
		{
		s = lMinus+1;
		while (s < lDot)
			{
			if (!isdigit(*s))
				break;
			s++;
			}

		if (s != lDot)				/* break, it's not all numbers */
			return NULL;

		strncpy (version, lMinus+1, lDot-lMinus);
		version[lDot-lMinus] = '\0';
		val = atoi (version);
		snprintf (version, 80, "-%02d", val+1);
		strncpy (dst, name, lMinus-name);
		dst[lMinus-name] = '\0';
		strcat (dst, version);
		strcat (dst, lDot);
		}

	/* printf ("%s ---> %s\n", name, dst); */
	return (dst);
}


/*
 * filename_new_extension:  Return a new filename by replacing the
 * extension of the filename passed in.  If it has no extension, add one.
 * The extension parameter should not have a preceding dot-- it will be added.
 * If extension == NULL, then a new extension is not added.
 */

gchar *
filename_new_extension (const gchar *name,
                        const gchar *extension)
{
  gchar *dot, *slash;
  gchar *temp, *out;

  temp = g_strdup (name);

  dot = strrchr (temp, '.');
  slash = strrchr (temp, '/');

  if (dot != NULL && dot > slash)
    {
      /* Filename has an extension, remove it. */
      *dot = '\0';
    }

  if (extension != NULL)
    {
      out = g_strdup_printf ("%s.%s", temp, extension);
      g_free (temp);
      return out;
    }

  return temp;
}

/*
 * filename_get_base: Return the base filename minus the paths.
 */

gchar *
filename_get_base (const gchar *name)
{
  gchar *slash;

  slash = strrchr (name, '/');
  if (slash != NULL)
    return g_strdup (&slash[1]);

  return g_strdup (name);
}

/*
 * strip_extension: remove the extension from a filename
 */

static gchar *
strip_extension (const gchar *name,
                 gboolean     remove_dot)
{
  gchar *dot, *slash;
  gchar *temp;

  temp = g_strdup (name);

  dot = strrchr (temp, '.');
  slash = strrchr (temp, '/');

  if (dot != NULL && dot > slash)
    {
      /* Filename has an extension, remove it. */

      if (remove_dot)
        *dot = '\0';
      else
        dot[1] = '\0';
    }

  return temp;
}

gchar *
filename_get_without_extension_and_dot (const gchar *name)
{
  return strip_extension (name, TRUE);
}

gchar *
filename_get_without_extension (const gchar *name)
{
  return strip_extension (name, FALSE);
}

gchar *
filename_get_base_without_extension_and_dot (const gchar *name)
{
  gchar *base = filename_get_base (name);
  gchar *out = strip_extension (base, TRUE);

  g_free (base);
  return out;
}

gchar *
filename_get_base_without_extension (const gchar *name)
{
  gchar *base = filename_get_base (name);
  gchar *out = strip_extension (base, FALSE);

  g_free (base);
  return out;
}

/*
 * filename_get_friendly: Return a user-friendly filename.
 */

gchar *
filename_get_friendly (const gchar *name)
{
  gchar *friendly, *pos;
  gint   first;

  friendly = filename_get_base (name);

  first = 1;
  pos = friendly;
  while (*pos != '\0')
    {
      if (first)
        *pos = toupper (*pos);

      if (*pos == '.')
        *pos = '\0';

      if (*pos == '_')
        *pos = ' ';

      first = (*pos == ' ');
      pos++;
    }

  return friendly;
}

gchar *
filename_get_extension (const gchar *name)
{
  gchar *dot, *slash;

  dot = strrchr (name, '.');
  slash = strrchr (name, '/');

  if (dot == NULL || dot < slash)
    return g_strdup ("");

  return g_strdup (&dot[1]);
}

TFileType
filename_determine_type (const gchar *name) /* case-insensitive */
{
  TFileType  type;
  gchar     *extension;

  type = FILE_NATIVE; // save as native if we can't determine filetype by extension
  extension = filename_get_extension (name);

  if (g_strcasecmp (extension, "terraform") == 0)
    type = FILE_NATIVE;
  else if (g_strcasecmp (extension, "bmp") == 0)
    type = FILE_BMP;
  else if (g_strcasecmp (extension, "pov") == 0)
    type = FILE_POV;
  else if (g_strcasecmp (extension, "tga") == 0)
    type = FILE_TGA;
  else if (g_strcasecmp (extension, "pgm") == 0)
    type = FILE_PGM;
  else if (g_strcasecmp (extension, "pg8") == 0)
    type = FILE_PG8;
  else if (g_strcasecmp (extension, "ppm") == 0)
    type = FILE_PPM;
  else if (g_strcasecmp (extension, "mat") == 0)
    type = FILE_MAT;
  else if (g_strcasecmp (extension, "oct") == 0)
    type = FILE_OCT;
  else if (g_strcasecmp (extension, "ac") == 0)
    type = FILE_AC;
  else if (g_strcasecmp (extension, "dem") == 0 ||
           g_strcasecmp (extension, "hdr") == 0)
    type = FILE_GTOPO;
  else if (g_strcasecmp (extension, "grd") == 0)
    type = FILE_GRD;
  else if (g_strcasecmp (extension, "gif") == 0)
    type = FILE_GIF;
  else if (g_strcasecmp (extension, "ico") == 0)
    type = FILE_ICO;
  else if (g_strcasecmp (extension, "jpg") == 0)
    type = FILE_JPG;
  else if (g_strcasecmp (extension, "png") == 0)
    type = FILE_PNG;
  else if (g_strcasecmp (extension, "ras") == 0)
    type = FILE_RAS;
  else if (g_strcasecmp (extension, "tif") == 0)
    type = FILE_TIF;
  else if (g_strcasecmp (extension, "xbm") == 0)
    type = FILE_XBM;
  else if (g_strcasecmp (extension, "xpm") == 0)
    type = FILE_XPM;
  else if (g_strcasecmp (extension, "vrml") == 0)
    type = FILE_VRML;
  else if (g_strcasecmp (extension, "ter") == 0 ||
           g_strcasecmp (extension, "terragen") == 0)
    type = FILE_TERRAGEN;
  else if (g_strcasecmp (extension, "xyz") == 0)
    type = FILE_XYZ;
  else if (g_strcasecmp (extension, "dxf") == 0)
    type = FILE_DXF;
  else if (g_strcasecmp (extension, "bna") == 0)
    type = FILE_BNA;
  else if (g_strcasecmp (extension, "bt") == 0)
    type = FILE_BT;
  else if (g_strcasecmp (extension, "dted") == 0)
    type = FILE_DTED;
  else if (g_strcasecmp (extension, "e00") == 0)
    type = FILE_E00;
  else if (g_strcasecmp (extension, "ini") == 0)
    type = FILE_INI;

  g_free (extension);

  return type;
}

/* POV-Ray doesn't like certain characters to exist in filenames. */
gchar *
filename_povray_safe (const gchar *name)
{
  gchar *safe;
  gint   i;

  safe = g_strdup (name);
  for (i = 0; safe[i] != '\0'; i++)
    if (safe[i] == ' ' || safe[i] == '\"')
      safe[i] = '_';

  return safe;
}

/* Safely create a directory in /tmp for temporary files.  Return null if one
   could not be created (i.e. out of space, no permissions, etc).  Caller
   must g_free () the directory name returned by this function.
 */
gchar *
filename_create_tempdir ()
{
  gchar         *tempdir;
  gint           i;
  struct timeval tv;
  gchar         *login;
  gint           random;

  /* Determine user id, if possible */
  login = getlogin ();
  if (login == NULL)
    login = "unknown";

  /* Try to create a directory 50 times, each time with a new name */
  random = 0;
  for (i = 0; i < 50; i++)
    {
      /* Generate a random number */
      gettimeofday (&tv, NULL);
      srand (tv.tv_usec);
      random = (random + rand ()) & 0xffff;

      tempdir = g_strdup_printf ("/tmp/terraform-%s-%x", login, random);
      if (mkdir (tempdir, 0700) == 0)
        return tempdir;

      g_free (tempdir);
    }

  return NULL;
}
