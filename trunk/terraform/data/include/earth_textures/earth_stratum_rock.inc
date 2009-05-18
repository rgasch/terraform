//----------------------------------------------------
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
//----------------------------------------------------

//---------------------------------------------
//  Parameters
//---------------------------------------------

//#local SCALE_pigment = 25 ; //global scale for the textures
//#local TF_scale = <1/TF_X_SCALE, 1/TF_Y_SCALE, 1/TF_Z_SCALE> * SCALE_pigment ;

//#local SCALE_normal = 1 ; // global scale for the bump mapping
//#local TF_bumpyscale = <1/TF_X_SCALE, 1/TF_Y_SCALE, 1/TF_Z_SCALE> * SCALE_normal ;

//#local DEPTH_bump = 0.5 ; // global scale for the depth of the bump mapping

#local LOCAL_RATIO = 2.5 ; // 
#local LOCAL_ANGLE = 30 ; //

//---------------------------------------------
//  Basic Textures
//---------------------------------------------

#include "basic_textures.inc"
// Variables : BTEX_SCALE, BTEX_BUMPSCALE, BTEX_BUMPDEPTH
// Normal, Finish : BTEX_N_Grass, BTEX_F_Grass, BTEX_F_Snow
// Textures : BTEX_Rock, BTEX_Snow, BTEX_Grass

//---------------------------------------------
//  Main Texture
//---------------------------------------------

#declare TF_LANDSCAPE_TEXTURE =
	texture {	
                gradient y
		texture_map {
			[0	BTEX_Rock ]
			[1.00	BTEX_Rock finish{ ambient 0.1*LOCAL_RATIO} ]
                }
                scale 0.025
                rotate <0, 0, LOCAL_ANGLE>
        }













