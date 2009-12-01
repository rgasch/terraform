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

#include <gnome.h>
#include <libgnomeui/gnome-window-icon.h>
#include "contour.h"
#include "tterrain.h"
#include "tterrainview.h"
#include "interface.h"
#include "support.h"
#include "support2.h"
#include "mainwindow.h"
#include "terrainwindow.h"
#include "tundo.h"
#include "colormaps.h"
#include "filenameops.h"

static gchar *terrain_view_radio[] = {
  "terrain_2d_plane", "terrain_2d_contour",
  "terrain_3d_wire", "terrain_3d_height",
  "terrain_3d_light", NULL
};

static gchar *terrain_colormap_radio[] = {
  "terrain_land", "terrain_desert",
  "terrain_grayscale", "terrain_heat", 
  "terrain_wasteland", NULL
};

static void
on_terrain_configure_event (GtkWidget         *drawing_area,
                            GdkEventConfigure *event,
                            GtkWidget         *terrain_window)
{
  TTerrain *terrain;
  gint     *size;

  terrain = gtk_object_get_data (GTK_OBJECT (terrain_window), "data_t_terrain");

  /* Ignore move-only events.  All we care about are the resizes */
  size = gtk_object_get_data (GTK_OBJECT (terrain_window), "data_t_last_size");
  if (size == NULL || size[0] != event->width || size[1] != event->height)
    {
      size = g_new (gint, 2);
      size[0] = event->width;
      size[1] = event->height;
      gtk_object_set_data_full (GTK_OBJECT (terrain_window),
                                "data_t_last_size", size,
                                (GtkDestroyNotify) g_free);
    }
}

GtkWidget *
terrain_window_new (GtkWidget *main_window,
                    TTerrain  *terrain)
{
  GtkWidget *terrain_window;
  GtkWidget *drawing_area;
  MainState *main_state;
  TUndo     *undo;
  GtkWidget *aspect_frame;
  GtkWidget *terrain_widget;

  terrain_window = create_terrain_window ();

  /* Add pointer to terrain */
  aspect_frame = lookup_widget (terrain_window, "aspect_frame");
  terrain_widget = GTK_BIN (aspect_frame)->child;
  gtk_widget_ref (terrain_widget);
  gtk_object_set_data_full (GTK_OBJECT (terrain_window),
                            "terrain", terrain_widget,
                            (GtkDestroyNotify) gtk_widget_unref);

  gtk_object_set_data (GTK_OBJECT (terrain_window), "data_parent", main_window);
  gnome_window_icon_set_from_default (GTK_WINDOW (terrain_window));

  terrain_window_set_terrain (terrain_window, terrain);

  main_state = gtk_object_get_data (GTK_OBJECT (main_window), "data_state");
  undo = t_undo_new (main_state->max_undo);
  gtk_object_set_data_full (GTK_OBJECT (terrain_window),
                            "data_t_undo", undo,
                            (GtkDestroyNotify) t_undo_free);

  drawing_area = lookup_widget (terrain_window, "terrain");
  gtk_signal_connect (GTK_OBJECT (drawing_area), "configure_event",
                      GTK_SIGNAL_FUNC (on_terrain_configure_event),
                      terrain_window);

  main_window_add_terrain_window (main_window, GTK_WIDGET (terrain_window));
  terrain_window_unpack_menu (terrain_window);

  gtk_widget_show (terrain_window);

  return terrain_window;
}


static void
title_modified (GtkObject *terrain,
                gpointer   user_data)
{
  GtkWidget *terrain_window = GTK_WIDGET (user_data);
  GtkWidget *main_window = lookup_widget (terrain_window, "data_parent");

  terrain_window_set_title (terrain_window, terrain_window);
  main_window_update_terrain_window (main_window, terrain_window);
}

typedef struct DisconnectInfo DisconnectInfo;
struct DisconnectInfo
{
  GtkObject *object;
  guint      id;
};

static void
disconnect (DisconnectInfo *info)
{
  gtk_signal_disconnect (info->object, info->id);
  gtk_object_unref (info->object);
  g_free (info);
}


void
terrain_window_set_terrain (GtkWidget    *terrain_window,
                            TTerrain     *terrain)
{
  TTerrainView   *view;
  guint           id;
  DisconnectInfo *info;

  /* Set the Terrain's filename if it has none. */
  if (terrain->filename[0] == '\0')
    {
      GtkWidget *main_window;

      main_window =
        gtk_object_get_data (GTK_OBJECT (terrain_window), "data_parent");

      g_free (terrain->filename);
      terrain->filename = main_window_filename_new (main_window);
      terrain->autogenned = TRUE;
    }

  view = T_TERRAIN_VIEW (lookup_widget (terrain_window, "terrain"));

  id = gtk_signal_connect (GTK_OBJECT (terrain), "title_modified",
                           (GtkSignalFunc) &title_modified, terrain_window);

  info = g_new0 (DisconnectInfo, 1);
  info->object = GTK_OBJECT (terrain);
  info->id = id;
  gtk_object_ref (GTK_OBJECT (terrain));
  gtk_object_set_data_full (GTK_OBJECT (terrain_window),
                            "data_title_modified_id",
                            info,
                            (GtkDestroyNotify) disconnect);

  t_terrain_view_set_terrain (view, terrain);

  terrain_window_set_title (terrain_window, terrain_window);
}


gchar *
terrain_window_get_title (GtkWidget *terrain_window)
{
  TTerrain  *terrain;
  GtkWidget *view;

  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  return t_terrain_get_title (terrain);
}

void
terrain_window_set_title (GtkWidget    *terrain_window,
                          GtkWidget    *toplevel_window)
{
  gchar        *title;
  gchar        *title_affix;

  /* Set the window's title. */
  title = gtk_object_get_data (GTK_OBJECT (toplevel_window), "data_title");
  if (title == NULL)
    {
      title = g_strdup (GTK_WINDOW (toplevel_window)->title);

      gtk_object_set_data_full (GTK_OBJECT (toplevel_window), "data_title",
        title, g_free);
    }

  title_affix = terrain_window_get_title (terrain_window);
  title = g_strdup_printf ("%s: %s", title, title_affix);
  gtk_window_set_title (GTK_WINDOW (toplevel_window), title);
  g_free (title);
  g_free (title_affix);
}


/*
 * Add an undo state
 */

void
terrain_window_save_state (GtkWidget    *terrain_window,
                           gchar        *name)
{
  TUndo     *undo;
  GtkWidget *view;
  TTerrain  *terrain;

  undo = gtk_object_get_data (GTK_OBJECT (terrain_window), "data_t_undo");
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_undo_add_state (undo, terrain, name);
}

gboolean
terrain_window_exit_sequence (GtkWidget *terrain_window)
{
  GtkWidget *close_dialog;
  GtkWidget *label;
  GtkWidget *parent;
  GtkWidget *modal;
  GtkWidget *view;
  TTerrain  *terrain;
  gchar     *terrain_name;
  gchar     buf[1024];

  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  if (terrain->modified)
    {
      close_dialog = create_close_terrain_dialog ();
      terrain_name = terrain->filename;
      snprintf (buf, 1024, _("%s has been changed.\nWould you like to save the changes?"), filename_get_base(terrain_name));
      label = lookup_widget (close_dialog, "label");
      gtk_label_set_text (GTK_LABEL(label), buf);
      gtk_object_set_data (GTK_OBJECT (close_dialog),
                           "data_parent", terrain_window);
      gtk_widget_show (close_dialog);

      return TRUE;
    }

  modal = gtk_object_get_data (GTK_OBJECT (terrain_window), "data_modal");
  if (modal != NULL)
    gtk_widget_destroy (modal);

  parent = gtk_object_get_data (GTK_OBJECT (terrain_window), "data_parent");
  main_window_remove_terrain_window (parent, terrain_window);

  return FALSE;
}

void
terrain_window_modal_destroy (gpointer user_data)
{
  gtk_widget_set_sensitive (GTK_WIDGET (user_data), TRUE);
  gtk_object_remove_data (GTK_OBJECT (user_data), "data_modal");
}

void
terrain_window_popup_destroy (gpointer user_data)
{
  gtk_object_remove_data (GTK_OBJECT (user_data), "data_popup");
}

void
terrain_window_pack_menu (GtkWidget    *terrain_window)
{
  GtkWidget     *view;
  TTerrain      *terrain;
  TViewModel     model;
  TColormapType  colormap;
  gboolean       auto_rotate;
  gboolean       do_tile;

  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  model = get_radio_menu (terrain_window, terrain_view_radio);

  t_terrain_view_set_model (T_TERRAIN_VIEW (view), model);

  colormap = get_radio_menu (terrain_window, terrain_colormap_radio);
  t_terrain_view_set_colormap (T_TERRAIN_VIEW (view), colormap);

  auto_rotate = get_boolean (terrain_window, "terrain_auto_rotate");
  t_terrain_view_set_auto_rotate (T_TERRAIN_VIEW (view), auto_rotate);
  gtk_widget_set_sensitive (lookup_widget(terrain_window,"terrain_auto_rotate"),
                            T_TERRAIN_VIEW (view)->model == T_VIEW_3D_WIRE);

  do_tile = get_boolean (terrain_window, "terrain_do_tile");
  t_terrain_view_set_do_tile (T_TERRAIN_VIEW (view), do_tile);
  gtk_widget_set_sensitive (lookup_widget (terrain_window, "terrain_do_tile"),
      (T_TERRAIN_VIEW (view)->model != T_VIEW_2D_PLANE && 
       T_TERRAIN_VIEW (view)->model != T_VIEW_2D_CONTOUR)); 

  terrain_window_update_menu (terrain_window);

  if (T_TERRAIN_VIEW (view)->model == T_VIEW_2D_PLANE)
    gtk_widget_show (lookup_widget (terrain_window, "handlebox"));
  else
    gtk_widget_hide (lookup_widget (terrain_window, "handlebox"));
}

void
terrain_window_unpack_menu (GtkWidget    *terrain_window)
{
  GtkWidget *view;
  TTerrain  *terrain;

  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  if (T_TERRAIN_VIEW (view)->model != T_VIEW_3D_WIRE)
    gtk_widget_set_sensitive (
      lookup_widget (terrain_window, "terrain_auto_rotate"), FALSE);

  set_boolean (terrain_window, "terrain_auto_rotate", T_TERRAIN_VIEW (view)->auto_rotate);
  set_radio_menu (terrain_window, terrain_view_radio, T_TERRAIN_VIEW (view)->model);
  set_radio_menu (terrain_window, terrain_colormap_radio, T_TERRAIN_VIEW (view)->colormap);

  terrain_window_update_menu (terrain_window);
}

/* update menu items which can become in/sensitive */
void
terrain_window_update_menu (GtkWidget    *terrain_window)
{
  GtkWidget *view;
  GArray    *objects;
  GArray    *riversources;
  gboolean   do_rivers;
  gboolean   do_objects;

  view = lookup_widget (terrain_window, "terrain");
  objects = T_TERRAIN_VIEW(view)->terrain->objects;

  riversources = T_TERRAIN_VIEW(view)->terrain->riversources;
  do_rivers = (riversources && riversources->len>0);
  do_objects = (objects && objects->len>0);

  gtk_widget_set_sensitive (
		  lookup_widget (terrain_window, "prune_placed"), 
                  do_objects);
  gtk_widget_set_sensitive (
		  lookup_widget (terrain_window, "rescale_placed"), 
                  do_objects);
  gtk_widget_set_sensitive (
		  lookup_widget (terrain_window, "remove_all"), 
                  do_objects);
  gtk_widget_set_sensitive (
		  lookup_widget (terrain_window, "remove_selected"), 
                  do_objects);
  gtk_widget_set_sensitive (
                  lookup_widget (terrain_window, "remove_all_rivers"),
                  do_rivers);
  gtk_widget_set_sensitive (
                  lookup_widget (terrain_window, "redraw_rivers"),
                  do_rivers);
}

