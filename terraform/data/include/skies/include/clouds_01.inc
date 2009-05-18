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
//

#local P_cloud_gradient =
pigment
{
  planar
  turbulence 0.2
  omega 0.2
  pigment_map
  {
    [0.0 color sky_color_2 filter 0.8] // Mid in sky
    [1.0 color sky_color_1 filter 0.8] // Low in sky
  }
  scale 8
}

#declare TF_clouds =
texture
{
  pigment
  {
    bozo
    turbulence 0.5
    omega      0.5
    scale      0.05
    pigment_map
    {
      [0.0 P_cloud_gradient]
      [0.7 color rgbf 1]
      [1.0 color rgbf 1]
    }
  }
}
