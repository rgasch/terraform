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

#ifndef __T_PIXBUF_H__
#define __T_PIXBUF_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gdk/gdk.h>

typedef struct TPixbuf TPixbuf;

struct TPixbuf
{
  guchar *data;
  gint    width;
  gint    height;
  guchar  fore[3];
  guchar  back[3];
};

TPixbuf *t_pixbuf_new           (void);
void     t_pixbuf_free          (TPixbuf  *pixbuf);
void     t_pixbuf_set_size      (TPixbuf  *pixbuf,
                                 gint      width,
                                 gint      height);
void     t_pixbuf_set_fore      (TPixbuf  *pixbuf,
                                 gint      red,
                                 gint      green,
                                 gint      blue);
void     t_pixbuf_set_back      (TPixbuf  *pixbuf,
                                 gint      red,
                                 gint      green,
                                 gint      blue);
void     t_pixbuf_draw_raster   (TPixbuf  *pixbuf,
                                 gint      y,
                                 guchar   *data);
void     t_pixbuf_draw_pel      (TPixbuf  *pixbuf,
                                 gint      x,
                                 gint      y);
void     t_pixbuf_get_pel       (TPixbuf  *pixbuf,
                                 gint      x,
                                 gint      y,
                                 gint     *r,
                                 gint     *g,
                                 gint     *b);
void     t_pixbuf_draw_line     (TPixbuf  *pixbuf,
                                 gint      x1,
                                 gint      y1,
                                 gint      x2,
                                 gint      y2);
void     t_pixbuf_draw_line_add (TPixbuf  *pixbuf,
                                 gint      x1,
                                 gint      y1,
                                 gint      x2,
                                 gint      y2);
void     t_pixbuf_draw_line_add_aa (TPixbuf  *pixbuf,
                                 gint      x1,
                                 gint      y1,
                                 gint      x2,
                                 gint      y2);
void     t_pixbuf_draw_arrow    (TPixbuf  *pixbuf,
                                 gint      x1,
                                 gint      y1,
                                 gint      x2,
                                 gint      y2);
void     t_pixbuf_draw_rect     (TPixbuf  *pixbuf,
                                 gint      x1,
                                 gint      y1,
                                 gint      x2,
                                 gint      y2);
void     t_pixbuf_draw_poly     (TPixbuf  *pixbuf,
                                 gint      point_count,
                                 GdkPoint *points);
void     t_pixbuf_render_static (TPixbuf  *pixbuf);
void     t_pixbuf_clear         (TPixbuf  *pixbuf);
void     t_pixbuf_fill_triangle (TPixbuf  *pixbuf,
                                 GdkPoint *points);
void     t_pixbuf_fill_rect     (TPixbuf  *pixbuf,
                                 gint       x1,
                                 gint       y1,
                                 gint       x2,
                                 gint       y2);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __T_PIXBUF_H__ */
