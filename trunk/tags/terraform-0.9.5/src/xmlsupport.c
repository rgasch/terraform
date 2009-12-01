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

#include <string.h>
#include <stdlib.h>
#include "xmlsupport.h"
#include "clocale.h"

xmlNodePtr
xmlFindChild (xmlNodePtr     ptr,
              const xmlChar *name)
{
  ptr = ptr->children;
  while (ptr != NULL)
    {
      if (!strcmp (ptr->name, name))
        break;

      ptr = ptr->next;
    }

  return ptr;
}

void
xml_pack_float (xmlNodePtr     ptr,
                const xmlChar *setting,
                float          value)
{
  char buf[80];

  sprintf_C (buf, "%f", value);
  xmlNewChild (ptr, NULL, setting, buf);
}

void
xml_unpack_float (xmlNodePtr     ptr,
                  const xmlChar *name,
                  float         *value)
{
  xmlNodePtr setting;
  float      temp;

  setting = xmlFindChild (ptr, name);
  if (setting != NULL)
    {
      gchar *content;

      content = xmlNodeGetContent (setting);
      if (content != NULL)
        {
          if (sscanf_C (content, "%f", &temp) == 1)
            *value = temp;

          free (content);
        }
    }
}

void
xml_pack_int (xmlNodePtr     ptr,
              const xmlChar *setting,
              int            value)
{
  char buf[80];

  sprintf (buf, "%i", value);
  xmlNewChild (ptr, NULL, setting, buf);
}

void
xml_unpack_int (xmlNodePtr     ptr,
                const xmlChar *name,
                int           *value)
{
  xmlNodePtr setting;
  int        temp;

  setting = xmlFindChild (ptr, name);
  if (setting != NULL)
    {
      gchar *content;

      content = xmlNodeGetContent (setting);
      if (content != NULL)
        {
          if (sscanf (content, "%i", &temp) == 1)
            *value = temp;
          free (content);
        }
    }
}

void
xml_pack_boolean (xmlNodePtr     ptr,
                  const xmlChar *setting,
                  gboolean       value)
{
  xmlNewChild (ptr, NULL, setting, value ? "TRUE" : "FALSE");
}

void
xml_unpack_boolean (xmlNodePtr     ptr,
                    const xmlChar *name,
                    gboolean      *value)
{
  xmlNodePtr setting;

  setting = xmlFindChild (ptr, name);
  if (setting != NULL)
    {
      gchar *content;

      content = xmlNodeGetContent (setting);
      if (content != NULL)
        {
          if (!strcmp (content, "TRUE"))
            *value = TRUE;
          else if (!strcmp (content, "FALSE"))
            *value = FALSE;

          free (content);
        }
    }
}

void
xml_pack_string (xmlNodePtr     ptr,
                 const xmlChar *setting,
                 gchar         *value)
{
  xmlNewChild (ptr, NULL, setting, value);
}

void
xml_unpack_string (xmlNodePtr     ptr,
                   const xmlChar *name,
                   gchar        **value)
{
  xmlNodePtr setting;

  setting = xmlFindChild (ptr, name);
  if (setting != NULL)
    {
      gchar *content;

      g_free (*value);
      content = xmlNodeGetContent (setting);
      if (content == NULL)
        *value = g_strdup ("");
      else
        {
          *value = g_strdup (content);
          free (content);
        }
    }
}

void
xml_pack_prop_int (xmlNodePtr     ptr,
                   const xmlChar *setting,
                   gint           value)
{
  gchar buf[80];

  sprintf (buf, "%i", value);
  xmlSetProp (ptr, setting, buf);
}

void
xml_unpack_prop_int (xmlNodePtr     ptr,
                     const xmlChar *name,
                     gint          *value)
{
  gchar *setting;
  gint   temp;

  setting = xmlGetProp (ptr, name);
  if (setting != NULL && sscanf (setting, "%i", &temp) == 1)
    *value = temp;
}

void
xml_pack_prop_float (xmlNodePtr     ptr,
                     const xmlChar *setting,
                     gfloat         value)
{
  gchar buf[80];

  sprintf_C (buf, "%f", value);
  xmlSetProp (ptr, setting, buf);
}

void
xml_unpack_prop_float (xmlNodePtr     ptr,
                       const xmlChar *name,
                       gfloat        *value)
{
  gchar  *setting;
  gfloat  temp;

  setting = xmlGetProp (ptr, name);
  if (setting != NULL && sscanf_C (setting, "%f", &temp) == 1)
    *value = temp;
}
