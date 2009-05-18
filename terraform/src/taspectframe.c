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
#include "taspectframe.h"

#define MAX_RATIO 10000.0
#define MIN_RATIO 0.0001

static void t_aspect_frame_class_init    (TAspectFrameClass *klass);
static void t_aspect_frame_init          (TAspectFrame      *canvas);
static void t_aspect_frame_size_allocate (GtkWidget         *widget,
                                          GtkAllocation     *allocation);

GtkType
t_aspect_frame_get_type (void)
{
  static GtkType frame_type = 0;
  
  if (!frame_type)
    {
      static const GtkTypeInfo frame_info =
      {
        "TAspectFrame",
        sizeof (TAspectFrame),
        sizeof (TAspectFrameClass),
        (GtkClassInitFunc) t_aspect_frame_class_init,
        (GtkObjectInitFunc) t_aspect_frame_init,
        /* reserved_1 */ NULL,
        /* reserved_2 */ NULL,
        (GtkClassInitFunc) NULL
      };
      
      frame_type = gtk_type_unique (GTK_TYPE_ASPECT_FRAME, &frame_info);
    }
  
  return frame_type;
}

GtkWidget *
t_aspect_frame_new (void)
{
  TAspectFrame *frame;

  frame = gtk_type_new (T_TYPE_ASPECT_FRAME);

  return GTK_WIDGET (frame);
}

static GtkAspectFrameClass *parent_class;

static void
t_aspect_frame_class_init (TAspectFrameClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  parent_class = gtk_type_class (GTK_TYPE_ASPECT_FRAME);

  widget_class->size_allocate = &t_aspect_frame_size_allocate;
}

static void
t_aspect_frame_init (TAspectFrame *frame)
{
  frame->enabled = FALSE;
}

static void
t_aspect_frame_size_allocate (GtkWidget     *widget,
                              GtkAllocation *allocation)
{
  TAspectFrame *t_aspect_frame = T_ASPECT_FRAME (widget);

  if (t_aspect_frame->enabled == FALSE)
    {
      GtkAspectFrame *aspect_frame = GTK_ASPECT_FRAME (widget);
      gdouble ratio;
      gdouble new_ratio;
      gint    x, y, width, height;

      x = (GTK_CONTAINER (aspect_frame)->border_width +
           GTK_WIDGET (aspect_frame)->style->xthickness);
      width = allocation->width - x * 2;
      
      y = (GTK_CONTAINER (aspect_frame)->border_width +
           MAX (0 /* FIXME GTK_FRAME (aspect_frame)->label_height */,
                GTK_WIDGET (aspect_frame)->style->ythickness));
      height = (allocation->height - y -
                GTK_CONTAINER (aspect_frame)->border_width -
                GTK_WIDGET (aspect_frame)->style->ythickness);


      if (height != 0)
        new_ratio = ((gdouble) width) / height;
      else
        new_ratio = MAX_RATIO;
      new_ratio = CLAMP (new_ratio, MIN_RATIO, MAX_RATIO);

      ratio = aspect_frame->ratio;
      aspect_frame->ratio = new_ratio;
      GTK_WIDGET_CLASS (parent_class)->size_allocate (widget, allocation);
      aspect_frame->ratio = ratio;
    }
  else
    GTK_WIDGET_CLASS (parent_class)->size_allocate (widget, allocation);
}

void
t_aspect_frame_enable (TAspectFrame *frame,
                       gboolean      enable)
{
  GtkWidget *widget = GTK_WIDGET (frame);

  if (frame->enabled != enable)
    {
      frame->enabled = enable;
      if (GTK_WIDGET_DRAWABLE(widget))
        gtk_widget_queue_clear (widget);
      
      gtk_widget_queue_resize (widget);
    }
}

gboolean
t_aspect_frame_is_enabled (TAspectFrame *frame)
{
  return frame->enabled;
}
