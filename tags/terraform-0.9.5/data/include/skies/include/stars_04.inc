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

#declare stars_pigment =
pigment {
	granite
	color_map {
				[ 0.000000 rgbft<0.000000,0.000000,0.000000,0.000000,1.000000> ]
				[ 0.270000 rgbft<0.000000,0.000000,0.000000,0.000000,1.000000> ]
				[ 0.271000 rgbft<0.500000,0.500000,0.400000,0.000000,stars_transmit> ]
				[ 0.300000 rgbft<0.800000,0.800000,0.400000,0.000000,stars_transmit> ]
				[ 0.301000 rgbft<0.000000,0.000000,0.000000,0.000000,1.000000> ]
				[ 0.470000 rgbft<0.000000,0.000000,0.000000,0.000000,1.000000> ]
				[ 0.471000 rgbft<0.400000,0.400000,0.500000,0.000000,stars_transmit> ]
				[ 0.500000 rgbft<0.400000,0.400000,0.800000,0.000000,stars_transmit> ]
				[ 0.501000 rgbft<0.000000,0.000000,0.000000,0.000000,1.000000> ]
				[ 0.670000 rgbft<0.000000,0.000000,0.000000,0.000000,1.000000> ]
				[ 0.671000 rgbft<0.500000,0.400000,0.400000,0.000000,stars_transmit> ]
				[ 0.700000 rgbft<0.800000,0.400000,0.400000,0.000000,stars_transmit> ]
				[ 0.701000 rgbft<0.000000,0.000000,0.000000,0.000000,1.000000> ]
				[ 0.870000 rgbft<0.000000,0.000000,0.000000,0.000000,1.000000> ]
				[ 0.871000 rgbft<0.500000,0.500000,0.500000,0.000000,stars_transmit> ]
				[ 0.900000 rgbft<1.000000,1.000000,1.000000,0.000000,stars_transmit> ]
				[ 0.901000 rgbft<0.000000,0.000000,0.000000,0.000000,1.000000> ]
				[ 1.000000 rgbft<0.000000,0.000000,0.000000,0.000000,1.000000> ]
	}
	frequency 1.000000 phase 0.000000 sine_wave
   turbulence <1.000000,1.000000,1.000000>
	octaves 6
	lambda 2.000000
	omega 0.500000
   translate <0.000000,0.000000,0.000000>
   scale <0.500000,0.500000,0.500000>
   rotate <0.000000,0.000000,0.000000>
}
