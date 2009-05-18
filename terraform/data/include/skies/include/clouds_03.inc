// Copyright (c) 2002 Raymond Ostertag
// Generated with Truevision
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

#declare TF_clouds =
	texture {
		pigment {
			bozo
			color_map {
				[ 0.000000 rgbft<0.750000,0.750000,0.750000,0.000000,0.000000> ]
				[ 0.500000 rgbft<0.900000,0.900000,0.900000,0.900000,0.000000> ]
				[ 0.700000 rgbft<1.000000,1.000000,1.000000,1.000000,0.000000> ]
				[ 1.000000 rgbft<1.000000,1.000000,1.000000,1.000000,0.000000> ]
			}
		 turbulence <1.500000,1.500000,1.500000>
		 octaves 10
		 lambda 2.500000
		 omega 0.500000
		}
		finish {
			ambient 0.35
			diffuse 0.100000
			brilliance 1.000000
			phong 0.000000
			phong_size 40
			specular 0.000000
			roughness 0.050000
			metallic 0.000000
			reflection rgb<0.000000,0.000000,0.000000>
			reflection_exponent 1.000000
		}
	}
