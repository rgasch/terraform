// Copyright (c) 1999, 2000 David A. Bartold
// Copyright (c) 2002 Raymond Ostertag
// 2002 fev 13 Add parameters for water
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

///////////////////////////////////////////////////////////////////////////////
/////// Start of User Definable Parameters and Their Defaults /////////////////
///////////////////////////////////////////////////////////////////////////////
// The Parameters used in the POV code are described here. Feel free to modify
// them manually if they are not implemented in the dialog boxes. If they are
// implemented your change will have no effect. You can overwrite the value
// from the dialog box, just comment the #ifndef #end statements.
// The structure of this document follow what you can see in the SCENE OPTIONS
// dialog. 
// At the end of this document there is the TRACE OBJECT declarations.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/////// Main file
///////////////////////////////////////////////////////////////////////////////

// TF_FILE and TF_HEIGHT_FIELD
// in the dialog box : never
// type : TGA graphic file
// file to render as Height-field

#ifndef (TF_HEIGHT_FIELD)
#declare TF_HEIGHT_FIELD = height_field { tga "TF_FILE" smooth }
#end

///////////////////////////////////////////////////////////////////////////////
/////// Tile terrains
///////////////////////////////////////////////////////////////////////////////

// TF_TILE_TERRAIN
// in the dialog box : no
// type : boolean
// Terrain can be a tile, then you can render how many you want
// Tiling parameters

//#ifndef (TF_TILE_TERRAIN)
  #declare TF_TILE_TERRAIN = false;
  #declare TF_Z_TILE = 10; // number of tiles on Z axis
  #declare TF_X_TILE = 10; // nulber of tile on X axis
//#end

///////////////////////////////////////////////////////////////////////////////
/////// Camera
///////////////////////////////////////////////////////////////////////////////

// TF_CAMERA_LOCATION
// in the dialog box : POSITION
// type : spin button
// Set the camera's location.

#ifndef (TF_CAMERA_LOCATION)
#declare TF_CAMERA_LOCATION = <0,  (TF_X_SCALE+TF_Z_SCALE)/2*1.5, -1.4*TF_Z_SCALE>;
#end

// TF_CAMERA_LOOK_AT
// in the dialog box : LOOK AT
// type : spin button
// Set the location to which the camera points.

#ifndef (TF_CAMERA_LOOK_AT)
#declare TF_CAMERA_LOOK_AT = <0, 0, 0>;
#end

// TF_CAMERA_ZOOM
// in the dialog box : CAMERA ZOOM
// type : [0..1]
// Set the angle of the vision of the camera

#ifndef (TF_CAMERA_ZOOM)
#declare TF_CAMERA_ZOOM = 0.5 ;
#end

#declare TF_CAMERA_ANGLE = 175 - 170*TF_CAMERA_ZOOM;

///////////////////////////////////////////////////////////////////////////////
/////// Global Settings
///////////////////////////////////////////////////////////////////////////////

// TF_CAMERA_TYPE
// in the dialog box : CAMERA TYPE
// type : menu
// Set the type of projection used for the camera
// 0 : Perspective
// 1 : Fisheye
// 2 : Panoramic

#ifndef (TF_CAMERA_TYPE)
#declare TF_CAMERA_TYPE = 0; // Perspective
#end

// TF_[XZ]_SCALE
// in the dialog box : SIZE X - SIZE Z
// type : hscale [500-25000]
// Scales along the X and Z axis
#ifndef (TF_X_SCALE)
#declare TF_X_SCALE = 1000;
#end
#ifndef (TF_Z_SCALE)
#declare TF_Z_SCALE = 1000;
#end

// TF_Y_SCALE_FACTOR and TF_Y_SCALE
// in the dialog box : SIZE Y
// type : hscale [0-1]
// Basically, it sets the height of the rendered terrain

#ifndef (TF_Y_SCALE_FACTOR)
#declare TF_Y_SCALE_FACTOR = 0.33;
#end

#ifndef (TF_Y_SCALE)
#declare TF_Y_SCALE = (TF_X_SCALE+TF_Z_SCALE)/2*TF_Y_SCALE_FACTOR;
#end

// TF_SCALE
// Level 1
// in the dialog box : never
// type : POV vector
// Not a parameter, it's useful to have this vector for the POV code.

#ifndef (TF_SCALE)
#declare TF_SCALE = <TF_X_SCALE, TF_Y_SCALE, TF_Z_SCALE>;
#end

///////////////////////////////////////////////////////////////////////////////
/////// Atmosphere
///////////////////////////////////////////////////////////////////////////////

// in the dialog box : ATMOSPHERE TYPE
// type : POV include file
// Not a parameter, you choose here wich file is used for the atmosphere theme
// earth_fog.inc : standard POV fogs
// earth_haze_slow.inc and earth_haze_fast.inc : some kind of haze effect with 
// scattering and POV ground fog

// TF_HAVE_RAINBOW
// in the dialog box : RAINBOW
// type : boolean
// Turn on/off for a rainbow.

#ifndef (TF_HAVE_RAINBOW)
#declare TF_HAVE_RAINBOW = false;
#end

// TF_HAVE_FOG
// in the dialog box : FOG
// type : boolean
// Turn on/off for a atmospheric fog.

#ifndef (TF_HAVE_FOG)
#declare TF_HAVE_FOG = false;
#end

// TF_FOG_BLUE
// in the dialog box : BLUE COLORATION
// type : hscale [0,1]
// Determine the amount of blue in the fog

#ifndef (TF_FOG_BLUE)
#declare TF_FOG_BLUE = 0.3;
#end

// TF_FOG_TURBULENCE
// in the dialog box : TURBULENCE
// type : hscale [0-1]
// Control of the density turbulence for all types of fog

#ifndef (TF_FOG_TURBULENCE)
#declare TF_FOG_TURBULENCE = 0;
#end

// TF_FOG_DENSITY
// in the dialog box : DENSITY
// type : hscale [0-1]
// Control of the density for all types of fog

#ifndef (TF_FOG_DENSITY)
#declare TF_FOG_DENSITY = 0.25;
#end

// TF_HAVE_GROUND_FOG
// in the dialog box : GROUND FOG
// type : boolean
// Turn on/off for a ground-level fog.

#ifndef (TF_HAVE_GROUND_FOG)
#declare TF_HAVE_GROUND_FOG = false;
#end

// TF_FOG_OFFSET
// in the dialog box : OFFSET
// type : hscale [0..1]
// the value 0 means that the fog is as the same level as the water
// the value 1 means that the fog is as the same level of the camera
// or, if the camera is upper that 1.0 (the summit of the land),
// the fog cover only the whole land

#ifndef (TF_FOG_OFFSET)
#declare TF_FOG_OFFSET = 0.40;
#end

// TF_FOG_ALT
// in the dialog box : ALT
// type : hscale []0..1]
// control how the fog disappear in the atmosphere,
// with the value 0 there is no fading

#ifndef (TF_FOG_ALT)
#declare TF_FOG_ALT = 0.40;
#end
// TF_GROUND_FOG_DENSITY
// in the dialog box : DENSITY
// type : hscale [0-1]
// Control of the density only for ground fog

#ifndef (TF_GROUND_FOG_DENSITY)
#declare TF_GROUND_FOG_DENSITY = TF_FOG_DENSITY;
#end

///////////////////////////////////////////////////////////////////////////////
/////// Sky 
///////////////////////////////////////////////////////////////////////////////

// in the dialog box : TF_SKY SYSTEM
// type : menu
// You choose here wich system is used for the sky 
// 0 :Regular sky : complet built-in sky with celest objects
// 1 : Mapped sky : simple model, an image is projected as background

#ifndef (TF_SKY_SYSTEM)
#declare TF_SKY_SYSTEM = 0;
#end

// TF_SKY_COLORDESCRIPTION
// in the dialog box : SKY COLOR TYPE
// type : POV include file
// This file contain the nine colours used to set the colour of the background sky
// for Regular Sky only

#ifndef (TF_SKY_COLORDESCRIPTION)
#declare TF_SKY_COLORDESCRIPTION = "skycolor_earth.inc"
#end

// TF_SKY_COLORATION_[RED..BLUE]
// in the dialog box : SKY COLORATION
// type : spin button
// Permanent additionnal coloration for the background sky
// for Regular Sky only

#ifndef (TF_SKY_COLORATION_RED)
#declare TF_SKY_COLORATION_RED =  <1.0>; // no coloration
#end

#ifndef (TF_SKY_COLORATION_GREEN)
#declare TF_SKY_COLORATION_GREEN =  <1.0>; // no coloration
#end

#ifndef (TF_SKY_COLORATION_BLUE)
#declare TF_SKY_COLORATION_BLUE =  <1.0>; // no coloration
#end

// TF_SKY_COLORATION
// in the dialog box : never
// type : POV vector
// Permanent additionnal coloration for the background sky
// for Regular Sky only

#declare TF_SKY_COLORATION=  <TF_SKY_COLORATION_RED, TF_SKY_COLORATION_GREEN, TF_SKY_COLORATION_BLUE>; // no coloration

// TF_HAVE_CLOUDS
// in the dialog box : CLOUDS
// type : boolean
// Create background clouds (texture on a sphere)
// for Regular Sky only

#ifndef (TF_HAVE_CLOUDS)
#declare TF_HAVE_CLOUDS = false;
#end

// TF_CLOUDS_TEXTURE
// in the dialog box : CLOUD TYPE
// type : POV include file
// Selection of a texture for background clouds
// for Regular Sky only

#ifndef (TF_CLOUDS_TEXTURE)
#declare TF_CLOUDS_TEXTURE = "clouds_01.inc"
#end

// TF_HAVE_CLOUDS_ROOF
// in the dialog box : no
// type : boolean
// Create a roof of clouds (texture on a plane)(experimental)
// used in regular model only

#ifndef (TF_HAVE_CLOUDS_ROOF)
#declare TF_HAVE_CLOUDS_ROOF = false;
#end

// TF_CLOUDS_ROOF_DENSITY
// in the dialog box : no
// type hscale [1, 50]
// Number of iterations for clouds creation,
// It's experimental, seems that there is only two results [1-4], [5-50]
// used in regular model only

#ifndef (TF_CLOUDS_ROOF_DENSITY)
#declare TF_CLOUDS_ROOF_DENSITY = 5;
#end

// TF_CLOUDS_ROOF_HEIGHT
// in the dialog box : no
// type hscale [1, 5]
// Heigth of the plane (roof) - 1 means at the summit of the heigth-field
// used in regular model only

#ifndef (TF_CLOUDS_ROOF_HEIGHT)
#declare TF_CLOUDS_ROOF_HEIGHT = 2.0;
#end

// TF_HAVE_STARS
// in the dialog box : STARS
// type : boolean
// Create stars (texture on a sphere)
// for Regular Sky only

#ifndef (TF_HAVE_STARS)
#declare TF_HAVE_STARS = false;
#end

// TF_STARS_TEXTURE
// in the dialog box : STARS
// type : POV include file
// Selection of a file for the stars texture, you can choose between 6 files
// for Regular Sky only

#ifndef (TF_STARS_TEXTURE)
#declare TF_STARS_TEXTURE =  "stars_01.inc"
#end

// TF_STARS_BRIGHTNESS
// in the dialog box : no
// type : hscales [0, 1]
// Brightness of the stars
// for Regular Sky only

#ifndef (TF_STARS_BRIGHTNESS)
#declare TF_STARS_BRIGHTNESS =  0.5;
#end

// TF_SKY_IMAGE
// in the dialog box : BACKGROUND IMAGE
// type : TGA graphic file
// Selection of the image mapped as background
// Default format is 16/9. See mapped_sky.inc to change this.
// for Mapped Sky only

#ifndef (TF_SKY_IMAGE)
#declare TF_SKY_IMAGE = "sky_test_320x180.tga"
#end

// TF_SKY_IMAGE_SCALE
// in the dialog box : SCALE
// type : hscale [0.01 , 10.0]
// Scale of image mapped to the sky, use this if the image don't really fill the background
// for Mapped Sky only

#ifndef (TF_SKY_IMAGE_SCALE)
#declare TF_SKY_IMAGE_SCALE = 1.0;
#end

// TF_SKY_IMAGE_ELEVATION_OFFSET
// in the dialog box : ELEVATION_OFFSET
// type : hscale [0.0, 0.6]
// Adjustment of the elevation offset of the image mapped to the sky
// for Mapped Sky only

#ifndef (TF_SKY_IMAGE_ELEVATION_OFFSET)
#declare TF_SKY_IMAGE_ELEVATION_OFFSET = 0.3;
#end

#declare TF_SKY_IMAGE_ELEVATION_OFFSET = TF_SKY_IMAGE_ELEVATION_OFFSET - 0.5;

// TF_SKY_IMAGE_LUMINOSITY
// in the dialog box : ENLIGHTMENT
// type : hscale [0.0, 2.0]
// Adjustment of the luminosity of the image mapped to the sky
// for Mapped Sky only

#ifndef (TF_SKY_IMAGE_LUMINOSITY)
#declare TF_SKY_IMAGE_ LUMINOSITY= 0.2;
#end

///////////////////////////////////////////////////////////////////////////////
/////// Sun and Moon
///////////////////////////////////////////////////////////////////////////////

// TF_HAVE_CELEST_OBJECTS
// in the dialog box : SHOW CELEST OBJECTS
// type : boolean
// Create visible sun and moon
// for Regular Sky only

#ifndef (TF_HAVE_CELEST_OBJECTS)
#declare TF_HAVE_CELEST_OBJECTS = true;
#end

// TF_SUN_APPARENT_SIZE
// in the dialog box : SUN_APPARENT_SIZE
// hscale :[0.1 , 2.0]
// apparent size of the sun from earth, don't increase this too much
// for Regular Sky only

#ifndef (TF_SUN_APPARENT_SIZE)
#declare TF_SUN_APPARENT_SIZE  = 1.0;
#end

// TF_SUN_LIGHT_COLOR_[RED..BLUE]
// in the dialog box : SUN_LIGHT_COLOR
// type : spin button
// color and brightness of the light that simulate the sun lightning
// for Regular Sky only

#ifndef (TF_SUN_LIGHT_COLOR_RED)
#declare TF_SUN_LIGHT_COLOR_RED  = <1.0>;
#end

#ifndef (TF_SUN_LIGHT_COLOR_GREEN)
#declare TF_SUN_LIGHT_COLOR_GREEN  = <0.95>;
#end

#ifndef (TF_SUN_LIGHT_COLOR_BLUE)
#declare TF_SUN_LIGHT_COLOR_BLUE  = <0.60>;
#end

// TF_SUN_LIGHT_COLOR
// in the dialog box : never
// type : POV vector
// color and brightness of the light that simulate the sun lightning
// for Regular Sky only

#ifndef (TF_SUN_LIGHT_COLOR)
#declare TF_SUN_LIGHT_COLOR  = <TF_SUN_LIGHT_COLOR_RED, TF_SUN_LIGHT_COLOR_GREEN, TF_SUN_LIGHT_COLOR_BLUE>;
#end

// TF_MOON_IMAGE
// in the dialog box : IMAGE MAP
// type : PNG graphic file
// Selection of the image mapped to the moon
// You can download nice free maps here, you need to convert them in PNG.
// http://www.evildrganymede.demon.co.uk/render/render_f.htm
// http://maps.jpl.nasa.gov/
// http://www.radcyberzine.com/xglobe/ (earth maps))
// for Regular Sky only

#ifndef (TF_MOON_IMAGE)
#declare TF_MOON_IMAGE = "moon_moon.png"
#end

// TF_MOON_[YZ]_ROT
// in the dialog box : MAP ROTATION
// type : spin buton [-180 , 180]
// Y (in degrees) rotate the moon along is natural axis (longitude)
// Z (in degrees) rotate the moon if you want see poles (latitude)
// See also earth_regular_sky.inc at moon section for useful hints
// for Regular Sky only

#ifndef (TF_MOON_Y_ROT)
#declare TF_MOON_Y_ROT = 0;
#end
#ifndef (TF_MOON_Z_ROT)
#declare TF_MOON_Z_ROT = 0;
#end

// TF_MOON_IMAGE_LUMINOSITY
// in the dialog box : MAP ENLIGHTMENT
// type : hscale [0,2]
// brightness of the map applied to the moon
// for Regular Sky only

#ifndef (TF_MOON_IMAGE_LUMINOSITY)
#declare TF_MOON_IMAGE_LUMINOSITY = 1.0;
#end

// TF_MOON_APPARENT_SIZE
// in the dialog box : MOON APPARENT SIZE
// type : hscale [0.1 , 2.0]
// apparent size of the moon from earth, don't increase this too much
// for Regular Sky only

#ifndef (TF_MOON_APPARENT_SIZE)
#declare TF_MOON_APPARENT_SIZE = 1.0;
#end

// TF_MOON_LIGHT_COLOR_[RED..BLUE]
// in the dialog box : MOON LIGHT COLOR
// type : spin button
// color and brightness of the light that simulate the moon lightning
// for Regular Sky only

#ifndef (TF_MOON_LIGHT_COLOR_RED)
#declare TF_MOON_LIGHT_COLOR_RED  = 0.35;
#end

#ifndef (TF_MOON_LIGHT_COLOR_GREEN)
#declare TF_MOON_LIGHT_COLOR_GREEN  = 0.35;
#end

#ifndef (TF_MOON_LIGHT_COLOR_BLUE)
#declare TF_MOON_LIGHT_COLOR_BLUE  = 0.35;
#end

// TF_MOON_LIGHT_COLOR
// in the dialog box : never
// type : POV vector
// color and brightness of the light that simulate the moon lightning
// for Regular Sky only

#declare TF_MOON_LIGHT_COLOR  = <TF_MOON_LIGHT_COLOR_RED, TF_MOON_LIGHT_COLOR_GREEN, TF_MOON_LIGHT_COLOR_BLUE>;

///////////////////////////////////////////////////////////////////////////////
/////// Lights
///////////////////////////////////////////////////////////////////////////////

// TF_LIGHTS_SYSTEM
// in the dialog box : TF LIGHTS SYSTEM
// type : menu
// You choose here wich system is used for the sky 
// 0 :Dynamic  lights: Sun and moon simulation .The light changes with time of day 
// and west (sunrise) direction.
// 1 : Static Lights: simple model without celest objects , it's always noon

#ifndef (TF_LIGHTS_SYSTEM)
#declare TF_LIGHTS_SYSTEM = 0;
#end

// TF_LIGHTS_LUMINOSITY
// in the dialog box : MAIN LIGHTS
// type : hscale [0, 2]
// Luminosity for the lights

#ifndef (TF_LIGHTS_LUMINOSITY)
#declare  TF_LIGHTS_LUMINOSITY= 1.0;
#end

// TF_AMBIENT_LIGHT_LUMINOSITY
// in the dialog box : AMBIENT LIGHT LUMINOSITY
// type : hscale [0, 5]
// Luminosity for ambient enlightment

#ifndef (TF_AMBIENT_LIGHT_LUMINOSITY)
#declare  TF_AMBIENT_LIGHT_LUMINOSITY= 1.0;
#end

// TF_RADIOSITY
// in the dialog box : RADIOSITY
// type : boolean
// Switch on/off the radiosity 

#ifndef (TF_RADIOSITY)
#declare TF_RADIOSITY= false;
#end

// TF_RADIOSITY_QUALITY
// in the dialog box : RADIOSITY QUALITY
// type : integer [0,10]
// Quality of the radiosity, 0 : poor, 10 : high

#ifndef (TF_RADIOSITY_QUALITY)
#declare TF_RADIOSITY_QUALITY = 0;
#end

// TF_TIME_OF_DAY
// in the dialog box : TIME OF DAY
// type : spin button [0, 24]
// Time of day is expressed as the number of hours, the moon location depend on
// TF_MOON_TIME_OFFSET parameter
// e.x.  0.0  = (12:00am) Midnight
//       6.25 = ( 6:15am) Around sunrise
//      12.0  = (12:00pm) Noon
// For Dynamic Lights only

#ifndef (TF_TIME_OF_DAY)
#declare TF_ TIME_OF_DAY= 10.5;
#end

// TF_MOON_TIME_OFFSET
// in the dialog box : MOON TIME OFFSET
// type : spin button [0, 12]
// (in hours) offset between moon and sun
// 0 they are at the same location, 12 at the opposite
// For Dynamic Lights only

#ifndef (TF_MOON_TIME_OFFSET)
#declare TF_MOON_TIME_OFFSET = 12;
#end

// TF_WEST_DIR
// in the dialog box : WEST (SUNSET) DIRECTION
// type : hscale [-90, 90]
// Set the direction for West (sunset).
// Moon and Sun goes from east to west as in the real world, so it's important
// to know were the West is.
// West = -90 or 90 , look along the X axis to see celest objects
// West = 0, look along the Z axis to see celest objects (camera presets look along the Z axis)
// For Dynamic Lights only

#ifndef (TF_WEST_DIR)
#declare TF_WEST_DIR = 0.0;
#end

#declare TF_NORTH_DIR = TF_WEST_DIR + 90.0;

// TF_LIGHTS_DESCRIPTION
// in the dialog box : LIGHTS TYPE
// type : POV include file
// You choose here wich file is used for the static lights theme
// For Static Lights only

#ifndef (TF_LIGHTS_DESCRIPTION)
#declare TF_LIGHTS_DESCRIPTION =  "lights_standard.inc"
#end

// TF_HAVE_CAMERA_LIGHT
// in the dialog box : CAMERA LIGHT
// type : boolean
// Add a light over the camera
// For Static Lights only

#ifndef (TF_HAVE_CAMERA_LIGHT)
#declare  TF_HAVE_CAMERA_LIGHT = false;
#end

// TF_CAMERA_LIGHT_LUMINOSITY
// in the dialog box : CAMERA LIGHT LUMINOSITY
// type : hscale [0, 2]
// Luminosity for the camera light
// For Static Lights only

#ifndef (TF_CAMERA_LIGHT_LUMINOSITY)
#declare  TF_CAMERA_LIGHT_LUMINOSITY= 0.5;
#end

///////////////////////////////////////////////////////////////////////////////
/////// Sea
///////////////////////////////////////////////////////////////////////////////

// TF_HAVE_WATER
// in the dialog box : FILLED SEA
// type : boolean
// Self explanatory.

#ifndef (TF_HAVE_WATER)
#declare  TF_HAVE_WATER= TRUE;
#end

// in the dialog box : WATER TYPE
// type : POV include file
// Not a parameter, you choose here wich file is used for the water theme

// TF_WATER_LEVEL
// in the dialog box : SEA LEVEL
// type : hscale [0, 1]
// The smaller the number, the more land that is above water.

#ifndef (TF_WATER_LEVEL)
#declare TF_WATER_LEVEL = 0.33;
#end

#if (TF_HAVE_WATER=false)
#declare TF_WATER_LEVEL = 0; //could be important for some HF textures
#end

// TF_WATER_CLARITY
// in the dialog box : WATER CLARITY
// type : hscale [0, 1]
// It has no effect if water is disabled. O: it's opaque

#ifndef (TF_WATER_CLARITY)
#declare TF_WATER_CLARITY = 0.30;
#end

// TF_WATER_REFLECTION
// in the dialog box : WATER REFLECTION
// type : hscale [0 , 1]
// Control the reflection of the water. 1 : like a mirror

#ifndef (TF_WATER_REFLECTION)
#declare TF_WATER_REFLECTION = 0.30;
#end

// TF_WATER_COLOR_[RED..BLUE]
// in the dialog box : WATER COLOR
// type : spin button [0,2]
// Control the colour of the water

#ifndef (TF_WATER_COLOR_RED)
#declare TF_WATER_COLOR_RED  = 0.60;
#end

#ifndef (TF_WATER_COLOR_GREEN)
#declare TF_WATER_COLOR_GREEN = 0.75;
#end

#ifndef (TF_WATER_COLOR_BLUE)
#declare TF_WATER_COLOR_BLUE = 0.85;
#end

// TF_WATER_COLOR_[RED..BLUE]
// in the dialog box : never
// type : POV vector
// Control the colour of the water

#ifndef (TF_WATER_COLOR)
#declare TF_WATER_COLOR = <TF_WATER_COLOR_RED, TF_WATER_COLOR_GREEN, TF_WATER_COLOR_BLUE>;
#end

// TF_WATER_BOTTOMCOLOR
// in the dialog box : no
// type : POV vector
// Not a parameter, you should use this at level 0 in the declaration of the
// final texture_map of TF_LANDSCAPE_TEXTURE

#ifndef (TF_WATER_BOTTOMCOLOR)
#declare TF_WATER_BOTTOMCOLOR = <TF_WATER_COLOR_RED, TF_WATER_COLOR_GREEN, TF_WATER_COLOR_BLUE>* 0.4;
#end

// TF_WATER_FREQUENCY
// in the dialog box : WATER FREQUENCY
// type : hscale [0 , 5000]
// Control the frequency of the waves

#ifndef (TF_WATER_FREQUENCY)
#declare TF_WATER_FREQUENCY = 1300;
#end

// TF_WATER_HEIGHT
// in the dialog box : WATER WAVES HEIGHT
// type : hscale [0, 10]
// Control height of the waves. 0 : flat like ice

#ifndef (TF_WATER_HEIGHT)
#declare TF_WATER_HEIGHT = 1.0;
#end

///////////////////////////////////////////////////////////////////////////////
/////// Terrain
///////////////////////////////////////////////////////////////////////////////

// in the dialog box : TERRAIN THEME
// type : POV include file
// Not a parameter, you choose here wich file is used for the terrain theme
// desert, mountain, green,...

// TF_LANDSCAPE_DISTRIBUTION
// in the dialog box : TEXTURE SPREADING
// type : hscale [0, 10]
// Control the distribution of the spots for a spotted texture (0:very big and 10:very little)

#ifndef (TF_LANDSCAPE_DISTRIBUTION)
#declare TF_LANDSCAPE_DISTRIBUTION = 1.0;
#end

// TF_LANDSCAPE_TURBULENCE
// in the dialog box : TURBULENCE
// type : Spin button [0.01, 100]
// Control the turbulence in both axis

#ifndef (TF_LANDSCAPE_TURBULENCE)
#declare TF_LANDSCAPE_TURBULENCE = 1.0;
#end

#declare TF_LANDSCAPE_TURBULENCE_X = TF_LANDSCAPE_TURBULENCE ;
#declare TF_LANDSCAPE_TURBULENCE_Y = TF_LANDSCAPE_TURBULENCE ;
#declare TF_LANDSCAPE_TURBULENCE_Z = TF_LANDSCAPE_TURBULENCE ;

// TF_[ROCK..SNOW]_AMOUNT
// in the dialog box : AMOUNT
// type : Spin button [0, 1]
// Control the amount for each basis texture used by Texture Constructor

#ifndef (TF_ROCK_AMOUNT)
#declare  TF_ROCK_AMOUNT= 0.33 ;
#end

#ifndef (TF_GRASS_AMOUNT)
#declare  TF_GRASS_AMOUNT= 0.33 ;
#end

#ifndef (TF_SNOW_AMOUNT)
#declare  TF_SNOW_AMOUNT= 0.33 ;
#end

#local LOCAL_ROCK_LEVEL = TF_ROCK_AMOUNT / (TF_ROCK_AMOUNT+TF_GRASS_AMOUNT+TF_SNOW_AMOUNT );
#local LOCAL_GRASS_LEVEL = TF_GRASS_AMOUNT / (TF_ROCK_AMOUNT+TF_GRASS_AMOUNT+TF_SNOW_AMOUNT );
#local LOCAL_SNOW_LEVEL = TF_SNOW_AMOUNT / (TF_ROCK_AMOUNT+TF_GRASS_AMOUNT+TF_SNOW_AMOUNT );

#declare TF_ROCK_LEVEL = LOCAL_ROCK_LEVEL;
#declare TF_GRASS_LEVEL = LOCAL_GRASS_LEVEL;
#declare TF_SNOW_LEVEL = LOCAL_SNOW_LEVEL;

// BTEX_[ROCK..SAND]_COLOR_[RED..BLUE]
// in the dialog box : RED GREEN BLUE
// type : Spin button [0, 1]
// Color for the basis textures used by the constructor

#ifndef (BTEX_ROCK_COLOR_RED)
#declare BTEX_ROCK_COLOR_RED = 0.5 ;
#end

#ifndef (BTEX_ROCK_COLOR_GREEN)
#declare BTEX_ROCK_COLOR_GREEN = 0.5 ;
#end

#ifndef (BTEX_ROCK_COLOR_BLUE)
#declare BTEX_ROCK_COLOR_BLUE = 0.5 ;
#end

#ifndef (BTEX_GRASS_COLOR_RED)
#declare BTEX_GRASS_COLOR_RED = 0.75 ;
#end

#ifndef (BTEX_GRASS_COLOR_GREEN)
#declare BTEX_GRASS_COLOR_GREEN = 0.90 ;
#end

#ifndef (BTEX_GRASS_COLOR_BLUE)
#declare BTEX_GRASS_COLOR_BLUE = 0.45 ;
#end

#ifndef (BTEX_SNOW_COLOR_RED)
#declare BTEX_SNOW_COLOR_RED = 1.0 ;
#end

#ifndef (BTEX_SNOW_COLOR_GREEN)
#declare BTEX_SNOW_COLOR_GREEN = 1.0 ;
#end

#ifndef (BTEX_SNOW_COLOR_BLUE)
#declare BTEX_SNOW_COLOR_BLUE = 1.0 ;
#end

#ifndef (BTEX_SAND_COLOR_RED)
#declare BTEX_SAND_COLOR_RED = 0.90 ;
#end

#ifndef (BTEX_SAND_COLOR_GREEN)
#declare BTEX_SAND_COLOR_GREEN = 0.85 ;
#end

#ifndef (BTEX_SAND_COLOR_BLUE)
#declare BTEX_SAND_COLOR_BLUE = 0.70 ;
#end

#declare BTEX_ROCK_COLOR = <BTEX_ROCK_COLOR_RED,  BTEX_ROCK_COLOR_GREEN, BTEX_ROCK_COLOR_BLUE> ;
#declare BTEX_GRASS_COLOR = <BTEX_GRASS_COLOR_RED,  BTEX_GRASS_COLOR_GREEN, BTEX_GRASS_COLOR_BLUE>;
#declare BTEX_SNOW_COLOR =  <BTEX_SNOW_COLOR_RED,  BTEX_SNOW_COLOR_GREEN, BTEX_SNOW_COLOR_BLUE>;
#declare BTEX_SAND_COLOR =  <BTEX_SAND_COLOR_RED,  BTEX_SAND_COLOR_GREEN, BTEX_SAND_COLOR_BLUE>;

// BTEX_[ROCK..SAND]_GRAIN
// in the dialog box : Texture grain
// type : Spin button [0, 5]
// Granularity for the basis textures used by the constructor

#ifndef (BTEX_ROCK_GRAIN)
#declare BTEX_ROCK_GRAIN = 1.0 ;
#end

#ifndef (BTEX_GRASS_GRAIN)
#declare BTEX_GRASS_GRAIN = 1.5 ;
#end

#ifndef (BTEX_SNOW_GRAIN)
#declare BTEX_SNOW_GRAIN = 1.0 ;
#end

#ifndef (BTEX_SAND_GRAIN)
#declare BTEX_SAND_GRAIN = 0.7 ;
#end

// TF_ROCK_STRATUM
// in the dialog box : STRATUM
// type : Boolean
// Switch on/off the creation of stratum for the rock

#ifndef (TF_ROCK_STRATUM)
#declare TF_ROCK_STRATUM = false ;
#end

// BTEX_STRATUM_SIZE
// in the dialog box : STRATUM SIZE
// type : Spin button [0.1, 10]
// Control the size for each stratum

#ifndef (BTEX_STRATUM_SIZE)
#declare BTEX_STRATUM_SIZE = 1.0 ;
#end

// TF_GRASSTEX 
// in the dialog box : GRASSTEX
// type : Boolean
// Switch on/off the use of Grasstex.inc for the grass

#ifndef (TF_GRASSTEX)
#declare  TF_GRASSTEX = false ;
#end

// TF_LANDSCAPE_WATER_TRANSITION
// in the dialog box : WATER BORDER
// type : Boolean
// Create a second very flat Height-field covered by Ice or Sand
// This take place between water and terrain

#ifndef (TF_LANDSCAPE_WATER_TRANSITION)
#declare  TF_LANDSCAPE_WATER_TRANSITION = false ;
#end

// TF_TRANSITION_VALUE
// in the dialog box : WATER BORDER SIZE
// type : Spin button [0,1]
// Size of the border between [0:no transition, 
// and 1:no water only ice or sand until the limit of the heigth-field]

#ifndef (TF_TRANSITION_VALUE)
#declare   TF_TRANSITION_VALUE = 0.8 ;
#end

// TF_LANDSCAPE_WATER_TRANSITION_SAND
// in the dialog box : Sand
// type : Boolean
// Use Sand for the very flat Height-field (create a beach)

#ifndef (TF_LANDSCAPE_WATER_TRANSITION_SAND)
#declare  TF_LANDSCAPE_WATER_TRANSITION_SAND = true ;
#end

// TF_LANDSCAPE_WATER_TRANSITION_ICE
// in the dialog box : Ice
// type : Boolean
// Use Ice for the very flat Height-field

#ifndef (TF_LANDSCAPE_WATER_TRANSITION_ICE)
#declare  TF_LANDSCAPE_WATER_TRANSITION_ICE = true ;
#end

/////// TRACE Objects list /////////////////////////////////////////////////////////

// You can use this to declare several "single trace objects" while the dialog let you
// place only one....
// in the dialog box : no
// type : POV include file
// Not a parameter, list of the single objects included in the scene by the TRACE include file 
// uncomment if you wish to include a single object in the scene. Be sure that the file is in
// the include directory.

#ifndef (RENDER_TRACE_OBJECT)
  #declare RENDER_TRACE_OBJECT = true;
#end

//#declare TF_TRACE_OBJECT_1 = "_example.inc" // don't declare this object if you already have one from the terraform object menu
//#declare TF_TRACE_OBJECT_2 = "_bee.inc"
//#declare TF_TRACE_OBJECT_3 = "_boat.inc"
//#declare TF_TRACE_OBJECT_3 = "_butterfly.inc"
//#declare TF_TRACE_OBJECT_4 = "_grass_multiflowers.inc"
//#declare TF_TRACE_OBJECT_5 = "_grass_prairie.inc"
//#declare TF_TRACE_OBJECT_6 = "_needlebranch.inc"
//#declare TF_TRACE_OBJECT_7 = "_tree_8.inc"
//#declare TF_TRACE_OBJECT_8 = "_tree_palm_1.inc"
//#declare TF_TRACE_OBJECT_9 = "_tree_parametric_1.inc"
//#declare TF_TRACE_OBJECT_10 = "_tree_parametric_1.inc"
//#declare TF_TRACE_OBJECT_11 = "_tree_parametric_2.inc"
//#declare TF_TRACE_OBJECT_12 = "_tree_parametric_3.inc"
//#declare TF_TRACE_OBJECT_13 = "_tree_parametric_4.inc"
//#declare TF_TRACE_OBJECT_14 = "_tree_parametric_5.inc"
//#declare TF_TRACE_OBJECT_15 = "_tree_parametric_6.inc"
//#declare TF_TRACE_OBJECT_16 = "_tree_parametric_7.inc"

/////////////////////////////////////////////////////////////////////////////
/////// End of User Definable Parameters ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// Well not really the end, you can find many other local parameters in the POV
// files...

/////////////////////////////////////////////////////////////////////////////
/////// POV-Ray Global Settings
/////////////////////////////////////////////////////////////////////////////

global_settings {
     #if (TF_RADIOSITY)
        
        #local LOCAL_PRETRACE_END = 0.04 - (0.03 *(TF_RADIOSITY_QUALITY/10));
        #local LOCAL_COUNT = 10 + (40 *(TF_RADIOSITY_QUALITY/10));
        #local LOCAL_NEAREST_COUNT = 1 + (19 *(TF_RADIOSITY_QUALITY/10));
        #local LOCAL_ERROR_BOUND = 1 + (19 *(TF_RADIOSITY_QUALITY/10));
        #local LOCAL_ERROR_FACTOR = 2 - (1.8 *(TF_RADIOSITY_QUALITY/10));
        #local LOCAL_REUSE = 0.2 - (0.18 *(TF_RADIOSITY_QUALITY/10));

         radiosity {
              pretrace_start 0.08
              pretrace_end   LOCAL_PRETRACE_END
              count LOCAL_COUNT
              nearest_count LOCAL_NEAREST_COUNT
              error_bound LOCAL_ERROR_BOUND
              recursion_limit 1
              low_error_factor LOCAL_ERROR_FACTOR
              gray_threshold 0.0
              minimum_reuse LOCAL_REUSE
              brightness TF_AMBIENT_LIGHT_LUMINOSITY
              adc_bailout 0.01/2
         }
         ambient_light 0
      #else
         ambient_light TF_AMBIENT_LIGHT_LUMINOSITY * 3
      #end // RADIOSITY
      max_trace_level 20
}
 
