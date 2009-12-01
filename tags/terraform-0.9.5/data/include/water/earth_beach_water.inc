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

#local LOCAL_SAND_COLOR = <0.95 0.9, 0.7>; 
#local LOCAL_SAND_HEIGHT = 1.5;
#local LOCAL_TRANSITION_VALUE = 0.5; // between [0:no sand, 
                                     // and 1:no water only sand until the limit of the heigth-field]
                                     // with low values the sand field disappear completly, it depends on the height-field

//---------------------------------------------
//  Water texture
//---------------------------------------------

#if (TF_HAVE_WATER)

#declare f_ridge = function { internal(58) }

#local LOCAL_SAND_TEXTURE =
   texture
   {
      pigment 
      {  
        color LOCAL_SAND_COLOR
      } 
      finish {
        ambient 0.3          
        //diffuse 0.6 
        //brilliance 0.0  
        crand LOCAL_SAND_HEIGHT / 10              
      }
   }

#declare LOCAL_WATER_MATERIAL = 
material {
  texture {
    pigment {
      color TF_WATER_COLOR filter TF_WATER_CLARITY transmit 0
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
    fade_distance 0.1 * TF_WATER_LEVEL / vlength(TF_SCALE)
    fade_power 1001
    fade_color TF_WATER_COLOR    
  }
}

//---------------------------------------------
//  Water creation
//---------------------------------------------

plane
{
  <0.00, 1.00, 0.00>, 0.00
  material { LOCAL_WATER_MATERIAL }
  translate <0.00, TF_WATER_LEVEL * TF_Y_SCALE, 0.00>
}

//---------------------------------------------
//  Sand beach
//---------------------------------------------

   #local LOCAL_TRANSITION_RATIO = 1000; // the icefield is a flatten copy of the original height-field
                                         // this parameter set how flat is the copy
   object
    {
      TF_HEIGHT_FIELD
      texture
      {
         LOCAL_SAND_TEXTURE
      }
      clipped_by { plane { -y, LOCAL_TRANSITION_VALUE -1 } }
      scale <TF_X_SCALE, TF_Y_SCALE/LOCAL_TRANSITION_RATIO, TF_Z_SCALE>    
      translate <0.0, TF_Y_SCALE*TF_WATER_LEVEL, 0.0>
    }

#end // TF_HAVE_WATER
