// Copyright (c) 1999, 2000 Robert Gasch
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
texture {
   pigment {
     bozo
     color_map {
       [ 0.000  color rgb<0.900, 0.754, 0.600> ]
       [ 0.356  color rgb<0.899, 0.755, 0.614> ]
       [ 0.629  color rgb<0.900, 0.757, 0.623> ]
       [ 1.000  color rgb<0.900, 0.754, 0.600> ]
     }
     turbulence 1.000 
   }
   normal {
     ripples 0.5000
     frequency 2.000
     turbulence 0.200 * TF_LANDSCAPE_TURBULENCE
     octaves 10
     lambda 1.000
     omega TF_LANDSCAPE_DISTRIBUTION
     scale     <0.1000, 0.1000, 1.0000>
   }
   finish {
     diffuse 0.800
     ambient 0.200
     roughness 0.0500
   }
}

