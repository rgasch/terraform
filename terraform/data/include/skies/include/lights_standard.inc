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

///////////////////// Area Light //////////////////////////////////////////////

  #local X_center = 0.5; // X Coordinate of the center point
  #local Z_center = 0.5; // Z Coordinate of the center point
  #local X_size   = 1.0; // X size of the area
  #local Z_size   = 1.0; // Z size of the area
  #local X_lights = 3;   // Number of lights in X
  #local Z_lights = 3;   // Number of lights in Z
  #local Y_height = 4; // Y Coordinate of the lights

  light_source
  {
    < ( X_center - ( X_size / 2 ) ) * TF_X_SCALE,
      Y_height * TF_Y_SCALE,
      ( Z_center - (Z_size / 2 ) ) * TF_Z_SCALE >
    color rgb 0.8 * TF_LIGHTS_LUMINOSITY 
    area_light
      < X_size * TF_X_SCALE , 0.0,  >
      < 0.0 , 0.0, Z_size * TF_Z_SCALE >
      X_lights Z_lights
  }

///////////////////// Camera Light ////////////////////////////////////////////

#if (TF_HAVE_CAMERA_LIGHT)

  light_source
  {
    TF_CAMERA_LOCATION
    color ( light_color * TF_CAMERA_LIGHT_LUMINOSITY )
    translate < 0, 1.0, 0 >
  }

#end // TF_HAVE_CAMERA_LIGHT

///////////////////// Additional various Light ////////////////////////////////////////////

// You should edit them manually
//

#local Additional_light = false;

#if ( Additional_light )

  #local x_light1 = 0.5; // X Coordinate of the center point
  #local y_light1 = 3.0; // Y Coordinate of the center point
  #local z_light1 = 0.5; // Z Coordinate of the center point
  #local color_light1 = <1.0, 1.0, 1.0>; // Color of the light

  #local Point_light_1 = true;
  #local Spot_light_1 = false;

  #if ( Point_light_1 )

    light_source
    {
      < x_light1, y_light1, z_light1 > * TF_SCALE
      color ( color_light1 )
    }

  #end // Point_light

  #if ( Spot_light_1 )

    light_source
    {
      < x_light1, y_light1, z_light1 > * TF_SCALE
      color ( color_light1 )
      spotlight
        radius 30
        falloff 60
        tightness 1.0
        point_at < 0.5, 0.0, 0.5 > * TF_SCALE // Point to center of the Height-field
    }

  #end // Point_light

#end // Additionnal light

