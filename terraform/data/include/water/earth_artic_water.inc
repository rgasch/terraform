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

#local LOCAL_ICE_TURBULENCE = 0.25; //increase or decrease this make the ice blocks bigger or smaller
#local LOCAL_ICE_ROUGHNESS = 1.0; //increase or decrease this make the ice blocks bigger or smaller

// Create a transitionnal Icefield around the earth that cover the water
#local LOCAL_HAVE_ICECOVER = true;
#local LOCAL_TRANSITION_VALUE = 0.8; // between [0:no transition, 
                                     // and 1:no water only ice until the limit of the heigth-field]
                                     // with low values the ice field disappear completly, it depends on the height-field

//---------------------------------------------
//  Water texture
//---------------------------------------------

#local LOCAL_ICE_TEXTURE =
   texture
   {
		pigment 
      {  
          color rgb<1.0, 1.0, 1.0>    
      } 
		finish {
         ambient 0.1
     		diffuse 0.80
			brilliance 1.000000
			phong 0.000000
			phong_size 40
			specular 0.000000
			roughness 0.25
			metallic 0.000000
			reflection rgb<1.000000,1.000000,1.005000> * TF_WATER_REFLECTION
			reflection_exponent 1.000000
		} 
		normal {
			bumps
			bump_size 0.150000
			
		   turbulence <0.500000,0.500000,0.500000>
		   octaves 6
		   lambda 2.000000
		   omega 1.000000
		}
   }

#local LOCAL_WATER_TEXTURE =
  texture
  {
    pigment
    {
      color rgb TF_WATER_COLOR transmit TF_WATER_CLARITY
    }

    finish
    {
      ambient 0.15
      brilliance 0.9
      specular 0.4
      reflection TF_WATER_REFLECTION
      diffuse    0.3
      roughness  0.01
      metallic 0.0
    }

    normal
    {
      average
      normal_map
      {
        [1.0 ripples 1.0*TF_WATER_HEIGHT  frequency TF_WATER_FREQUENCY*1.0 phase 720.0 * clock]
        [1.0 ripples 1.5*TF_WATER_HEIGHT  frequency TF_WATER_FREQUENCY*0.4 phase 720.0 * clock]
      }
    }
  }

//---------------------------------------------
//  Water creation
//---------------------------------------------

#declare TF_WATER_MATERIAL =
material
{
  texture
  {
     crackle
     texture_map
     {
       [0.000 LOCAL_WATER_TEXTURE]
       [0.270 LOCAL_WATER_TEXTURE]
       [0.290 LOCAL_ICE_TEXTURE]
       [1.000 LOCAL_ICE_TEXTURE]
     }
	  turbulence <0.800000,0.800000,0.800000> * LOCAL_ICE_TURBULENCE
	  octaves 2.5
	  lambda 5
	  omega 0.5
     scale 20   
  }
  interior
  {
    ior 1.33
  }
}

#if (TF_HAVE_WATER)

plane
{
  <0.00, 1.00, 0.00>, 0.00
  material { TF_WATER_MATERIAL}
  translate <0.00, TF_WATER_LEVEL * TF_Y_SCALE, 0.00>
}

//---------------------------------------------
//  Additonnal Icefield for the water near of the earth
//---------------------------------------------

  #if (LOCAL_HAVE_ICECOVER)

    #local LOCAL_TRANSITION_RATIO = 1000; // the icefield is a flatten copy of the original height-field
                                          // this parameter set how flat is the copy

    object
    {
      TF_HEIGHT_FIELD
      texture
      {
         LOCAL_ICE_TEXTURE
      }
      clipped_by { plane { -y, LOCAL_TRANSITION_VALUE -1 } }
      scale <TF_X_SCALE, TF_Y_SCALE/LOCAL_TRANSITION_RATIO, TF_Z_SCALE>    
      translate <0.0, TF_Y_SCALE*TF_WATER_LEVEL, 0.0>
    }

  #end // LOCAL_HAVE_ICECOVER

#end // TF_HAVE_WATER



