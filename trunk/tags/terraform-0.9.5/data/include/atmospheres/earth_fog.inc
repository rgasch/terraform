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


///////////// FOG SECTION ////////////////////

//// Global declarations for fog

#local LOCAL_FOG_DISTANCE = -0.999 + 1 / TF_FOG_DENSITY;
//calculation of fog_distance
//density 0 , distance is bigger as possible
//density 1 , distance is almost 0 (O is wrong because it's no fog at all)

#local TF_FOG_COLOUR = < 0.9 * (1 -TF_FOG_BLUE/5), 
                                                                  0.9 * (1 -TF_FOG_BLUE/5), 
                                                                  0.9 * (1 +TF_FOG_BLUE/5) 
                                                               >;

#local LOCAL_FOG_TRANSMIT = 1 - TF_FOG_DENSITY;
// when density=0 (no fog) it's transparent

//// Atmospheric Fog

#if (TF_HAVE_FOG)

fog
{
  fog_type 1
  distance LOCAL_FOG_DISTANCE * TF_Z_SCALE
  color  TF_FOG_COLOUR transmit LOCAL_FOG_TRANSMIT
  turbulence TF_FOG_TURBULENCE
  turb_depth 0.25
}
#end // TF_HAVE_FOG

//// Ground Fog

#if (TF_HAVE_GROUND_FOG)

  #include "fog_ground.inc"

#end // TF_HAVE_GROUND_FOG

//// Rainbow

#if (TF_HAVE_RAINBOW)

  #include "rainbow.inc"

#end // TF_HAVE_RAINBOW
