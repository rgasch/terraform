/*  Terraform - (C) 1997-2002 Robert Gasch (r.gasch@chello.nl)
 *   - http://terraform.sourceforge.net
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
 *
 */


#include "selection.h"

void
t_terrain_select_invert (TTerrain *terrain)
{
  gint i;

  if (terrain->selection == NULL)
    terrain->selection = g_new0 (gfloat, terrain->width * terrain->height);

  for (i = 0; i < terrain->width * terrain->height; i++)
    terrain->selection[i] = 1.0 - terrain->selection[i];
}

void
t_terrain_select_all (TTerrain *terrain)
{
  gint i, size;

  if (terrain->selection == NULL)
    terrain->selection = g_new0 (gfloat, terrain->width * terrain->height);

  size = terrain->width * terrain->height;
  for (i = 0; i < size; i++)
    terrain->selection[i] = 1.0;
}

void
t_terrain_select_none (TTerrain *terrain)
{
  if (terrain->selection != NULL)
    {
      g_free (terrain->selection);
      terrain->selection = NULL;
    }
}

void
t_terrain_select_feather (TTerrain *terrain,
                          gint      radius)
{
  gint i, y, x, pos;
  gint size;

  if (terrain->selection == NULL)
    return;

  size = terrain->width * terrain->height;
  for (i = 0; i < radius; i++)
    {
      pos = 0;
      for (y = 0; y < terrain->height; y++)
        for (x = 0; x < terrain->width; x++)
          {
            gint   count;
            gfloat average;

            count = 1;
            average = terrain->selection[pos];

            if (x > 0)
              average += terrain->selection[pos - 1], count++;

            if (x < terrain->width - 1)
              average += terrain->selection[pos + 1], count++;

            if (y > 0)
              average += terrain->selection[pos - terrain->width], count++;

            if (y < terrain->height - 1)
              average += terrain->selection[pos + terrain->width], count++;

            terrain->selection[pos] = average / count;
            pos++;
          }
   }
}
