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
// Basic Textures
// Textures : BTEX_Rock, BTEX_Rock_stratum, BTEX_Grass, BTEX_Snow, BTEX_Grasstex,
//---------------------------------------------
#include "basic_textures.inc"  
#if (TF_LANDSCAPE_WATER_TRANSITION_ICE)
#declare TF_LANDSCAPE_TRANSITION_TEXTURE = BTEX_Ice;
#end

#if (TF_LANDSCAPE_WATER_TRANSITION_SAND)
#declare TF_LANDSCAPE_TRANSITION_TEXTURE = BTEX_Sand;
#end

//---------------------------------------------
//  Additionnal Parameters
// (put here your own parameters prefixed by LOCAL_)
//---------------------------------------------
#local LOCAL_FILTER = 0.5 ;

//---------------------------------------------
//  Terrain texture declaration
//---------------------------------------------

#if ( TF_ROCK_STRATUM )
  #local LOCAL_Rock = BTEX_Rock_stratum;
#else
  #local LOCAL_Rock = BTEX_Rock;
#end

#if (TF_LANDSCAPE_WATER_TRANSITION)
  
  #if (TF_LANDSCAPE_WATER_TRANSITION_ICE)
    #local TF_TRANSITION_MAIN_TEXTURE = BTEX_Ice;
    #local TF_TRANSITION_MAIN_COLOR = BTEX_SNOW_COLOR;
    #local TF_TRANSITION_MAIN_PIGMENT = BTEX_P_Snow;
    #local TF_TRANSITION_MAIN_FINISH = BTEX_F_Ice;
    #local TF_TRANSITION_MAIN_NORMAL = BTEX_N_Snow;
  #end

  #if (TF_LANDSCAPE_WATER_TRANSITION_SAND)
    #local TF_TRANSITION_MAIN_TEXTURE = BTEX_Sand;
    #local TF_TRANSITION_MAIN_COLOR = BTEX_SAND_COLOR;
    #local TF_TRANSITION_MAIN_PIGMENT = BTEX_P_Sand;
    #local TF_TRANSITION_MAIN_FINISH = BTEX_F_Sand;
    #local TF_TRANSITION_MAIN_NORMAL = BTEX_N_Sand;
  #end

  #local TF_LANDSCAPE_TRANSITION_TEXTURE =
    texture {
      LOCAL_Rock
    }
    texture {
      pigment {
        slope {
          y, 0.5, 1.0
          altitude -y, 1.0, 0.0
        }
        pigment_map {
          [0.00 color TF_TRANSITION_MAIN_COLOR filter LOCAL_FILTER transmit 1] // vertical high
          [0.95 color TF_TRANSITION_MAIN_COLOR filter LOCAL_FILTER transmit 1]
          [0.95 TF_TRANSITION_MAIN_PIGMENT ]
          [1.00 TF_TRANSITION_MAIN_PIGMENT ] // horizontal low
        }
        turbulence TF_LANDSCAPE_TURBULENCE
        octaves 1
        lambda 5
        omega TF_LANDSCAPE_DISTRIBUTION
      }
      finish {
        TF_TRANSITION_MAIN_FINISH
      }
      normal {
        TF_TRANSITION_MAIN_NORMAL
      }
    }
#end // TF_LANDSCAPE_WATER_TRANSITION
  
#local MAX_GRASS_LEVEL = TF_GRASS_LEVEL + TF_ROCK_LEVEL ;
#local MAX_SNOW_LEVEL = TF_SNOW_LEVEL + TF_GRASS_LEVEL + TF_ROCK_LEVEL ;

#local LOCAL_TERRAIN_TEXTURE =
  texture {
    LOCAL_Rock
  }
  texture {
    pigment {
      slope {
        y, 0.5, 1.0
        altitude y, 0.0, 1.0
      }
      pigment_map {
        [0.00 BTEX_P_Grass ]
        [TF_GRASS_LEVEL/MAX_GRASS_LEVEL BTEX_P_Grass ]
        [TF_GRASS_LEVEL/MAX_GRASS_LEVEL color BTEX_GRASS_COLOR filter LOCAL_FILTER transmit 1]
        [1 color BTEX_GRASS_COLOR filter LOCAL_FILTER transmit 1] // horizontal high
      }
      turbulence TF_LANDSCAPE_TURBULENCE
      octaves 1
      lambda 5
      omega TF_LANDSCAPE_DISTRIBUTION
    }
    finish {
      BTEX_F_Grass
    }
    normal {
      BTEX_N_Grass
    }
  }
  texture {
    pigment {
      slope {
        y, 0.5, 1.0
        altitude -y, 1.0, 0.0
      }
      pigment_map {
        [0.00 BTEX_P_Snow ]
        [TF_SNOW_LEVEL/MAX_SNOW_LEVEL BTEX_P_Snow ]
        [TF_SNOW_LEVEL/MAX_SNOW_LEVEL color BTEX_SNOW_COLOR filter LOCAL_FILTER transmit 1]
        [1 color BTEX_SNOW_COLOR filter LOCAL_FILTER transmit 1] // horizontal bas
      }
      turbulence TF_LANDSCAPE_TURBULENCE
      octaves 1
      lambda 5
      omega TF_LANDSCAPE_DISTRIBUTION
    }
    finish {
      BTEX_F_Snow
    }
    normal {
      BTEX_N_Snow
    }
  }

// finally we apply this texture but only from the water level
// the texture under the water level is covered by a gradient
//
#if (TF_LANDSCAPE_WATER_TRANSITION)
  #declare TF_LANDSCAPE_TEXTURE =
  texture {
    gradient y
    texture_map {
      [0                                        pigment { color TF_WATER_BOTTOMCOLOR } ]  //bottom of the sea
      [TF_WATER_LEVEL                           TF_TRANSITION_MAIN_TEXTURE ]  // water level
      [TF_WATER_LEVEL+0.025*TF_TRANSITION_VALUE TF_TRANSITION_MAIN_TEXTURE ]  // water level
      [TF_WATER_LEVEL+0.025*TF_TRANSITION_VALUE TF_LANDSCAPE_TRANSITION_TEXTURE ]
      [TF_WATER_LEVEL+0.030*TF_TRANSITION_VALUE LOCAL_TERRAIN_TEXTURE ]
      [1                                        LOCAL_TERRAIN_TEXTURE ] // summit
    }
  }
#else
  #declare TF_LANDSCAPE_TEXTURE =
  texture {
    gradient y
    texture_map {
      [0                    pigment { color TF_WATER_BOTTOMCOLOR } ]  //bottom of the sea
      [TF_WATER_LEVEL       LOCAL_TERRAIN_TEXTURE ] // water level
      [1                    LOCAL_TERRAIN_TEXTURE ] // summit
    }
  }
#end
