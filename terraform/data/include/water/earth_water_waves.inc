// Copyright (c) 1999, 2000 David A. Bartold
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


// Create water:

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
      average
      normal_map
      {
        [1.0 waves 1.0 *TF_WATER_HEIGHT  frequency TF_WATER_FREQUENCY*1.0 phase 720.0 * clock]
        [1.0 waves 1.5 *TF_WATER_HEIGHT  frequency TF_WATER_FREQUENCY*0.4 phase 720.0 * clock]
      }
    }
  }
  interior
  {
    ior 1.33
  }
  scale vlength (<TF_X_SCALE, TF_Y_SCALE, TF_Z_SCALE>)
}

#if (TF_HAVE_WATER)
plane
{
  <0.00, 1.00, 0.00>, 0.00
  material { TF_WATER_MATERIAL }
  translate <0.00, TF_WATER_LEVEL * TF_Y_SCALE, 0.00>
}
#end // TF_HAVE_WATER
