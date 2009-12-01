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
#include "tterrainview.h"
#include "contour.h"
#include "filters.h"
#include "gensubdiv.h"
#include "support.h"
#include "support2.h"
#include "taspectframe.h"
#include "terrainwindow.h"
#include "trandom.h"

#define SQR(x) ((x)*(x))

#ifndef G_PI
#define G_PI 3.14159265358979323846264
#endif

#define CLIP(x,m,M) MIN (MAX (x, m), M)

static void t_terrain_view_class_init    (TTerrainViewClass   *klass);
static void t_terrain_view_init          (TTerrainView        *canvas);

static void t_terrain_view_destroy       (GtkObject      *object);
static void t_terrain_view_finalize      (GObject        *object);
static gint t_terrain_view_expose_event  (GtkWidget      *widget,
                                          GdkEventExpose *event);
static void t_terrain_view_size_allocate (GtkWidget      *widget,
                                          GtkAllocation  *allocation);
static gint t_terrain_view_button_press  (GtkWidget      *widget,
                                          GdkEventButton *event);
static gint t_terrain_view_scroll        (GtkWidget      *widget,
                                          GdkEventScroll *event);
static gint t_terrain_view_motion_notify (GtkWidget      *widget,
                                          GdkEventMotion *event);
static gint t_terrain_view_button_release(GtkWidget      *widget,
                                          GdkEventButton *event);
static gint t_terrain_view_key_press     (GtkWidget      *widget,
                                          GdkEventKey    *event);
static gint t_terrain_view_focus_in      (GtkWidget      *widget,
                                          GdkEventFocus  *event);
static gint t_terrain_view_focus_out     (GtkWidget      *widget,
                                          GdkEventFocus  *event);
static void on_object_deleted            (GtkObject      *terrain,
                                          gint            item,
                                          GtkWidget      *user_data);


GtkType
t_terrain_view_get_type (void)
{
  static GtkType view_type = 0;
  
  if (!view_type)
    {
      static const GtkTypeInfo view_info =
      {
        "TTerrainView",
        sizeof (TTerrainView),
        sizeof (TTerrainViewClass),
        (GtkClassInitFunc) t_terrain_view_class_init,
        (GtkObjectInitFunc) t_terrain_view_init,
        /* reserved_1 */ NULL,
        /* reserved_2 */ NULL,
        (GtkClassInitFunc) NULL
      };
      
      view_type = gtk_type_unique (T_TYPE_CANVAS, &view_info);
    }
  
  return view_type;
}


GtkWidget *
t_terrain_view_new (void)
{
  TTerrainView *view;

  view = gtk_type_new (T_TYPE_TERRAIN_VIEW);

  return GTK_WIDGET (view);
}


static TTerrainViewClass *parent_class;


static void
t_terrain_view_class_init (TTerrainViewClass *klass)
{
  GObjectClass   *g_object_class = G_OBJECT_CLASS (klass);
  GtkObjectClass *object_class = GTK_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  parent_class = gtk_type_class (T_TYPE_CANVAS);

  object_class->destroy              = &t_terrain_view_destroy;
  g_object_class->finalize           = &t_terrain_view_finalize;
  widget_class->expose_event         = &t_terrain_view_expose_event;
  widget_class->size_allocate        = &t_terrain_view_size_allocate;
  widget_class->button_press_event   = &t_terrain_view_button_press;
  widget_class->scroll_event         = &t_terrain_view_scroll;
  widget_class->motion_notify_event  = &t_terrain_view_motion_notify;
  widget_class->button_release_event = &t_terrain_view_button_release;
  widget_class->key_press_event      = &t_terrain_view_key_press;
  widget_class->focus_in_event       = &t_terrain_view_focus_in;
  widget_class->focus_out_event      = &t_terrain_view_focus_out;
}

static void
t_terrain_view_init (TTerrainView *view)
{
  view->contour_dirty           = FALSE;
  view->dirty                   = FALSE;
  view->size                    = 0;
  view->rotate_timeout          = 0;
  view->terrain                 = NULL;
  view->model                   = T_VIEW_2D_PLANE;
  view->colormap                = T_COLORMAP_LAND;
  view->angle                   = 0.0;
  view->elevation               = 0.25;
  view->auto_rotate             = FALSE;
  view->crosshair_x             = -1.0;
  view->crosshair_y             = -1.0;
  view->anchor_x                = 0;
  view->anchor_y                = 0;
  view->anchor_object           = -1;
  view->contour_lines           = NULL;
  view->heightfield_modified_id = 0;
  view->selection_modified_id   = 0;
  view->object_added_id         = 0;
  view->object_modified_id      = 0;
  view->object_deleted_id       = 0;
  view->tool_mode               = T_TOOL_NONE;
  view->compose_op              = T_COMPOSE_NONE;
}


static void
t_terrain_view_destroy (GtkObject *object)
{
  TTerrainView *view = T_TERRAIN_VIEW (object);

  /* Disconnect first before unreffing TTerrain object */
  if (view->heightfield_modified_id != 0)
    {
      gtk_signal_disconnect (GTK_OBJECT (view->terrain),
                             view->heightfield_modified_id);
      view->heightfield_modified_id = 0;
    }

  if (view->selection_modified_id != 0)
    {
      gtk_signal_disconnect (GTK_OBJECT (view->terrain),
                             view->selection_modified_id);
      view->selection_modified_id = 0;
    }

  if (view->object_added_id != 0)
    {
      gtk_signal_disconnect (GTK_OBJECT (view->terrain),
                             view->object_added_id);
      view->object_added_id = 0;
    }

  if (view->object_modified_id != 0)
    {
      gtk_signal_disconnect (GTK_OBJECT (view->terrain),
                             view->object_modified_id);
      view->object_modified_id = 0;
    }

  if (view->object_deleted_id != 0)
    {
      gtk_signal_disconnect (GTK_OBJECT (view->terrain),
                             view->object_deleted_id);
      view->object_deleted_id = 0;
    }

  view->contour_dirty  = FALSE;
  view->dirty          = FALSE;
  view->size           = 0;
  if (view->rotate_timeout != 0)
    gtk_timeout_remove (view->rotate_timeout);
  view->rotate_timeout = 0;
  if (view->terrain != NULL)
    t_terrain_unref (view->terrain);
  view->terrain        = NULL;
  view->model          = T_VIEW_2D_PLANE;
  view->colormap       = T_COLORMAP_LAND;
  view->angle          = 0.0;
  view->elevation      = 0.0;
  view->auto_rotate    = FALSE;
  view->crosshair_x    = -1.0;
  view->crosshair_y    = -1.0;
  view->anchor_x       = 0;
  view->anchor_y       = 0;
  t_terrain_contour_list_free (view->contour_lines);
  view->contour_lines  = NULL;
  view->tool_mode      = T_TOOL_NONE;
  view->compose_op     = T_COMPOSE_NONE;

  GTK_OBJECT_CLASS (parent_class)->destroy (object);
}


static void
t_terrain_view_finalize (GObject *object)
{
  TTerrainView *view = T_TERRAIN_VIEW (object);

  if (view->heightfield_modified_id != 0)
    gtk_signal_disconnect (GTK_OBJECT (view->terrain),
                           view->heightfield_modified_id);

  if (view->selection_modified_id != 0)
    gtk_signal_disconnect (GTK_OBJECT (view->terrain),
                           view->selection_modified_id);

  if (view->object_added_id != 0)
    gtk_signal_disconnect (GTK_OBJECT (view->terrain),
                           view->object_added_id);

  if (view->object_modified_id != 0)
    gtk_signal_disconnect (GTK_OBJECT (view->terrain),
                           view->object_modified_id);

  if (view->object_deleted_id != 0)
    gtk_signal_disconnect (GTK_OBJECT (view->terrain),
                           view->object_deleted_id);

  if (view->terrain != NULL)
    t_terrain_unref (view->terrain);
  if (view->rotate_timeout != 0)
    gtk_timeout_remove (view->rotate_timeout);
  t_terrain_contour_list_free (view->contour_lines);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}


static void
t_terrain_paint_2d_plane (TTerrain *terrain,
                          TPixbuf  *pixbuf,
                          guchar   *palette)
{
  gint    x, y;
  gint    count;
  guchar *raster;

  g_return_if_fail (palette != NULL);

  raster = g_new (guchar, pixbuf->width * 3);

  for (y = 0; y < pixbuf->height; y++)
    {
      gint hf_pos;

      hf_pos = y * terrain->height / pixbuf->height * terrain->width;

      count = 0;
      for (x = 0; x < pixbuf->width * 3; x += 3)
        {
          gint value;

          value = terrain->heightfield[hf_pos] * 256;
          value = MAX (MIN (value, 255), 0) * 3;

	  /* rivers are blue */
	  if (terrain->riverfield != NULL &&
              terrain->riverfield[hf_pos] > terrain->heightfield[hf_pos])
            value = 0;

          if (terrain->selection != NULL)
            {
              gint select;

              select = 255 - terrain->selection[hf_pos] * 127.0;

              raster[x + 0] = 255 - (((255 - palette[value + 0]) * select) >> 8);
              raster[x + 1] = 255 - (((255 - palette[value + 1]) * select) >> 8);
              raster[x + 2] = 255 - (((255 - palette[value + 2]) * select) >> 8);
            }
          else
            {
              raster[x + 0] = palette[value + 0];
              raster[x + 1] = palette[value + 1];
              raster[x + 2] = palette[value + 2];
            }

          count += terrain->width;
          while (count >= pixbuf->width)
            {
              count -= pixbuf->width;
              hf_pos++;
            }
        }

      t_pixbuf_draw_raster (pixbuf, y, raster);
    }

  g_free (raster);
}


static void
t_terrain_paint_2d_contour (TTerrain     *terrain,
                            TTerrainView *view,
                            TPixbuf      *pixbuf,
                            guchar       *palette)
{
  GList  *pos;
  gint    i, x, y, x2, y2;
  gfloat  scale_x, scale_y;
  gint    count;
  guchar *raster;

  g_return_if_fail (palette != NULL);

  raster = g_new (guchar, pixbuf->width * 3);

  for (y = 0; y < pixbuf->height; y++)
    {
      gint hf_pos;

      hf_pos = y * terrain->height / pixbuf->height * terrain->width;

      count = 0;
      for (x = 0; x < pixbuf->width * 3; x += 3)
        {
          gint value;

          value = terrain->heightfield[hf_pos] * 256;
          value = MAX (MIN (value, 255), 0) * 3;

	  /* rivers are blue */
	  if (terrain->riverfield != NULL &&
              terrain->riverfield[hf_pos] > terrain->heightfield[hf_pos])
            value = 0;

          raster[x + 0] = palette[value + 0];
          raster[x + 1] = palette[value + 1];
          raster[x + 2] = palette[value + 2];

          count += terrain->width;
          while (count >= pixbuf->width)
            {
              count -= pixbuf->width;
              hf_pos++;
            }
        }

      t_pixbuf_draw_raster (pixbuf, y, raster);
    }

  g_free (raster);

  scale_x = ((gfloat) pixbuf->width) / terrain->width;
  scale_y = ((gfloat) pixbuf->height) / terrain->height;

  t_pixbuf_set_fore (pixbuf, 0, 0, 0);
  pos = g_list_first (view->contour_lines);
  while (pos != NULL)
    {
      GArray *points;

      points = pos->data;
      x = g_array_index (points, gfloat, 1) * scale_x;
      y = g_array_index (points, gfloat, 2) * scale_y;
      for (i = 3; i < points->len; i += 2)
        {
          x2 = g_array_index (points, gfloat, i + 0) * scale_x;
          y2 = g_array_index (points, gfloat, i + 1) * scale_y;
          t_pixbuf_draw_line (pixbuf, x, y, x2, y2);
          x = x2;
          y = y2;
        }

      pos = pos->next;
    }
}


/*
 * The following implements the Cohen-Sutherland line clipping algorithm, 
 * with some custom additions. 
 * It was adapeted from the code at 
 * http://www.daimi.au.dk/~mbl/cgcourse/wiki/cohen-sutherland_line-cli.html
 */


/*
 * define the coding of end points
 */
typedef unsigned int clip_code;
enum {TOP = 0x1, BOTTOM = 0x2, RIGHT = 0x4, LEFT = 0x8};


/*
 * a rectangle is defined by 4 sides
 */
typedef struct {
  int xmin, ymin, xmax, ymax;
} rectangle;


/*
 * compute the encoding of a point relative to a rectangle
 */
static clip_code 
compute_clip_code (gdouble x, gdouble y, rectangle r)
{
  clip_code c = 0;

  if (y > r.ymax)
    c |= TOP;
  else 
  if (y < r.ymin)
    c |= BOTTOM;

  if (x > r.xmax)
    c |= RIGHT;
  else 
  if (x < r.xmin)
    c |= LEFT;

  return c;
}


/*
 * clip line P0(x0,y0)-P1(x1,y1) against a plane/rectangle. 
 * returns TRUE if the line is to be drawn, FALSE if it can be ignored.
 */
static gboolean
clip_line (gdouble *x0, gdouble *y0, gdouble *x1, gdouble *y1, 
           rectangle clip_rect)
{
  clip_code C0, C1, C;
  gdouble x, y;

  C0 = compute_clip_code (*x0, *y0, clip_rect);
  C1 = compute_clip_code (*x1, *y1, clip_rect);

  for (;;) 
    {
      /* trivial accept: both ends in rectangle */
      if ((C0 | C1) == 0) 
        return TRUE;
		
      /* trivial reject: both ends on the external side of the rectangle */
      if ((C0 & C1) != 0)
        return FALSE;

      /* RNG: extra check which is not part of the C&S algorithm.
       * Due to the fact the we're dealing with a rotated wireframe 
       * drawing of the terrain, we can reject any line which has both 
       * ends off the screen. This has the added benefit of removing 
       * lines which would be rotated behind the camera and above the 
       * screen, which otherwise would show up as incorrect vertical 
       * lines across the screen. 
       */
      if (C0 != 0 && C1 != 0)
        return FALSE;

      /* normal case: clip end outside rectangle */
      C = C0 ? C0 : C1;
      if (C & TOP) 
        {
          x = *x0 + (*x1 - *x0) * (clip_rect.ymax - *y0) / (*y1 - *y0);
          y = clip_rect.ymax;
        } 
      else if (C & BOTTOM) 
        {
          x = *x0 + (*x1 - *x0) * (clip_rect.ymin - *y0) / (*y1 - *y0);
          y = clip_rect.ymin;
        }  
      else if (C & RIGHT) 
        {
          x = clip_rect.xmax;
          y = *y0 + (*y1 - *y0) * (clip_rect.xmax - *x0) / (*x1 - *x0);
        } 
      else 
        {
          x = clip_rect.xmin;
          y = *y0 + (*y1 - *y0) * (clip_rect.xmin - *x0) / (*x1 - *x0);
        }

       /* set new end point and iterate */
       if (C == C0) 
         {
           *x0 = x; 
           *y0 = y;
           C0 = compute_clip_code (*x0, *y0, clip_rect);
         } 
       else 
         {
           *x1 = x; 
           *y1 = y;
           C1 = compute_clip_code (*x1, *y1, clip_rect);
         }
     }

  /* Not Reached */
}


static gboolean
clip_line_int (gint *x0, gint *y0, gint *x1, gint *y1, rectangle clip_rect)
{
  gdouble dx0 = *x0;
  gdouble dy0 = *y0;
  gdouble dx1 = *x1;
  gdouble dy1 = *y1;

  gboolean rc = clip_line (&dx0, &dy0, &dx1, &dy1, clip_rect);

  *x0 = (gint) dx0;
  *y0 = (gint) dy0;
  *x1 = (gint) dx1;
  *y1 = (gint) dy1;

  return rc;
}


static void
draw_line (TPixbuf   *pixbuf,
           gint       x1,
           gint       y1,
           gint       x2,
           gint       y2,
           rectangle  screen_rect)
{
  if (clip_line_int (&x1, &y1, &x2, &y2, screen_rect))
    t_pixbuf_draw_line_add_aa (pixbuf, x1, y1, x2, y2);
}


static void
t_terrain_paint_3d_wire (TTerrain *terrain,
                         TPixbuf  *pixbuf,
                         gfloat    angle,
                         gfloat    elevation /* -2.0 - 2.0 */,
                         gint      resolution,
                         guchar   *palette, 
                         gboolean  do_tile)
{
  gint   y;
  gint   division;
  gint   width_offset, height_offset;
  gint   lim_x, lim_y;
  gfloat scrn_offset_x, scrn_offset_y;
  gfloat y_scale;
  gfloat terrain_size, pixbuf_size;
  gfloat cos_angle, sin_angle;
  gfloat camera_y, camera_z;
  gfloat zoom;
  gfloat half_width, half_height;
  rectangle screen_rect;

  terrain_size = MAX (terrain->width, terrain->height);
  pixbuf_size = MIN (pixbuf->width, pixbuf->height);

  division = (gint) ceil (((gfloat) terrain_size) / pixbuf_size * resolution);
  y_scale = terrain->y_scale_factor * terrain_size;

  t_pixbuf_clear (pixbuf);

  /* Add 0.5 to the elevation so that an elevation parameter of 0.0 centers
     the camera at the midpoint of a normalized heightfield. */
  /* camera_y = terrain_size * (elevation + 0.5); */
  camera_y = terrain_size * ((elevation + 0.5) + terrain->camera_height_factor);
  camera_z = -terrain_size * 1.5;
  zoom = pixbuf_size * 1.0;

  /* Use the offsets to center the plot onscreen. */
  scrn_offset_x = pixbuf->width * 0.5;
  scrn_offset_y =
    pixbuf->height * 0.5 + zoom * (camera_y - 0.5 * y_scale) / camera_z; 

  cos_angle = cos (angle);
  sin_angle = sin (angle);

  width_offset = (terrain->width % division) >> 1;
  height_offset = (terrain->height % division) >> 1;

  screen_rect.xmin = 0;
  screen_rect.ymin = 0;
  screen_rect.xmax = pixbuf->width;
  screen_rect.ymax = pixbuf->height;

  if (do_tile) 
    {
      half_width = terrain->width;
      half_height = terrain->height;

      lim_x = terrain->width * 2;
      lim_y = terrain->height * 2;
    }
  else
    {
      half_width = terrain->width * 0.5;
      half_height = terrain->height * 0.5;

      lim_x = terrain->width;
      lim_y = terrain->height;
    }

  for (y = height_offset; y < lim_y; y += division)
    {
      gint x;

      for (x = width_offset; x < lim_x; x += division)
        {
          gfloat   temp_x, temp_y, temp_z;
          gfloat   coord_x, coord_y;
          gint     scrn_x_1, scrn_y_1;
          gint     scrn_x_2=0, scrn_y_2=0; /* silence compiler warnings */
          gint     scrn_x_3, scrn_y_3;
          gint     value;
          gboolean right;
      
	  gint pos = y%terrain->height * terrain->width + x%terrain->width;

          value = ((terrain->heightfield[pos] * 255.0) + 0.5);
          value = MAX (MIN (value, 255), 0) * 3;
 
          coord_x = x - half_width; coord_y = y - half_height;
          temp_x = cos_angle * coord_x + sin_angle * coord_y;
          temp_y = terrain->heightfield[pos];
          if (terrain->do_filled_sea && temp_y < terrain->sealevel)
            temp_y = terrain->sealevel;
          temp_y *= y_scale;
          temp_z = sin_angle * coord_x - cos_angle * coord_y;
          scrn_x_1 = rint (scrn_offset_x + zoom * temp_x / (temp_z - camera_z));
          scrn_y_1 = rint (scrn_offset_y - zoom * (temp_y - camera_y) / (temp_z - camera_z));

          t_pixbuf_set_fore (pixbuf, palette[value],
                             palette[value + 1], palette[value + 2]);

          right = FALSE;
          if (x + division < lim_x)
            {
              coord_x = x + division - half_width; coord_y = y - half_height;
              temp_x = cos_angle * coord_x + sin_angle * coord_y;
              temp_y = terrain->heightfield[pos + division];
              if (terrain->do_filled_sea && temp_y < terrain->sealevel)
                temp_y = terrain->sealevel;
              temp_y *= y_scale;
              temp_z = sin_angle * coord_x - cos_angle * coord_y;
              scrn_x_2 = rint (scrn_offset_x + zoom * temp_x / (temp_z - camera_z));
              scrn_y_2 = rint (scrn_offset_y - zoom * (temp_y - camera_y) / (temp_z - camera_z));

              right = TRUE;
              draw_line (pixbuf, scrn_x_1, scrn_y_1, scrn_x_2, scrn_y_2, screen_rect);
            }

          if (y + division < lim_y)
            {
              coord_x = x - half_width; coord_y = y + division - half_height;
              temp_x = cos_angle * coord_x + sin_angle * coord_y;

	      if (y%terrain->height + division < terrain->height)
                temp_y = terrain->heightfield[pos + division * terrain->width];
	      else
                {
                  gint yoff = terrain->height - y % terrain->height - 1; 
                  temp_y = terrain->heightfield[pos + yoff * terrain->width];
                }

              if (terrain->do_filled_sea && temp_y < terrain->sealevel)
                temp_y = terrain->sealevel;
              temp_y *= y_scale;
              temp_z = sin_angle * coord_x - cos_angle * coord_y;
              scrn_x_3 = rint (scrn_offset_x + zoom * temp_x / (temp_z - camera_z));
              scrn_y_3 = rint (scrn_offset_y - zoom * (temp_y - camera_y) / (temp_z - camera_z));

              draw_line (pixbuf, scrn_x_1, scrn_y_1, scrn_x_3, scrn_y_3, screen_rect);

              if (right)
                draw_line (pixbuf, scrn_x_2, scrn_y_2, scrn_x_3, scrn_y_3, screen_rect);
            }
        }
    }
}


static void
t_terrain_paint_3d_light (TTerrain  *terrain,
                          TPixbuf   *pixbuf,
                          gfloat    elevation, 
                          guchar    *palette,
			  gboolean   do_tile)
{
  gint   y;
  gint   scrn_offset_x, scrn_offset_y;
  gint   lim_x, lim_y;
  gfloat y_scale;
  gfloat terrain_size, pixbuf_size;
  gfloat camera_y, camera_z;
  gfloat zoom;
  gfloat half_width, half_height;

  terrain_size = MAX (terrain->width, terrain->height);
  pixbuf_size = MIN (pixbuf->width, pixbuf->height);
  y_scale = terrain->y_scale_factor * terrain_size;

  t_pixbuf_clear (pixbuf);

  /* camera_y = terrain_size * 0.75; */
  camera_y = terrain_size * ((elevation + 0.5) + terrain->camera_height_factor);
  camera_z = -terrain_size * 1.5;

  if (do_tile) 
    {
      zoom = pixbuf_size * 0.5;

      half_width = terrain->width;
      half_height = terrain->height;

      lim_x = terrain->width * 2;
      lim_y = terrain->height * 2;
    } 
  else
    {
      zoom = pixbuf_size * 1.0;

      half_width = terrain->width * 0.5;
      half_height = terrain->height * 0.5;

      lim_x = terrain->width;
      lim_y = terrain->height;
    }

  /* Use the offsets to center the plot onscreen. */
  scrn_offset_x = pixbuf->width / 2;
  /* scrn_offset_y = -pixbuf->height / 8 +
   *   pixbuf->height / 2 + zoom * (camera_y - 0.5 * y_scale) / camera_z;
   */
  scrn_offset_y =
    pixbuf->height * 0.5 + zoom * (camera_y - 0.5 * y_scale) / camera_z;

  for (y=0; y+1 < lim_y; y++)
    {
      gint   x;
      gint   pos;
      gint   temp_x_1, temp_y_1;
      gint   temp_x_2, temp_y_2;
      gfloat coord_x_1, coord_y_1, coord_z_1;
      gfloat coord_x_2, coord_y_2, coord_z_2;

      pos = y%terrain->height * terrain->width;

      coord_x_1 = -half_width;
      coord_y_1 = terrain->heightfield[pos];
      if (terrain->do_filled_sea && coord_y_1 < terrain->sealevel)
        coord_y_1 = terrain->sealevel;
      coord_y_1 *= y_scale;
      coord_z_1 = half_height - y;
      temp_x_1 = zoom * coord_x_1 / (coord_z_1 - camera_z);
      temp_y_1 = zoom * (coord_y_1 - camera_y) / (coord_z_1 - camera_z);

      coord_x_2 = -half_width;
      /* Required to tile correctly */
      if (y % terrain->height + 1 < terrain->height)
        coord_y_2 = terrain->heightfield[pos + terrain->width];
      else 
        {
          gint yoff = terrain->height - y%terrain->height - 1;
          coord_y_2 = terrain->heightfield[yoff * terrain->width];
        }
      if (terrain->do_filled_sea && coord_y_2 < terrain->sealevel)
        coord_y_2 = terrain->sealevel;
      coord_y_2 *= y_scale;
      coord_z_2 = half_height - y - 1;
      temp_x_2 = zoom * coord_x_2 / (coord_z_2 - camera_z);
      temp_y_2 = zoom * (coord_y_2 - camera_y) / (coord_z_2 - camera_z);

      for (x=0; x+1 < lim_x; x++)
        {
          gint     temp_x_3, temp_y_3;
          gint     temp_x_4, temp_y_4;
          gfloat   coord_x_3, coord_y_3, coord_z_3;
          gfloat   coord_x_4, coord_y_4, coord_z_4;
          gfloat   vector_y_2, vector_y_3;
          gfloat   light_x, light_y, light_z;
          gfloat   magnitude;
          gint     value;
          gfloat   light;
          GdkPoint points[3];

	  /* Simple fix for tiling edges -> skip border pixel */
	  if (do_tile && x == terrain->width-1)
            x++;

          pos = y%terrain->height * terrain->width + x%terrain->width;

          value = ((terrain->heightfield[pos] * 255.0) + 0.5);
          value = MAX (MIN (value, 255), 0) * 3;
 
          coord_x_3 = x + 1 - half_width;
          coord_y_3 = terrain->heightfield[pos + 1];
          if (terrain->do_filled_sea && coord_y_3 < terrain->sealevel)
            coord_y_3 = terrain->sealevel;
          coord_y_3 *= y_scale;
          coord_z_3 = half_height - y;
          temp_x_3 = zoom * coord_x_3 / (coord_z_3 - camera_z);
          temp_y_3 = zoom * (coord_y_3 - camera_y) / (coord_z_3 - camera_z);

          coord_x_4 = x + 1 - half_width;
          coord_y_4 = terrain->heightfield[pos + terrain->width + 1];
          if (terrain->do_filled_sea && coord_y_4 < terrain->sealevel)
            coord_y_4 = terrain->sealevel;
          coord_y_4 *= y_scale;
          coord_z_4 = half_height - y - 1;
          temp_x_4 = zoom * coord_x_4 / (coord_z_4 - camera_z);
          temp_y_4 = zoom * (coord_y_4 - camera_y) / (coord_z_4 - camera_z);

          /* Calculate the light for the upper left triangle.  We
             also draw the lower right triangle using the same value
             and lighting to save CPU time. */
          vector_y_2 = coord_y_2 - coord_y_1;
          magnitude = sqrt (1.0 + vector_y_2 * vector_y_2);
          vector_y_2 /= magnitude;

          vector_y_3 = coord_y_3 - coord_y_1;
          magnitude = sqrt (1.0 + vector_y_3 * vector_y_3);
          vector_y_3 /= magnitude;

          /* Dot product */
          magnitude = sqrt (vector_y_2 * vector_y_2 + 1.0 +
                            vector_y_3 * vector_y_3);
          coord_x_1 = vector_y_2 / magnitude;
          coord_y_1 = 1.0 / magnitude;
          coord_z_1 = vector_y_3 / magnitude;

          light_x = 0.5773;
          light_y = 0.5773;
          light_z = 0.5773;

          light = pow ((coord_x_1 * light_x +
                        coord_y_1 * light_y +
                        coord_z_1 * light_z + 1.0) *
                        0.5, 2.0) * 0.5;

          t_pixbuf_set_fore (pixbuf,
            MIN ((palette[value] >> 1) + light * 255, 255),
            MIN ((palette[value + 1] >> 1) + light * 255, 255),
            MIN ((palette[value + 2] >> 1) + light * 255, 255));

          points[0].x = scrn_offset_x + temp_x_1;
          points[0].y = scrn_offset_y - temp_y_1;
          points[1].x = scrn_offset_x + temp_x_2;
          points[1].y = scrn_offset_y - temp_y_2;
          points[2].x = scrn_offset_x + temp_x_3;
          points[2].y = scrn_offset_y - temp_y_3;
          t_pixbuf_fill_triangle (pixbuf, points);

          points[0].x = scrn_offset_x + temp_x_4;
          points[0].y = scrn_offset_y - temp_y_4;
          t_pixbuf_fill_triangle (pixbuf, points);

          temp_x_1 = temp_x_3;
          temp_y_1 = temp_y_3;
          temp_x_2 = temp_x_4;
          temp_y_2 = temp_y_4;
          coord_x_1 = coord_x_3;
          coord_y_1 = coord_y_3;
          coord_x_2 = coord_x_4;
          coord_y_2 = coord_y_4;
        }
    }
}


static void
t_terrain_paint_3d_height (TTerrain  *terrain,
                           TPixbuf   *pixbuf,
                           gfloat     elevation, 
                           guchar    *palette, 
                           gboolean   do_tile)
{
  gint   y;
  gint   scrn_offset_x, scrn_offset_y;
  gint   lim_x, lim_y;
  gfloat y_scale;
  gfloat terrain_size, pixbuf_size;
  gfloat camera_y, camera_z;
  gfloat zoom;
  gfloat half_width, half_height;

  terrain_size = MAX (terrain->width, terrain->height);
  pixbuf_size = MIN (pixbuf->width, pixbuf->height);
  y_scale = terrain->y_scale_factor * terrain_size;

  t_pixbuf_clear (pixbuf);
  /* camera_y = terrain_size * 0.75; */
  camera_y = terrain_size * ((elevation + 0.5) + terrain->camera_height_factor);
  camera_z = -terrain_size * 1.5;

  if (do_tile) 
    {
      zoom = pixbuf_size * 0.5;

      half_width = terrain->width;
      half_height = terrain->height;

      lim_x = terrain->width * 2;
      lim_y = terrain->height * 2;
    }
  else
    {
      zoom = pixbuf_size * 1.0;

      half_width = terrain->width * 0.5;
      half_height = terrain->height * 0.5;

      lim_x = terrain->width;
      lim_y = terrain->height;
    }

  /* Use the offsets to center the plot onscreen. */
  scrn_offset_x = pixbuf->width / 2;
  /* scrn_offset_y = -pixbuf->height / 8 +
   *   pixbuf->height / 2 + zoom * (camera_y - 0.5 * y_scale) / camera_z;
   */
  scrn_offset_y =
    pixbuf->height * 0.5 + zoom * (camera_y - 0.5 * y_scale) / camera_z;

  for (y = 0; y + 1 < lim_y; y++)
    {
      gint   x;
      gint   pos;
      gfloat coord_x_1, coord_y_1, coord_z_1, coord_y_2;
      gint   temp_x_1, temp_y_1;
      gint   temp_x_2, temp_y_2;

      pos = y%terrain->height * terrain->width;

      coord_x_1 = -half_width;
      coord_y_1 = terrain->heightfield[pos];
      if (terrain->do_filled_sea && coord_y_1 < terrain->sealevel)
        coord_y_1 = terrain->sealevel;
      coord_y_1 *= y_scale;
      coord_z_1 = half_height - y;
      temp_x_1 = zoom * coord_x_1 / (coord_z_1 - camera_z);
      temp_y_1 = zoom * (coord_y_1 - camera_y) / (coord_z_1 - camera_z);

      /* Required to tile correctly */
      if (y % terrain->height + 1 < terrain->height)
        coord_y_2 = terrain->heightfield[pos + terrain->width];
      else 
        {
          gint yoff = terrain->height - y % terrain->height - 1;
          coord_y_2 = terrain->heightfield[yoff * terrain->width];
        }
      if (terrain->do_filled_sea && coord_y_2 < terrain->sealevel)
        coord_y_2 = terrain->sealevel;
      coord_y_2 *= y_scale;
      temp_x_2 = zoom * coord_x_1 / ((coord_z_1 - 1) - camera_z);
      temp_y_2 = zoom * (coord_y_2 - camera_y) / ((coord_z_1 - 1) - camera_z);

      for (x = 0; x + 1 < lim_x; x++)
        {
          gint     temp_x_3, temp_y_3;
          gint     temp_x_4, temp_y_4;
          gfloat   coord_y_3, coord_y_4;
          gint     value;
          GdkPoint pts[3];

	  /* Simple fix for tiling edges -> skip border pixel */
	  if (do_tile && x == terrain->width-1)
            x++;

          pos = y%terrain->height * terrain->width + x%terrain->width;

          coord_x_1 = x - half_width;

          coord_y_3 = terrain->heightfield[pos + 1];
          if (terrain->do_filled_sea && coord_y_3 < terrain->sealevel)
            coord_y_3 = terrain->sealevel;
          coord_y_3 *= y_scale;
          temp_x_3 = zoom * (coord_x_1 + 1) / (coord_z_1 - camera_z);
          temp_y_3 = zoom * (coord_y_3 - camera_y) / (coord_z_1 - camera_z);

          coord_y_4 = terrain->heightfield[pos + terrain->width + 1];
          if (terrain->do_filled_sea && coord_y_4 < terrain->sealevel)
            coord_y_4 = terrain->sealevel;
          coord_y_4 *= y_scale;
          temp_x_4 = zoom * (coord_x_1 + 1) / ((coord_z_1 - 1) - camera_z);
          temp_y_4 = zoom * (coord_y_4 - camera_y) / ((coord_z_1 - 1) - camera_z);

          value = ((terrain->heightfield[pos] * 255.0) + 0.5);
          value = MAX (MIN (value, 255), 0) * 3;

          t_pixbuf_set_fore (pixbuf, palette[value],
                             palette[value + 1], palette[value + 2]);

          pts[0].x = scrn_offset_x + temp_x_1;
          pts[0].y = scrn_offset_y - temp_y_1;
          pts[1].x = scrn_offset_x + temp_x_2;
          pts[1].y = scrn_offset_y - temp_y_2;
          pts[2].x = scrn_offset_x + temp_x_3;
          pts[2].y = scrn_offset_y - temp_y_3;

          t_pixbuf_fill_triangle (pixbuf, pts);

          pts[0].x = scrn_offset_x + temp_x_4;
          pts[0].y = scrn_offset_y - temp_y_4;

          t_pixbuf_fill_triangle (pixbuf, pts);

          temp_x_1 = temp_x_3;
          temp_y_1 = temp_y_3;
          temp_x_2 = temp_x_4;
          temp_y_2 = temp_y_4;
        }
    }
}


static void
draw_crosshair (TTerrainView *view)
{
  if (GTK_WIDGET_REALIZED (GTK_WIDGET (view)) &&
      view->model == T_VIEW_2D_PLANE &&
      view->crosshair_x >= 0.0 &&
      view->crosshair_y >= 0.0)
    {
      TCanvas   *canvas = T_CANVAS (view);
      GtkWidget *widget = GTK_WIDGET (view);
      gint       x, y;

      x = view->crosshair_x * widget->allocation.width + 0.5;
      y = view->crosshair_y * widget->allocation.height + 0.5;

      /* Horizontal */
      gdk_draw_line (widget->window, canvas->xor_gc,
                     0, y, widget->allocation.width - 1, y);
      /* Vertical */
      gdk_draw_line (widget->window, canvas->xor_gc,
                     x, 0, x, widget->allocation.height - 1);
    }
}


static void
t_terrain_paint (TTerrain     *terrain,
                 TTerrainView *view,
                 TPixbuf      *pixbuf)
{
  guchar *colormap;

  if (pixbuf->width == 0 || pixbuf->height == 0)
    return;

  colormap = colormap_new (view->colormap, terrain->sealevel);

  switch (view->model)
    {
      case T_VIEW_2D_PLANE: 
        t_terrain_paint_2d_plane (terrain, pixbuf, colormap); 
        break;
      case T_VIEW_2D_CONTOUR: 
        t_terrain_paint_2d_contour (terrain, view, pixbuf, colormap); 
        break;
      case T_VIEW_3D_WIRE: 
        t_terrain_paint_3d_wire (terrain, pixbuf, view->angle, 
          view->elevation, terrain->wireframe_resolution, colormap, 
	  view->do_tile); 
        break;
      case T_VIEW_3D_HEIGHT: 
        t_terrain_paint_3d_height (terrain, pixbuf, view->elevation,
          colormap, view->do_tile); 
        break;
      case T_VIEW_3D_LIGHT: 
        t_terrain_paint_3d_light (terrain, pixbuf, view->elevation, 
          colormap, view->do_tile); 
        break;
    }

  g_free (colormap);
}


static gint
t_terrain_view_expose_event (GtkWidget      *widget,
                             GdkEventExpose *event)
{
  TTerrainView *view = T_TERRAIN_VIEW (widget);
  gint          status;

  if (view->dirty)
    {
      if (view->terrain != NULL)
        {
          if (view->model == T_VIEW_2D_CONTOUR)
            {
              if (view->contour_dirty)
                {
                  t_terrain_contour_list_free (view->contour_lines);

                  view->contour_lines =
                    t_terrain_contour_lines (view->terrain,
                      view->terrain->contour_levels, 5);
                }
            }

          t_terrain_paint (view->terrain, view, T_CANVAS (view)->pixbuf);
        }
      else
        {
          t_pixbuf_clear (t_canvas_get_pixbuf (T_CANVAS (view)));
        }

      view->dirty = FALSE;
      view->contour_dirty = FALSE;
    }

  status = 0;
  if (GTK_WIDGET_CLASS (parent_class)->expose_event != NULL)
    status = GTK_WIDGET_CLASS (parent_class)->expose_event (widget, event);

  draw_crosshair (view);

  return status;
}


static void
t_terrain_view_size_allocate (GtkWidget     *widget,
                              GtkAllocation *allocation)
{
  TTerrainView *view = T_TERRAIN_VIEW (widget);

  if (T_CANVAS (view)->pixbuf->width != allocation->width ||
      T_CANVAS (view)->pixbuf->height != allocation->height)
    view->dirty = TRUE;

  GTK_WIDGET_CLASS (parent_class)->size_allocate (widget, allocation);
}


static gint
t_terrain_view_button_press (GtkWidget      *widget,
                             GdkEventButton *event)
{
  TTerrainView *view = T_TERRAIN_VIEW (widget);

  view->anchor_x      = event->x;
  view->anchor_y      = event->y;
  view->anchor_object = -1;

  if ((event->button == 1 || event->button == 3) &&
      view->model == T_VIEW_2D_PLANE && 
      view->tool_mode == T_TOOL_MOVE)
    {
      gint      i;
      gint      objidx = -1;
      gfloat    x, y;
      gfloat    limit;
      GArray   *array;
      TPixbuf  *pixbuf;
      TTerrain *terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW(view));

      pixbuf = T_CANVAS (view)->pixbuf;
      array = view->terrain->objects;

      x = ((gfloat) event->x) / (pixbuf->width - 1);
      y = ((gfloat) event->y) / (pixbuf->height - 1);

      limit = 4.0 / MIN (pixbuf->width, pixbuf->height);

      for (i=0; i<array->len && objidx==-1; i++)
        {
          TTerrainObject *object;

          object = &g_array_index (array, TTerrainObject, i);

          if (fabs (x - object->x) < limit &&
              fabs (y - object->y) < limit)
            objidx = i;
        }

      if (objidx != -1)
        {
          /* button 1 selects an object */
          if (event->button == 1)
            view->anchor_object = objidx;
          else if (event->button == 3)
            {
              /* button 3 deletes an object */

              TTerrain *terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW(view));
              on_object_deleted (GTK_OBJECT (terrain), objidx, GTK_WIDGET (view));
	    }
        }

      /* clicking in the move tool mode clears out the current selection */
      terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW(view));
      if (terrain->selection != NULL)
        {
          t_terrain_select_destroy (terrain);
          t_terrain_view_export (view, pixbuf);
        }
    }
  else if (event->button == 2 &&
           view->model == T_VIEW_3D_WIRE)
    {
      view->anchor_x  = 0;
      view->anchor_y  = 0;
      t_terrain_view_set_angle (view, 0.0);
      t_terrain_view_set_elevation (view, 0.25);
    }

  if (event->button == 1)
    gtk_grab_add (widget);

  return FALSE;
}


static gint
t_terrain_view_scroll (GtkWidget      *widget,
                       GdkEventScroll *event)
{
  TTerrainView *view = T_TERRAIN_VIEW (widget);
  int dirty = FALSE;

  view->anchor_x      = event->x;
  view->anchor_y      = event->y;
  view->anchor_object = -1;

  if (view->model == T_VIEW_3D_WIRE) 
  {
    if (event->direction == GDK_SCROLL_UP) 
    {
      if (view->terrain->wireframe_resolution < 50) 
      {
        view->terrain->wireframe_resolution++;
        dirty = TRUE;
      }
    } 
    else if (event->direction == GDK_SCROLL_DOWN) 
    {
      if (view->terrain->wireframe_resolution > 1)  
      {
        view->terrain->wireframe_resolution--;
        dirty = TRUE;
      }
    }
  }

  if (dirty) {
      t_terrain_paint (view->terrain, view, T_CANVAS (view)->pixbuf);
      gtk_widget_queue_draw (GTK_WIDGET (view));
  }

  return FALSE;
}


static gint
t_terrain_view_motion_notify (GtkWidget      *widget,
                              GdkEventMotion *event)
{
  TTerrainView *view = T_TERRAIN_VIEW (widget);
  gint        x, y;

  /* Get x and y coordinates on hint. */
  if (event->is_hint)
    {
      gint xint, yint;
      GdkModifierType state;

      gdk_window_get_pointer (event->window, &xint, &yint, &state);

      if ((state & GDK_BUTTON1_MASK) == 0)
        return FALSE;

      x = xint;
      y = yint;
    }
  else
    x = event->x, y = event->y;

  if (view->model == T_VIEW_3D_WIRE)
    {
      gfloat   angle_delta, elevation_delta;
      TPixbuf *pixbuf = view->canvas.pixbuf;

      angle_delta = 2.0 * (x - view->anchor_x) / (pixbuf->width + 1.0);
      elevation_delta = 2.0 * (y - view->anchor_y) / (pixbuf->height + 1.0);

      t_terrain_view_set_angle (view, view->angle + angle_delta);
      t_terrain_view_set_elevation (view, view->elevation + elevation_delta);

      view->anchor_x = x;
      view->anchor_y = y;
    }

  if (view->model == T_VIEW_2D_PLANE && view->anchor_object != -1)
    {
      gfloat          obj_x, obj_y;
      TPixbuf        *pixbuf;

      pixbuf = t_canvas_get_pixbuf (T_CANVAS (view));

      obj_x = ((gfloat) x) / (pixbuf->width - 1);
      obj_y = ((gfloat) y) / (pixbuf->height - 1);

      t_terrain_move_object (view->terrain, view->anchor_object,
                              obj_x, obj_y);
    }

  if (view->model == T_VIEW_2D_PLANE &&
      view->tool_mode == T_TOOL_SELECT_RECTANGLE)
    t_canvas_set_selection (T_CANVAS (view), T_SELECTION_RECTANGLE,
                            view->anchor_x, view->anchor_y, x, y);

  if (view->model == T_VIEW_2D_PLANE &&
      view->tool_mode == T_TOOL_SELECT_ELLIPSE)
    t_canvas_set_selection (T_CANVAS (view), T_SELECTION_ELLIPSE,
                            view->anchor_x, view->anchor_y, x, y);

  if (view->model == T_VIEW_2D_PLANE &&
      view->tool_mode == T_TOOL_SELECT_SQUARE_ZOOM)
    {
    int size = MIN (x-view->anchor_x, y-view->anchor_y);
    t_canvas_set_selection (T_CANVAS (view), T_SELECTION_SQUARE_ZOOM,
                            view->anchor_x, view->anchor_y, 
			    view->anchor_x+size, view->anchor_y+size);
    }

  if (view->model == T_VIEW_2D_PLANE &&
      view->tool_mode == T_TOOL_SELECT_CROP_NEW)
    t_canvas_set_selection (T_CANVAS (view), T_SELECTION_RECTANGLE,
                            view->anchor_x, view->anchor_y, x, y);

  if (view->model == T_VIEW_2D_PLANE &&
      view->tool_mode == T_TOOL_SELECT_SQUARE_SEED)
    {
    int size = MIN (x-view->anchor_x, y-view->anchor_y);
    t_canvas_set_selection (T_CANVAS (view), T_SELECTION_SQUARE_SEED,
                            view->anchor_x, view->anchor_y, 
			    view->anchor_x+size, view->anchor_y+size);
    }

  return TRUE;
}


static gint
t_terrain_view_key_press (GtkWidget      *widget,
                          GdkEventKey    *event)
{
  printf ("In keypress handler ...\n"); fflush (stdout);
  return FALSE;
}

static gint
t_terrain_view_focus_in  (GtkWidget      *widget,
                          GdkEventFocus  *event)
{
  printf ("In focus in handler ...\n"); fflush (stdout);
  GTK_WIDGET_SET_FLAGS (widget, GTK_HAS_FOCUS);
  return FALSE;
}

static gint
t_terrain_view_focus_out (GtkWidget      *widget,
                          GdkEventFocus  *event)
{
  printf ("In focus out handler ...\n"); fflush (stdout);
  GTK_WIDGET_UNSET_FLAGS (widget, GTK_HAS_FOCUS);
  return FALSE;
}


static gint
t_terrain_view_button_release (GtkWidget      *widget,
                               GdkEventButton *event)
{
  TTerrainView   *view = T_TERRAIN_VIEW (widget);
  TSelectionType  type;
  gfloat          x1, y1, x2, y2;

  if (event->button != 1)
    return FALSE;

  type = t_canvas_get_selection (T_CANVAS (view), &x1, &y1, &x2, &y2);

  if (x2 != 0 || y2 != 0)
    {
      TTerrain  *terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW(view));
      GtkWidget *terrain_window = lookup_toplevel (widget);
      gint       twidth = terrain->width;
      gint       theight = terrain->height;

      t_terrain_select (view->terrain, x1, y1, x2, y2, type, view->compose_op);
      t_canvas_set_selection (T_CANVAS (view), T_SELECTION_NONE, 0, 0, 0, 0);

      if (view->tool_mode == T_TOOL_SELECT_SQUARE_ZOOM)
        {
        t_terrain_zoom (terrain, x1 * twidth, y1 * theight,
                        x2 * twidth, y2 * theight);
	terrain_window_save_state (terrain_window, _("Zoom"));
        }
      else
      if (view->tool_mode == T_TOOL_SELECT_CROP_NEW)
        {
          terrain = t_terrain_crop_new (terrain, x1 * twidth, y1 * theight, 
                                        x2 * twidth, y2 * theight);

	  if (terrain != NULL)
            {
              GtkWidget *main_window;

              main_window = lookup_data (GTK_WIDGET (widget), "data_parent");
              terrain_window = terrain_window_new (main_window, terrain);
              terrain_window_save_state (terrain_window, _("Crop"));
            }
        }
      else
      if (view->tool_mode == T_TOOL_SELECT_SQUARE_SEED)
        {
	t_terrain_generate_subdiv_seed_ns (0, twidth, theight, 0.5, -1,
                                           terrain, x1*twidth, y1*theight,
                                           (x2-x1)*twidth, (y2-y1)*theight);
	terrain_window_save_state (terrain_window, _("Seed"));
        }
    }

  gtk_grab_remove (widget);

  return FALSE;
}


void
t_terrain_view_export (TTerrainView *view,
                       TPixbuf      *pixbuf)
{
  t_terrain_paint (view->terrain, view, pixbuf);
}


void
t_terrain_view_set_crosshair (TTerrainView  *view,
                              gfloat         x,
                              gfloat         y)
{
  if (x == view->crosshair_x && y == view->crosshair_y)
    return;

  draw_crosshair (view);

  view->crosshair_x = x;
  view->crosshair_y = y;

  draw_crosshair (view);
}


gint
t_terrain_view_get_size (TTerrainView *view)
{
  return view->size;
}


void
t_terrain_view_set_size (TTerrainView *view,
                         gint          size)
{
  gint width, height;

  if (view->terrain != NULL)
    {
      gint max;

      if (GTK_IS_ASPECT_FRAME (GTK_WIDGET (view)->parent))
        gtk_aspect_frame_set (GTK_ASPECT_FRAME (GTK_WIDGET (view)->parent),
                              0.5, 0.5,
                              ((gfloat) view->terrain->width) /
                              view->terrain->height, FALSE);

      max = MAX (view->terrain->width, view->terrain->height);
      width = MAX (1, size * view->terrain->width / max);
      height = MAX (1, size * view->terrain->height / max);

      //printf ("tterrainview setsize: %d %d\n", width, height);

      gtk_drawing_area_size (GTK_DRAWING_AREA (view), width, height);

      /* HACK:
       *
       * This function queues up one event: size allocate.  However,
       * there may already be a paint event queued.  In that case,
       * Gtk+ will process the paint event first before the
       * size change.  The size allocate then creates a paint event
       * (after reallocating the pixbuf to the new size).
       * We avoid the first paint by making the call below to set
       * the pixbuf size to 0,0.  Thus, we avoid flicker.
       */

      t_pixbuf_set_size (T_CANVAS (view)->pixbuf, 0, 0);
    }

  view->size = size;
}


TTerrain *
t_terrain_view_get_terrain (TTerrainView *view)
{
  return view->terrain;
}


static void
on_heightfield_modified (GtkObject *object,
                         GtkWidget *user_data)
{
  TTerrainView *view = T_TERRAIN_VIEW (user_data);

  view->dirty = TRUE;
  view->contour_dirty = TRUE;

  t_terrain_view_set_size (view, view->size);

  gtk_widget_queue_draw (GTK_WIDGET (view));
}


static void
on_selection_modified (GtkObject *object,
                       GtkWidget *user_data)
{
  TTerrainView *view = T_TERRAIN_VIEW (user_data);

  if (view->model == T_VIEW_2D_PLANE)
    {
      view->dirty = TRUE;
      gtk_widget_queue_draw (GTK_WIDGET (view));
    }
}


static void
on_object_added (GtkObject *terrain,
                 gint       item,
                 GtkWidget *user_data)
{
  TTerrainView   *view = T_TERRAIN_VIEW (user_data);
  TTerrainObject *object;

  if (view->model != T_VIEW_2D_PLANE)
    return;

  object = &g_array_index (view->terrain->objects, TTerrainObject, item);
  t_canvas_add_object (T_CANVAS (view), object->x, object->y, object->angle);
}


static void
on_object_modified (GtkObject *terrain,
                    gint       item,
                    GtkWidget *user_data)
{
  TTerrainView   *view = T_TERRAIN_VIEW (user_data);
  TTerrainObject *object;

  if (view->model != T_VIEW_2D_PLANE)
    return;

  object = &g_array_index (view->terrain->objects, TTerrainObject, item);
  t_canvas_modify_object (T_CANVAS (view), item, object->x, object->y, object->angle);
}


static void
on_object_deleted (GtkObject *terrain,
                   gint       item,
                   GtkWidget *user_data)
{
  TTerrainView   *view = T_TERRAIN_VIEW (user_data);

  if (view->model != T_VIEW_2D_PLANE)
    return;

  t_terrain_remove_object (view->terrain, item);
  t_canvas_delete_object (T_CANVAS (view), item);
}


void 
t_terrain_view_delete_selected_objects (TTerrainView *view)
{
  gint      i, size;
  TTerrain *terrain;

  terrain = view->terrain;
  size = terrain->objects->len;
  for (i = size - 1; i >= 0; i--)
    {
    TTerrainObject *obj = &g_array_index (terrain->objects, TTerrainObject, i);
    gint pos = obj->oy * terrain->height + obj->ox;

    if (terrain->selection != NULL && terrain->selection[pos] > 0.5)
      on_object_deleted (GTK_OBJECT (terrain), i, GTK_WIDGET (view));
    }
}

void 
t_terrain_view_prune_objects (TTerrainView *view, 
                              gfloat factor, 
                              gint seed)
{
  gint      i, size;
  TTerrain *terrain;
  TRandom  *rnd;

  terrain = view->terrain;

  if (seed == -1)
    seed = new_seed ();
  rnd = t_random_new (seed);

  terrain = view->terrain;
  size = terrain->objects->len;
  for (i = size - 1; i >= 0; i--)
    {
    TTerrainObject *obj = &g_array_index (terrain->objects, TTerrainObject, i);
    gint pos = obj->oy * terrain->height + obj->ox;

    if (terrain->selection == NULL || terrain->selection[pos] > 0.5)
      if (t_random_rnd1(rnd) < factor)
        on_object_deleted (GTK_OBJECT (terrain), i, GTK_WIDGET (view));
    }

  t_random_free (rnd);
  
}


void
t_terrain_view_set_terrain (TTerrainView *view,
                            TTerrain     *terrain)
{
  if (view->terrain != NULL)
    {
      gtk_signal_disconnect (GTK_OBJECT (view->terrain),
                             view->heightfield_modified_id);
      view->heightfield_modified_id = 0;
      gtk_signal_disconnect (GTK_OBJECT (view->terrain),
                             view->selection_modified_id);
      view->selection_modified_id = 0;
      gtk_signal_disconnect (GTK_OBJECT (view->terrain),
                             view->object_added_id);
      view->object_added_id = 0;
      gtk_signal_disconnect (GTK_OBJECT (view->terrain),
                             view->object_modified_id);
      view->object_modified_id = 0;
      gtk_signal_disconnect (GTK_OBJECT (view->terrain),
                             view->object_deleted_id);
      view->object_deleted_id = 0;

      t_terrain_unref (view->terrain);
      t_terrain_contour_list_free (view->contour_lines);
      view->contour_lines = NULL;
    }

  view->terrain = terrain;

  if (view->terrain != NULL)
    {
      gint i;

      /* Add objects to canvas */
      if (view->model == T_VIEW_2D_PLANE)
        for (i = 0; i < terrain->objects->len; i++)
          on_object_added (GTK_OBJECT (terrain), i, GTK_WIDGET (view));

      if (GTK_IS_ASPECT_FRAME (GTK_WIDGET (view)->parent))
        gtk_aspect_frame_set (GTK_ASPECT_FRAME (GTK_WIDGET (view)->parent),
                              0.5, 0.5,
                              ((gfloat) terrain->width) / terrain->height,
                              FALSE);
      if (T_IS_ASPECT_FRAME (GTK_WIDGET (view)->parent))
        t_aspect_frame_enable (T_ASPECT_FRAME (GTK_WIDGET (view)->parent),
                               view->model == T_VIEW_2D_PLANE ||
                               view->model == T_VIEW_2D_CONTOUR);

      view->heightfield_modified_id =
        gtk_signal_connect (GTK_OBJECT (terrain), "heightfield_modified",
                            (GtkSignalFunc) &on_heightfield_modified, view);

      view->selection_modified_id =
        gtk_signal_connect (GTK_OBJECT (terrain), "selection_modified",
                            (GtkSignalFunc) &on_selection_modified, view);

      view->object_added_id =
        gtk_signal_connect (GTK_OBJECT (terrain), "object_added",
                            (GtkSignalFunc) &on_object_added, view);

      view->object_modified_id =
        gtk_signal_connect (GTK_OBJECT (terrain), "object_modified",
                            (GtkSignalFunc) &on_object_modified, view);

      view->object_deleted_id =
        gtk_signal_connect (GTK_OBJECT (terrain), "object_deleted",
                            (GtkSignalFunc) &on_object_deleted, view);

      t_terrain_view_set_size (view, view->size);

      t_terrain_ref (view->terrain);
    }

  view->contour_dirty = TRUE;
  view->dirty = TRUE;
  gtk_widget_queue_draw (GTK_WIDGET (view));
}


TViewModel
t_terrain_view_get_model (TTerrainView *view)
{
  return view->model;
}


void
t_terrain_view_set_model (TTerrainView *view,
                          TViewModel    model)
{
  if (view->model != model)
    {
      gint i;

      view->model = model;
      view->dirty = TRUE;
      view->contour_dirty = TRUE;

      t_terrain_contour_list_free (view->contour_lines);
      view->contour_lines = NULL;

      if (T_IS_ASPECT_FRAME (GTK_WIDGET (view)->parent))
        t_aspect_frame_enable (T_ASPECT_FRAME (GTK_WIDGET (view)->parent),
                               view->model == T_VIEW_2D_PLANE ||
                               view->model == T_VIEW_2D_CONTOUR);

      gtk_widget_queue_draw (GTK_WIDGET (view));

      if (view->model == T_VIEW_2D_PLANE)
        for (i = 0; i < view->terrain->objects->len; i++)
          on_object_added (GTK_OBJECT (view->terrain), i, GTK_WIDGET (view));
      else
        t_canvas_clear_objects (T_CANVAS (view));
    }
}


TColormapType
t_terrain_view_get_colormap (TTerrainView *view)
{
  return view->colormap;
}


void
t_terrain_view_set_colormap (TTerrainView  *view,
                             TColormapType  colormap)
{
  if (view->colormap != colormap)
    {
      view->colormap = colormap;

      view->dirty = TRUE;
      gtk_widget_queue_draw (GTK_WIDGET (view));
    }
}


gfloat
t_terrain_view_get_angle (TTerrainView *view)
{
  return view->angle;
}


void
t_terrain_view_set_angle (TTerrainView *view,
                          gfloat        angle)
{
  while (angle >= 2.0 * G_PI)
    angle -= 2.0 * G_PI;

  while (angle < 0.0)
    angle += 2.0 * G_PI;

  view->angle = angle;

  view->dirty = TRUE;
  gtk_widget_queue_draw (GTK_WIDGET (view));
}


gfloat
t_terrain_view_get_elevation (TTerrainView *view)
{
  return view->elevation;
}


void
t_terrain_view_set_elevation (TTerrainView *view,
                              gfloat        elevation)
{
  if (elevation > 2.0)
    elevation = 2.0;

  if (elevation < -2.0)
    elevation = -2.0;

  view->elevation = elevation;

  view->dirty = TRUE;
  gtk_widget_queue_draw (GTK_WIDGET (view));
}


gboolean
t_terrain_view_get_auto_rotate (TTerrainView *view)
{
  return view->auto_rotate;
}


gboolean
t_terrain_view_get_do_tile (TTerrainView *view)
{
  return view->do_tile;
}


static gint
auto_rotate_func (gpointer user_data)
{
  TTerrainView *view = T_TERRAIN_VIEW (user_data);

  if (view->model == T_VIEW_3D_WIRE)
    t_terrain_view_set_angle (view, t_terrain_view_get_angle (view) + 0.01);

  return TRUE;
}


void
t_terrain_view_set_auto_rotate (TTerrainView *view,
                                gboolean      auto_rotate)
{
  if (auto_rotate != view->auto_rotate)
    {
      if (auto_rotate)
        view->rotate_timeout =
          gtk_timeout_add (100, (GtkFunction) &auto_rotate_func, view);
      else
        {
          gtk_timeout_remove (view->rotate_timeout);
          view->rotate_timeout = 0;
        }

      view->auto_rotate = auto_rotate;
    }
}


void
t_terrain_view_set_do_tile (TTerrainView *view,
                            gboolean      do_tile)
{
  view->do_tile = do_tile;
  view->dirty = TRUE;
  gtk_widget_queue_draw (GTK_WIDGET (view));
}


TToolMode
t_terrain_view_get_tool (TTerrainView  *view)
{
  return view->tool_mode;
}


void
t_terrain_view_set_tool (TTerrainView  *view,
                         TToolMode      tool)
{
  view->tool_mode = tool;
}


TComposeOp
t_terrain_view_get_compose_op (TTerrainView  *view)
{
  return view->compose_op;
}


void
t_terrain_view_set_compose_op (TTerrainView  *view,
                               TComposeOp     op)
{
  view->compose_op = op;
}
