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


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <math.h>
#include <gnome.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libgnomeui/gnome-window-icon.h>
#include <sys/stat.h>

#include "callbacks.h"
#include "genrandom.h"
#include "interface.h"
#include "support.h"
#include "support2.h"
#include "mainwindow.h"
#include "tterrainview.h"
#include "terrainwindow.h"
#include "xmlsupport.h"
#include "filenameops.h"

MainState *
main_state_new (void)
{
  MainState *state;

  state = g_new (MainState, 1);
  state->povray_executable = g_strdup ("povray");
  state->max_undo = 10;
  state->default_size = 800;
  /* don't use TYPE_1 (faulting) by default because it takes much 
   * longer than the other generation modes */
  state->gentypes = GEN_TYPE_2 | GEN_TYPE_3 | GEN_TYPE_4;
  state->gamma = 1.5;
  state->genfactor = 0.5;

  return state;
}

void
main_state_free (MainState *state)
{
  g_free (state->povray_executable);
  g_free (state);
}

void
main_window_state_pack_options (GtkWidget *main_window,
                                GtkWidget *option_window)
{
  MainState *state;
  GtkWidget *editable;
  gint       gentypes = 0;

  state = gtk_object_get_data (GTK_OBJECT (main_window), "data_state");
  editable = lookup_widget (option_window, "option_povray");

  g_free (state->povray_executable);
  state->povray_executable = gtk_editable_get_chars (GTK_EDITABLE (editable), 0, -1);

  state->max_undo = get_int (option_window, "max_undo");
  state->default_size = get_int (option_window, "default_size");
  state->gamma = get_float (option_window, "gamma");

  if (get_boolean (option_window, "rand_gen_faulting"))
    gentypes += pow (2, 0);
  if (get_boolean (option_window, "rand_gen_perlin"))
    gentypes += pow (2, 1);
  if (get_boolean (option_window, "rand_gen_spectral"))
    gentypes += pow (2, 2);
  if (get_boolean (option_window, "rand_gen_subdiv"))
    gentypes += pow (2, 3);

  if (gentypes != 0)
    state->gentypes = gentypes;
}

void
main_window_state_unpack_options (GtkWidget *main_window,
                                  GtkWidget *option_window)
{
  MainState *state;
  GtkWidget *editable;
  gint       pos;

  state = gtk_object_get_data (GTK_OBJECT (main_window), "data_state");
  editable = lookup_widget (option_window, "option_povray");

  gtk_editable_delete_text (GTK_EDITABLE (editable), 0, -1);
  pos = 0;
  gtk_editable_insert_text (GTK_EDITABLE (editable), state->povray_executable,
                            strlen (state->povray_executable), &pos);

  set_int (option_window, "max_undo", state->max_undo);
  set_int (option_window, "default_size", state->default_size);
  set_float (option_window, "gamma", state->gamma);

  set_boolean (option_window, "rand_gen_faulting", state->gentypes & GEN_TYPE_1);
  set_boolean (option_window, "rand_gen_perlin", state->gentypes & GEN_TYPE_2);
  set_boolean (option_window, "rand_gen_spectral", state->gentypes & GEN_TYPE_3);
  set_boolean (option_window, "rand_gen_subdiv", state->gentypes & GEN_TYPE_4);
}

MainState *
main_window_state_load (gchar *filename)
{
  xmlDocPtr   doc;
  MainState  *state;

  doc = xmlParseFile (filename);
  if (doc == NULL)
    return NULL;

  if (strcmp (doc->children->name, "Options") != 0)
    {
      xmlFreeDoc (doc);
      return NULL;
    }

  state = main_state_new ();
  xml_unpack_string (doc->children, "povray_executable", &state->povray_executable);
  xml_unpack_int (doc->children, "max_undo", &state->max_undo);
  xml_unpack_int (doc->children, "default_size", &state->default_size);
  xml_unpack_float (doc->children, "gamma", &state->gamma);
  xml_unpack_int (doc->children, "gentypes", &state->gentypes);
  xml_unpack_float (doc->children, "genfactor", &state->genfactor);

  xmlFreeDoc (doc);

  return state;
}

void
main_window_state_save (MainState *state,
                        gchar     *filename)
{
  xmlDocPtr  doc;

  doc = xmlNewDoc ("1.0");
  doc->children = xmlNewDocNode (doc, NULL, "Options", NULL);
  xml_pack_string (doc->children, "povray_executable", state->povray_executable);
  xml_pack_int (doc->children, "max_undo", state->max_undo);
  xml_pack_int (doc->children, "default_size", state->default_size);
  xml_pack_float (doc->children, "gamma", state->gamma);
  xml_pack_int (doc->children, "gentypes", state->gentypes);
  xml_pack_float (doc->children, "genfactor", state->genfactor);

  xmlSaveFile (filename, doc);
  xmlFreeDoc (doc);
}

void
main_window_initialize (GtkWidget *main_window)
{
  MainState *state;
  gchar     *file;
  gchar     *home_dir;

  state = NULL;

  gnome_window_icon_set_from_default (GTK_WINDOW (main_window));
  gtk_widget_set_sensitive (lookup_widget (main_window, "erode_flowmap"),FALSE);
  gtk_widget_set_sensitive (lookup_widget (main_window, "merge"), FALSE);
  gtk_widget_set_sensitive (lookup_widget (main_window, "join"), FALSE);
  gtk_widget_set_sensitive (lookup_widget (main_window, "warp"), FALSE);
  gtk_widget_set_sensitive (lookup_widget (main_window, "main_print_settings"),
                            FALSE);
  home_dir = getenv ("HOME");
  if (home_dir != NULL)
    {
      gchar *terraform_dir= g_strdup_printf("%s/.terraform", home_dir);
      struct stat statdir;

      gint rc = stat (terraform_dir, &statdir);
      if (rc != -1 && S_ISDIR(statdir.st_mode))
	{
	  file = g_strdup_printf ("%s/terraformrc", terraform_dir);
	  state = main_window_state_load (file);
	  g_free (file);
	}
      g_free (terraform_dir);
    }

  if (state == NULL)
    state = main_state_new ();
  gtk_object_set_data_full (GTK_OBJECT (main_window), "data_state",
                            state, (GtkDestroyNotify) main_state_free);
}

gboolean
main_window_exit_sequence (GtkWidget *main_window)
{
  GtkWidget *exit_anyways;
  GtkWidget *label;
  GtkCList  *clist;
  gpointer   data;
  gboolean   modified;
  gchar      buf[256];
  int        row;

  clist = GTK_CLIST (lookup_widget (main_window, "main_list"));
  modified = 0;
  for (row = 0; (data = gtk_clist_get_row_data (clist, row)) != NULL; row++)
    {
      GtkWidget *view = lookup_widget (GTK_WIDGET (data), "terrain");
      TTerrain *terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

      if (terrain->modified)
        {
          modified = TRUE;
          break;
        }
    }

  if (modified)
    {
    exit_anyways = create_close_terraform_dialog();
    strncpy (buf, _("You have unsaved terrains!\nWould you like to save the changes before exiting the program?"), 256);
    label = lookup_widget (exit_anyways, "label");
    gtk_label_set_text (GTK_LABEL(label), buf);
    gtk_object_set_data (GTK_OBJECT (exit_anyways), "data_parent", main_window);

    gtk_widget_show (exit_anyways);

    return TRUE;
    }

  gtk_main_quit ();

  return FALSE;
}

gchar *
main_window_filename_new (GtkWidget *main_window)
{
  gchar *old, *new;

  old = gtk_object_get_data (GTK_OBJECT (main_window), "data_filename");
  if (old == NULL)
    old = "Untitled-01.terraform";

  new = filename_age (old);

  old = g_strdup (old);
  gtk_object_set_data_full (GTK_OBJECT (main_window),
                            "data_filename", new, g_free);

  return old;
}

void
main_window_add_terrain_window (GtkWidget *main_window,
                                GtkWidget *terrain_window)
{
  GtkCList     *clist;
  int           row;
  gchar        *rows[2];

  clist = GTK_CLIST (lookup_widget (main_window, "main_list"));

  rows[0] = filename_get_base(terrain_window_get_title (terrain_window));
  rows[1] = NULL;
  row = gtk_clist_append (clist, rows);
  g_free (rows[0]);
  gtk_clist_set_row_data (clist, row, terrain_window);

  if (row == 1)
    {
    gtk_widget_set_sensitive (lookup_widget (main_window,"erode_flowmap"),TRUE);
    gtk_widget_set_sensitive (lookup_widget (main_window, "merge"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (main_window, "join"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (main_window, "warp"), TRUE);
    }
}

void
main_window_remove_terrain_window (GtkWidget *main_window,
                                   GtkWidget *terrain_window)
{
  gpointer data;
  GtkCList  *clist;
  int        row;

  clist = GTK_CLIST (lookup_widget (main_window, "main_list"));

  for (row = 0; (data = gtk_clist_get_row_data (clist, row)) != NULL; row++)
    {
      if (data == terrain_window)
        {
          gtk_clist_remove (clist, row);
          gtk_widget_destroy (terrain_window);
          break;
        }
    }

  for (row = 0; (data = gtk_clist_get_row_data (clist, row)) != NULL; row++)
    if (row == 2)
      break;

  if (row < 2)
    {
    gtk_widget_set_sensitive (lookup_widget (main_window,"erode_flowmap"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (main_window, "merge"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (main_window, "join"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (main_window, "warp"), FALSE);
    }
}

void
main_window_update_terrain_window (GtkWidget *main_window,
                                   GtkWidget *terrain_window)
{
  gpointer   data;
  GtkCList  *clist;
  int        row;
  gchar     *rows[2];

  clist = GTK_CLIST (lookup_widget (main_window, "main_list"));

  gtk_clist_freeze (clist);
  for (row = 0; (data = gtk_clist_get_row_data (clist, row)) != NULL; row++)
    {
      if (data == terrain_window)
        {
          gtk_clist_remove (clist, row);
          rows[0] = filename_get_base(terrain_window_get_title (terrain_window));
          rows[1] = NULL;
          gtk_clist_insert (clist, row, rows);
          gtk_clist_set_row_data (clist, row, terrain_window);
          g_free (rows[0]);
          break;
        }
    }
  gtk_clist_thaw (clist);
}

TTerrain *
main_window_get_terrain (GtkWidget *main_window,
                         gint       position)
{
  GtkCList  *clist;
  GtkObject *object;
  TTerrain  *terrain;

  clist = GTK_CLIST (lookup_widget (main_window, "main_list"));
  object = GTK_OBJECT (gtk_clist_get_row_data (clist, position));

  terrain = gtk_object_get_data (object, "data_t_terrain");
  t_terrain_ref (terrain);

  return terrain;
}

GtkWidget *
main_window_get_terrain_window (GtkWidget *main_window,
                                gint       position)
{
  GtkCList  *clist;

  clist = GTK_CLIST (lookup_widget (main_window, "main_list"));
  return GTK_WIDGET (gtk_clist_get_row_data (clist, position));
}

void
main_window_copy_objects (GtkWidget *main_window,
                          GtkCList  *list)
{
  gpointer   data;
  gint       row;
  GtkCList  *clist;
  gchar     *rows[2];

  rows[1] = NULL;
  clist = GTK_CLIST (lookup_widget (main_window, "main_list"));
  for (row = 0; (data = gtk_clist_get_row_data (clist, row)) != NULL; row++)
    {
      rows[0] = terrain_window_get_title (GTK_WIDGET (data));
      gtk_clist_append (list, rows);
      g_free (rows[0]);
      gtk_clist_set_row_data (list, row, data);
    }
}
