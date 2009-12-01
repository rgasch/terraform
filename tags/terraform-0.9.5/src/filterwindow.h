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


#include <gtk/gtk.h>
#include "tterrain.h"

typedef GtkWidget* (*CreateWindowFunc) (TTerrain  *terrain, GtkWidget *terrain_window);
typedef void       (*FilterApply)      (GtkWidget *from_window,
                                        TTerrain  *to_terrain);

void filter_window_update_preview (GtkWidget         *filter_window);
void filter_window_perform        (GtkWidget         *filter_window);
void filter_window_new            (GtkWidget         *a_popup_menu_widget,
                                   CreateWindowFunc   create_window_func,
                                   FilterApply        filter_apply_func,
                                   gchar            **hscales,
                                   gboolean           add_undo_state);
GtkWidget *filter_window_new_extended (GtkWidget     *a_popup_menu_widget,
                                   CreateWindowFunc   create_window_func,
                                   FilterApply        filter_apply_func,
                                   gchar            **hscales,
                                   gchar            **spinbuttons,
                                   gchar            **checkboxes,
                                   gboolean           add_undo_state);
void filter_window_add_crosshairs (GtkWidget         *filter_window,
                                   gchar             *view_name,
                                   gchar             *center_x_name,
                                   gchar             *center_y_name,
                                   gchar             *use_preview_name);
