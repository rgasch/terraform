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

////////////////// Parameters /////////////////////////////////////////////////

#local SKY_image_ratio = (16/9); // images with a height/width 16/9 ratio are used
#local SKY_image_lateral_adjustment = -0.475; // if the image is not centered, change this

// some known limitations
// the image is not X centered because the calculation is not complete, only the basic orientatioN
// is corrected.
// the perspective is not corrected, you should use same Y levels for the camera settings

////////////////// Macros /////////////////////////////////////////////////////

#macro VISUAL_ORIENTATION_ANGLE ( LOC_CAMERA, LOC_LOOK_AT ) // look_at angle from Z axis
  #local X0 = LOC_CAMERA.x;
  #local Z0 = LOC_CAMERA.z;
  #local X1 = LOC_LOOK_AT.x;
  #local Z1 = LOC_LOOK_AT.z;

  #local ANGLE_A = degrees( atan2( (X1-X0),(Z1-Z0) ) );
  /*
  #debug str( ANGLE_A, 15, 5 )
  */
  (ANGLE_A)
#end

////////////////// Mapped BACKGROUND IMAGE ///////////////////////////////////
//
#declare pigment_sky_map = pigment {
	image_map {
		tga TF_SKY_IMAGE
		once
		interpolate 4
		map_type 0
	}
	translate <SKY_image_lateral_adjustment, TF_SKY_IMAGE_ELEVATION_OFFSET , 0.0>
	scale 15 * vlength ( TF_SCALE ) * TF_SKY_IMAGE_SCALE
   scale y * ( 1/SKY_image_ratio )
}

plane {
   <0,0,1>
   10 * vlength ( TF_SCALE )
   texture {
      pigment { pigment_sky_map } // projected image
      finish { ambient TF_SKY_IMAGE_LUMINOSITY }
   }
   rotate y * VISUAL_ORIENTATION_ANGLE( TF_CAMERA_LOCATION, TF_CAMERA_LOOK_AT )
}



