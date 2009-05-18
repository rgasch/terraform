// Copyright (c) 1999, 2000 David A. Bartold
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

// Declare a TF_LANDSCAPE_TEXTURE before #including this file.
// TF_LANDSCAPE_TEXTURE (if it's a texture_map) should be defined as such:
//   0.0 = seafloor level
//   1.0 = highest point

// Create a camera

camera
{  
  #if (TF_CAMERA_TYPE = 0)
  perspective
  #else
    #if (TF_CAMERA_TYPE = 1)
    fisheye
    #else
      #if (TF_CAMERA_TYPE = 2)
      panoramic
      #else// you should'nt go there
          perspective
       #end  // 2
    #end  // 1
  #end  // 0
  angle TF_CAMERA_ANGLE
  location TF_CAMERA_LOCATION
  look_at  TF_CAMERA_LOOK_AT
}

// Create land:

#declare TF_LAND = object
{
  TF_HEIGHT_FIELD      
  texture
  {
    TF_LANDSCAPE_TEXTURE
  }
  scale <TF_X_SCALE, TF_Y_SCALE, TF_Z_SCALE>
}
object { TF_LAND }

#if (TF_TILE_TERRAIN)

  #local iz = 0;
  #while (iz < TF_Z_TILE)
    #local ix = 0;
    #while (ix < TF_X_TILE)
      #local POS_X = ix - int( TF_X_TILE/2 );
      #local POS_Z = iz - int( TF_Z_TILE/2 );
      #if ( !(POS_X=0) | !(POS_Z=0) )
        object {
          TF_HEIGHT_FIELD      
          texture
            {
              TF_LANDSCAPE_TEXTURE
            }
            translate <POS_X, 0, POS_Z>
            scale <TF_X_SCALE, TF_Y_SCALE, TF_Z_SCALE>
        }
      #end //if
      #local ix = ix +1;
    #end
    #local iz = iz +1;
  #end

#end //TF_TILE_TERRAIN

// Create a bottommost plane for water
plane
{
    <0.00, 1.00, 0.00>,-0.01
  
    texture
    {
      pigment
      {
        color TF_WATER_BOTTOMCOLOR  // Bottom-of-the-sea
      }
    }
}

// Add additonnal TRACE objects
// See end of the defaults.inc file for the declarations

#ifndef (TF_TRACE_OBJECT_1) #else #include TF_TRACE_OBJECT_1 #end
#ifndef (TF_TRACE_OBJECT_2) #else #include TF_TRACE_OBJECT_2 #end 
#ifndef (TF_TRACE_OBJECT_3) #else #include TF_TRACE_OBJECT_3 #end 
#ifndef (TF_TRACE_OBJECT_4) #else #include TF_TRACE_OBJECT_4 #end 
#ifndef (TF_TRACE_OBJECT_5) #else #include TF_TRACE_OBJECT_5 #end 
#ifndef (TF_TRACE_OBJECT_6) #else #include TF_TRACE_OBJECT_6 #end 
#ifndef (TF_TRACE_OBJECT_7) #else #include TF_TRACE_OBJECT_7 #end 
#ifndef (TF_TRACE_OBJECT_8) #else #include TF_TRACE_OBJECT_8 #end 
#ifndef (TF_TRACE_OBJECT_9) #else #include TF_TRACE_OBJECT_9 #end 
#ifndef (TF_TRACE_OBJECT_10) #else #include TF_TRACE_OBJECT_10 #end 
#ifndef (TF_TRACE_OBJECT_11) #else #include TF_TRACE_OBJECT_11 #end 
#ifndef (TF_TRACE_OBJECT_12) #else #include TF_TRACE_OBJECT_12 #end 
#ifndef (TF_TRACE_OBJECT_13) #else #include TF_TRACE_OBJECT_13 #end 
#ifndef (TF_TRACE_OBJECT_14) #else #include TF_TRACE_OBJECT_14 #end 
#ifndef (TF_TRACE_OBJECT_15) #else #include TF_TRACE_OBJECT_15 #end 
#ifndef (TF_TRACE_OBJECT_16) #else #include TF_TRACE_OBJECT_16 #end 
#ifndef (TF_TRACE_OBJECT_17) #else #include TF_TRACE_OBJECT_17 #end 

// Create a bottommost plane for water
plane
{
    <0.00, 1.00, 0.00>,-0.01
  
    texture
    {
      pigment
      {
        color TF_WATER_BOTTOMCOLOR  // Bottom-of-the-sea
      }
    }
}
