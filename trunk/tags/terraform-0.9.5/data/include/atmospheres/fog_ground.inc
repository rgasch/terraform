// Copyright (c) 1999, 2000 Robert Gasch
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

//// Ground Fog
//
// This a file to include if TF_HAVE_GROUND_FOG is TRUE

//// Global declarations for fog

#local LOCAL_FOG_DISTANCE = -0.999 + 1 / TF_GROUND_FOG_DENSITY;
//calculation of fog_distance
//density 0 , distance is bigger as possible
//density 1 , distance is almost 0 (O is wrong because it's no fog at all)

#local TF_FOG_COLOUR = <0.9, 0.9, 0.9>;
// fog is white

#local LOCAL_FOG_TRANSMIT = 1 - TF_GROUND_FOG_DENSITY;
// when density=0 (no fog) it's transparent

  //  Calculate the offset and fading_offset
  //  Offset is between Water level (0) and Camera or Height of the land.
  //  You can not go higher but this is more precise
  #local LOCAL_CAMERA_HEIGHT = vlength ( TF_CAMERA_LOCATION * y ) / TF_Y_SCALE;
  #if ( LOCAL_CAMERA_HEIGHT < 1.0 )
    #local LOCAL_FOG_OFFSET = (TF_WATER_LEVEL + (LOCAL_CAMERA_HEIGHT-TF_WATER_LEVEL) * TF_FOG_OFFSET);
  #else
    #local LOCAL_FOG_OFFSET = (TF_WATER_LEVEL + (1-TF_WATER_LEVEL) * TF_FOG_OFFSET );
  #end

fog // first basic fog
{
  fog_type 2
  distance LOCAL_FOG_DISTANCE * TF_Z_SCALE
  color  TF_FOG_COLOUR transmit LOCAL_FOG_TRANSMIT + TF_GROUND_FOG_DENSITY * 0.50
  fog_offset ( LOCAL_FOG_OFFSET * TF_Y_SCALE)
  fog_alt (TF_FOG_ALT + 0.001) * 2.0
  turbulence TF_FOG_TURBULENCE
  turb_depth 0.25
  up y
}

fog // additionnal layered more consistant fog
{
  fog_type 2
  distance LOCAL_FOG_DISTANCE * TF_Z_SCALE
  color  TF_FOG_COLOUR transmit 1 - TF_GROUND_FOG_DENSITY * 0.33
  fog_offset ( LOCAL_FOG_OFFSET * TF_Y_SCALE)
  fog_alt (TF_FOG_ALT + 0.001) * 1.0
  turbulence TF_FOG_TURBULENCE
  turb_depth 0.25
  up y
}


fog // additionnal layered atmospheric fog
{
  fog_type 2
  distance LOCAL_FOG_DISTANCE * TF_Z_SCALE
  color  TF_FOG_COLOUR transmit 1 - TF_GROUND_FOG_DENSITY * 0.17
  fog_offset ( LOCAL_FOG_OFFSET * TF_Y_SCALE)
  fog_alt (TF_FOG_ALT + 0.001) * 5.0
  turbulence TF_FOG_TURBULENCE
  turb_depth 0.25
  up y
}



