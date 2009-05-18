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
 * This file is copyright (c) 2000 David A. Bartold
 */


#include <sys/types.h>	/* OS/X requires this */
#include <dirent.h>
#include <regex.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "filelist.h"
#include "filenameops.h"

static int 
str_cmp_func (const void *_v1_, const void *_v2_)
{
  gchar *s1 = (gchar *)_v1_;
  gchar *s2 = (gchar *)_v2_;

  return g_strcasecmp (s1, s2);
}

static GList *
file_list_add_file_sorted (GList   *list,
                           gchar   *file,
                           gchar   *base,
		           gboolean do_sort)
{
  struct stat stat_buf;
  int    (*fnptr) (const void *, const void *) = str_cmp_func;

  stat (file, &stat_buf);
  if (S_ISDIR (stat_buf.st_mode))
    return list;

  if (!S_ISREG (stat_buf.st_mode))
    {
      g_print ("Ignoring file '%s'.  It might be a symlink.\n", file);
      return list;
    }

  /* prevents adding a filename to the list which is already there */
  if(g_list_find_custom(list, g_strdup (base), fnptr))
    return list;

  if (do_sort)
    return g_list_insert_sorted (list, g_strdup (base), fnptr);

  return g_list_append (list, g_strdup (base));
}

static GList *
file_list_add_file (GList   *list,
                    gchar   *file,
                    gchar   *base)
{
  return file_list_add_file_sorted (list, file, base, TRUE);
}

static GList *
file_list_add_path_with_regexp (GList *list,
                                gchar *path,
                                gchar *pattern)
{
  DIR     *dir;
  struct   dirent *dirent;
  regex_t  re;

  dir = opendir (path);
  if (dir == NULL)
    return list;

  if (regcomp (&re, pattern, REG_EXTENDED|REG_ICASE|REG_NOSUB) != 0)
    return list;

  dirent = readdir (dir);
  while (dirent != NULL)
    {
      gchar *filename, *base;
      int    status;

      filename = g_strdup_printf ("%s/%s", path, dirent->d_name);
      base = filename_get_base (filename);

      status = regexec (&re, base, (size_t)0, NULL, 0);
      if (status == 0)
        list = file_list_add_file (list, filename, base);

      dirent = readdir (dir);
    }

  regfree (&re);
  closedir (dir);

  return list;
}

static GList *
file_list_add_path_with_extension (GList *list,
                                   gchar *path,
                                   gchar *extension)
{
  DIR   *dir;
  struct dirent *dirent;

  dir = opendir (path);
  if (dir == NULL)
    return list;

  dirent = readdir (dir);
  while (dirent != NULL)
    {
      gchar *filename, *base, *ext;

      filename = g_strdup_printf ("%s/%s", path, dirent->d_name);
      base = filename_get_base (filename);
      ext = filename_get_extension (filename);

      if (extension && !strcmp (ext, extension))
        list = file_list_add_file (list, filename, base);
      else
      if (!extension)
        list = file_list_add_file (list, filename, base);

      g_free (ext);
      g_free (base);
      g_free (filename);

      dirent = readdir (dir);
    }
  
  closedir (dir);

  return list;
}

static GList *
file_list_add_path (GList *list,
                    gchar *path)
{
  return file_list_add_path_with_extension (list, path, NULL);
}

GList *
texture_list_new ()
{
  gchar *home_dir;
  gchar *extension = "inc";

  home_dir = getenv ("HOME");
  if (home_dir != NULL){
    GList *textures_list;
    gchar *textures_path;

    textures_path = g_strdup_printf("%s/.terraform/include/earth_textures", home_dir);
    textures_list = file_list_add_path_with_extension (NULL, textures_path, extension);
    g_free (textures_path);
    return file_list_add_path_with_extension (textures_list, 
                              TERRAFORM_DATA_DIR "/include/earth_textures", 
                              extension);
  }
  else
    return file_list_add_path_with_extension (NULL, 
                              TERRAFORM_DATA_DIR "/include/earth_textures", 
                              extension);
}

/*
 * This part modified by Raymond Ostertag
 */

GList *
object_list_new ()
{
  gchar *home_dir;
  gchar *re = "^[^_].*\\.inc$";

  home_dir = getenv ("HOME");
  if (home_dir != NULL){
    GList *object_list;
    gchar *object_path;

    object_path = g_strdup_printf("%s/.terraform/objects",
                                  home_dir);
    object_list = file_list_add_path_with_regexp (NULL, object_path, re);
    g_free (object_path);
    return file_list_add_path_with_regexp (object_list,
		              TERRAFORM_DATA_DIR "/objects", re);
  }
  else
    return file_list_add_path_with_extension (NULL,
		              TERRAFORM_DATA_DIR "/objects", re);
}

/*
 * End by Raymond Ostertag
 */

GList *
cloud_list_new ()
{
  gchar *home_dir;
  gchar *re = "^clouds_.*\\.inc$";
  
  home_dir = getenv ("HOME");
  if (home_dir != NULL){
    GList *object_list;
    gchar *object_path;

    object_path = g_strdup_printf("%s/.terraform/include/skies/include", 
                                  home_dir);
    object_list = file_list_add_path_with_regexp (NULL, object_path, re);
    g_free (object_path);
    return file_list_add_path_with_regexp (object_list, 
		              TERRAFORM_DATA_DIR "/include/skies/include", re);
  }
  else
    return file_list_add_path_with_extension (NULL, 
		              TERRAFORM_DATA_DIR "/include/skies/include", re);
}

GList *
atmosphere_list_new ()
{
  gchar *home_dir;
  gchar *re = "^earth_.*\\.inc$";
  
  home_dir = getenv ("HOME");
  if (home_dir != NULL){
    GList *object_list;
    gchar *object_path;

    object_path = g_strdup_printf("%s/.terraform/include/atmospheres", 
                                  home_dir);
    object_list = file_list_add_path_with_regexp (NULL, object_path, re);
    g_free (object_path);
    return file_list_add_path_with_regexp (object_list, 
		              TERRAFORM_DATA_DIR "/include/atmospheres", re);
  }
  else
    return file_list_add_path_with_extension (NULL, 
		              TERRAFORM_DATA_DIR "/include/atmospheres", re);
}

GList *
star_list_new ()
{
  gchar *home_dir;
  gchar *re = "^stars_.*\\.inc$";
  
  home_dir = getenv ("HOME");
  if (home_dir != NULL){
    GList *object_list;
    gchar *object_path;

    object_path = g_strdup_printf("%s/.terraform/include/skies/include", 
                                  home_dir);
    object_list = file_list_add_path_with_regexp (NULL, object_path, re);
    g_free (object_path);
    return file_list_add_path_with_regexp (object_list, 
		              TERRAFORM_DATA_DIR "/include/skies/include", re);
  }
  else
    return file_list_add_path_with_extension (NULL, 
		              TERRAFORM_DATA_DIR "/include/skies/include", re);
}

GList *
water_list_new ()
{
  gchar *home_dir;
  gchar *re = "^earth_.*\\.inc$";
  
  home_dir = getenv ("HOME");
  if (home_dir != NULL){
    GList *object_list;
    gchar *object_path;

    object_path = g_strdup_printf("%s/.terraform/include/water", 
                                  home_dir);
    object_list = file_list_add_path_with_regexp (NULL, object_path, re);
    g_free (object_path);
    return file_list_add_path_with_regexp (object_list, 
		              TERRAFORM_DATA_DIR "/include/water", re);
  }
  else
    return file_list_add_path_with_extension (NULL, 
		              TERRAFORM_DATA_DIR "/include/water", re);
}

GList *
light_list_new ()
{
  gchar *home_dir;
  gchar *re = "^lights_.*\\.inc$";
  
  home_dir = getenv ("HOME");
  if (home_dir != NULL){
    GList *object_list;
    gchar *object_path;

    object_path = g_strdup_printf("%s/.terraform/include/skies/include", 
                                  home_dir);
    object_list = file_list_add_path_with_regexp (NULL, object_path, re);
    g_free (object_path);
    return file_list_add_path_with_regexp (object_list, 
		              TERRAFORM_DATA_DIR "/include/skies/include", re);
  }
  else
    return file_list_add_path_with_extension (NULL, 
		              TERRAFORM_DATA_DIR "/include/skies/include", re);
}

GList *
scene_theme_list_new ()
{
  gchar *home_dir;
  gchar *re = "^scene_.*\\.inc$";
  
  home_dir = getenv ("HOME");
  if (home_dir != NULL){
    GList *object_list;
    gchar *object_path;

    object_path = g_strdup_printf("%s/.terraform/include", 
                                  home_dir);
    object_list = file_list_add_path_with_regexp (NULL, object_path, re);
    g_free (object_path);
    return file_list_add_path_with_regexp (object_list, 
		              TERRAFORM_DATA_DIR "/include", re);
  }
  else
    return file_list_add_path_with_extension (NULL, 
		              TERRAFORM_DATA_DIR "/include", re);
}

GList *
skycolor_list_new ()
{
  gchar *home_dir;
  gchar *re = "^skycolor_.*\\.inc$";
  
  home_dir = getenv ("HOME");
  if (home_dir != NULL){
    GList *object_list;
    gchar *object_path;

    object_path = g_strdup_printf("%s/.terraform/include/skies/include", 
                                  home_dir);
    object_list = file_list_add_path_with_regexp (NULL, object_path, re);
    g_free (object_path);
    return file_list_add_path_with_regexp (object_list, 
		              TERRAFORM_DATA_DIR "/include/skies/include", re);
  }
  else
    return file_list_add_path_with_extension (NULL, 
		              TERRAFORM_DATA_DIR "/include/skies/include", re);
}

GList *
background_image_list_new ()
{
  gchar *home_dir;
  gchar *re = "^sky_.*\\.tga$";
  
  home_dir = getenv ("HOME");
  if (home_dir != NULL){
    GList *object_list;
    gchar *object_path;

    object_path = g_strdup_printf("%s/.terraform/image_maps", 
                                  home_dir);
    object_list = file_list_add_path_with_regexp (NULL, object_path, re);
    g_free (object_path);
    return file_list_add_path_with_regexp (object_list, 
		              TERRAFORM_DATA_DIR "/image_maps", re);
  }
  else
    return file_list_add_path_with_extension (NULL, 
		              TERRAFORM_DATA_DIR "/image_maps", re);
}

GList *
moon_image_list_new ()
{
  gchar *home_dir;
  gchar *re = "^moon_.*\\.png$";
  
  home_dir = getenv ("HOME");
  if (home_dir != NULL){
    GList *object_list;
    gchar *object_path;

    object_path = g_strdup_printf("%s/.terraform/image_maps", 
                                  home_dir);
    object_list = file_list_add_path_with_regexp (NULL, object_path, re);
    g_free (object_path);
    return file_list_add_path_with_regexp (object_list, 
		              TERRAFORM_DATA_DIR "/image_maps", re);
  }
  else
    return file_list_add_path_with_extension (NULL, 
		              TERRAFORM_DATA_DIR "/image_maps", re);
}


/*
 * This part added for Trace Objects by Raymond Ostertag
 */

GList *
trace_object_list_new ()
{
  gchar *home_dir;
  gchar *re = "^_.*\\.inc$";

  home_dir = getenv ("HOME");
  if (home_dir != NULL){
    GList *object_list;
    gchar *object_path;

    object_path = g_strdup_printf("%s/.terraform/objects",
                                  home_dir);
    object_list = file_list_add_path_with_regexp (NULL, object_path, re);
    g_free (object_path);
    return file_list_add_path_with_regexp (object_list,
		              TERRAFORM_DATA_DIR "/objects", re);
  }
  else
    return file_list_add_path_with_extension (NULL,
		              TERRAFORM_DATA_DIR "/objects", re);
}

/*
 * End for Trace Objects by Raymond Ostertag
 */


void
file_list_free (GList *list)
{
  g_list_foreach (list, (GFunc) g_free, NULL);
  g_list_free (list);
}

GList *
file_list_copy (GList *list)
{
  GList *out;

  out = NULL;
  for (list = g_list_first (list); list != NULL; list = list->next)
    out = g_list_append (out, g_strdup (list->data));

  return out;
}
