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


#include <glib.h>

GList *texture_list_new  (void);
GList *object_list_new (void);
GList *cloud_list_new (void);
GList *atmosphere_list_new (void);
GList *star_list_new (void);
GList *water_list_new (void);
GList *light_list_new (void);
GList *scene_theme_list_new (void);
GList *skycolor_list_new (void);
GList *background_image_list_new (void);
GList *moon_image_list_new (void);

void   file_list_free  (GList *list);
GList *file_list_copy  (GList *list);
GList *trace_object_list_new (void);

