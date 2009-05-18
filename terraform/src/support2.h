/*  Physically Modeled Media Plug-In for The GIMP
 *  Copyright (c) 2000-2001 David A. Bartold
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

GtkWidget *lookup_toplevel     (GtkWidget *widget);
gpointer   lookup_data         (GtkWidget *widget,
                                gchar     *name);
gfloat     get_float           (GtkWidget *options,
                                gchar     *name);
gint       get_int             (GtkWidget *options,
                                gchar     *name);
gboolean   get_boolean         (GtkWidget *options,
                                gchar     *name);
gint       get_option          (GtkWidget *options,
                                gchar     *name);
gchar     *get_option_text     (GtkWidget *options,
                                gchar     *name);
gint       get_radio_menu      (GtkWidget *menu,
                                gchar    **options);
gchar     *get_text            (GtkWidget *widget,
                                gchar     *name);
void       set_float           (GtkWidget *options,
                                gchar     *name,
                                gfloat     value);
void       set_int             (GtkWidget *options,
                                gchar     *name,
                                gint       value);
void       set_boolean         (GtkWidget *options,
                                gchar     *name,
                                gboolean   value);
void       set_option          (GtkWidget *options,
                                gchar     *name,
                                gint       value);
void       set_radio_menu      (GtkWidget *menu,
                                gchar    **options,
                                gint       value);
void       set_radio_toolbar   (GtkWidget *toolbar,
                                gchar    **options,
                                GtkWidget *value);
void       set_text            (GtkWidget *widget,
                                gchar     *name,
                                gchar     *value);
void       widget_remove_timer (GtkWidget *widget);
void       widget_set_timer    (GtkWidget   *widget,
                                guint32      interval,
                                GtkFunction  function);
void       hscale_callbacks    (GtkWidget      *widget,
                                gchar         **hscales,
                                GtkSignalFunc   func);

