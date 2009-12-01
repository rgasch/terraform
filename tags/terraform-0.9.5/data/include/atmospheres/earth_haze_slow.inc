// Copyright (c) 1999, 2000 David A. Bartold
// Copyright (c) 2002 Raymond Ostertag
// 2002 fev 17 Redoing Fog Seztion that was previously in the file earth_sky
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

////////////// FOG SECTION ///////////////
//
// Mist or Haze (slow rendering but better quality)
//

#if (TF_HAVE_FOG)

   #local LOCAL_INTERVALS = 10 ; //slow
   #local WORLD_atmosphere_radius = 55.0 * vlength( TF_SCALE); // Radius for atmosphere
   #local LOCAL_MIST_DENSITY = 0.25 * ( TF_FOG_DENSITY / WORLD_atmosphere_radius ) ;
   #local TF_FOG_COLOUR = <0.9, 0.9, 0.9>; // fog is white

   sphere {
     <0.5*TF_X_SCALE, 0.0, 0.5*TF_Z_SCALE>, WORLD_atmosphere_radius
     hollow
     pigment {color rgb<1,1,1> transmit 1}

     interior {
       media
       {
         intervals LOCAL_INTERVALS
         scattering { 2,  color TF_FOG_COLOUR * LOCAL_MIST_DENSITY }
         samples    1, 5
         confidence 0.99
         variance   0.001
         ratio      0.9
         # if (TF_FOG_TURBULENCE != 0)
           density { bozo
                     scale 1 * (1 - TF_FOG_TURBULENCE)
                     color_map {
                       [0 color rgb <0.0, 0.0, 0.0>]
                       [1 color rgb <1.0, 1.0, 1.0>]
                     }
                    }
         #end
       }
     }
   }

#end // TF_HAVE_FOG

//// Ground Fog

#if (TF_HAVE_GROUND_FOG)

  #include "fog_ground.inc"

#end // TF_HAVE_GROUND_FOG
