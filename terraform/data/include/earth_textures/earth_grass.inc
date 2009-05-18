// Copyright (c) 2002 Raymond Ostertag
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

//---------------------------------------------
//  Parameters
//---------------------------------------------

#local LOCAL_SCALE = 0.001;
#local LOCAL_GRASS_COLOR = <0.50, 0.65, 0.30>;
#local LOCAL_FLOWERCOLOR_1 = <1, 1, 1>;
#local LOCAL_FLOWERCOLOR_2 = <1, 1, 0>;
#local LOCAL_GRASS_GRAIN = 1;

//---------------------------------------------
//  Basic Textures
//---------------------------------------------

#include "grasstex.inc"    // See grastex.inc for more informations
#declare gt_height  = 2.0 ; 
#declare gt_samples = 80 *  TF_LANDSCAPE_DISTRIBUTION;   //increase this to have more density
#declare gt_tweak   = 0.025; //increase this to have bigger blades
#declare gt_turb = TF_LANDSCAPE_TURBULENCE / 100;
#declare gt_groundcolors   =
   color_map {
      [0.0, color LOCAL_GRASS_COLOR  * ( 1 - (LOCAL_GRASS_GRAIN / 5)) ]
      [0.5, color LOCAL_GRASS_COLOR  * ( 1 - (LOCAL_GRASS_GRAIN / 5)) ]
      [1.0, color LOCAL_GRASS_COLOR  * ( 1 - (LOCAL_GRASS_GRAIN / 5)) ]
   }

#declare camera_location = TF_CAMERA_LOCATION ;

// *********************************************

#local GRASS_BG_TEXTURE =
      texture {
         gt_ground()
         normal {
           bumps 0.5 
           scale 0.01
         }
      }
#local GRASS_FG_TEXTURE =
      texture {
         #local gt_color   = LOCAL_GRASS_COLOR ;
         #local gt_offset  = -0.0;
         #local gt_seed    = 0;
         gt_grass ()
         normal {
           bumps 0.5 
           scale 2 
           warp {turbulence 2} 
           scale 1 
         }
         scale LOCAL_SCALE/TF_SCALE 
      }

#local GRASS_WHITEFLOWER_TEXTURE =
      texture {
         #local gt_flowercolor = LOCAL_FLOWERCOLOR_1 ;
         #local gt_seed = 0;
         gt_flowers ()
         normal {
           bumps 0.5 
           scale 2 
           warp {turbulence 2} 
           scale 1
         }
         scale LOCAL_SCALE/TF_SCALE
      }

#local GRASS_YELLOWFLOWER_TEXTURE =
      texture {
         #local gt_flowercolor = LOCAL_FLOWERCOLOR_2 ;
         #local gt_seed = 10000;
         gt_flowers ()
         normal {
           bumps 0.5 
           scale 2 
           warp {turbulence 2} 
           scale 1
         }
         scale LOCAL_SCALE/TF_SCALE
      }


#declare TF_LANDSCAPE_TEXTURE =  
//    The very first layer should be the ground layer:
      texture {
        GRASS_BG_TEXTURE
      }
      texture {
        GRASS_FG_TEXTURE
      }
      texture {
        GRASS_WHITEFLOWER_TEXTURE
      }
      texture {
        GRASS_YELLOWFLOWER_TEXTURE
      }