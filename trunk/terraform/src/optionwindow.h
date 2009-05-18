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

gchar     *g_string_get_image_size           (const gchar *input,
					      gboolean     width);
void       option_window_update_adjustments  (GtkWidget  *options_window,
					      gchar     **hscales,
					      gchar     **spinbuttons);
void       update_spinbuttons                (GtkWidget  *options_window);
void       update_hscales                    (GtkWidget  *options_window);
GList     *render_size_list_new              (void);
