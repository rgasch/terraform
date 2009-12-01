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
//  Parameters
//---------------------------------------------

#local BTEX_SAND_COLOR = <0.95, 0.90, 0.70>;
#local BTEX_SAND_FREQUENCY = 100000 ;
#local BTEX_SAND_GRAIN = 0.7 ;

#local LOCAL_SCALE_PIGMENT = 25 ; //global scale for the textures
#local LOCAL_SCALE_NORMAL = 1; // global scale for the bump mapping

#local BTEX_SCALE = <1/TF_X_SCALE, 1/TF_Y_SCALE, 1/TF_Z_SCALE> * LOCAL_SCALE_PIGMENT ;
#local BTEX_BUMPYSCALE = <1/TF_X_SCALE, 1/TF_Y_SCALE, 1/TF_Z_SCALE> * LOCAL_SCALE_NORMAL ;
#local BTEX_BUMPDEPTH = 0.5 ; // global scale for the depth of the bump mapping

#declare BTEX_N_Sand = 
    normal
    {
      average
      normal_map
      {
        [1.0 waves BTEX_SAND_GRAIN  frequency BTEX_SAND_FREQUENCY*1.0 phase 720.0 * clock]
        [1.0 waves BTEX_SAND_GRAIN  frequency BTEX_SAND_FREQUENCY*0.4 phase 720.0 * clock]
        [1.0 waves BTEX_SAND_GRAIN  frequency BTEX_SAND_FREQUENCY*0.1 phase 720.0 * clock]
      }
      turbulence TF_LANDSCAPE_TURBULENCE
      octaves 1
      omega TF_LANDSCAPE_DISTRIBUTION
    }

#declare BTEX_F_Sand = 
		finish {
			ambient rgb 0.1
			diffuse 0.6 
			brilliance 1.0
			phong 0.0 
			phong_size 40
			specular 0.0 
			roughness 0.15 
			metallic 0.0 
			reflection 0
			reflection_exponent 1.0 
		}

 #declare TF_LANDSCAPE_TEXTURE = 
	texture {
		pigment {
			granite
			color_map {
				[ 0.02  color BTEX_SAND_COLOR * 1.3 ]
				[ 0.20  color BTEX_SAND_COLOR * 1.3 ]
				[ 0.85  color BTEX_SAND_COLOR * 1.0 ]
				[ 1.00  color BTEX_SAND_COLOR * 0.5 ]
			}
			scale 0.05 * LOCAL_SCALE_PIGMENT
		}
     normal {
        BTEX_N_Sand
     }   
	  finish {
        BTEX_F_Sand
		}
	}
