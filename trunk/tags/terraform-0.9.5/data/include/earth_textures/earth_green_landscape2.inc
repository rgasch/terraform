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


#local T_silt =
texture
{
  pigment
  {
    // Sienna
    color <0.9, 0.8, 0.6>
  }
}


// Soil near shore-line -- light-colored and wet.
#local T_soil =
texture
{
  // Burnt Sienna
  pigment
  {
    color <0.8, 0.6, 0.4>
  }

  finish
  {
    roughness 0.0001
    diffuse   0.85
  }
}


// Green stuff, like trees and shrubs
#local T_shrubbery =
texture
{
  pigment
  {
    gradient y
    turbulence 0.4
    omega 20.0
    pigment_map
    {
      [0.0 color <0.10, 0.80, 0.05>]   // Alive Green
      [1.0 color <0.05, 0.45, 0.20>]   // Forest Green
    }
    scale 0.1
  }

  finish
  {
    reflection 0.0
    diffuse    0.5
    roughness  0.5
  }

  normal
  {
    bumps 0.4
    scale 0.0002
  }
}


// Deep green plants near rivers and lakes.
#local T_greenery =
texture
{ 
  pigment
  {
    bozo
    color_map
    {
      [0 color <0.60, 0.75, 0.30>]
      [1 color <0.20, 0.50, 0.10>]
    }
    scale 0.005
  }

  normal
  {
    bumps 0.25
    scale 0.0001
  }
}


#local T_yellowed =
texture
{
  pigment
  {
    gradient y
    turbulence 0.2
    omega 1.0
    pigment_map
    {
      [0.0 color <0.67, 0.70, 0.15>]  // Yellow Green
      [0.5 color <0.80, 0.60, 0.50>]  // Brownish Color
      [1.0 color <0.67, 0.70, 0.15>]  // Yellow Green
    }
  }

  finish
  {
    diffuse   0.9
    roughness 1.0
  }

  normal
  {
    bumps 0.2
    scale 0.001
  }

  scale 0.05
}


#local T_snowtop =
texture
{
  pigment
  {
    color <1.0, 1.0, 1.0>
  }

  finish
  {
    specular   0.6
    reflection 0.0
    diffuse    0.9
    roughness  0.01
  }

  normal
  {
    bumps 0.1
    scale 0.0002
  }
}


#local T_mountaincap =
texture
{
  planar
  turbulence 0.005
  omega 0.005
  texture_map
  {
    [0.000 T_snowtop]
    [0.013 T_snowtop]     // White (peaks)
    [0.030 T_yellowed]    // Not-so-green foliage
    [0.070 T_shrubbery]   // Green plants
    [1.000 T_shrubbery]   // Green plants
  }
}


// Reddish Soil.
#local P_reddish =
pigment
{
  color <0.65, 0.5, 0.3>
}


// Mixture of plants and sand near water sources.
#local T_landwater =
texture
{
  pigment
  {
    gradient y
    turbulence 0.5
    omega 1.0
    pigment_map
    {
      [0.00 color <0.2, 0.5, 0.1>]
      [0.10 color <0.2, 0.5, 0.1>]
      [0.40 P_reddish]
      [0.60 P_reddish]
      [0.90 color <0.2, 0.5, 0.1>]
      [1.00 color <0.2, 0.5, 0.1>]
    }

    scale 0.01
  }

  normal
  {
    bumps 0.2
    scale 0.00005
  }
}


#declare TF_LAND_TEXTURE =
texture
{
  slope {
    y, 0.5, 1
    altitude y, 0, 0.85
  }

/*
  texture_map
  {
    [0.500 T_mountaincap]  // Additional foliage and possibly an ice cap
    [0.655 T_greenery]                            // Dark, Rough Green
    [0.720 T_landwater]                           // Between land and water
    [0.735 pigment { P_reddish }]                 // Reddish soil
    [0.770 T_soil]                                // Burnt Sienna
    [0.800 T_silt]                                // Sienna
    [0.850 pigment { color <0.50, 0.45, 0.35> }]  // Under water
    [1.000 pigment { color <0.40, 0.34, 0.22> }]  // Bottom of sea
  } 
*/
  texture_map {
	[0	T_silt ]
	[0.06	T_soil ]
	[0.09	T_soil ]
	[0.12	pigment { P_reddish } ]
	[0.15	T_landwater ]
	[0.23	T_landwater ]			
        [0.26   T_greenery ] 
        [0.50   T_greenery ] 
        [0.52   T_mountaincap ]
	[1.0	T_mountaincap ]
  }
}

#declare TF_LANDSCAPE_TEXTURE =
  texture {	
    gradient y
    texture_map {
	[0              pigment { color <0.40, 0.34, 0.22> } ]
	[TF_WATER_LEVEL pigment { color <0.50, 0.45, 0.35> } ]
	[TF_WATER_LEVEL	TF_LAND_TEXTURE ]
	[1.00	        TF_LAND_TEXTURE ]
    }
  }


