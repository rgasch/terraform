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
//

////////////////// Macro utilities for skies ///////////////////////////////////////////

// CelesteCoord function return XYZ coord of a celest object
   #macro CelesteCoord( Distance, Hours, North )
     #local ELEVATION_ANGLE = radians ( 15 * Hours - 90 );
     #local Y_FACTOR = sin ( ELEVATION_ANGLE );
     #local ROTATION_ANGLE = radians ( North + 90 );
     #local X_FACTOR = cos ( ELEVATION_ANGLE ) * sin ( ROTATION_ANGLE );
     #local Z_FACTOR = cos ( ELEVATION_ANGLE ) * cos ( ROTATION_ANGLE );
     /*
     #debug str( X_FACTOR, 15, 5 )
     #debug str( Y_FACTOR, 15, 5 )
     #debug str( Z_FACTOR, 15, 5 )
     */
     ( <X_FACTOR, Y_FACTOR, Z_FACTOR> * Distance );
   #end

// Perspective correction (old macro)
   /*
   #macro SphereSizeFactor()
     (abs (sin (TF_TIME_OF_DAY * 2.0 * pi / 24.0)) * 0.6 + 0.4)
   #end
   */

// Perspective correction (new macro not achieved)
   #macro SphereVisualFactor( LOC_CAMERA, LOC_LOOK_AT, LOC_OBJECT )
     #local X0 = LOC_CAMERA.x;
     #local Y0 = LOC_CAMERA.y;
     #local Z0 = LOC_CAMERA.z;
     #local X1 = LOC_LOOK_AT.x;
     #local Y1 = LOC_LOOK_AT.y;
     #local Z1 = LOC_LOOK_AT.z;
     #local X2 = LOC_OBJECT.x;
     #local Y2 = LOC_OBJECT.y;
     #local Z2 = LOC_OBJECT.z;

     #local DIST_XZ = sqrt( (X2-X0)*(X2-X0) + (Z2-Z0)*(Z2-Z0) );
     #local DIST_YZ = sqrt( (Y2-Y0)*(Y2-Y0) + (Z2-Z0)*(Z2-Z0) );

   //  #local ALPHA4 = atan2( (Z2-Z0),(X2-X0) );
   //  #local ALPHA2 = atan2( (Z1-Z0),(X1-X0) );
   //  #local ANGLE_A = abs( ALPHA2 - ALPHA4 );
   //  #local DIST_A = sin( ANGLE_A ) * DIST_XZ ;

     #local ALPHA4 = atan2( (Y2-Y0), DIST_XZ );
     #local ALPHA2 = atan2( (Y1-Y0), DIST_XZ );
     #local ANGLE_B = abs( ALPHA2 - ALPHA4 );
     #local DIST_B = sin( ANGLE_B ) * DIST_YZ ;

   //  #declare VISUAL_ORIENTATION_ANGLE = ANGLE_A;
     #declare VISUAL_ELEVATION_ANGLE = ANGLE_B;
     #declare VISUAL_ELEVATION_FACTOR = cos( ANGLE_B );

   //  #debug str( degrees(VISUAL_ELEVATION_ANGLE), 15, 5 )
   //  #debug str( VISUAL_ELEVATION_FACTOR, 15, 5 )

   //  #if ( ( DIST_A = 0 ) & ( DIST_B = 0 ) )
   //      #declare VISUAL_ANGLE = 0;
   //      #declare VISUAL_Y_FACTOR = 1;
   //  #else
   //      #declare VISUAL_ANGLE = atan2( DIST_A,DIST_B );
   //      #declare VISUAL_Y_FACTOR = ( cos(ANGLE_A) * cos(ANGLE_B) );
   //  #end

     /*
     #debug str( degrees(VISUAL_ORIENTATION_ANGLE), 15, 5 )
     #debug str( degrees(VISUAL_ELEVATION_ANGLE), 15, 5 )
     #debug str( degrees(VISUAL_ANGLE), 15, 5 )
     #debug str( VISUAL_FACTOR, 15, 5 )
     */

   #end
