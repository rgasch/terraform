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

// Load CelesteCoord and SphereVisualFactor routines
#include "macro_skies.inc"

////////////////// DYNAMIC (TOY) MODEL /////////////////////

// Simulation of realistic light sources (sun and moon)

// Sun light

#local SUN_LIGHT_LOCATION = CelesteCoord ( WORLD_lights_distance, TF_TIME_OF_DAY, TF_NORTH_DIR );

light_source
{
  SUN_LIGHT_LOCATION * <TF_X_SCALE, TF_Y_SCALE, TF_Z_SCALE>
  color  <1.0, 0.95, 0.8> * TF_AMBIENT_LIGHT_LUMINOSITY * 1.5
  translate <0.5*TF_X_SCALE, 0, 0.5*TF_Z_SCALE> // center in the middle of the HF
}

// Moon light

#local MOON_LIGHT_LOCATION = CelesteCoord ( WORLD_lights_distance, TF_TIME_OF_DAY+12.0, TF_NORTH_DIR );

light_source
{
  MOON_LIGHT_LOCATION * <TF_X_SCALE, TF_Y_SCALE, TF_Z_SCALE>
  color  <1.0, 0.95, 0.8> * TF_AMBIENT_LIGHT_LUMINOSITY * 0.5
  translate <0.5*TF_X_SCALE, 0, 0.5*TF_Z_SCALE> // center in the middle of the HF
}

// Ambient area Light 

#if (TF_HAVE_AMBIENT_LIGHT)

  #local X_center = 0.5; // X Coordinate of the center point
  #local Z_center = 0.5; // Z Coordinate of the center point
  #local X_size   = 1.0; // X size of the area
  #local Z_size   = 1.0; // Z size of the area
  #local X_lights = 4;   // Number of lights in X
  #local Z_lights = 4;   // Number of lights in Z

  #if (TF_TIME_OF_DAY >= 18.0) 
    #declare TF_AMBIENT_LIGHT_LUMINOSITY = TF_AMBIENT_LIGHT_LUMINOSITY * 0.5;
  #end
  #if (TF_TIME_OF_DAY <= 6.0) 
    #declare TF_AMBIENT_LIGHT_LUMINOSITY = TF_AMBIENT_LIGHT_LUMINOSITY * 0.5;
  #end

  light_source
  {
    < ( X_center - ( X_size / 2 ) ) * TF_X_SCALE,
      WORLD_lights_distance * TF_Y_SCALE,
      ( Z_center - (Z_size / 2 ) ) * TF_Z_SCALE >
    color ( <1.0, 0.95, 0.8> * TF_AMBIENT_LIGHT_LUMINOSITY )
    area_light
      < X_size * TF_X_SCALE , 0.0,  >
      < 0.0 , 0.0, Z_size * TF_Z_SCALE >
      X_lights Z_lights
    shadowless
  }

#end // TF_HAVE_AMBIENT_LIGHT
