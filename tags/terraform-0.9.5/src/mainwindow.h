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


#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <glib.h>
#include "tterrain.h"

typedef struct MainState MainState;

struct MainState
{
  gchar  *povray_executable;
  gint    max_undo;
  gint    default_size;
  gint    gentypes;
  gfloat  gamma;
  gfloat  genfactor;
};

MainState *main_state_new                    (void);
void       main_state_free                   (MainState *state);

void       main_window_state_pack_options    (GtkWidget *main_window,
                                              GtkWidget *option_window);
void       main_window_state_unpack_options  (GtkWidget *main_window,
                                              GtkWidget *option_window);
MainState *main_window_state_load            (gchar     *filename);
void       main_window_state_save            (MainState *state,
                                              gchar     *filename);
void       main_window_initialize            (GtkWidget *main_window);
gboolean   main_window_exit_sequence         (GtkWidget *main_window);
gchar     *main_window_filename_new          (GtkWidget *main_window);
void       main_window_add_terrain_window    (GtkWidget *main_window,
                                              GtkWidget *terrain_window);
void       main_window_remove_terrain_window (GtkWidget *main_window,
                                              GtkWidget *terrain_window);
void       main_window_update_terrain_window (GtkWidget *main_window,
                                              GtkWidget *terrain_window);
TTerrain  *main_window_get_terrain           (GtkWidget *main_window,
                                              gint       position);
GtkWidget *main_window_get_terrain_window    (GtkWidget *main_window,
                                              gint       position);
void       main_window_copy_objects          (GtkWidget *main_window,
                                              GtkCList  *list);

#endif /* __MAINWINDOW_H__ */
