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

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tpixbuf.h"

#ifndef G_PI
#define G_PI 3.14159265358979323846264
#endif

#define SQR(x) ((x)*(x))

gint gamma_curve[32];

/* t_pixbuf_new: Create a new null-sized pixbuf */
TPixbuf *
t_pixbuf_new ()
{
  TPixbuf *pixbuf;
  gint     i;

  for (i = 0; i < 32; i++)
    gamma_curve[i] =
      (gint) (pow ((i + 0.5) / 32.0, 1.0 / 1.8) * 31.0 + 0.5);

  pixbuf = g_new0 (TPixbuf, 1);
  pixbuf->fore[0] = 0xff;
  pixbuf->fore[1] = 0xff;
  pixbuf->fore[2] = 0xff;

  return pixbuf;
}

/* t_pixbuf_free: Delete the pixbuf */
void
t_pixbuf_free (TPixbuf *pixbuf)
{
  g_free (pixbuf->data);
  g_free (pixbuf);
}

/* t_pixbuf_set_size: Resize the pixbuf */
void
t_pixbuf_set_size (TPixbuf  *pixbuf,
                   gint      width,
                   gint      height)
{
  if (width != pixbuf->width || height != pixbuf->height)
    {
      g_free (pixbuf->data);
      if (width == 0 || height == 0)
        pixbuf->data = NULL;
      else
        pixbuf->data = g_new (guchar, width * height * 3);
      pixbuf->width = width;
      pixbuf->height = height;
      t_pixbuf_clear (pixbuf);
    }
}

void
t_pixbuf_set_fore (TPixbuf *pixbuf,
                   gint     red,
                   gint     green,
                   gint     blue)
{
  pixbuf->fore[0] = red;
  pixbuf->fore[1] = green;
  pixbuf->fore[2] = blue;
}

void
t_pixbuf_set_back (TPixbuf *pixbuf,
                   gint     red,
                   gint     green,
                   gint     blue)
{
  pixbuf->back[0] = red;
  pixbuf->back[1] = green;
  pixbuf->back[2] = blue;
}

void
t_pixbuf_draw_raster (TPixbuf *pixbuf,
                      gint     y,
                      guchar  *data)
{
  if (y >= 0 && y < pixbuf->height)
    memcpy (&pixbuf->data[y * pixbuf->width * 3], data, pixbuf->width * 3);
}

void
t_pixbuf_draw_pel (TPixbuf  *pixbuf,
                   gint      x,
                   gint      y)
{
  if (x >= 0 && y >= 0 && x < pixbuf->width && y < pixbuf->height)
    {
      guchar *pos;

      pos = &pixbuf->data[(y * pixbuf->width + x) * 3];
      pos[0] = pixbuf->fore[0];
      pos[1] = pixbuf->fore[1];
      pos[2] = pixbuf->fore[2];
    }
}

void
t_pixbuf_get_pel (TPixbuf *pixbuf,
                  gint     x,
                  gint     y,
                  gint    *r,
                  gint    *g,
                  gint    *b)
{
  if (x >= 0 && y >= 0 && x < pixbuf->width && y < pixbuf->height)
    {
      guchar *pos;

      pos = &pixbuf->data[(y * pixbuf->width + x) * 3];
      *r = pos[0];
      *g = pos[1];
      *b = pos[2];
    }
  else
    {
      *r = 0;
      *g = 0;
      *b = 0; 
    }
}

void
t_pixbuf_draw_line (TPixbuf *pixbuf,
                    gint     x1,
                    gint     y1,
                    gint     x2,
                    gint     y2)
{
  gint x, y;
  gint count;
  gint delta_x, delta_y;
  gint dir_x, dir_y;

  delta_x = x2 - x1;
  if (delta_x < 0) delta_x = -delta_x, dir_x = -1; else dir_x = 1;

  delta_y = y2 - y1;
  if (delta_y < 0) delta_y = -delta_y, dir_y = -1; else dir_y = 1;

  x = x1; 
  y = y1;
  count = 0;
  if (delta_x > delta_y)
    {
      for (; x != x2 + dir_x; x += dir_x)
        {
          count += delta_y;
          if (count > delta_x) count -= delta_x, y += dir_y;

          if (x >= 0 && y >= 0 && x < pixbuf->width && y < pixbuf->height)
            {
              guchar *pos;

              pos = &pixbuf->data[(y * pixbuf->width + x) * 3];
              pos[0] = pixbuf->fore[0];
              pos[1] = pixbuf->fore[1];
              pos[2] = pixbuf->fore[2];
            }
        }
    }
  else
    {
      for (; y != y2 + dir_y; y += dir_y)
        {
          count += delta_x;
          if (count > delta_y) count -= delta_y, x += dir_x;

          if (x >= 0 && y >= 0 && x < pixbuf->width && y < pixbuf->height)
            {
              guchar *pos;

              pos = &pixbuf->data[(y * pixbuf->width + x) * 3];
              pos[0] = pixbuf->fore[0];
              pos[1] = pixbuf->fore[1];
              pos[2] = pixbuf->fore[2];
            }
        }
    }
}

void
t_pixbuf_draw_line_add (TPixbuf *pixbuf,
                        gint     x1,
                        gint     y1,
                        gint     x2,
                        gint     y2)
{
  gint x, y;
  gint count;
  gint delta_x, delta_y;
  gint dir_x, dir_y;

  delta_x = x2 - x1;
  if (delta_x < 0) delta_x = -delta_x, dir_x = -1; else dir_x = 1;

  delta_y = y2 - y1;
  if (delta_y < 0) delta_y = -delta_y, dir_y = -1; else dir_y = 1;

  x = x1; 
  y = y1;
  count = 0;
  if (delta_x > delta_y)
    {
      for (; x != x2 + dir_x; x += dir_x)
        {
          count += delta_y;
          if (count > delta_x) count -= delta_x, y += dir_y;

          if (x >= 0 && y >= 0 && x < pixbuf->width && y < pixbuf->height)
            {
              guchar *pos;

              pos = &pixbuf->data[(y * pixbuf->width + x) * 3];
              pos[0] = MIN (pos[0] + pixbuf->fore[0], 255);
              pos[1] = MIN (pos[1] + pixbuf->fore[1], 255);
              pos[2] = MIN (pos[2] + pixbuf->fore[2], 255);
            }
        }
    }
  else
    {
      for (; y != y2 + dir_y; y += dir_y)
        {
          count += delta_x;
          if (count > delta_y) count -= delta_y, x += dir_x;

          if (x >= 0 && y >= 0 && x < pixbuf->width && y < pixbuf->height)
            {
              guchar *pos;

              pos = &pixbuf->data[(y * pixbuf->width + x) * 3];
              pos[0] = MIN (pos[0] + pixbuf->fore[0], 255);
              pos[1] = MIN (pos[1] + pixbuf->fore[1], 255);
              pos[2] = MIN (pos[2] + pixbuf->fore[2], 255);
            }
        }
    }
}

void
t_pixbuf_draw_line_add_aa (TPixbuf *pixbuf,
                           gint     x1,
                           gint     y1,
                           gint     x2,
                           gint     y2)
{
  gint x, y;
  gint count;
  gint delta_x, delta_y;
  gint dir_x, dir_y;

  delta_x = x2 - x1;
  if (delta_x < 0) delta_x = -delta_x, dir_x = -1; else dir_x = 1;

  delta_y = y2 - y1;
  if (delta_y < 0) delta_y = -delta_y, dir_y = -1; else dir_y = 1;

  x = x1; 
  y = y1;
  count = 0;
  if (delta_x > delta_y)
    {
      /* Mostly horizontal */
      for (; x != x2 + dir_x; x += dir_x)
        {
          if (x >= 0 && x < pixbuf->width)
            {
              guchar *pos;
              gint    y2;
              gint    alpha = 32 * count / delta_x;
              gint    value;

              pos = &pixbuf->data[(y * pixbuf->width + x) * 3];
              if (y >= 0 && y < pixbuf->height)
                {
                  value = gamma_curve[31 - alpha];
                  // value = 31 - alpha;

                  pos[0] = MIN (pos[0] + ((pixbuf->fore[0] * value) >> 5), 255);
                  pos[1] = MIN (pos[1] + ((pixbuf->fore[1] * value) >> 5), 255);
                  pos[2] = MIN (pos[2] + ((pixbuf->fore[2] * value) >> 5), 255);
                }

              y2 = y + dir_y;
              pos += dir_y * pixbuf->width * 3;
              if (y2 >= 0 && y2 < pixbuf->height)
                {
                  value = gamma_curve[alpha];
                  // value = alpha;

                  pos[0] = MIN (pos[0] + ((pixbuf->fore[0] * value) >> 5), 255);
                  pos[1] = MIN (pos[1] + ((pixbuf->fore[1] * value) >> 5), 255);
                  pos[2] = MIN (pos[2] + ((pixbuf->fore[2] * value) >> 5), 255);
                }
            }

          count += delta_y;
          if (count >= delta_x) count -= delta_x, y += dir_y;
        }
    }
  else if (delta_y > 0)
    {
      /* Mostly vertical */
      for (; y != y2 + dir_y; y += dir_y)
        {
          if (y >= 0 && y < pixbuf->height)
            {
              guchar *pos;
              gint    alpha = 32 * count / delta_y;
              gint    value;

              pos = &pixbuf->data[(y * pixbuf->width + x) * 3];
              if (x >= 0 && x < pixbuf->width)
                {
                  value = gamma_curve[31 - alpha];
                  // value = 31 - alpha;

                  pos[0] = MIN (pos[0] + ((pixbuf->fore[0] * value) >> 5), 255);
                  pos[1] = MIN (pos[1] + ((pixbuf->fore[1] * value) >> 5), 255);
                  pos[2] = MIN (pos[2] + ((pixbuf->fore[2] * value) >> 5), 255);
                }

              x2 = x + dir_x;
              pos += dir_x * 3;
              if (x2 >= 0 && x2 < pixbuf->width)
                {
                  value = gamma_curve[alpha];
                  // value = alpha;

                  pos[0] = MIN (pos[0] + ((pixbuf->fore[0] * value) >> 5), 255);
                  pos[1] = MIN (pos[1] + ((pixbuf->fore[1] * value) >> 5), 255);
                  pos[2] = MIN (pos[2] + ((pixbuf->fore[2] * value) >> 5), 255);
                }
            }

          count += delta_x;
          if (count >= delta_y) count -= delta_y, x += dir_x;
        }
    }
  else
    {
      /* One pel */
      if (x1 >= 0 && y1 >= 0 && x1 < pixbuf->width && y1 < pixbuf->height)
        {
          guchar *pos;

          pos = &pixbuf->data[(y1 * pixbuf->width + x1) * 3];
          pos[0] = MIN (pos[0] + pixbuf->fore[0], 255);
          pos[1] = MIN (pos[1] + pixbuf->fore[1], 255);
          pos[2] = MIN (pos[2] + pixbuf->fore[2], 255);
        }
    }
}

void
t_pixbuf_draw_arrow (TPixbuf *pixbuf,
                     gint     x1,
                     gint     y1,
                     gint     x2,
                     gint     y2)
{
  const gfloat arrow_angle = 0.25 * G_PI;
  const gfloat arrow_length = 8.0;

  t_pixbuf_draw_line (pixbuf, x1, y1, x2, y2);
  if (x1 != x2 || y1 != y2)
    {
      gint   x3, y3, x4, y4;
      gfloat angle, dist;

      dist = sqrt (SQR (x2 - x1) + SQR (y2 - y1));
      angle = acos ((y1 - y2) / dist); /* 0 -> PI */
      if (x2 - x1 < 0)
        angle = -angle;

      dist = MIN (dist, arrow_length);

      x3 = rint (x2 + cos (angle + arrow_angle) * dist);
      x4 = rint (x2 - cos (angle - arrow_angle) * dist);
      y3 = rint (y2 + sin (angle + arrow_angle) * dist);
      y4 = rint (y2 - sin (angle - arrow_angle) * dist);

      t_pixbuf_draw_line (pixbuf, x2, y2, x3, y3);
      t_pixbuf_draw_line (pixbuf, x2, y2, x4, y4);
    }
}

void
t_pixbuf_draw_rect (TPixbuf  *pixbuf,
                    gint      x1,
                    gint      y1,
                    gint      x2,
                    gint      y2)
{
  t_pixbuf_draw_line (pixbuf, x1, y1, x2, y1);
  t_pixbuf_draw_line (pixbuf, x2, y1, x2, y2);
  t_pixbuf_draw_line (pixbuf, x2, y2, x1, y2);
  t_pixbuf_draw_line (pixbuf, x1, y2, x1, y1);
}

void
t_pixbuf_draw_poly (TPixbuf  *pixbuf,
                    gint      point_count,
                    GdkPoint *points)
{
  gint i;

  for (i = 0; i < point_count - 1; i++)
    t_pixbuf_draw_line (pixbuf, points[i].x, points[i].y,
                     points[i + 1].x, points[i + 1].y);

  t_pixbuf_draw_line (pixbuf, points[i].x, points[i].y,
                      points[0].x, points[0].y);
}

/*
 * Fill the pixmap with TV static.  A cheezy LFSR is used to generate
 * pseudorandom noise.
 */

void
t_pixbuf_render_static (TPixbuf *pixbuf)
{
  gint           i, size;
  static guint16 lfsr;

  if (lfsr == 0)
    lfsr = time (NULL);

  srand (lfsr);
  lfsr = rand ();
  if (lfsr == 0) lfsr++;

  size = pixbuf->width * pixbuf->height * 3;
  for (i = 0; i < size; i += 3)
    {
      lfsr = ((lfsr >> 1) | (lfsr << 15)) ^ (lfsr & 0x7f);

      pixbuf->data[i + 0] = lfsr;
      pixbuf->data[i + 1] = lfsr;
      pixbuf->data[i + 2] = lfsr;
    }
}

void
t_pixbuf_clear (TPixbuf *pixbuf)
{
  gint     i, length;

  length = pixbuf->width * pixbuf->height * 3;

  for (i = 0; i < length; i += 3)
    {
      pixbuf->data[i + 0] = pixbuf->back[0];
      pixbuf->data[i + 1] = pixbuf->back[1];
      pixbuf->data[i + 2] = pixbuf->back[2];
    }
}

static void
t_pixbuf_draw_horiz_line (TPixbuf *pixbuf,
                          gint     x1,
                          gint     y,
                          gint     x2)
{
  guchar *pos;

  if (x2 < 0 || x1 >= pixbuf->width || y < 0 || y >= pixbuf->height)
    return;

  x1 = MAX (x1, 0);
  x2 = MIN (x2, pixbuf->width - 1);

  pos = &pixbuf->data[(y * pixbuf->width + x1) * 3];

  for (; x1 <= x2; x1++)
    {
      pos[0] = pixbuf->fore[0];
      pos[1] = pixbuf->fore[1];
      pos[2] = pixbuf->fore[2];

      pos += 3;
    }
}

static int
point_comparison (const void *point_1, const void *point_2)
{
  return ((GdkPoint*) point_1)->y > ((GdkPoint*) point_2)->y;
}

void
t_pixbuf_fill_triangle (TPixbuf  *pixbuf,
                        GdkPoint *points)
{
  GdkPoint pts[3];
  gint     count_1, count_2;
  gint     max_1, max_2;
  gint     inc_1, inc_2;
  gint     x_1, x_2;
  gint     dir_1, dir_2;
  gint     y;

  if (points[0].y == points[1].y && points[1].y == points[2].y)
    {
      t_pixbuf_draw_horiz_line (
        pixbuf, MIN (points[0].x, MIN (points[1].x, points[2].x)),
             points[0].y,
             MAX (points[0].x, MAX (points[1].x, points[2].x)));

      return;
    }

  /* Copy and sort points by y-coordinate. */
  memcpy (pts, points, sizeof (GdkPoint) * 3);
  qsort (pts, 3, sizeof (GdkPoint), &point_comparison);

  x_1 = x_2 = pts[0].x;
  dir_1 = (pts[0].x < pts[2].x) ? 1 : -1;
  dir_2 = (pts[0].x < pts[1].x) ? 1 : -1;
  max_1 = abs (pts[0].y - pts[2].y);
  max_2 = abs (pts[0].y - pts[1].y);
  inc_1 = abs (pts[0].x - pts[2].x);
  count_1 = 0;
  inc_2 = abs (pts[0].x - pts[1].x);
  count_2 = 0;
  for (y = pts[0].y; y <= pts[2].y; y++)
    {
      if (y == pts[1].y)
        {
          x_2 = pts[1].x;
          max_2 = abs (pts[1].y - pts[2].y);
          inc_2 = abs (pts[1].x - pts[2].x);
          count_2 = 0;
          dir_2 = (pts[1].x < pts[2].x) ? 1 : -1;
        }

      t_pixbuf_draw_horiz_line (pixbuf, MIN (x_1, x_2), y, MAX (x_1, x_2));

      count_1 += inc_1;
      if (max_1 > 0)
        while (count_1 >= max_1)
          {
            count_1 -= max_1;
            x_1 += dir_1;
          }

      count_2 += inc_2;
      if (max_2 > 0)
        while (count_2 >= max_2)
          {
            count_2 -= max_2;
            x_2 += dir_2;
          }
    }
}

void
t_pixbuf_fill_rect (TPixbuf *pixbuf,
                    gint      x1,
                    gint      y1,
                    gint      x2,
                    gint      y2)
{
  gint y;

  if (x2 < 0 || y2 < 0 || x1 >= pixbuf->width || y1 >= pixbuf->height) return;
  x1 = MAX (x1, 0);
  y1 = MAX (y1, 0);
  x2 = MIN (x2, pixbuf->width - 1);
  y2 = MIN (y2, pixbuf->height - 1);

  g_return_if_fail (x1 <= x2 && y1 <= y2);

  for (y = y1; y <= y2; y++)
    {
      gint    x;
      guchar *pos;

      pos = &pixbuf->data[(y * pixbuf->width + x1) * 3];
      for (x = x1; x <= x2; x++)
        {
          pos[0] = pixbuf->fore[0];
          pos[1] = pixbuf->fore[1];
          pos[2] = pixbuf->fore[2];

          pos += 3;
        }
    }
}

