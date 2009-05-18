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


#ifndef __T_CANVAS_H__
#define __T_CANVAS_H__

#include <gtk/gtk.h>
#include "tpixbuf.h"

#define T_TYPE_CANVAS            (t_canvas_get_type ())
#define T_CANVAS(obj)            (GTK_CHECK_CAST ((obj), T_TYPE_CANVAS, TCanvas))
#define T_CANVAS_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), T_TYPE_CANVAS, TCanvas))
#define T_IS_CANVAS(obj)         (GTK_CHECK_TYPE ((obj), T_TYPE_CANVAS))
#define T_IS_CANVAS_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), T_TYPE_CANVAS))


typedef enum TSelectionType TSelectionType;

enum TSelectionType
{
  T_SELECTION_NONE,
  T_SELECTION_RECTANGLE,
  T_SELECTION_ELLIPSE,
  T_SELECTION_SQUARE_ZOOM,
  T_SELECTION_SQUARE_SEED
};


typedef struct TCanvas      TCanvas;
typedef struct TCanvasClass TCanvasClass;

struct TCanvasClass
{
  GtkDrawingAreaClass parent_class;
};

struct TCanvas
{
  GtkDrawingArea  drawing_area;

  TPixbuf        *pixbuf;
  GdkGC          *xor_gc;

  TSelectionType  selection_type;
  GdkRectangle    selection_bounds;

  GArray         *object_array;
};

GtkType         t_canvas_get_type      (void);
GtkWidget      *t_canvas_new           (void);

TPixbuf        *t_canvas_get_pixbuf    (TCanvas        *canvas);
TSelectionType  t_canvas_get_selection (TCanvas        *canvas,
                                        gfloat         *x1,
                                        gfloat         *y1,
                                        gfloat         *x2,
                                        gfloat         *y2);
void            t_canvas_set_selection (TCanvas        *canvas,
                                        TSelectionType  select_type,
                                        gint            x1,
                                        gint            y1,
                                        gint            x2,
                                        gint            y2);
gint            t_canvas_add_object    (TCanvas        *canvas,
                                        gfloat          x,
                                        gfloat          y,
                                        gfloat          angle);
void            t_canvas_modify_object (TCanvas        *canvas,
                                        gint            item,
                                        gfloat          x,
                                        gfloat          y,
                                        gfloat          angle);
void            t_canvas_get_object    (TCanvas        *canvas,
                                        gint            item,
                                        gfloat         *x,
                                        gfloat         *y,
                                        gfloat         *angle);
void            t_canvas_delete_object (TCanvas        *canvas,
                                        gint            item);
void            t_canvas_clear_objects (TCanvas        *canvas);

#endif /* __T_CANVAS_H__ */
