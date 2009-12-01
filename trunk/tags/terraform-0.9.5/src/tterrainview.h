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


#ifndef __T_TERRAIN_VIEW_H__
#define __T_TERRAIN_VIEW_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gtk/gtk.h>
#include "tcanvas.h"
#include "tterrain.h"
#include "colormaps.h"

#define T_TYPE_TERRAIN_VIEW            (t_terrain_view_get_type ())
#define T_TERRAIN_VIEW(obj)            (GTK_CHECK_CAST ((obj), T_TYPE_TERRAIN_VIEW, TTerrainView))
#define T_TERRAIN_VIEW_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), T_TYPE_TERRAIN_VIEW, TTerrainViewClass))
#define T_IS_TERRAIN_VIEW(obj)         (GTK_CHECK_TYPE ((obj), T_TYPE_TERRAIN_VIEW))
#define T_IS_TERRAIN_VIEW_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), T_TYPE_TERRAIN_VIEW))


typedef struct TTerrainView      TTerrainView;
typedef struct TTerrainViewClass TTerrainViewClass;
typedef enum   TViewModel        TViewModel;
typedef enum   TToolMode         TToolMode;


enum TViewModel
{
  T_VIEW_2D_PLANE,
  T_VIEW_2D_CONTOUR,
  T_VIEW_3D_WIRE,
  T_VIEW_3D_HEIGHT,
  T_VIEW_3D_LIGHT
};


enum TToolMode
{
  T_TOOL_NONE = -1,
  T_TOOL_MOVE,
  T_TOOL_SELECT_RECTANGLE,
  T_TOOL_SELECT_ELLIPSE,
  T_TOOL_SELECT_SQUARE_ZOOM,
  T_TOOL_SELECT_CROP_NEW, 
  T_TOOL_SELECT_SQUARE_SEED
};

struct TTerrainViewClass
{
  TCanvasClass parent_class;
};

struct TTerrainView
{
  TCanvas        canvas;

  gboolean       contour_dirty;
  gboolean       dirty;

  gint           size;
  guint          rotate_timeout;
  TTerrain      *terrain;
  TViewModel     model;
  TColormapType  colormap;
  gfloat         angle;
  gfloat         elevation;
  gboolean       auto_rotate;
  gboolean       do_tile;

  gfloat         crosshair_x;
  gfloat         crosshair_y;

  /* Where the mouse button was first pressed. */
  gint           anchor_x;
  gint           anchor_y;
  gint           anchor_object;

  /* contour_lines is a GList of GArrays of a gfloat height + gfloat pairs */
  GList         *contour_lines;

  /* Ids of signal connections made to TTerrain object */
  guint          heightfield_modified_id;
  guint          selection_modified_id;
  guint          object_added_id;
  guint          object_modified_id;
  guint          object_deleted_id;

  /* Modes of user interaction */
  TToolMode      tool_mode;
  TComposeOp     compose_op;
};

GtkType         t_terrain_view_get_type        (void);
GtkWidget      *t_terrain_view_new             (void);

void            t_terrain_view_delete_selected_objects (TTerrainView  *view);
void            t_terrain_view_prune_objects   (TTerrainView  *view, 
                                                gfloat         factor, 
                                                gint           seed); 
void            t_terrain_view_export          (TTerrainView  *view,
                                                TPixbuf       *pixbuf);
void            t_terrain_view_set_crosshair   (TTerrainView  *view,
                                                gfloat         x,
                                                gfloat         y);
gint            t_terrain_view_get_size        (TTerrainView  *view);
void            t_terrain_view_set_size        (TTerrainView  *view,
                                                gint           size);
TTerrain       *t_terrain_view_get_terrain     (TTerrainView  *view);
void            t_terrain_view_set_terrain     (TTerrainView  *view,
                                                TTerrain      *terrain);
TViewModel      t_terrain_view_get_model       (TTerrainView  *view);
void            t_terrain_view_set_model       (TTerrainView  *view,
                                                TViewModel     model);
TColormapType   t_terrain_view_get_colormap    (TTerrainView  *view);
void            t_terrain_view_set_colormap    (TTerrainView  *view,
                                                TColormapType  colormap);
gfloat          t_terrain_view_get_angle       (TTerrainView  *view);
void            t_terrain_view_set_angle       (TTerrainView  *view,
                                                gfloat         angle);
gfloat          t_terrain_view_get_elevation   (TTerrainView  *view);
void            t_terrain_view_set_elevation   (TTerrainView  *view,
                                                gfloat         elevation);
gboolean        t_terrain_view_get_auto_rotate (TTerrainView  *view);
void            t_terrain_view_set_auto_rotate (TTerrainView  *view,
                                                gboolean       auto_rotate);
gboolean        t_terrain_view_get_do_tile     (TTerrainView  *view);
void            t_terrain_view_set_do_tile     (TTerrainView  *view,
                                                gboolean       do_tile);
TToolMode       t_terrain_view_get_tool        (TTerrainView  *view);
void            t_terrain_view_set_tool        (TTerrainView  *view,
                                                TToolMode      tool);
TComposeOp      t_terrain_view_get_compose_op  (TTerrainView  *view);
void            t_terrain_view_set_compose_op  (TTerrainView  *view,
                                                TComposeOp     op);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __T_TERRAIN_VIEW_H__ */
