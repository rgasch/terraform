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

// No additionnal parameter

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
			bump_size 0.500000
			translate <0.000000,0.000000,0.000000> 
                        scale <0.050000,0.050000,0.050000> 
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

#local LOCAL_LANDSCAPE_TEXTURE =
	texture {
          LOCAL_ROCK
	}
	texture {
		pigment {
			wrinkles
			color_map {
				[ 0.550000 rgbft<1.000000,1.000000,1.000000,0.000000,0.000000> ]
				[ 0.600000 rgbft<1.000000,1.000000,1.000000,0.800000,0.000000> ]
				[ 0.800000 rgbft<1.000000,1.000000,1.000000,0.901961,0.000000> ]
				[ 1.000000 rgbft<1.000000,1.000000,1.000000,1.000000,0.000000> ]
			}			
		 turbulence <0.800000,0.300000,0.800000> * TF_LANDSCAPE_TURBULENCE
		 octaves 2
		 lambda 5
		 omega 1.0 * TF_LANDSCAPE_DISTRIBUTION
       translate <0.000000,0.000000,0.000000> 
       scale <0.050000,0.050000,0.050000> 
       rotate <0.000000,0.000000,0.000000>
		}
		finish {
			ambient rgb<0.200000,0.200000,0.200000>
			diffuse 0.800000
			brilliance 1.000000
			phong 0.000000
			phong_size 40
			specular 0.000000
			roughness 0.250000
			metallic 0.000000
			reflection rgb<0.150000,0.150000,0.150000>
			reflection_exponent 1.000000
		}
 	}

//---------------------------------------------
//  Main Texture
//---------------------------------------------

#local WL= TF_WATER_LEVEL;

#declare TF_LANDSCAPE_TEXTURE =
	texture {	
		gradient <0.000000,1.000000,0.000000>
		texture_map {
			[0	pigment{ color< 0.0, 0.0, 0.0 > } ] //bottom of the lake
			[WL	LOCAL_ROCK]                         //under water
			[WL	LOCAL_LANDSCAPE_TEXTURE]
			[1	LOCAL_LANDSCAPE_TEXTURE] 
		}
	}
