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
#include "tcanvas.h"

#define SQR(x) ((x)*(x))

static void t_canvas_class_init    (TCanvasClass   *klass);
static void t_canvas_init          (TCanvas        *canvas);

static void t_canvas_destroy       (GtkObject      *object);
static void t_canvas_finalize      (GObject        *object);
static gint t_canvas_expose_event  (GtkWidget      *widget,
                                    GdkEventExpose *event);
static void t_canvas_size_allocate (GtkWidget      *widget,
                                    GtkAllocation  *allocation);
static void t_canvas_realize       (GtkWidget      *widget);
static void t_canvas_unrealize     (GtkWidget      *widget);

GtkType
t_canvas_get_type (void)
{
  static GtkType canvas_type = 0;
  
  if (!canvas_type)
    {
      static const GtkTypeInfo canvas_info =
      {
        "TCanvas",
        sizeof (TCanvas),
        sizeof (TCanvasClass),
        (GtkClassInitFunc) t_canvas_class_init,
        (GtkObjectInitFunc) t_canvas_init,
        /* reserved_1 */ NULL,
        /* reserved_2 */ NULL,
        (GtkClassInitFunc) NULL
      };
      
      canvas_type = gtk_type_unique (GTK_TYPE_DRAWING_AREA, &canvas_info);
    }
  
  return canvas_type;
}

GtkWidget *
t_canvas_new (void)
{
  TCanvas *canvas;

  canvas = gtk_type_new (T_TYPE_CANVAS);

  return GTK_WIDGET (canvas);
}

TPixbuf *
t_canvas_get_pixbuf (TCanvas *canvas)
{
  return canvas->pixbuf;
}

static GtkDrawingAreaClass *parent_class;

static void
t_canvas_class_init (TCanvasClass *klass)
{
  GObjectClass   *g_object_class = G_OBJECT_CLASS (klass);
  GtkObjectClass *object_class = GTK_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  parent_class = gtk_type_class (GTK_TYPE_DRAWING_AREA);

  object_class->destroy       = &t_canvas_destroy;
  g_object_class->finalize    = &t_canvas_finalize;
  widget_class->expose_event  = &t_canvas_expose_event;
  widget_class->size_allocate = &t_canvas_size_allocate;
  widget_class->realize       = &t_canvas_realize;
  widget_class->unrealize     = &t_canvas_unrealize;
}

static void
t_canvas_init (TCanvas *canvas)
{
  canvas->pixbuf                  = t_pixbuf_new ();
  canvas->xor_gc                  = NULL;
  canvas->selection_type          = T_SELECTION_NONE;
  canvas->selection_bounds.x      = 0;
  canvas->selection_bounds.y      = 0;
  canvas->selection_bounds.width  = 0;
  canvas->selection_bounds.height = 0;
  canvas->object_array            = g_array_new (FALSE, FALSE, sizeof (gfloat));

  gtk_widget_set_double_buffered (GTK_WIDGET (canvas), FALSE);
}

static void
t_canvas_destroy (GtkObject *object)
{
  TCanvas *canvas = T_CANVAS (object);

  t_pixbuf_free (canvas->pixbuf);
  canvas->pixbuf                  = t_pixbuf_new ();
  canvas->xor_gc                  = NULL;
  canvas->selection_type          = T_SELECTION_NONE;
  canvas->selection_bounds.x      = 0;
  canvas->selection_bounds.y      = 0;
  canvas->selection_bounds.width  = 0;
  canvas->selection_bounds.height = 0;
  g_array_free (canvas->object_array, TRUE);
  canvas->object_array            = g_array_new (FALSE, FALSE, sizeof (gfloat));

  GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
t_canvas_finalize (GObject *object)
{
  TCanvas *canvas = T_CANVAS (object);

  t_pixbuf_free (canvas->pixbuf);
  g_array_free (canvas->object_array, TRUE);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
draw_object (TCanvas *canvas,
             gint     item)
{
  GtkWidget *widget = GTK_WIDGET (canvas);
  gfloat     x, y, *data;

  /* FIXME: Show angle */
  data = &g_array_index (canvas->object_array, gfloat, item * 3);
  x = data[0] * canvas->pixbuf->width;
  y = data[1] * canvas->pixbuf->height;
  gdk_draw_line (widget->window, canvas->xor_gc, x - 4, y - 4, x + 4, y + 4);
  gdk_draw_line (widget->window, canvas->xor_gc, x + 4, y - 4, x - 4, y + 4);
}

static void
draw_objects (TCanvas *canvas)
{
  gint i, size;

  size = canvas->object_array->len / 3;
  for (i = 0; i < size; i++)
    draw_object (canvas, i);
}

static void
draw_selection (TCanvas *canvas)
{
  GtkWidget *widget = GTK_WIDGET (canvas);

  if (!GTK_WIDGET_REALIZED (canvas))
    return;

  switch (canvas->selection_type)
    {
    case T_SELECTION_NONE:
      break;

    case T_SELECTION_RECTANGLE:
    case T_SELECTION_SQUARE_ZOOM:
    case T_SELECTION_SQUARE_SEED:
      gdk_draw_rectangle (widget->window, canvas->xor_gc, FALSE,
                          canvas->selection_bounds.x,
                          canvas->selection_bounds.y,
                          canvas->selection_bounds.width,
                          canvas->selection_bounds.height);
      break;

    case T_SELECTION_ELLIPSE:
      gdk_draw_arc (widget->window, canvas->xor_gc, FALSE,
                    canvas->selection_bounds.x,
                    canvas->selection_bounds.y,
                    canvas->selection_bounds.width,
                    canvas->selection_bounds.height,
                    0, 360 * 64 - 1);
      break;
    }
}

static gint
t_canvas_expose_event (GtkWidget      *widget,
                       GdkEventExpose *event)
{
  TCanvas      *canvas = T_CANVAS (widget);
  GdkGC        *gc;
  GdkWindow    *window;
  gint          x1, y1, x2, y2;

  /* Return immediately if pixbuf is empty */
  if (canvas->pixbuf->width == 0 || canvas->pixbuf->height == 0)
    return FALSE;

  x1 = MIN (MAX (0, event->area.x), canvas->pixbuf->width - 1);
  y1 = MIN (MAX (0, event->area.y), canvas->pixbuf->height - 1);
  x2 = MIN (MAX (0, event->area.x + event->area.width - 1), canvas->pixbuf->width - 1);
  y2 = MIN (MAX (0, event->area.y + event->area.height - 1), canvas->pixbuf->height - 1);

  gc = widget->style->black_gc;
  window = widget->window;
  gdk_draw_rgb_image_dithalign (window, gc, x1, y1,
                                x2 - x1 + 1, y2 - y1 + 1,
                                GDK_RGB_DITHER_NORMAL,
                                &canvas->pixbuf->data[(y1 * canvas->pixbuf->width + x1) * 3],
                                canvas->pixbuf->width * 3, x1, y1);

  gdk_gc_set_clip_rectangle (canvas->xor_gc, &event->area);
  draw_selection (canvas);
  draw_objects (canvas);
  gdk_gc_set_clip_mask (canvas->xor_gc, NULL);

  if (GTK_WIDGET_CLASS (parent_class)->expose_event != NULL)
    return GTK_WIDGET_CLASS (parent_class)->expose_event (widget, event);

  return FALSE;
}

static void
t_canvas_size_allocate (GtkWidget     *widget,
                        GtkAllocation *allocation)
{
  TCanvas *canvas = T_CANVAS (widget);

  t_pixbuf_set_size (canvas->pixbuf, allocation->width, allocation->height);

  GTK_WIDGET_CLASS (parent_class)->size_allocate (widget, allocation);
}

static void
t_canvas_realize (GtkWidget *widget)
{
  TCanvas     *canvas = T_CANVAS (widget);
  GdkGCValues  values;
  gint         mask;

  GTK_WIDGET_CLASS (parent_class)->realize (widget);

  values.foreground = (widget->style->white.pixel == 0) ?
                      widget->style->black : widget->style->white;
  values.function = GDK_XOR;
  values.subwindow_mode = GDK_INCLUDE_INFERIORS;
  mask = GDK_GC_FOREGROUND | GDK_GC_FUNCTION | GDK_GC_SUBWINDOW;

  canvas->xor_gc = gdk_gc_new_with_values (widget->window, &values, mask);
}

static void
t_canvas_unrealize (GtkWidget *widget)
{
  TCanvas *canvas = T_CANVAS (widget);

  gdk_gc_destroy (canvas->xor_gc);
  canvas->xor_gc = NULL;

  GTK_WIDGET_CLASS (parent_class)->unrealize (widget);
}

TSelectionType
t_canvas_get_selection (TCanvas *canvas,
                        gfloat  *x1,
                        gfloat  *y1,
                        gfloat  *x2,
                        gfloat  *y2)
{
  if (canvas->selection_type == T_SELECTION_NONE)
    {
    *x1 = *y1 = *x2 = *y2 = 0;
    return canvas->selection_type;
    }

  *x1 = ((gfloat) canvas->selection_bounds.x) / canvas->pixbuf->width;
  *y1 = ((gfloat) canvas->selection_bounds.y) / canvas->pixbuf->height;
  *x2 = *x1 + ((gfloat) canvas->selection_bounds.width) / canvas->pixbuf->width;
  *y2 = *y1 + ((gfloat) canvas->selection_bounds.height) / canvas->pixbuf->height;

  /* 
   * if we drag the selection area outside the window boundry, 
   * we can get selection values <0 or >1; we fix these here.
   */
  if (*x1 < 0) *x1 = 0;
  if (*y1 < 0) *y1 = 0;
  if (*x2 > 1) *x2 = 1;
  if (*y2 > 1) *y2 = 1;

  return canvas->selection_type;
}

void
t_canvas_set_selection (TCanvas        *canvas,
                        TSelectionType  select_type,
                        gint            x1,
                        gint            y1,
                        gint            x2,
                        gint            y2)
{
  draw_selection (canvas);

  if (x2 < x1 || y2 < y1)
    select_type = T_SELECTION_NONE;

  canvas->selection_type = select_type;
  canvas->selection_bounds.x = x1;
  canvas->selection_bounds.y = y1;
  canvas->selection_bounds.width = x2 - x1 + 1;
  canvas->selection_bounds.height = y2 - y1 + 1;
  draw_selection (canvas);
}

gint
t_canvas_add_object (TCanvas *canvas,
                     gfloat   x,
                     gfloat   y,
                     gfloat   angle)
{
  gfloat array[3];

  array[0] = x;
  array[1] = y;
  array[2] = angle;
  g_array_append_vals (canvas->object_array, &array, 3);

  return canvas->object_array->len / 3 - 1;
}

void
t_canvas_modify_object (TCanvas *canvas,
                        gint     item,
                        gfloat   x,
                        gfloat   y,
                        gfloat   angle)
{
  gint index;

  draw_object (canvas, item);
  index = item * 3;
  g_array_index (canvas->object_array, gfloat, index) = x;
  g_array_index (canvas->object_array, gfloat, index + 1) = y;
  g_array_index (canvas->object_array, gfloat, index + 2) = angle;
  draw_object (canvas, item);
}

void
t_canvas_get_object (TCanvas *canvas,
                     gint     item,
                     gfloat  *x,
                     gfloat  *y,
                     gfloat  *angle)
{
  gfloat *object;

  object = &g_array_index (canvas->object_array, gfloat, item * 3);
  *x     = object[0];
  *y     = object[1];
  *angle = object[2];
}

void
t_canvas_delete_object (TCanvas        *canvas,
                        gint            item)
{
  GArray *array = canvas->object_array;

  draw_object (canvas, item);

  g_array_remove_index_fast (array, item * 3 + 2);
  g_array_remove_index_fast (array, item * 3 + 1);
  g_array_remove_index_fast (array, item * 3 + 0);
}

void
t_canvas_clear_objects (TCanvas *canvas)
{
  g_array_free (canvas->object_array, TRUE);
  canvas->object_array = g_array_new (FALSE, FALSE, sizeof (gfloat));
  gtk_widget_queue_draw (GTK_WIDGET (canvas));
}
