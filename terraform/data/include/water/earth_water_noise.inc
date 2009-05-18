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

// Defaults Parameters are in defaults.inc file

// ----------------------------------------
// note that #declare of float, vector and color require semicolon at the end

#declare f_ridge = function { internal(58) }

#declare TF_WATER_MATERIAL = 
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
      bozo 0.15 * TF_WATER_HEIGHT
      scale 1000 / TF_WATER_FREQUENCY
    }
   }
  interior {
    ior 1.31    
    fade_distance 0.1 * TF_WATER_LEVEL / vlength(TF_SCALE)
    fade_power 1001
    fade_color TF_WATER_COLOR    
  }
}

// ----------------------------------------
// Create water:

#if (TF_HAVE_WATER)

plane
{
  <0.00, 1.00, 0.00>, 0.00
  material { TF_WATER_MATERIAL }
  translate <0.00, TF_WATER_LEVEL * TF_Y_SCALE, 0.00>
}

#end // TF_HAVE_WATER

