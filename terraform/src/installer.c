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
 *  This file is copyright (c) 2002 Koos Jan Niesink (k.j.niesink@students.geog.uu.nl)
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <dirent.h>
#include <errno.h>
#include <gnome.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "interface.h"
#include "mainwindow.h"
#include "installer.h"

static gint delete_event_cb(GtkWidget* w, GdkEventAny* e, gpointer data);
static void button_click_cb(GtkWidget* w, gpointer data);
static int  copy_file (gchar *source, gchar *dest);
static int  copy_directory (gchar *source, gchar *dest);
static int  create_rcfile (gchar *dest);
static gchar *install_error_message;

gboolean
tf_install_verify (void)
{
  gchar *home_dir;

  home_dir = g_strdup(getenv ("HOME"));
  if (home_dir)
    {
      gchar       *terraform_dir;
      struct stat  statdir;
      gint         rc;

      terraform_dir = g_strdup_printf ("%s/.terraform", home_dir);
      rc = stat (terraform_dir, &statdir);
      if (rc == -1) /* terraform is not installed, ~/.terraform is not present */
	{
	  g_free (terraform_dir);
	  g_free (home_dir);

	  return FALSE;
	}

      g_free (terraform_dir);
    }

  g_free (home_dir);

  return TRUE;
}

void
tf_user_install (void)
{

  GtkWidget *window, *button, *label;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  button = gtk_button_new();
  label  = gtk_label_new("This is the Terraform installer!\nClick on this button to install terraform.");

  gtk_container_add(GTK_CONTAINER(button), label);
  gtk_container_add(GTK_CONTAINER(window), button);

  gtk_window_set_title(GTK_WINDOW(window), "Terraform user installation");
  gtk_container_set_border_width(GTK_CONTAINER(button), 10);

  gtk_signal_connect(GTK_OBJECT(window),
                      "delete_event",
                      GTK_SIGNAL_FUNC(delete_event_cb),
                      NULL);

  gtk_signal_connect(GTK_OBJECT(button),
                      "clicked",
                      GTK_SIGNAL_FUNC(button_click_cb),
                      label);

  gtk_widget_show_all(window);

} 

static gint 
 delete_event_cb(GtkWidget* window, GdkEventAny* e, gpointer data)
 {
   GtkWidget *main_window;
   MainState *main_state;

   gtk_widget_destroy (window);

   /* g_print ("delete event\n"); */

   main_window = create_main_window ();
   main_window_initialize (main_window);
   gtk_widget_show (main_window);

   main_state = gtk_object_get_data (GTK_OBJECT (main_window), "data_state");

   return FALSE;
 }

static void 
button_click_cb(GtkWidget* w, gpointer data)
{
  GtkWidget* label;
  gchar *source_dir, *dest_dir, *home_dir;
  int result;

  label = GTK_WIDGET(data);
  /* determine source and destination directory */
  source_dir = g_strdup (TERRAFORM_DATA_DIR);
  home_dir = g_strdup (getenv ("HOME")); 
  //FIXME: check for existance of home_dir
  dest_dir = g_strdup_printf ("%s/.terraform", home_dir);
  /* g_print ("%s to %s\n", source_dir, dest_dir); */

  result = copy_directory (source_dir, dest_dir);
  if (result == 0) {
      result = create_rcfile (dest_dir);
  }
  if (result == 0)
    gtk_label_set_text (GTK_LABEL (label), "Installation is finished\nClose this window to start terraform");
  else if (result == -1)
    gtk_label_set_text (GTK_LABEL (label), install_error_message);

  g_free (source_dir);
  g_free (dest_dir);
  g_free (home_dir);
}

static int
copy_file (gchar *source,
	   gchar *dest)
{
  char buffer[4096];
  FILE *sfile, *dfile;
  int nbytes;

  sfile = fopen (source, "rb");
  if (sfile == NULL)
    {
      install_error_message = g_strdup_printf (_("Cannot open %s for reading"),
					       source);
      return -1;
    }

  //ook nog even testen of bestand al bestaat
  //eventueel bestand overschrijven
  dfile = fopen (dest, "wb");
  if (dfile == NULL)
    {
      install_error_message = g_strdup_printf (_("Cannot open %s for writing"),
					       dest);
      fclose (sfile);
      return -1;
    }

  while ((nbytes = fread (buffer, 1, sizeof (buffer), sfile)) > 0)
    {
      if (fwrite (buffer, 1, nbytes, dfile) < nbytes)
	{
	  install_error_message = g_strdup_printf (_("Error while writing %s"),
						   dest); 
	  fclose (sfile);
	  fclose (dfile);
	  return -1;
	}
    }

  if (ferror (sfile))
    {
      install_error_message = g_strdup_printf (_("Error while reading %s"),
					       source);
      fclose (sfile);
      fclose (dfile);
      return -1;
    }

  fclose (sfile);
  fclose (dfile);
  return 0;
}

static int
copy_directory (gchar *source,
		gchar *dest)
{
  DIR *sdir;
  struct dirent *entry;
  gchar source_dir_name[1000];
  gchar dest_dir_name[1000];
  gchar source_file_name[1000];
  gchar dest_file_name[1000];
  struct stat st;

  strncpy (source_dir_name, source, sizeof (source_dir_name));
  if (source_dir_name[strlen (source_dir_name) - 1] == G_DIR_SEPARATOR)
    source_dir_name[strlen (source_dir_name) - 1] = '\0';

  strncpy (dest_dir_name, dest, sizeof (dest_dir_name));
  if (dest_dir_name[strlen (dest_dir_name) - 1] == G_DIR_SEPARATOR)
    dest_dir_name[strlen (dest_dir_name) - 1] = '\0';

  sdir = opendir (source_dir_name);
  /* g_print ("source_dir_name: %s\n", source_dir_name); */
  if (sdir == NULL)
    {
      install_error_message = g_strdup_printf (_("Cannot open directory %s"),
					       source_dir_name);
      return -1;
    }
  
  /* g_print ("dest_dir_name: %s\n", dest_dir_name); */
  //test if directory exist if not create it
  stat (dest_dir_name, &st);
  /* if (S_ISDIR (st.st_mode))
     g_print ("Directory alraedy exists.\n"); */
  //FIXME: make this code shorter
  //else 
  if (!S_ISDIR(st.st_mode))
    {
      if (mkdir (dest_dir_name, 0777) == -1)
      { 
	install_error_message = g_strdup_printf (_("Cannot create directory %s -- %s"),
						 dest_dir_name, g_strerror (errno)); 
	  closedir (sdir);
	  return -1;
      }
    }
  
  while ((entry = readdir (sdir)) != NULL)
    {
      if (strcmp (entry->d_name, ".") == 0 ||
	  strcmp (entry->d_name, "..") == 0)
	continue;

      g_snprintf (source_file_name, sizeof (source_file_name), "%s%c%s",
		  source_dir_name, G_DIR_SEPARATOR, entry->d_name);

      g_snprintf (dest_file_name, sizeof (dest_file_name), "%s%c%s",
		  dest_dir_name, G_DIR_SEPARATOR, entry->d_name);

      if (stat (source_file_name, &st) == -1)
	{
	  install_error_message = g_strdup_printf (_("Cannot get status of file %s"),
						   source_file_name);
	  closedir (sdir);
	  return -1;
	}

      if (S_ISDIR (st.st_mode))
	{
	  if (copy_directory (source_file_name, dest_file_name) == -1)
	    {
	      closedir (sdir);
	      return -1;
	    }
	}
      else if (S_ISREG (st.st_mode))
	{
	  if (copy_file (source_file_name, dest_file_name) == -1)
	    {
	      closedir (sdir);
	      return -1;
	    }
	}
    }
  closedir (sdir);

  return 0;
}


static int
create_rcfile (gchar *dest)
{
  MainState  *state;
  gchar rc_file_name[1000];

  g_snprintf (rc_file_name, sizeof(rc_file_name), "%s%c%s", dest, G_DIR_SEPARATOR, "terraformrc");
  state = main_state_new ();
  main_window_state_save (state, rc_file_name);
  return 0;
}

