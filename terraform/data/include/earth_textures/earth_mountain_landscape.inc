//----------------------------------------------------
// Originally Created with Truevision, version 0.3.10
// Adapted for Terraform 0.9.0
//----------------------------------------------------
// Copyright (c) Raymond Ostertag
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
//  Basic Textures
//---------------------------------------------

#include "basic_textures.inc"
// Variables : BTEX_SCALE, BTEX_BUMPSCALE, BTEX_BUMPDEPTH*
// Variables : BTEX_SCALE, BTEX_BUMPSCALE, BTEX_BUMPDEPTH
// Normal, Finish : BTEX_N_Grass, BTEX_F_Grass, BTEX_F_Snow
// Textures : BTEX_Rock, BTEX_Snow, BTEX_Grass

//---------------------------------------------
//  Composed Textures
//---------------------------------------------

#local LOCAL_MIX_UP = <3.0, 1.5, 2.2> ; //control the XZ size of the spot snow-rock
#local LOCAL_MIX_DOWN = <0.5, 0.5, 0.5> ; //control the XZ size of the spot grass-rock

// Rock covered by some grass
#declare Rock_grass =
	texture {
		BTEX_Rock
	}
	texture {
		pigment {
			wrinkles
			color_map {
				[ 0.550000 rgbft<0.080000,0.600000,0.250000,0.000000,0.000000> ]
				[ 0.600000 rgbft<0.008000,0.600000,0.250000,0.800000,0.000000> ]
				[ 0.850000 rgbft<0.080000,0.600000,0.250000,1.000000,0.000000> ]
				[ 1.000000 rgbft<0.078431,0.596078,0.247059,1.000000,0.000000> ]
			}
			translate <0.000000,0.000000,0.000000> 
			scale LOCAL_MIX_DOWN * BTEX_SCALE
			rotate <0.000000,0.000000,0.000000>
		}
		normal {
			BTEX_N_Grass
		}
		finish {
			BTEX_F_Grass
		}
	}


// Rock covered by a lot of grass snow spotted
#declare Rock_grass_snow =
	texture {
		BTEX_Rock
	}

	texture {
		pigment {
			wrinkles
			color_map {
				[ 0.650000 rgbft<0.080000,0.600000,0.250000,0.000000,0.000000> ]
				[ 0.700000 rgbft<0.008000,0.600000,0.250000,0.800000,0.000000> ]
				[ 0.950000 rgbft<0.080000,0.600000,0.250000,1.000000,0.000000> ]
				[ 1.000000 rgbft<0.080000,0.600000,0.250000,1.000000,0.000000> ]
			}
			translate <0.000000,0.000000,0.000000> 
			scale LOCAL_MIX_DOWN * BTEX_SCALE
			rotate <0.000000,0.000000,0.000000>
		}
		normal {
			BTEX_N_Grass
		}
		finish {
			BTEX_F_Grass
		}
	}

	texture {
		pigment {
			wrinkles
			color_map {
				[ 0.250000 rgbft<1.000000,1.000000,1.000000,0.000000,0.000000> ]
				[ 0.300000 rgbft<1.000000,1.000000,1.000000,0.800000,0.000000> ]
				[ 0.400000 rgbft<1.000000,1.000000,1.000000,1.000000,0.000000> ]
				[ 1.000000 rgbft<1.000000,1.000000,1.000000,1.000000,0.000000> ]
			}
			translate <0.000000,0.000000,0.000000> 
			scale LOCAL_MIX_DOWN * BTEX_SCALE
			rotate <0.000000,0.000000,0.000000>
		}
		finish {
			BTEX_F_Snow
		}
	}

// Rock covered by a lot of snow
#declare Rock_snow = 
	texture {
		BTEX_Rock
	}

	texture {
		pigment {
			wrinkles
			color_map {
				[ 0.550000 rgbft<1.000000,1.000000,1.000000,0.000000,0.000000> ]
				[ 0.600000 rgbft<1.000000,1.000000,1.000000,0.800000,0.000000> ]
				[ 0.800000 rgbft<1.000000,1.000000,1.000000,1.000000,0.000000> ]
				[ 1.000000 rgbft<1.000000,1.000000,1.000000,1.000000,0.000000> ]
			}
			translate <0.000000,0.000000,0.000000> 
			scale LOCAL_MIX_UP * BTEX_SCALE
			rotate <0.000000,0.000000,0.000000>
		}
		finish {
			BTEX_F_Snow
		}
	}

//---------------------------------------------
//  Main Texture
//---------------------------------------------

#declare TF_LAND_TEXTURE =
	texture {	
                slope {
                  y, 0.5, 1.0
                  altitude y, 0, 1
                }
		texture_map {
			[0	BTEX_Rock ]
			[0.13	BTEX_Rock ]
			[0.16	Rock_grass ]
			[0.33	Rock_grass ]
			[0.36	Rock_grass_snow ]
			[0.53	Rock_grass_snow ]			
                        [0.56   Rock_snow ] 
                        [0.7    Rock_snow ] 
                        [0.72   BTEX_Snow ]
			[1.0	BTEX_Snow ]
		}
      turbulence TF_LANDSCAPE_TURBULENCE
      octaves 1
      omega TF_LANDSCAPE_DISTRIBUTION
  	}

#declare TF_LANDSCAPE_TEXTURE =
	texture {	
      gradient y
		texture_map {
			[0	BTEX_Rock ]
			[TF_WATER_LEVEL	Rock_grass ]
			[TF_WATER_LEVEL	TF_LAND_TEXTURE ]
			[1.00	TF_LAND_TEXTURE ]
      }
    }
