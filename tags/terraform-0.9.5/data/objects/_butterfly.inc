// Persistence of Vision Ray Tracer Scene Description File
// File: buterfly.pov
// Vers: 3.5
// Desc: Butterfly (swallow-tail)
// Date: 21.06.1998
//       08.01.2003 update of my email address and web url, 
//                  now GPL license
//                  added missing semicolons to be POV3.5 compatible
// Auth: Micha Riser (mriser@gmx.net)
//       http://povworld.de/michariser/

/*************************************************************************
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  To contact the author send an email to:
        mriser@gmx.net

  These macros are part of the POV-Ray Objects Collection. See
  http://objects.povworld.org/ for further information and more
  interesting objects!
*************************************************************************/

//============================================
// Adapted for Terraform by Raymond Ostertag 
//============================================

//---------------------------------------------
// Parameters 
//---------------------------------------------

#ifndef (TF_TRACE_OBJECT_X)
   #local LOCAL_EYE_POINT = < 0.2, -0.2, 0.5 > / 500 ; 
#else
   #local LOCAL_EYE_POINT = < TF_TRACE_OBJECT_X, TF_TRACE_OBJECT_Y, TF_TRACE_OBJECT_Z > ; 
#end
// coordinates are like in the camera dialog 
// X=0,Z=0 and X=1,Z=1 are the bounding of the terrain
// if X and Z coordinates are in the bounding then the object is placed on the terrain (Y must be greater than the height of the terrain)
// if X or Z coordinates are out of the bounding then the object is placed on the water

#ifndef (TF_TRACE_OBJECT_RELATIV)
   #local LOCAL_CAMERA_RELATIV = true;
#else
   #local LOCAL_CAMERA_RELATIV = TF_TRACE_OBJECT_RELATIV;
#end
// if this boolean is true the coordinate are relativ to the camera location instead of the origin of the HF
// if the eye_point is over the terrain the object is placed on the terrain, if it's over the water then the object is placed on the water

#ifndef (TF_TRACE_OBJECT_CONSERVE)
   #local LOCAL_CONSERVE_ELEVATION = true;
#else
   #local LOCAL_CONSERVE_ELEVATION = TF_TRACE_OBJECT_CONSERVE;
#end
// if this boolean is true the object is placed in the air (for a plane) or in the water (for a submarine) at the coordinates of the eye_point. 

#ifndef (TF_TRACE_OBJECT_FLOAT)
   #local LOCAL_OBJECT_FLOAT = true;
#else
   #local LOCAL_OBJECT_FLOAT = TF_TRACE_OBJECT_FLOAT;
#end
// if this boolean is false the object can be on earth but under the water.

#ifndef (TF_TRACE_OBJECT_SCALE)
   #local LOCAL_OBJECT_SCALE = 0.13;
#else
   #local LOCAL_OBJECT_SCALE = TF_TRACE_OBJECT_SCALE;
#end
// additonnal scale to fit whit the terraform's scales

//---------------------------------------------
// Other Parameters:
//---------------------------------------------
#local wing_angle=45;   //half angle between the two wings
#local wing_file="_butterfly_wing.gif"  //wing texture file
// End of Parameters

#local Brown = color red 0.647059 green 0.164706 blue 0.164706;

//--- Butterfly Wing
#declare fluegel=box{<0,0,0>,<1,1,0>
 pigment{
  image_map{
   gif wing_file
   filter 0,1
     }
   }
   finish{ambient 0.75}
   finish{diffuse 2}
   scale <0.5596,1,1>
   translate<-0.5796,0,0>
 }

//--- Butterfly Body
#declare butterfly_body=union{
sphere{0,0.1
  scale<0.35,1.45,0.35>
  translate<0,0.4,0>
  }
sphere{0,0.1
  scale<0.39,1,0.39>
  translate<0,0.54,0>
  }
sphere{0,0.09
  scale<0.18,0.5,0.18>
  translate<0,0.63,0>
  }
cylinder{<0.007,0.65,-0.005>,<0.07,0.77,-0.075>,0.003 }
cylinder{<-0.007,0.65,-0.005>,<-0.07,0.77,-0.075>,0.003 }
sphere{<-0.07,0.77,-0.075>,0.008 }
sphere{<0.07,0.77,-0.075>,0.008 }
pigment{Brown}
finish{ambient 0.2 diffuse 1}
}

//--- Butterfly eyes
#declare butterfly_eyes=union{
sphere{0,0.01
  scale 1
  translate<0.013,0.648,-0.0095>
  }
sphere{0,0.01
  scale 1
  translate<-0.013,0.648,-0.0095>
  }
 pigment{rgb<0.2,0.2,0.2>}
  finish{ roughness 0.05 specular 0.2}
}

#declare butterfly=union{
 object{butterfly_body}
 object{butterfly_eyes}
 object{fluegel rotate y*(-90+wing_angle)}
 object{fluegel rotate y*180 rotate y*(90-wing_angle)}
}

//---------------------------------------------
//  Object declaration
//---------------------------------------------

#declare objet_toplace = 
object {
  butterfly 
  scale LOCAL_OBJECT_SCALE // additonnal scale to fit whit the terraform's scales
}

//---------------------------------------------
//  trace object include file
//---------------------------------------------

#if (RENDER_TRACE_OBJECT)
  #include "trace_objects.inc"
#end


