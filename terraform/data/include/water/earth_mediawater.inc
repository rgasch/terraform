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

#local LOCAL_MEDIA_XSIZE = 10 ; // ratio between the size of the height-field and the container box
#local LOCAL_MEDIA_ZSIZE = 10 ; // ratio between the size of the height-field and the container box
#local LOCAL_MEDIA_EXTINCTION = 0.1 ; // change the density of the media

//---------------------------------------------
//  Water texture
//---------------------------------------------

#declare f_ridge = function { internal(58) }

#declare TF_WATER_MATERIAL = 
material {
  texture {
    pigment {
      color TF_WATER_COLOR transmit TF_WATER_CLARITY/2
    }
    finish {
      ambient 0.15
      diffuse 0.3

      reflection {
        0.3 , 0.9 * TF_WATER_REFLECTION
        falloff 2
      }
      specular 0.8
      roughness 0.008
    }    
    normal {
      bozo 4.0 * TF_WATER_HEIGHT
      scale 1 / TF_WATER_FREQUENCY
    }
  }
  interior {
    ior 1.31    
    media {
       scattering {
       3 TF_WATER_COLOR
       extinction LOCAL_MEDIA_EXTINCTION
       }
    }
  }
}

//---------------------------------------------
//  Water creation
//---------------------------------------------

#if (TF_HAVE_WATER)

object {
  box
  {
    <-0.5 *TF_X_SCALE *LOCAL_MEDIA_XSIZE, 0, -0.5 *TF_Z_SCALE *LOCAL_MEDIA_ZSIZE>
    <0.5 *TF_X_SCALE *LOCAL_MEDIA_XSIZE, TF_WATER_LEVEL*TF_Y_SCALE, 0.5 *TF_Z_SCALE *LOCAL_MEDIA_ZSIZE >
    hollow
    material { TF_WATER_MATERIAL scale 4 }
  }
  translate <0.5 *TF_X_SCALE, 0, 0.5 * TF_Z_SCALE>
}


//---------------------------------------------
//  Water bottom
//---------------------------------------------


object {
  box
  {
    <-0.5 *TF_X_SCALE *LOCAL_MEDIA_XSIZE, 0, -0.5 *TF_Z_SCALE *LOCAL_MEDIA_ZSIZE>
    <0.5 *TF_X_SCALE *LOCAL_MEDIA_XSIZE, -0.01, 0.5 *TF_Z_SCALE *LOCAL_MEDIA_ZSIZE >
    texture {
      pigment {
        color TF_WATER_BOTTOMCOLOR
      }
    }
  }
  translate <0.5 *TF_X_SCALE, 0, 0.5 * TF_Z_SCALE>
}


#declare TF_WATER_BOTTOMCOLOR= <1,1,1,1,1>; // suppress the standard bottom plane under the water

#end // TF_HAVE_WATER
