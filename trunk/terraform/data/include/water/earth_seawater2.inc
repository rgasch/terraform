// Copyright (c) 2002 Raymond Ostertag
// Inspired by  "p_watr4.pov" created by  Friedrich A. Lohmueller, 2001
// A surface animated by the wind
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

#local LOCAL_X_DIRECTION = 0.18;
#local LOCAL_Z_DIRECTION = 0.09;
#local LOCAL_ROTATION = 85; //direction rectangular to the wind

//---------------------------------------------
//  Water texture
//---------------------------------------------

#declare TF_WATER_MATERIAL =
material {
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
      crackle 0.35 
      scale <LOCAL_X_DIRECTION, 0.09 * TF_WATER_HEIGHT, LOCAL_Z_DIRECTION> * ( 300 / TF_WATER_FREQUENCY  )
      rotate <0.0, LOCAL_ROTATION , 0.0> 
      turbulence 1.7 
      translate <0.0, 0.0, 0.0>
    }
  }
  interior
  {
    ior 1.33
  }
  scale vlength (<TF_X_SCALE, TF_Y_SCALE, TF_Z_SCALE>)
}

#if (TF_HAVE_WATER)

plane{
  <0,1,0>, 0 
  material { TF_WATER_MATERIAL }
  translate <0.00, TF_WATER_LEVEL * TF_Y_SCALE, 0.00>
}

#end // TF_HAVE_WATER
