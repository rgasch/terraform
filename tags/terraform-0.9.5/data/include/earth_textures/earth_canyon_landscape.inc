// Copyright (c) 1999, 2000 David A. Bartold
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

#declare TF_LANDSCAPE_TEXTURE =
texture
{
  pigment
  {
    bozo
    turbulence 0.04
    omega      0.75
    pigment_map 
    {
      [0.0 color <0.7, 0.6, 0.3>]
      [1.0 color <0.8, 0.6, 0.4>]
    } 
  }
  normal
  {
    bumps 0.2
    turbulence TF_LANDSCAPE_TURBULENCE
    omega TF_LANDSCAPE_DISTRIBUTION
    scale 0.1  
  }
  scale 0.1
}
