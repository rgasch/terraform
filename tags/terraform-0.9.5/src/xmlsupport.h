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


#include <libxml/tree.h>
#include <glib.h>

xmlNodePtr xmlFindChild          (xmlNodePtr     ptr,
                                  const xmlChar *name);
void       xml_pack_float        (xmlNodePtr     ptr,
                                  const xmlChar *setting,
                                  gfloat         value);
void       xml_unpack_float      (xmlNodePtr     ptr,
                                  const xmlChar *setting,
                                  gfloat        *value);
void       xml_pack_int          (xmlNodePtr     ptr,
                                  const xmlChar *setting,
                                  gint           value);
void       xml_unpack_int        (xmlNodePtr     ptr,
                                  const xmlChar *setting,
                                  gint          *value);
void       xml_pack_boolean      (xmlNodePtr     ptr,
                                  const xmlChar *setting,
                                  gint           value);
void       xml_unpack_boolean    (xmlNodePtr     ptr,
                                  const xmlChar *setting,
                                  gint          *value);
void       xml_pack_string       (xmlNodePtr     ptr,
                                  const xmlChar *setting,
                                  gchar         *value);
void       xml_unpack_string     (xmlNodePtr     ptr,
                                  const xmlChar *setting,
                                  gchar        **value);
void       xml_pack_prop_int     (xmlNodePtr     ptr,
                                  const xmlChar *setting,
                                  gint           value);
void       xml_unpack_prop_int   (xmlNodePtr     ptr,
                                  const xmlChar *setting,
                                  gint          *value);
void       xml_pack_prop_float   (xmlNodePtr     ptr,
                                  const xmlChar *setting,
                                  gfloat         value);
void       xml_unpack_prop_float (xmlNodePtr     ptr,
                                  const xmlChar *setting,
                                  gfloat        *value);
