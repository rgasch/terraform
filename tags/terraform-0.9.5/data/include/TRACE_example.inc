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

//---------------------------------------------
// Parameters 
//---------------------------------------------

#local LOCAL_EYE_POINT = < 0.0, -0.5, 1.25 >; 
// coordinates are like in the camera dialog 
// X=0,Z=0 and X=1,Z=1 are the bounding of the terrain
// if X and Z coordinates are in the bounding then the object is placed on the terrain (Y must be greater than the height of the terrain)
// if X or Z coordinates are out of the bounding then the object is placed on the water

#local LOCAL_CAMERA_RELATIV = true;
// if this boolean is true the coordinate are relativ to the camera location instead of the origin of the HF
// if the eye_point is over the terrain the object is placed on the terrain, if it's over the water then the object is placed on the water

#local LOCAL_CONSERVE_ELEVATION = true;
// if this boolean is true the object is placed in the air (for a plane) or in the water (for a submarine) at the coordinates of the eye_point. 

#local LOCAL_OBJECT_FLOAT = true;
// if this boolean is false the object can be on earth but under the water.

#local LOCAL_OBJECT_SCALE = vlength (TF_SCALE/100);
// additonnal scale to fit whit the terraform's scales

//---------------------------------------------
//  Object declaration
//---------------------------------------------

#local objet_toplace = 
object {
  sphere {
    <0, 0, 0> // center of sphere <X Y Z>
    2       // radius of sphere
    texture {
      pigment {
        color red 0.9 green 0.9 blue 0.1 // filter 0.8
      }
    }
  }
  scale LOCAL_OBJECT_SCALE 
}

//---------------------------------------------
//  trace object include file
//---------------------------------------------

#include "trace_objects.inc"





