//
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
//  TRACE objects include file
//---------------------------------------------

#if (RENDER_TRACE_OBJECT)

  #local LOCAL_EYE_POINT_SCALED = < TF_X_SCALE * LOCAL_EYE_POINT.x , TF_Y_SCALE * LOCAL_EYE_POINT.y , TF_Z_SCALE * LOCAL_EYE_POINT.z >;

  #if (LOCAL_CAMERA_RELATIV)
    #local LOCAL_EYE_POINT_SCALED = TF_CAMERA_LOCATION + LOCAL_EYE_POINT_SCALED;
  #end  

  #if (LOCAL_CONSERVE_ELEVATION) // object is placed at is original location
     object { 
       objet_toplace
       translate LOCAL_EYE_POINT_SCALED
     } 
  #else
    #local LOCAL_SLOPE=<0,0,0>; // Normal vector initialisation  
    #local LOCAL_TRACE_POINT = trace( TF_LAND, LOCAL_EYE_POINT_SCALED, -y, LOCAL_SLOPE ); // trace function on the HF
    #if ( LOCAL_SLOPE.x=0 & LOCAL_SLOPE.y=0 & LOCAL_SLOPE.z=0 ) // if true you are out of the HF, the object go on the water plane
      object { 
        objet_toplace
        translate < LOCAL_EYE_POINT_SCALED.x , TF_Y_SCALE * TF_WATER_LEVEL , LOCAL_EYE_POINT_SCALED.z >
      }
    #else // the object go on the HF
      #if (LOCAL_OBJECT_FLOAT) // floating object can not go under the water
        #if ( (TF_Y_SCALE * TF_WATER_LEVEL) > LOCAL_TRACE_POINT.y ) // if true the object is replaced on the water
          object { 
            objet_toplace
            translate < LOCAL_EYE_POINT_SCALED.x , TF_Y_SCALE * TF_WATER_LEVEL , LOCAL_EYE_POINT_SCALED.z >
          }
        #else 
          object { 
            objet_toplace
            translate LOCAL_TRACE_POINT
          }
        #end  // end float object      
      #else
        object { 
          objet_toplace
          translate LOCAL_TRACE_POINT
        }
      #end
    #end
  #end
#end











