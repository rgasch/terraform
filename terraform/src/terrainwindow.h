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

#define TERRAIN_MIN_VIEW 300

GtkWidget *terrain_window_new              (GtkWidget    *main_window,
                                            TTerrain     *terrain);
void       terrain_window_set_terrain      (GtkWidget    *terrain_window,
                                            TTerrain     *terrain);
gchar     *terrain_window_get_title        (GtkWidget    *terrain_window);
void       terrain_window_set_title        (GtkWidget    *terrain_window,
                                            GtkWidget    *toplevel_window);

void       terrain_window_save_state       (GtkWidget    *terrain_window,
                                            gchar        *name);
gboolean   terrain_window_exit_sequence    (GtkWidget    *terrain_window);
void       terrain_window_modal_destroy    (gpointer      user_data);
void       terrain_window_popup_destroy    (gpointer      user_data);
void       terrain_window_pack_menu        (GtkWidget    *terrain_window);
void       terrain_window_unpack_menu      (GtkWidget    *terrain_window);
void       terrain_window_update_menu      (GtkWidget    *terrain_window);
