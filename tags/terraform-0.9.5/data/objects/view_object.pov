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
//
// ----------------------------------------

global_settings { assumed_gamma 1 }

// Output_File_Name (change the path) :: -o/home/raymond/terraform_object.png
// Width :: +w160     
// Height :: +h120              
// Library :: +L/usr/local/share/terraform/objects

//#local SCENE_SCALE=0.5;
//#local SCENE_SCALE=1;  // small objects
//#local SCENE_SCALE=3;  // medium objects
#local SCENE_SCALE=5;  // big objects

#declare TF_CAMERA_LOCATION = <0.0, 0.1*SCENE_SCALE, -1.0*SCENE_SCALE>;

camera { 
  location  TF_CAMERA_LOCATION
  direction 1.5*z
  right     x*image_width/image_height
  look_at   <0.0, 0.2*SCENE_SCALE,0.0>

  //translate x*1  // some patch need to be translated
}

sky_sphere {
  pigment {
    gradient y
    color_map {
      [0.0 rgb <0.6,0.7,1.0>]
      [0.7 rgb <0.0,0.1,0.8>]
    }
  }
}

light_source {
  <0, 0, 0>               // light's position (translated below)
  color rgb <1, 1, 1> *3  // light's color
  translate <-10, 10, -10>
}

light_source {
  <0, 0, 0>                // light's position (translated below)
  color rgb <1, 1, 1> *1.5 // light's color
  translate <10, 10, -10>
}

light_source {
  <0, 0, 0>                // light's position (translated below)
  color rgb <1, 1, 1>      // light's color
  translate <-10, 10, 10>
}

#local TF_LAND = plane {
  y, 0
  pigment { color rgb <0.4, 0.3, 0.05> }
  normal {
    bumps 0.5         // any pattern optionally followed by an intensity value [0.5]
    //bump_size 2.0   // optional
    //accuracy 0.02   // changes the scale for normal calculation [0.02]
    scale 0.01        // any transformations
  }
}

TF_LAND

box {
  <1,0,0>
  <1.2,2,0>
  pigment { 
    checker
    color rgb <1, 1, 1>,
    color rgb <0, 0, 0>
    scale 1/10
  }
}

box {
  <0.3,0,0>
  <0.5,0.4,0>
  pigment { 
    checker
    color rgb <1, 1, 1>,
    color rgb <0, 0, 0>
    scale 1/10
  }
  //translate -x * 0.15 // form very small objects
}

// -----For standard objects --------------------------------

//#include "tree_1.inc"

//object {tree_1() scale 1}


// -----For TRACE objects -----------------------------------

#declare RENDER_TRACE_OBJECT = false;

#include "TRACE_grass_multiflowers.inc"
#local LOCAL_CONSERVE_ELEVATION = false;
#local LOCAL_OBJECT_SCALE = 1;

#local LOCAL_OBJECT_ELEVATION = 0.2 * SCENE_SCALE;
#local LOCAL_OBJECT_POINT = <0,LOCAL_OBJECT_ELEVATION,0> ;
#local LOCAL_SLOPE=<0,0,0>; // Normal vector initialisation  
#local LOCAL_TRACE_POINT = trace( TF_LAND, LOCAL_OBJECT_POINT, -y, LOCAL_SLOPE ); // trace function on the Plane
#if (LOCAL_CONSERVE_ELEVATION) // object is placed at is original location
     object { 
       objet_toplace
       scale LOCAL_OBJECT_SCALE
       translate LOCAL_OBJECT_ELEVATION * y 
     } 
#else // the object go on the HF
     object { 
       objet_toplace
       scale LOCAL_OBJECT_SCALE
       translate LOCAL_TRACE_POINT
       translate -x * 0.5
     }
#end


