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


#ifndef __COLORMAPS_H__
#define __COLORMAPS_H__

#include <glib.h>

typedef enum TColormapType TColormapType;
 
enum TColormapType
{
  T_COLORMAP_LAND,
  T_COLORMAP_DESERT,
  T_COLORMAP_GRAYSCALE,
  T_COLORMAP_HEAT,
  T_COLORMAP_WASTELAND
};

guchar *colormap_new (TColormapType which_colormap,
                      gfloat        water_level);

#endif /* __COLORMAPS_H__ */  
