// Copyright (c) 2003 Raymond Ostertag
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
//  Additonnal Parameters 
// (see defaults.inc for standard parameters )
//---------------------------------------------

//---------------------------------------------
//  Basic Textures
//---------------------------------------------

#include "basic_textures.inc"

#if (TF_LANDSCAPE_WATER_TRANSITION_ICE)
#declare TF_LANDSCAPE_TRANSITION_TEXTURE = BTEX_Ice;
#end

#if (TF_LANDSCAPE_WATER_TRANSITION_SAND)
#declare TF_LANDSCAPE_TRANSITION_TEXTURE = BTEX_Sand;
#end

//---------------------------------------------
//  Main Texture
//---------------------------------------------

#local WL= TF_WATER_LEVEL;
#local RL = TF_ROCK_LEVEL;
#local SL= TF_SNOW_LEVEL;

#if ( TF_ROCK_STRATUM )
  #local LOCAL_Rock = BTEX_Rock_stratum;
#else
  #local LOCAL_Rock = BTEX_Rock;
#end

#if ( TF_GRASSTEX )
  #local LOCAL_Grass = BTEX_Grasstex;
#else
  #local LOCAL_Grass = BTEX_Grass;
#end

#declare TF_LANDSCAPE_TEXTURE =
	texture {	
      slope {
         y, 0.5, 1.0
         altitude y, 0, 1
      }
		texture_map {
			[0	                 LOCAL_Rock]     // vertical and low level
			[0+RL         LOCAL_Rock]    
 			[0+RL         LOCAL_Grass]                     
 			[1-SL           LOCAL_Grass]        
			[1-SL           BTEX_Snow]                                
			[1  	              BTEX_Snow]   // Horizontal and high level               
		}
      turbulence <TF_LANDSCAPE_TURBULENCE_X, TF_LANDSCAPE_TURBULENCE_Y,TF_LANDSCAPE_TURBULENCE_Z >
		octaves 2
		lambda 5
		omega 1.0 *  TF_LANDSCAPE_DISTRIBUTION
	}       
     
#if (TF_LANDSCAPE_WATER_TRANSITION)

    #local LOCAL_TRANSITION_RATIO = 1000; // the icefield is a flatten copy of the original height-field
                                          // this parameter set how flat is the copy

    object
    {
      TF_HEIGHT_FIELD
      texture
      {
         TF_LANDSCAPE_TRANSITION_TEXTURE
      }
      clipped_by { plane { -y, TF_TRANSITION_VALUE -1 } }
      scale <TF_X_SCALE, TF_Y_SCALE/LOCAL_TRANSITION_RATIO, TF_Z_SCALE>    
      translate <0.0, TF_Y_SCALE*TF_WATER_LEVEL, 0.0>
    }

#end //TF_LANDSCAPE_WATERBORDER


