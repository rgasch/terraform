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


#ifndef __T_ASPECT_FRAME_H__
#define __T_ASPECT_FRAME_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gtk/gtk.h>

#define T_TYPE_ASPECT_FRAME            (t_aspect_frame_get_type ())
#define T_ASPECT_FRAME(obj)            (GTK_CHECK_CAST ((obj), T_TYPE_ASPECT_FRAME, TAspectFrame))
#define T_ASPECT_FRAME_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), T_TYPE_ASPECT_FRAME, TAspectFrameClass))
#define T_IS_ASPECT_FRAME(obj)         (GTK_CHECK_TYPE ((obj), T_TYPE_ASPECT_FRAME))
#define T_IS_ASPECT_FRAME_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), T_TYPE_ASPECT_FRAME))


typedef struct TAspectFrame      TAspectFrame;
typedef struct TAspectFrameClass TAspectFrameClass;

struct TAspectFrameClass
{
  GtkAspectFrameClass parent_class;
};

struct TAspectFrame
{
  GtkAspectFrame frame;

  gboolean       enabled;
};

GtkType         t_aspect_frame_get_type        (void);
GtkWidget      *t_aspect_frame_new             (void);

void            t_aspect_frame_enable          (TAspectFrame *frame,
                                                gboolean      enable);
gboolean        t_aspect_frame_is_enabled      (TAspectFrame *frame);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __T_ASPECT_FRAME_H__ */
