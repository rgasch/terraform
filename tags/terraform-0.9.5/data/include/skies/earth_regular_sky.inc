// Copyright (c) 1999, 2000 David A. Bartold
// Copyright (c) 2002, 2003 Raymond Ostertag
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

//---------------------------------------------
//  Additonnal Parameters 
// (see defaults.inc for standard parameters )
//---------------------------------------------

#local LOCAL_PERSPECTIVE_CORRECTION = false; //experimental 

////////////////// World Parameters ///////////////////////////////////////////
//
// Those parameters rules the world
//
// Radius for background clouds
  #local WORLD_clouds_radius = 50.0 * vlength( TF_SCALE);
//
// Heigth of the roof of clouds given by TF_CLOUDS_ROOF_HEIGHT * TF_Y_SCALE
//
// Heigth of the ambient light
  #local WORLD_lights_heigth = 5.0 * TF_Y_SCALE;
//
// Radius for celest objects ( moon, sun )
  #local WORLD_planets_radius = 100.0 * vlength( TF_SCALE);
//

////////////////// Background Sky and Stars ////////////////////////////////////
//
// Sky is themable, a skycolor_xxx given by TF_SKY_COLORDESCRIPTION is used for
// the declaration of the basic colors
//
// Stars are themable, a stars_xxx given by TF_STARS_TEXTURE is used for the
// the declaration of the texture

// Sky color blender:
  #local PURE_DAY_START   =  7.00;
  #local PURE_DAY_STOP    = 17.00;
  #local PURE_NIGHT_START = 19.00;
  #local PURE_NIGHT_STOP  =  5.00;
  #local SUN_RISE         =  6.00;
  #local SUN_SET          = 18.00;

// Sky basic colors description
  #include TF_SKY_COLORDESCRIPTION

// Sky colours

  #macro Blend(MinPos,MinClr,MaxPos,MaxClr,Pos)
    MaxClr * ((Pos - MinPos) / (MaxPos - MinPos)) +
    MinClr * ((MaxPos - Pos) / (MaxPos - MinPos))
  #end

  #macro StarsFading( Time_start, Time_stop )
    ( ( TF_TIME_OF_DAY - Time_start) / ( Time_stop - Time_start) )
  #end

  #if (TF_TIME_OF_DAY >= PURE_DAY_START & TF_TIME_OF_DAY <= PURE_DAY_STOP)
    #local sun_color   = noon_sun_color;
    #local sky_color_1 = noon_sky_color_1;
    #local sky_color_2 = noon_sky_color_2;
    #local sky_color_3 = noon_sky_color_3;
    #local light_color = noon_light_color;
    #declare stars_transmit = 1.0; // no stars
  #else

    #if (TF_TIME_OF_DAY >= PURE_NIGHT_START | TF_TIME_OF_DAY < PURE_NIGHT_STOP)
      #local sun_color   = night_sun_color;
      #local sky_color_1 = night_sky_color_1;
      #local sky_color_2 = night_sky_color_2;
      #local sky_color_3 = night_sky_color_3;
      #local light_color = night_light_color;
      #declare stars_transmit = (1 - TF_STARS_BRIGHTNESS); // stars on
      #else

        #if (TF_TIME_OF_DAY >= PURE_NIGHT_STOP & TF_TIME_OF_DAY <= PURE_DAY_START)

          #if (TF_TIME_OF_DAY >= SUN_RISE)
            #local sun_color   = Blend (SUN_RISE, srise_sun_color,   PURE_DAY_START, noon_sun_color, TF_TIME_OF_DAY);
            #local sky_color_1 = Blend (SUN_RISE, srise_sky_color_1, PURE_DAY_START, noon_sky_color_1, TF_TIME_OF_DAY);
            #local sky_color_2 = Blend (SUN_RISE, srise_sky_color_2, PURE_DAY_START, noon_sky_color_2, TF_TIME_OF_DAY);
            #local sky_color_3 = Blend (SUN_RISE, srise_sky_color_3, PURE_DAY_START, noon_sky_color_3, TF_TIME_OF_DAY);
            #local light_color = Blend (SUN_RISE, srise_light_color, PURE_DAY_START, noon_light_color, TF_TIME_OF_DAY);
            #declare stars_transmit = 1.0; // no stars
          #else
          // Time is between night and sunrise
            #local sun_color   = Blend (PURE_NIGHT_STOP, night_sun_color, SUN_RISE, srise_sun_color, TF_TIME_OF_DAY);
            #local sky_color_1 = Blend (PURE_NIGHT_STOP, night_sky_color_1, SUN_RISE, srise_sky_color_1, TF_TIME_OF_DAY);
            #local sky_color_2 = Blend (PURE_NIGHT_STOP, night_sky_color_2, SUN_RISE, srise_sky_color_2, TF_TIME_OF_DAY);
            #local sky_color_3 = Blend (PURE_NIGHT_STOP, night_sky_color_3, SUN_RISE, srise_sky_color_3, TF_TIME_OF_DAY);
            #local light_color = Blend (PURE_NIGHT_STOP, night_light_color, SUN_RISE, srise_light_color, TF_TIME_OF_DAY);
            #declare stars_transmit = 1 - ( TF_STARS_BRIGHTNESS * ( 1 - StarsFading( PURE_NIGHT_STOP, SUN_RISE ) ) );
          #end // Is between sunrise and day

        #else
        // Then time is during sunset

          #if (TF_TIME_OF_DAY >= SUN_SET)
            #local sun_color   = Blend (SUN_SET, srise_sun_color, PURE_NIGHT_START, night_sun_color, TF_TIME_OF_DAY);
            #local sky_color_1 = Blend (SUN_SET, srise_sky_color_1, PURE_NIGHT_START, night_sky_color_1, TF_TIME_OF_DAY);
            #local sky_color_2 = Blend (SUN_SET, srise_sky_color_2, PURE_NIGHT_START, night_sky_color_2, TF_TIME_OF_DAY);
            #local sky_color_3 = Blend (SUN_SET, srise_sky_color_3, PURE_NIGHT_START, night_sky_color_3, TF_TIME_OF_DAY);
            #local light_color = Blend (SUN_SET, srise_light_color, PURE_NIGHT_START, night_light_color, TF_TIME_OF_DAY);
            #declare stars_transmit = 1 - ( TF_STARS_BRIGHTNESS * StarsFading( SUN_SET ,PURE_NIGHT_START ) );
          #else
          // Is between day and sunset
            #local sun_color   = Blend (PURE_DAY_STOP, noon_sun_color, SUN_SET, srise_sun_color, TF_TIME_OF_DAY);
            #local sky_color_1 = Blend (PURE_DAY_STOP, noon_sky_color_1, SUN_SET, srise_sky_color_1, TF_TIME_OF_DAY);
            #local sky_color_2 = Blend (PURE_DAY_STOP, noon_sky_color_2, SUN_SET, srise_sky_color_2, TF_TIME_OF_DAY);
            #local sky_color_3 = Blend (PURE_DAY_STOP, noon_sky_color_3, SUN_SET, srise_sky_color_3, TF_TIME_OF_DAY);
            #local light_color = Blend (PURE_DAY_STOP, noon_light_color, SUN_SET, srise_light_color, TF_TIME_OF_DAY);
            #declare stars_transmit = 1.0; // no stars

          #end // Is between sunset and night

        #end // Is time during sunrise

      #end // Is time during pure night

    #end // Is time during pure day

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

// Star pigment declaration

#include TF_STARS_TEXTURE

// Sky sphere (sky and stars) declaration

sky_sphere
{
    pigment
    {
      sky_pigment
    }
    #if (TF_HAVE_STARS)
    pigment
    {
      stars_pigment
    }
   #end
}

////////////////// Planets (Moon and Sun) //////////////////////////////////////
//
// Moon is mappable, the map moon_xxx is given by TF_MOON_IMAGE

// Load CelesteCoord and SphereVisualFactor routines
#include "macro_skies.inc"

//
// MOON

// some useful hints
// TF_MOON_Y_ROT = 0 and TF_MOON_Z_ROT = 0 show the what-you-see side of the moon
// TF_MOON_Y_ROT = 180 and TF_MOON_Z_ROT = 0 show the dark side of the moon
// if you render the earth instead of the moon and want see a specific point of
// the earth you should know is geographical positions and then change the
// rotations like the following
// TF_MOON_Y_ROT = latitude coord as E20=20 W20=-20
// TF_MOON_Z_ROT = longitude coord as N0=90 S0=-90
//
// some known limitations
// You'd better apply X=0.5 or Z=0.5 everywhere for the camera, if not the moon is not round.
// Perspective correction yet only work on one single axis.

#if (TF_TIME_OF_DAY<PURE_DAY_START | TF_TIME_OF_DAY>PURE_DAY_STOP)

  #local MOON_DISTANCE =  WORLD_planets_radius;
  #local MOON_RADIUS = TF_MOON_APPARENT_SIZE * MOON_DISTANCE / 12 ;
  #local MOON_LOCATION = CelesteCoord ( MOON_DISTANCE, TF_TIME_OF_DAY + TF_MOON_TIME_OFFSET, TF_NORTH_DIR );

  SphereVisualFactor( TF_CAMERA_LOCATION , TF_CAMERA_LOOK_AT , MOON_LOCATION )
  /* SphereVisualFactor initialise VISUAL_xxx floats */

  #declare  moon = object
  {
     sphere
     {
        <0.0, 0.0, 0.0>,
        MOON_RADIUS
        pigment
        {
          image_map {
            png TF_MOON_IMAGE
            once
            interpolate 2
            map_type 1
          }
          rotate z * ( TF_MOON_Z_ROT + degrees( VISUAL_ELEVATION_ANGLE) )
          rotate y * ( TF_MOON_Y_ROT + TF_NORTH_DIR +180)
        }
        finish
        {
          ambient TF_MOON_IMAGE_LUMINOSITY
          diffuse 0.0
        }
        #if (LOCAL_PERSPECTIVE_CORRECTION) 
          scale <1,VISUAL_ELEVATION_FACTOR,1> // only Y factor available
        #end
     }
  }

   #if (TF_SHOW_CELEST_OBJECTS_LIGHTS)
    light_source
    {
      MOON_LOCATION
      color ( TF_MOON_LIGHT_COLOR ) * TF_LIGHTS_LUMINOSITY
   
      /*
      spotlight
      radius    15
      falloff   20
      tightness 10
      point_at <0.5, 0.0, 0.5> * TF_SCALE
      */

      #if (TF_HAVE_CELEST_OBJECTS)
        looks_like  { moon }
      #end // TF_HAVE_CELEST_OBJECTS

      media_attenuation on
      media_interaction on
      translate < 0.5 * TF_X_SCALE, 0.0, 0.5 * TF_Z_SCALE> // offset for the center of the field
    }
  #else   
    #if (TF_HAVE_CELEST_OBJECTS)
       object {
         moon
         translate MOON_LOCATION
         translate < 0.5 * TF_X_SCALE, 0.0, 0.5 * TF_Z_SCALE> // offset for the center of the field
        }
    #end // TF_HAVE_CELEST_OBJECTS
  #end // TF_SHOW_CELEST_OBJECTS_LIGHTS
#end // Moon

//
// SUN

#if (TF_TIME_OF_DAY > PURE_NIGHT_STOP & TF_TIME_OF_DAY < PURE_NIGHT_START)

  #local SUN_DISTANCE =  WORLD_planets_radius;
  #local SUN_RADIUS = TF_SUN_APPARENT_SIZE * SUN_DISTANCE / 10 ;
  #local SUN_LOCATION = CelesteCoord ( SUN_DISTANCE, TF_TIME_OF_DAY, TF_NORTH_DIR )

  SphereVisualFactor( TF_CAMERA_LOCATION , TF_CAMERA_LOOK_AT , SUN_LOCATION )
  /* SphereVisualFactor initialise VISUAL_xxx floats */

  #declare sun = object
  {
      sphere
      {
        <0.0, 0.0, 0.0>,
        SUN_RADIUS
        pigment
        {
          color TF_SUN_LIGHT_COLOR
        }
        finish
        {
          ambient 0.5
          diffuse 0.0
        }
        #if (LOCAL_PERSPECTIVE_CORRECTION) 
          scale <1,VISUAL_ELEVATION_FACTOR,1> // only Y factor available
        #end
      }
  }

   #if (TF_SHOW_CELEST_OBJECTS_LIGHTS)
    light_source
    {
      SUN_LOCATION
      color ( TF_SUN_LIGHT_COLOR )* TF_LIGHTS_LUMINOSITY

      /*
      spotlight
      radius    15
      falloff   20
      tightness 10
      point_at <0.5, 0.0, 0.5> * TF_SCALE
      */

      #if (TF_HAVE_CELEST_OBJECTS)
        looks_like  { sun }
      #end // TF_HAVE_CELEST_OBJECTS

      media_attenuation on
      media_interaction on
      translate < 0.5 * TF_X_SCALE, 0.0, 0.5 * TF_Z_SCALE> // offset for the center of the field
    }
  #else   
    #if (TF_HAVE_CELEST_OBJECTS)
       object {
         sun
         translate SUN_LOCATION
         translate < 0.5 * TF_X_SCALE, 0.0, 0.5 * TF_Z_SCALE> // offset for the center of the field
        }
    #end // TF_HAVE_CELEST_OBJECTS
  #end // TF_SHOW_CELEST_OBJECTS_LIGHTS
#end // Sun


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
    }
    scale WORLD_clouds_radius
  }

#end // TF_HAVE_CLOUDS

//
// Create some clouds on a plane/roof :
// Not themable, based on work from : "Michael" <mail@mhazelgrove.fsnet.co.uk>

#if (TF_HAVE_CLOUDS_ROOF)

  #local Cheight=2800;
  #local Bdist=30;
  #local Tdist=30;
  #local Nboxes= TF_CLOUDS_ROOF_DENSITY;
  #local Count=1;

  #while (Count<=Nboxes)

    #local CldtopCol = color rgbt<1,1,.9,.1>;
    #local BaseCol = color rgbt<.75,.6,.98,.125>*1.5;
    #local FBaseCol=BaseCol*(1-(Count/Nboxes))+CldtopCol*(Count/Nboxes);

    plane
    {
       <0,1,0>
       TF_CLOUDS_ROOF_HEIGHT * TF_Y_SCALE
       texture
       {
         pigment
         {
           spotted
           octaves 9
           lambda 2.3
           omega 0.575
           turbulence 3
           color_map
           {
             [0.0 rgbt 1]
             [0.5 rgbt 1]
             [1.0 FBaseCol filter .75*(Count/Nboxes)]
           }
           scale 10000/2
           scale <1.5,1,1>
         }
         translate <0,(Cheight+Tdist*Count),0>
        }
        translate <-2000,(Cheight+Bdist*Count),0>
        finish
        {
          ambient .45+(1.25*((Count/Nboxes)/1.875)) specular 1
        }
        rotate y*90
        rotate x*2.5
        hollow
    }
    #declare Count=Count+1;

  #end // while

#end // TF_HAVE_CLOUDS_ROOF
