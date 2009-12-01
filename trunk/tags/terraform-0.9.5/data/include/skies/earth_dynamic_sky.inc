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


////////////////// DYNAMIC (TOY) MODEL /////////////////////////////////////////
//
// Day lights moving with TF_NORTH_DIR and TF_TIME_OF_DAY
// Luminosity set by TF_AMBIENT_LIGHT_LUMINOSITY
// sky and clouds are themable.
// No stars, no sun, no moon, no roof of clouds
//

////////////////// World Parameters ///////////////////////////////////////////
//
// Those parameters rules the world
//
// Radius for background clouds
  #local WORLD_clouds_radius = 10.0 * vlength( TF_SCALE);
//
// Distance of the lights
  #local WORLD_lights_distance = 30.0 ;
//

////////////////// Background Sky and Stars ///////////////////////////////////
//
// Sky is themable, a skycolor_xxx given by TF_SKY_COLORDESCRIPTION is used for
// the declaration of the basic colors but only the noon_xxx_color are used
//
// No stars

// Sky basic colors description
  #include TF_SKY_COLORDESCRIPTION

// Sky colours

  #local sun_color   = noon_sun_color;
  #local sky_color_1 = noon_sky_color_1;
  #local sky_color_2 = noon_sky_color_2;
  #local sky_color_3 = noon_sky_color_3;
  #local light_color = noon_light_color;

// Sky pigment declaration

#declare sky_pigment =
pigment
{
  gradient y
  pigment_map
  {
    [0.0 color sky_color_1]
    [0.3 color sky_color_2]
    [1.0 color sky_color_3]
  }
}

// Sky sphere declaration

sky_sphere
{
    pigment
    {
      sky_pigment
    }
}

////////////////// Lights //////////////////////////////////////////////////////
//
// Lights are forced to be from the lights_toymodel description

// Lights description
 #declare TF_AMBIENT_LIGHT_LUMINOSITY = 1.0 * TF_AMBIENT_LIGHT_LUMINOSITY; // force to be almost equal to 10 hours of the regular sky
 #include "lights_toymodel.inc"

////////////////// Clouds //////////////////////////////////////////////////////
//
// Background clouds are themable, a clouds_xxx given by TF_CLOUDS_TEXTURE is
// used for the declaration of the background clouds

#if (TF_HAVE_CLOUDS)

  #include TF_CLOUDS_TEXTURE

  sphere
  {
    <0.0, 0.0, 0.0>, 1.0
    hollow
    texture
    {
      TF_clouds
      scale <2.0, 0.2, 1.0>
      translate <clock * 10.0, 0.0, 0.0>
    }
    scale WORLD_clouds_radius
  }

#end // TF_HAVE_CLOUDS



