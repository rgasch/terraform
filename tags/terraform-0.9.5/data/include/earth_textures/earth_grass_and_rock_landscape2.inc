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
//  Additonnal Parameters 
// (see defaults.inc for standard parameters )
//---------------------------------------------

#local LOCAL_BUMP_FACTOR = 1.0;
#local LOCAL_GRASS_COLOR = <110, 135 , 30> * (1/255) ; 

//---------------------------------------------
//  Basic Textures
//---------------------------------------------

#local LOCAL_ROCK =
	texture {
		pigment {
			granite
			color_map {
				[ 0.019791 rgbft<0.550000,0.550000,0.550000,0.000000,0.000000> ]
				[ 0.200000 rgbft<0.549020,0.549020,0.549020,0.000000,0.000000> ]
				[ 0.867708 rgbft<0.430000,0.430000,0.430000,0.000000,0.000000> ]
				[ 1.001000 rgbft<0.247059,0.247059,0.247059,0.000000,0.000000> ]
			}
			translate <0.000000,0.000000,0.000000> 
       scale <0.050000,0.050000,0.050000> 
       rotate <0.000000,0.000000,0.000000>
		}
		normal {
			granite
			bump_size 0.300000 * LOCAL_BUMP_FACTOR
			
		   turbulence <0.500000,0.500000,0.500000>
		   octaves 6
		   lambda 2.000000
		   omega 1.000000

		   translate <0.000000,0.000000,0.000000> 
        scale <0.300000,0.300000,0.300000>
        rotate <0.000000,0.000000,0.000000>
		}
		finish {
			ambient rgb<0.100000,0.100000,0.100000>
			diffuse 0.600000
			brilliance 1.000000
			phong 0.000000
			phong_size 40
			specular 0.000000
			roughness 0.150000
			metallic 0.000000
			reflection rgb<0.000000,0.000000,0.000000>
			reflection_exponent 1.000000
		}
   }

#local LOCAL_GRASS_NORMAL =
		normal {
			granite
			bump_size 0.1200000 * LOCAL_BUMP_FACTOR
			
		   turbulence <0.500000,0.500000,0.500000>
		   octaves 6
		   lambda 2.000000
		   omega 1.000000

		   translate <0.000000,0.000000,0.000000> 
        scale <0.030000,0.030000,0.030000>
        rotate <0.000000,0.000000,0.000000>
		}



#local LOCAL_GRASS =
	texture {
		pigment {
            color LOCAL_GRASS_COLOR
		}
		normal {
        LOCAL_GRASS_NORMAL
		}
	}

#local LOCAL_LANDSCAPE_TEXTURE =
	texture {
     LOCAL_ROCK
	}

	texture {
		pigment {
			wrinkles
			color_map {
				[ 0.550000 color LOCAL_GRASS_COLOR ]
				[ 0.600000 color LOCAL_GRASS_COLOR transmit 0.8 ]
				[ 0.850000 color LOCAL_GRASS_COLOR transmit 1.0 ]
				[ 1.000000 color LOCAL_GRASS_COLOR transmit 1.0 ]
			}
			frequency 1.100000 phase 0.000000 cubic_wave 
         
         translate <0.000000,0.000000,0.000000> 
         scale <0.050000,0.050000,0.050000> 
         rotate <0.000000,0.000000,0.000000>
		}
		normal {
        LOCAL_GRASS_NORMAL
		}
	}

	texture {
		pigment {
			wrinkles
			color_map {
				[ 0.550000 color LOCAL_GRASS_COLOR ]
				[ 0.600000 color LOCAL_GRASS_COLOR transmit 0.8 ]
				[ 0.850000 color LOCAL_GRASS_COLOR transmit 1.0 ]
				[ 1.000000 color LOCAL_GRASS_COLOR transmit 1.0 ]
			}
			frequency 1.100000 phase 0.000000 cubic_wave 
         
        translate <0.000000,0.000000,0.000000> 
        scale <0.050000,0.050000,0.050000> 
        rotate <0.000000,0.000000,0.000000>
		   
        turbulence <0.500000,0.500000,0.500000>
		   octaves 6
		   lambda 2.000000
		   omega 10.000000
		}
		normal {
        LOCAL_GRASS_NORMAL
		}
 	}

//---------------------------------------------
//  Main Texture
//---------------------------------------------

#local WL= TF_WATER_LEVEL;
#local SL= 1-(1-WL)/1.5; // Rocks on top level
#local TL= 0.05;

#declare TF_LANDSCAPE_TEXTURE =
	texture {	
                slope {
                  y, 0.5, 1.0
                  altitude y, 0, 1
                }
		texture_map {
			[0	    LOCAL_GRASS]                        // bottom of the lake
			[WL	    LOCAL_GRASS]                        // snow around the water
			[WL+TL      LOCAL_LANDSCAPE_TEXTURE]            // snow around the water
			[SL         LOCAL_LANDSCAPE_TEXTURE]            // snow around the water
			[SL+TL      LOCAL_ROCK]                         // begin of pure rocks
			[1  	    LOCAL_ROCK]                  
         
		}
	}






