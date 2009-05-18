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


#include "contour.h"
#include "filterwindow.h"
#include "support.h"
#include "support2.h"
#include "terrainwindow.h"
#include "tterrainview.h"

/* Helper functions. */

/*
 * filter_window_update_preview: Update a filter's preview window
 */

void
filter_window_update_preview (GtkWidget *filter_window)
{
  GtkWidget    *view;
  TTerrain     *terrain;
  TPixbuf      *pixbuf;
  TTerrain     *reference;
  FilterApply   filter_apply;

  if (lookup_widget (filter_window, "use_preview") != NULL)
    if (!get_boolean (filter_window, "use_preview"))
      return;

  if (lookup_widget (filter_window, "preview") == NULL)
    return;

  view = lookup_widget (filter_window, "preview");
  pixbuf = t_canvas_get_pixbuf (T_CANVAS (view));
  reference = gtk_object_get_data (GTK_OBJECT (filter_window),
                                   "data_t_terrain_reference");

  if (reference != NULL)
    {
      terrain = t_terrain_clone (reference);
      t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), terrain);
    }
  else
    terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  filter_apply = gtk_object_get_data (GTK_OBJECT (filter_window),
                                      "data_filter_apply_func");
  if (filter_apply != NULL)
    filter_apply (filter_window, terrain);

  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  if (terrain != NULL)
    t_terrain_heightfield_modified (terrain);

  gtk_widget_queue_resize (filter_window);
}


/*
 * filter_window_perform: Perform filter on the terrain and close
 * the filter window
 */

void
filter_window_perform (GtkWidget *filter_window)
{
  FilterApply   filter_apply;
  TTerrainView *view;
  TTerrain     *terrain;
  TPixbuf      *pixbuf;
  GtkWidget    *terrain_window;
  GtkWidget    *main_window;
  gchar        *filter_name;

  terrain_window = gtk_object_get_data (GTK_OBJECT (filter_window),
                                        "data_parent");
  view = T_TERRAIN_VIEW (lookup_widget (terrain_window, "terrain"));
  terrain = t_terrain_view_get_terrain (view);
  pixbuf = t_canvas_get_pixbuf (T_CANVAS (view));
  main_window = gtk_object_get_data (GTK_OBJECT (terrain_window),
                                     "data_parent");

  filter_name = gtk_object_get_data (GTK_OBJECT (filter_window), "data_title");
  filter_apply = gtk_object_get_data (GTK_OBJECT (filter_window),
                                      "data_filter_apply_func");

  filter_apply (filter_window, terrain);

  if (gtk_object_get_data (GTK_OBJECT (filter_window), "data_add_undo"))
    terrain_window_save_state (terrain_window, filter_name);

  t_terrain_heightfield_modified (terrain);

  gtk_widget_destroy (filter_window);
}


/*
 * filter_window_new: create an RGB, a reference preview of
 * terrain_window's terrain, a working copy of the preview, set up
 * the data pointers appropriately using gtk_object_set_data,
 * create connections from the hscales to the updated function, and
 * show the window
 */

void
filter_window_new (GtkWidget         *a_popup_menu_widget,
                   CreateWindowFunc   create_window_func,
                   FilterApply        filter_apply_func,
                   gchar            **hscales,
                   gboolean           add_undo_state)
{
  GtkWidget *filter_window;

  filter_window = filter_window_new_extended (a_popup_menu_widget, 
                          create_window_func, filter_apply_func, hscales, 
                          NULL, NULL, add_undo_state);
}


GtkWidget *
filter_window_new_extended (GtkWidget         *a_popup_menu_widget,
                            CreateWindowFunc   create_window_func,
                            FilterApply        filter_apply_func,
                            gchar            **hscales,
                            gchar            **spinbuttons,
                            gchar            **checkboxes,
                            gboolean           add_undo_state)
{
  GtkWidget *terrain_window;
  GtkWidget *modal;

  terrain_window = lookup_toplevel (a_popup_menu_widget);

  modal = gtk_object_get_data (GTK_OBJECT (terrain_window), "data_modal");
  if (modal == NULL)
    {
      gint          i;
      GtkWidget    *filter_window;
      TTerrainView *view;
      TTerrain     *terrain;
      TPixbuf      *pixbuf;
      TTerrain     *terrain_preview;

      view = T_TERRAIN_VIEW (lookup_widget (terrain_window, "terrain"));
      terrain = t_terrain_view_get_terrain (view);
      pixbuf = t_canvas_get_pixbuf (T_CANVAS (view));

      gtk_widget_set_sensitive (terrain_window, FALSE);
      filter_window = create_window_func (terrain, terrain_window);
      gtk_object_set_data (GTK_OBJECT (filter_window),
                           "data_filter_apply_func", filter_apply_func);

      if (hscales != NULL)
        for (i = 0; hscales[i] != NULL; i++)
          {
            GtkWidget *hscale;

            hscale = lookup_widget (filter_window, hscales[i]);
            gtk_signal_connect_object (
              GTK_OBJECT (GTK_RANGE (hscale)->adjustment),
              "value_changed",
              GTK_SIGNAL_FUNC (filter_window_update_preview),
              GTK_OBJECT (filter_window));
          }

      if (spinbuttons != NULL)
        for (i = 0; spinbuttons[i] != NULL; i++)
          {
            GtkWidget *spinbtn;

            spinbtn = lookup_widget (filter_window, spinbuttons[i]);
            gtk_signal_connect_object (
              GTK_OBJECT (gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(spinbtn))),
              "value_changed",
              GTK_SIGNAL_FUNC (filter_window_update_preview),
              GTK_OBJECT (filter_window));
          }

      if (checkboxes != NULL)
        for (i = 0; checkboxes[i] != NULL; i++)
          {
            GtkWidget *checkbox;

            checkbox = lookup_widget (filter_window, checkboxes[i]);
            gtk_signal_connect_object (GTK_OBJECT (checkbox), "toggled",
             GTK_SIGNAL_FUNC (filter_window_update_preview),
             GTK_OBJECT (filter_window));
          }

      gtk_object_set_data (GTK_OBJECT (terrain_window), "data_modal",
                           filter_window);
      gtk_object_set_data_full (GTK_OBJECT (filter_window), "data_parent",
                                terrain_window, terrain_window_modal_destroy);

      terrain_preview = NULL;
      if (lookup_widget (filter_window, "preview") != NULL)
        {
          GtkWidget *preview;

          terrain_preview = t_terrain_clone_preview (terrain);
          t_terrain_ref (terrain_preview);
          gtk_object_set_data_full (GTK_OBJECT (filter_window),
                                    "data_t_terrain_reference",
                                    terrain_preview,
                                    (GtkDestroyNotify) t_terrain_unref);
          terrain_preview = t_terrain_clone (terrain_preview);
          preview = lookup_widget (filter_window, "preview");
          t_terrain_view_set_terrain (T_TERRAIN_VIEW (preview), terrain_preview);
        }

#if 0
DELETE THIS CRAP
      if (terrain_preview != NULL &&
          gtk_object_get_data (GTK_OBJECT (filter_window), "center_x") != NULL)
        t_terrain_view_set_model (view, T_VIEW_2D_PLANE);
#endif

      if (add_undo_state)
        gtk_object_set_data (GTK_OBJECT (filter_window),
                             "data_add_undo", filter_window);

      terrain_window_set_title (terrain_window, filter_window);
      filter_window_update_preview (filter_window);
      gtk_widget_show (filter_window);
      return filter_window;
    }

  return NULL;
}


static void
center_scales_changed (GtkWidget *scale_widget,
                       gpointer   user_data)
{
  GtkWidget    *filter_window;
  GtkWidget    *widget;
  TTerrainView *view;

  view = gtk_object_get_data (GTK_OBJECT (scale_widget), "data_view");
  filter_window = lookup_toplevel (scale_widget);

  widget = lookup_widget (filter_window, "center_x");
  if (widget != NULL && GTK_WIDGET_IS_SENSITIVE (widget) &&
      t_terrain_view_get_model (T_TERRAIN_VIEW (view)) == T_VIEW_2D_PLANE)
    {
      gchar  *center_x_name;
      gchar  *center_y_name;
      gchar  *use_preview_name;
      gfloat  center_x;
      gfloat  center_y;

      center_x_name =
        gtk_object_get_data (GTK_OBJECT (view), "data_center_x_name");
      center_y_name =
        gtk_object_get_data (GTK_OBJECT (view), "data_center_y_name");
      use_preview_name =
        gtk_object_get_data (GTK_OBJECT (view), "data_use_preview_name");

      if (use_preview_name != NULL)
        if (get_boolean (widget, use_preview_name) != TRUE)
          return;

      center_x = get_float (filter_window, center_x_name);
      center_y = get_float (filter_window, center_y_name);

      t_terrain_view_set_crosshair (T_TERRAIN_VIEW (view), center_x, center_y);
    }
  else
    t_terrain_view_set_crosshair (T_TERRAIN_VIEW (view), -1.0, -1.0);
}


static void
center_scales_state_changed (GtkWidget      *widget,
                             GtkStateType    previous_state)
{
  /* Treat state change as a change of the scales */

  if (previous_state == GTK_STATE_INSENSITIVE ||
      GTK_WIDGET_STATE (widget) == GTK_STATE_INSENSITIVE)
    {
      center_scales_changed (widget, NULL);
    }
}


static gboolean
view_button_press_event (GtkWidget       *widget,
                         GdkEventButton  *event,
                         gpointer         user_data)
{
  TTerrainView *view = T_TERRAIN_VIEW (widget);
  GtkWidget    *window;

  window = lookup_toplevel (widget);

  if (event->button == 1)
    {
      GtkWidget *center_x_widget;
      GtkWidget *center_y_widget;
      gchar     *center_x_name;
      gchar     *center_y_name;
      gchar     *use_preview_name;
      gdouble    center_x;
      gdouble    center_y;

      center_x_name =
        gtk_object_get_data (GTK_OBJECT (widget), "data_center_x_name");
      center_y_name =
        gtk_object_get_data (GTK_OBJECT (widget), "data_center_y_name");
      use_preview_name =
        gtk_object_get_data (GTK_OBJECT (widget), "data_use_preview_name");

      center_x_widget = lookup_widget (widget, center_x_name);
      center_y_widget = lookup_widget (widget, center_y_name);

      if (!GTK_WIDGET_IS_SENSITIVE (center_x_widget))
        return FALSE;

      if (use_preview_name != NULL)
        if (get_boolean (widget, use_preview_name) != TRUE)
          return FALSE;

      center_x = ((gfloat) event->x) / widget->allocation.width;
      center_y = ((gfloat) event->y) / widget->allocation.height;

      if (t_terrain_view_get_model (T_TERRAIN_VIEW (view)) == T_VIEW_2D_PLANE)
        {
          gtk_object_set_data (GTK_OBJECT (window), "dont_update_preview", window);
          set_float (window, center_x_name, center_x);
          gtk_object_remove_data (GTK_OBJECT (window), "dont_update_preview");
          set_float (window, center_y_name, center_y);
        }
    }

  return FALSE;
}


static void
use_preview_toggled                    (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *filter_window;
  GtkWidget *view;

  filter_window = lookup_toplevel (GTK_WIDGET (togglebutton));
  view = gtk_object_get_data (GTK_OBJECT (togglebutton), "data_view");

  if (gtk_toggle_button_get_active (togglebutton))
    {
      gchar     *center_x_name;
      GtkWidget *center_x_widget;

      center_x_name =
        gtk_object_get_data (GTK_OBJECT (view), "data_center_x_name");
      center_x_widget =
        lookup_widget (GTK_WIDGET (togglebutton), center_x_name);

      center_scales_changed (center_x_widget, NULL);
    }
  else
    {
      t_terrain_view_set_crosshair (T_TERRAIN_VIEW (view), -1.0, -1.0);
    }
}


void
filter_window_add_crosshairs (GtkWidget *filter_window,
                              gchar     *view_name,
                              gchar     *center_x_name,
                              gchar     *center_y_name,
                              gchar     *use_preview_name)
{
  GtkWidget *view;
  GtkWidget *center_x;
  GtkWidget *center_y;
  GtkWidget *use_preview;

  view = lookup_widget (filter_window, view_name);
  center_x = lookup_widget (filter_window, center_x_name);
  center_y = lookup_widget (filter_window, center_y_name);
  if (use_preview_name == NULL)
    use_preview = NULL;
  else
    use_preview = lookup_widget (filter_window, use_preview_name);

  /* Make sure widgets can find view */
  gtk_object_set_data (GTK_OBJECT (center_x), "data_view", view);
  gtk_object_set_data (GTK_OBJECT (center_y), "data_view", view);
  if (use_preview != NULL)
    gtk_object_set_data (GTK_OBJECT (use_preview), "data_view", view);

  /* Make sure view can find widgets */
  gtk_object_set_data_full (GTK_OBJECT (view), "data_center_x_name",
                            g_strdup (center_x_name),
                            (GtkDestroyNotify) g_free);
  gtk_object_set_data_full (GTK_OBJECT (view), "data_center_y_name",
                            g_strdup (center_y_name),
                            (GtkDestroyNotify) g_free);
  if (use_preview_name != NULL)
    gtk_object_set_data_full (GTK_OBJECT (view), "data_use_preview_name",
                              g_strdup (use_preview_name),
                              (GtkDestroyNotify) g_free);

  /* Attach callbacks */
  gtk_signal_connect_object (GTK_OBJECT (GTK_RANGE (center_x)->adjustment),
                             "value_changed",
                             GTK_SIGNAL_FUNC (center_scales_changed),
                             GTK_OBJECT (center_x));
  gtk_signal_connect (GTK_OBJECT (center_x),
                      "state_changed",
                      GTK_SIGNAL_FUNC (center_scales_state_changed),
                      NULL);
  gtk_signal_connect_object (GTK_OBJECT (GTK_RANGE (center_y)->adjustment),
                             "value_changed",
                             GTK_SIGNAL_FUNC (center_scales_changed),
                             GTK_OBJECT (center_y));
  gtk_signal_connect (GTK_OBJECT (center_y),
                      "state_changed",
                      GTK_SIGNAL_FUNC (center_scales_state_changed),
                      NULL);
  gtk_signal_connect_object (GTK_OBJECT (view),
                             "button_press_event",
                             GTK_SIGNAL_FUNC (view_button_press_event),
                             GTK_OBJECT (view));
  if (use_preview != NULL)
    gtk_signal_connect (GTK_OBJECT (use_preview),
                        "toggled",
                        GTK_SIGNAL_FUNC (use_preview_toggled),
                        NULL);

  /* Update crosshairs */
  center_scales_changed (center_x, NULL);
}
