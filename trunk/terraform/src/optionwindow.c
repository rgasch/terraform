/* Terraform - (C) 1997-2002 Robert Gasch (r.gasch@chello.nl)
 *  - http://terraform.sourceforge.net
 *  this file (c) 2002 by Koos Jan Niesink (k.j.niesink@students.geog.uu.nl
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
#include "optionwindow.h"
#include "support.h"
#include "support2.h"

gchar *
g_string_get_image_size (const gchar *input,
		   gboolean     width)
{
  /* this function takes a string as input 
     e.g 320x240
     when width = TRUE it returns 320
     otherwise 240 */

  gchar *x_sign;
  gint len, l = strlen (input);
  gchar buf[l];

  x_sign = strrchr (input, 'x');
  len = l - (x_sign - input);

  if (width)
      strncpy (buf, input, (x_sign - input));
  else
      strncpy (buf, x_sign + 1, len - 1);

  buf[len] = '\0';

  return g_strdup (buf);
}


void 
option_window_update_adjustments (GtkWidget  *options_window,
				  gchar   **hscales,
				  gchar   **spinbuttons)
{
  /* this function should be changed:
     in the Gimp there is a better function available */
  gint i;

  if (hscales != NULL)
    for (i = 0; hscales[i] != NULL; i++)
      {
	GtkWidget *hscale;
	
	hscale = lookup_widget (options_window, hscales[i]);
	gtk_signal_connect_object (
				   GTK_OBJECT (GTK_RANGE (hscale)->adjustment),
				   "value_changed",
				   GTK_SIGNAL_FUNC (update_spinbuttons),
				   GTK_OBJECT (options_window));
      }
  if (spinbuttons != NULL)
    for (i = 0; spinbuttons[i] != NULL; i++)
      {
	GtkWidget *spinbtn;

	spinbtn = lookup_widget (options_window, spinbuttons[i]);
	gtk_signal_connect_object (
				   GTK_OBJECT (gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(spinbtn))),
				   "value_changed",
				   GTK_SIGNAL_FUNC (update_hscales),
				   GTK_OBJECT (options_window));
      }
}

void 
update_spinbuttons (GtkWidget *options_window)
{
  gfloat  value_x, value_z, value_y;

  value_x = get_float (options_window, "render_scale_x");
  value_z = get_float (options_window, "render_scale_z");
  value_y = get_float (options_window, "render_y_scale_factor");
  set_float (options_window, "spinbutton_render_scale_x", value_x);
  set_float (options_window, "spinbutton_render_scale_z", value_z);
  set_float (options_window, "spinbutton_render_y_scale_factor", value_y);
}

void 
update_hscales (GtkWidget *options_window)
{
  gfloat value_x, value_z, value_y;

  value_x = get_float (options_window, "spinbutton_render_scale_x");
  value_z = get_float (options_window, "spinbutton_render_scale_z");
  value_y = get_float (options_window, "spinbutton_render_y_scale_factor");
  set_float (options_window, "render_scale_x", value_x);
  set_float (options_window, "render_scale_z", value_z);
  set_float (options_window, "render_y_scale_factor", value_y);
}

GList *
render_size_list_new ()
{
  GList *list;

  list = g_list_append (NULL, g_strdup ("320x240"));
  list = g_list_append (list, g_strdup ("512x384"));
  list = g_list_append (list, g_strdup ("640x480"));
  list = g_list_append (list, g_strdup ("800x600"));
  list = g_list_append (list, g_strdup ("1024x768"));
  list = g_list_append (list, g_strdup ("1152x864"));
  list = g_list_append (list, g_strdup ("1200x1024"));

  return list;
}
