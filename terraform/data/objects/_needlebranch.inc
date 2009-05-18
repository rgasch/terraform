// Persistence of Vision Ray Tracer Scene Description File
// File: branch1.pov
// Vers: 3.5
// Desc: A single branch with needles (part of a fir tree).
//       By putting several of them together you can create bigger branches with branchings out.
// Date: Jun 30 1998
//       Jan 08 2003 update of my email address and web url, 
//                   now GPL license
//                   added missing semicolons to be POV3.5 compatible
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

// Adapted for Terraform by Raymond Ostertag

//---------------------------------------------
// Parameters 
//---------------------------------------------

#ifndef (TF_TRACE_OBJECT_X)
   #local LOCAL_EYE_POINT = < 0.5, 10, 0.05 >; 
#else
   #local LOCAL_EYE_POINT = < TF_TRACE_OBJECT_X, TF_TRACE_OBJECT_Y, TF_TRACE_OBJECT_Z > ; 
#end
// coordinates are like in the camera dialog 
// X=0,Z=0 and X=1,Z=1 are the bounding of the terrain
// if X and Z coordinates are in the bounding then the object is placed on the terrain (Y must be greater than the height of the terrain)
// if X or Z coordinates are out of the bounding then the object is placed on the water


#ifndef (TF_TRACE_OBJECT_RELATIV)
   #local LOCAL_CAMERA_RELATIV = false;
#else
   #local LOCAL_CAMERA_RELATIV = TF_TRACE_OBJECT_RELATIV;
#end
// if this boolean is true the coordinate are relativ to the camera location instead of the origin of the HF
// if the eye_point is over the terrain the object is placed on the terrain, if it's over the water then the object is placed on the water

#ifndef (TF_TRACE_OBJECT_CONSERVE)
   #local LOCAL_CONSERVE_ELEVATION = false;
#else
   #local LOCAL_CONSERVE_ELEVATION = TF_TRACE_OBJECT_CONSERVE;
#end
// if this boolean is true the object is placed in the air (for a plane) or in the water (for a submarine) at the coordinates of the eye_point. 

#ifndef (TF_TRACE_OBJECT_FLOAT)
   #local LOCAL_OBJECT_FLOAT = false;
#else
   #local LOCAL_OBJECT_FLOAT = TF_TRACE_OBJECT_FLOAT;
#end
// if this boolean is false the object can be on earth but under the water.

#ifndef (TF_TRACE_OBJECT_SCALE)
   #local LOCAL_OBJECT_SCALE = 0.4;
#else
   #local LOCAL_OBJECT_SCALE = TF_TRACE_OBJECT_SCALE;
#end

// ------

// Other Parameters:
#local nd=0.137177;  //--- density of the needles (large values means less needles)
#local ast_length=2;   //--- length of the branch

#local Green   = rgb <0, 1, 0>;
#local Gray = color red 0.752941 green 0.752941 blue 0.752941;

#local branch=union{

#local r=seed(0);
cylinder{<0,0,0>,<ast_length,0,0>,0.03
translate<0.5,0,2.25>
normal{bumps 1 scale .035}
pigment{color rgb<150/356,81/356,0>}}

#local i=0;
#while (i<ast_length*10)
#local xrot=(rand(r)*50+i*200);
#local xrotm=mod(xrot,360);
#switch (abs(xrotm-180))
  #range (45,360)
blob{
threshold 0.5
cylinder{<0,0,0>,<0,1,0>,0.2,1}
cylinder{<0,0.,-.1>,<0,1,-0.1>,0.05,-.35}

pigment{
  gradient z
  pigment_map{
  [0 Green]
  [0.5 Green]
  [0.5 Gray]
  [1 Gray]
}
translate<0,0,-.5>
}
scale<.25+(0.5-rand(r))*.03,.7+(0.5-rand(r))*.07,.1>*0.5
#declare j=i*150;
#if (mod(j,360)>180)
  #local k=-1;
#else
  #local k=1;
#end
translate<0,0.05,0>
rotate y*(xrotm-90)
rotate z*(45+rand(r)*3)
rotate x*xrotm
translate<i/10,0,0>
translate<0.5,0,2.25>

finish{specular 0.4 roughness 0.01 brilliance 2}
}

#break
#end
#local i=i+nd;
#end

}


//---------------------------------------------
//  Object declaration
//---------------------------------------------

#declare objet_toplace = 
object {
  branch translate <-0.5,0,-2.25>
  scale LOCAL_OBJECT_SCALE // additonnal scale to fit whit the terraform's scales
}

//---------------------------------------------
//  trace object include file
//---------------------------------------------

#if (RENDER_TRACE_OBJECT)
  #include "trace_objects.inc"
#end




