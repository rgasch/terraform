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
 *
 * The contour line code provided in this file is
 * Copyright (c) 2000 David A. Bartold
 */


#include <math.h>
#include <stdlib.h>
#include "filters.h"
#include "trandom.h"
#include "contour.h"


typedef struct Edgemap Edgemap;
struct Edgemap
{
  gint    width;
  gint    height;
  guchar *data;
};

static Edgemap *
edgemap_new (gint width, gint height)
{
  Edgemap *map;

  map = g_new (Edgemap, 1);
  map->width = width;
  map->height = height;
  map->data = g_new0 (guchar, width * height);

  return map;
}

static void
edgemap_free (Edgemap *map)
{
  g_free (map->data);
  g_free (map);
}

typedef struct Edge Edge;
struct Edge
{
  gint x, y, bit;
};


/*
 * Edge map: Each element of the edgemap corresponds to exactly one pel in the
 * bitmap.  The bit(s) set provide information about the edges for each pixel.
 * An element can have one of seven values:
 *   * 0     (no edge above or to the left of a pixel)
 *   * EDGE1 (a black pixel has a white one above it)
 *   * EDGE2 (a black pixel has a white one to the left of it)
 *   * EDGE1 | EDGE2 (both conditions for EDGE1 and EDGE2 exist)
 *   * or, subsititute EDGE3 for EDGE1 and EDGE4 for EDGE2 for a white pixel
 *
 * A typical edgemap submatrix:
 *  _______ _______ _____
 * |       |       |    /             ####
 * |       | | ### | |  |             ####  Black Pixel
 * |       | | ### | |  \             ####
 * |_______|_______|____/
 * |  ____ |       |    \                |
 * |  #### |  #### | |   |    ----  and  |  Edges
 * |  #### |  #### | |  /                |
 * |_______|_______|___|
 * |       |       |  .'
 * `--~~--~ ~-___-~~-~
 *
 *  Edge 1    Edge 2    Edge 3    Edge 4
 *   ___        .        ___        ,
 *  |   |     __|__     |###|     __|__
 *__|___|__  |  |oo|  __|###|__  |##|::|
 *  |ooo|    |__|oo|    |:::|    |##|::|
 *  |ooo|       |       |:::|       |
 *              '                   '
 * ':' = White heightfield pel
 * ' ' = White pel on the other side
 * 'o' = Black heightfield pel
 * '#' = Black pel on the other side
 */

#define EDGE1 1
#define EDGE2 2
#define EDGE3 4
#define EDGE4 8

#define UP    0
#define RIGHT 1
#define DOWN  2
#define LEFT  3

/* Turn a bitmap into an edgemap */
static void
to_edgemap (guchar *bitmap, gint level, Edgemap *edgemap)
{
  gint i, x, y;

  i = 0;
  for (y = 0; y < edgemap->height; y++)
    for (x = 0; x < edgemap->width; x++)
      {
        if (bitmap[i] > level) /* White */
          {
            if (y > 0 && bitmap[i - edgemap->width] <= level)
              edgemap->data[i] |= EDGE3;
            if (x > 0 && bitmap[i - 1] <= level)
              edgemap->data[i] |= EDGE4;
          }
        else /* Black */
          {
            if (y > 0 && bitmap[i - edgemap->width] > level)
              edgemap->data[i] |= EDGE1;
            if (x > 0 && bitmap[i - 1] > level)
              edgemap->data[i] |= EDGE2;
          }

        i++;
      }
}

/* Is an edge within the bounds of an edgemap? */
static gboolean
within_bounds (Edgemap *map, Edge *edge)
{
  if (edge->x < 0 || edge->y < 0 ||
      edge->x >= map->width || edge->y >= map->height)
    return FALSE;

  if (edge->x == 0 && (edge->bit == EDGE2 || edge->bit == EDGE4))
    return FALSE;

  if (edge->y == 0 && (edge->bit == EDGE1 || edge->bit == EDGE3))
    return FALSE;

  return TRUE;
}

/* Convert an edge to the nearest floating point x y coordinates */
static void
to_float (Edge *edge, gfloat *out_x, gfloat *out_y)
{
  if (edge->bit == EDGE2 || edge->bit == EDGE4)
    {
      *out_x = edge->x;
      *out_y = edge->y + 0.5;
      return;
    }

  *out_x = edge->x + 0.5;
  *out_y = edge->y;
}

/* Is the edge within bounds and is it recorded in the edgemap? */
static gboolean
move_ok (Edgemap *map, Edge *edge)
{
  if (within_bounds (map, edge))
    return (map->data[edge->y * map->width + edge->x] & edge->bit) ? 1 : 0;

  return FALSE;
}

/* Convert a white pixel edge to a black pixel edge and vice versa */
static gint
invert_color (gint edge)
{
  if (edge == EDGE1)
    return EDGE3;
  else if (edge == EDGE2)
    return EDGE4;
  else if (edge == EDGE3)
    return EDGE1;
  else if (edge == EDGE4)
    return EDGE2;

  return -1;
}

/* Convert a top border to a side border and vice versa */
static gint
invert_side (gint edge)
{
  if (edge == EDGE1)
    return EDGE2;
  else if (edge == EDGE2)
    return EDGE1;
  else if (edge == EDGE3)
    return EDGE4;
  else if (edge == EDGE4)
    return EDGE3;

  return -1;
}

/* Perform invert_side (invert_color (edge)) */
static gint
invert_side_and_color (gint edge)
{
  if (edge == EDGE1)
    return EDGE4;
  else if (edge == EDGE2)
    return EDGE3;
  else if (edge == EDGE3)
    return EDGE2;
  else if (edge == EDGE4)
    return EDGE1;

  return -1;
}

/* Reverse a cardinal direction UP <-> DOWN, LEFT <-> RIGHT */
static gint
reverse_direction (gint dir)
{
  return (dir + 2) & 3;
}

/*
 * Move in the specified direction starting with the given edge and
 * the last direction
 */

static void
move (Edgemap *map, gint last_dir, gint cur_dir, Edge *in, Edge *out)
{
#if DEBUG_CONTOUR
  if (last_dir == LEFT || last_dir == RIGHT)
    g_return_if_fail (in->bit == EDGE1 || in->bit == EDGE3);
  if (last_dir == UP || last_dir == DOWN)
    g_return_if_fail (in->bit == EDGE2 || in->bit == EDGE4);
  g_return_if_fail (last_dir != reverse_direction (cur_dir));
#endif

  *out = *in;

  if (last_dir == UP)
    {
      if (cur_dir == RIGHT)
        {
          out->bit = invert_side (in->bit);
        }
      else if (cur_dir == LEFT)
        {
          out->bit = invert_side_and_color (in->bit);
          out->x--;
        }
      else if (cur_dir == UP)
        {
          out->y--;
        }
    }
  else if (last_dir == DOWN)
    {
      if (cur_dir == RIGHT)
        {
          out->bit = invert_side_and_color (in->bit);
          out->y++;
        }
      else if (cur_dir == LEFT)
        {
          out->bit = invert_side (in->bit);
          out->x--;
          out->y++;
        }
      else if (cur_dir == DOWN)
        {
          out->y++;
        }
    }
  else if (last_dir == LEFT)
    {
      if (cur_dir == UP)
        {
          out->bit = invert_side_and_color (in->bit);
          out->y--;
        }
      else if (cur_dir == DOWN)
        {
          out->bit = invert_side (in->bit);
        }
      else if (cur_dir == LEFT)
        {
          out->x--;
        }
    }
  else if (last_dir == RIGHT)
    {
      if (cur_dir == UP)
        {
          out->bit = invert_side (in->bit);
          out->y--;
          out->x++;
        }
      else if (cur_dir == DOWN)
        {
          out->bit = invert_side (invert_color (in->bit));
          out->x++;
        }
      else if (cur_dir == RIGHT)
        {
          out->x++;
        }
    }
}

/* Follow an edge forward (i.e. clockwise) */
static GList *
follow_forward (Edgemap  *map,
                Edge     *start_edge,
                gint      start_dir,
                gboolean *closed)
{
  GList *list;
  Edge edge, temp;
  gint dir;

  list = NULL;
  edge = *start_edge;
  dir = start_dir;
  while (1)
    {
      gint   i;
      gint   last_dir;
      gfloat *position;

      last_dir = dir;
      for (i = 1; i <= 4; i++)
        {
          dir = (dir + 1) & 3;

          if (i != 2)
            {
              move (map, last_dir, dir, &edge, &temp);
              if (move_ok (map, &temp))
                {
                  edge = temp;

                  break;
                }
            }
        }

      if (i == 5)
        {
          *closed = FALSE;
          return list;
        }

      /* Erase edge and add it to the list */
      map->data[edge.y * map->width + edge.x] &= ~edge.bit;
      position = g_new (gfloat, 2);
      to_float (&edge, &position[0], &position[1]);
      list = g_list_append (list, position);

      if (edge.x == start_edge->x && edge.y == start_edge->y &&
          edge.bit == start_edge->bit)
        {
          *closed = TRUE;
          return list;
        }
    }
}

/* Follow an edge backwards (i.e. counter-clockwise) */
static GList *
follow_backward (GList   *list,
                 Edgemap *map,
                 Edge    *start_edge,
                 gint     start_dir)
{
  Edge edge, temp;
  gint dir;

  edge = *start_edge;
  dir = reverse_direction (start_dir);
  while (1)
    {
      gint    i;
      gint    last_dir;
      gfloat *position;

      /* Erase edge and add it to the list */
      map->data[edge.y * map->width + edge.x] &= ~edge.bit;
      position = g_new (gfloat, 2);
      to_float (&edge, &position[0], &position[1]);
      list = g_list_prepend (list, position);

      last_dir = dir;
      for (i = 1; i <= 4; i++)
        {
          dir = (dir + 3) & 3;

          if (i != 2)
            {
              move (map, last_dir, dir, &edge, &temp);
              if (move_ok (map, &temp))
                {
                  edge = temp;

                  break;
                }
            }
        }

      if (i == 5)
        {
          return list;
        }

      if (edge.x == start_edge->x && edge.y == start_edge->y &&
          edge.bit == start_edge->bit)
        {
          g_assert_not_reached ();
          return list;
        }
    }
}

/* Trace a line starting with the edge(s) at (x, y) */
static GList *
trace_at (Edgemap *map, gint x, gint y, gboolean *closed)
{
  GList *list;
  Edge   start_edge;
  gint   start_dir;
  gint   bit;

  start_edge.x = x;
  start_edge.y = y;
  bit = map->data[y * map->width + x];

  /* Pick one edge, preferring the top edge (if more than one) */
  if (bit & EDGE1)
    {
      start_edge.bit = EDGE1;
      start_dir = RIGHT;
    }
  else if (bit & EDGE3)
    {
      start_edge.bit = EDGE3;
      start_dir = RIGHT;
    }
  else if (bit & EDGE2)
    {
      start_edge.bit = EDGE2;
      start_dir = DOWN;
    }
  else if (bit & EDGE4)
    {
      start_edge.bit = EDGE4;
      start_dir = DOWN;
    }
  else
    {
      start_edge.bit = -1;
      start_dir = -1;
      g_assert_not_reached ();
    }

  list = follow_forward (map, &start_edge, start_dir, closed);
  if (!*closed)
    list = follow_backward (list, map, &start_edge, start_dir);

  return list;
}

/* Delete the unsimplified trace GList */
static void
trace_list_free (GList *list)
{
  GList *pos;

  pos = g_list_first (list);
  while (pos != NULL)
    {
      g_free (pos->data);
      pos = pos->next;
    }

  g_list_free (list);
}

/*
 * Simplify a contour edge list by averaging edge positions.
 * Use the resolution to determine how many to average at one time.
 */

static GArray *
simplify (GList *points, gfloat level, gint resolution, gboolean closed)
{
  GArray *array;
  gfloat  average_x, average_y;
  gint    count;

  array = g_array_new (FALSE, FALSE, sizeof (gfloat));
  array = g_array_append_val (array, level);

  average_x = average_y = 0.0;
  count = 0;
  points = g_list_first (points);
  while (points != NULL)
    {
      gfloat *pos;

      pos = points->data;
      average_x += pos[0];
      average_y += pos[1];
      count++;

      points = points->next;

      if (count == resolution || points == NULL)
        {
          average_x /= count;
          average_y /= count;

          array = g_array_append_val (array, average_x);
          array = g_array_append_val (array, average_y);

          average_x = 0.0;
          average_y = 0.0;
          count = 0;
        }
    }

  if (closed && array->len > 5)
    {
      gfloat x, y;

      x = g_array_index (array, gfloat, 1);
      y = g_array_index (array, gfloat, 2);

      array = g_array_append_val (array, x);
      array = g_array_append_val (array, y);
    }

  if (array->len < 9)
    {
      g_array_free (array, TRUE);
      return NULL;
    }

  return array;
}

/* Free a contour list */
void
t_terrain_contour_list_free (GList *contour)
{
  GList *pos;

  pos = g_list_first (contour);
  while (pos != NULL)
    {
      g_array_free (pos->data, TRUE);

      pos = pos->next;
    }

  g_list_free (contour);
}

/* Generate/update the contour lines for a terrain */
GList *
t_terrain_contour_lines (TTerrain *terrain,
                         gint      level_count,
                         gint      resolution)
{
  gint     i, x, y, pos, size;
  gfloat   level;
  guchar  *bitmap;
  Edgemap *edgemap;
  GList   *contour_list = NULL;

  g_return_val_if_fail (level_count > 0, NULL);

  bitmap = g_new (guchar, terrain->width * terrain->height);
  edgemap = edgemap_new (terrain->width, terrain->height);

  /* Create a bitmap using level as the threshold */
  level = 1.0 / level_count;
  size = terrain->width * terrain->height;
  for (pos = 0; pos < size; pos++)
    bitmap[pos] = terrain->heightfield[pos] / level + 0.5;

  for (i = 0; i < level_count; i++)
    {
      /* Convert to an edgemap */
      to_edgemap (bitmap, i, edgemap);

      pos = 0;
      for (y = 0; y < terrain->height; y++)
        {
          for (x = 0; x < terrain->width; x++)
            {
              /* Is there an edge at the given pixel location? */
              if (edgemap->data[pos] != 0)
                {
                  gboolean  closed;
                  GList    *list;
                  GArray   *simple_array;

                  /* Trace, simplify, and append the isogram */
                  list = trace_at (edgemap, x, y, &closed);
                  simple_array = simplify (list, level, resolution, closed);
                  trace_list_free (list);
                  if (simple_array != NULL)
                    contour_list = g_list_append (contour_list, simple_array);
                }

              pos++;
            }
        }
    }

  g_free (bitmap);
  edgemap_free (edgemap);

  return contour_list;
}
