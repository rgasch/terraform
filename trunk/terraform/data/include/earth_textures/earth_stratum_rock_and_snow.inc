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

#local LOCAL_RATIO = 2.0 ; // 
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

#declare TF_ROCK_TEXTURE =
	texture {	
                gradient y
		texture_map {
			[0	BTEX_Rock ]
			[1.00	BTEX_Rock finish{ ambient 0.1*LOCAL_RATIO} ]
                }
                scale 0.025
                rotate <0, 0, LOCAL_ANGLE>
        }


#declare TF_LAND_TEXTURE =
	texture {	
                slope {
                  y, 0.5, 1.0
                  altitude y, 0, 1
                }
		texture_map {
			[0	TF_ROCK_TEXTURE ]
			[0.70	TF_ROCK_TEXTURE ]
                        [0.72   BTEX_Snow ]
			[1.0	BTEX_Snow ]
		}
	}

#declare TF_LANDSCAPE_TEXTURE =
	texture {	
                gradient y
		texture_map {
			[0	        TF_ROCK_TEXTURE ]
			[TF_WATER_LEVEL	TF_ROCK_TEXTURE ]
			[TF_WATER_LEVEL	TF_LAND_TEXTURE ]
			[1.00	        TF_LAND_TEXTURE ]
                }
        }















