// Persistence of Vision Ray Tracer Scene Description File
// File: bee.pov
// Vers: 3.5
// Desc: Honey bee; a flying and a sitting version
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

//==============================================
// Adapted for Terraform by Raymond Ostertag (flying version)
//==============================================

//---------------------------------------------
// Parameters 
//---------------------------------------------

// use this when you see black aeras near the wings:
global_settings{max_trace_level 20}

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
   #local LOCAL_OBJECT_SCALE = 0.04;
#else
   #local LOCAL_OBJECT_SCALE = TF_TRACE_OBJECT_SCALE;
#end
// additonnal scale to fit whit the terraform's scales

//---------------------------------------------
//  Object declaration
//---------------------------------------------

#local beewing=prism { //--- bee wing (flying bee)
  linear_sweep
  quadratic_spline
  0,
  0,
  10,
  <0.8,1.2>,
  <0,0>, <2.3,0.9> ,<5,1.25> ,<5.2,2.75>, <4.5,3.3>, <2.3,2.2>, <0.8,1.2>, <0,0>
  ,<2.3,0.9>
rotate x*-90
pigment{rgbt<0.7,0.7,0.7,0.85>}
normal{crackle scale <1,0.4,0.4> rotate z*30}
finish{roughness 0.03 specular 0.3}
}

#local beesitting_wing=union{ //--- bee wing (sitting bee)
object{
beewing scale<0.23,0.27,0.27>*1.4
rotate x*-90
rotate y*-20
rotate x*-20
rotate z*-5
pigment{rgbt<.7,.7,.7,0.5>}
translate<-.65,0.2,-.25>
}

object{
beewing scale<0.23,0.27,0.27>*1.4
rotate x*90
rotate y*-20
rotate x*20
rotate z*-5
pigment{rgbt<.7,.7,.7,0.4>}
translate<-.65,0.2,-.25>
}

}

#local beewings=union{ //--- bee wings (flying bee)

#local bz=10;
#while (bz<170)
#local bz=bz+10;
object{beewing scale <0.23,0.27,0.27>*1.4
rotate z*30
rotate x*-bz
translate<-0.65,0,-0.25>
}

object{beewing scale <0.23,0.27,0.27>*1.4
rotate z*30
rotate x*bz
translate<-0.65,0,0.25>
}
#end

#local beeyellow=pigment{
checker rgb<1,0.8,0.15>, rgb<0.6,0.5,0.175> scale 0.4 turbulence 5}
}

#local beekoerper=union{

intersection{   //--- abdomen ---
sphere{<0,0,0>,1}
sphere{<1,0,0>,1.8}

pigment{gradient y
 pigment_map{
 [0 rgb<0.3,0.3,0.3>]
 [0.0 rgb<0.3,0.3,0.3>]
 [0.3 rgb<1,0.8,0.15>*0.7]
 [0.5
gradient x
 pigment_map{
  [0 rgb<0.2,0.2,0.2>*1.5]
  [0.35 rgb<0.2,0.2,0.2>]
  [0.5 beeyellow]//rgb<1,0.8,0.15>]
  [0.8 beeyellow]//rgb<1,0.8,0.15>]
  [0.9 rgb<0.2,0.2,0.2>*1.5]
  }
  translate -4.4
  scale 0.375
  turbulence 0.1]
  }
  translate<0,-0.4,0>
  scale<1,2,1>
  }

finish{specular 0.5 roughness 0.015}
scale<1,0.45,0.45>*0.9
translate <0.4,0,0>
rotate z*-10
}


sphere{<0,0,0>,0.4   //--- body ---
scale <1.01,1,1>
pigment{checker rgb<1,0.8,0.15>, rgb<0.6,0.5,0.175>*0.7 turbulence 0.4 scale <0.1,0.1,0.1>*0.4 }
translate <-0.6,0,0>
}

sphere{<0,0,0>,0.4   //---- head ---
scale<0.45,1,0.95>
rotate z*-10
translate <-1.15,0.07,0>
pigment{checker rgb<1,0.8,0.4>*0.7, rgb<0.2,0.2,0.2> turbulence 0.4 scale <0.1,0.1,0.1>*0.4 }
}

sphere{<0,0,0>,0.35 //--- eye ---
scale<0.25,0.7,0.45>
rotate x*5
rotate z*-10
translate <-1.135,0.1,-0.3>
rotate x*10
pigment{rgb<0.2,0.2,0.2>}
finish{specular 1 roughness 0.015 }
}

sphere{<0,0,0>,0.35  //--- eye ---
scale<0.25,0.7,0.45>
rotate x*5
rotate z*-10
translate <-1.135,0.1,0.3>
rotate x*-10
finish{specular 1 roughness 0.015 }
}

union{  //--- legs ----

union{
cylinder{<0,0,0>,<0,0.5,0>,0.05}
cylinder{<0,0,0>,<0,0.5,0>,0.045  rotate z*-30 translate<0,0.5,0>}
cylinder{<0,0,0>,<0,0.4,0>,0.04 rotate z*20 translate<0,0.5,0> rotate z*-30 translate<0,0.5,0>}
rotate z*-100
rotate y*20
translate<-0.4,-0.26,-0.25>
}

union{
cylinder{<0,0,0>,<0,0.5,0>,0.05}
cylinder{<0,0,0>,<0,0.5,0>,0.045  rotate z*-30 translate<0,0.5,0>}
cylinder{<0,0,0>,<0,0.4,0>,0.04 rotate z*20 translate<0,0.5,0> rotate z*-30 translate<0,0.5,0>}
rotate z*-100
rotate y*-20
translate<-0.4,-0.26,0.25>
}

union{
cylinder{<0,0,0>,<0,0.45,0>,0.045}
cylinder{<0,0,0>,<0,0.4,0>,0.04 rotate z*-30 rotate y*-40  translate<0,0.45,0>}
cylinder{<0,0,0>,<0,0.4,0>,0.036 rotate z*20 translate<0,0.4,0> rotate z*-30 rotate y*-40 translate<0,0.45,0>}
rotate z*-110
rotate y*40
translate<-0.55,-0.26,-0.25>
}

union{
cylinder{<0,0,0>,<0,0.45,0>,0.045}
cylinder{<0,0,0>,<0,0.4,0>,0.04 rotate z*-30 rotate y*40  translate<0,0.45,0>}
cylinder{<0,0,0>,<0,0.4,0>,0.036 rotate z*20 translate<0,0.4,0> rotate z*-30 rotate y*40 translate<0,0.45,0>}
rotate z*-110
rotate y*-40
translate<-0.55,-0.26,0.25>
}

union{
cylinder{<0,0,0>,<0,0.35,0>,0.04}
cylinder{<0,0,0>,<0,0.35,0>,0.037 rotate z*70 translate<0,0.35,0>}
cylinder{<0,0,0>,<0,0.3,0>,0.034  rotate z*-30 translate <0,0.35,0> rotate z*70 translate <0,0.35,0>}
rotate z*90
rotate y*-30
translate<-0.7,-0.3,-0.25>
}

union{
cylinder{<0,0,0>,<0,0.35,0>,0.04}
cylinder{<0,0,0>,<0,0.35,0>,0.037 rotate z*70 translate<0,0.35,0>}
cylinder{<0,0,0>,<0,0.3,0>,0.034  rotate z*-30 translate <0,0.35,0> rotate z*70 translate <0,0.35,0>}
rotate z*90
rotate y*30
translate<-0.7,-0.3,0.25>
}

pigment{rgb<0.2,0.2,0.2>}
finish{specular 0.5 roughness 0.015}
}

union{  //--- feelers ---
cylinder{<0,0,0>,<0,0.5,0>,0.02}
cylinder{<0,0,0>,<0,0.7,0>,0.02 rotate z*70 rotate y*20 translate<0,0.5,0>}

rotate z*40
rotate y*-40
translate<-1.25,0.1,-0.1>
pigment{rgb<0.2,0.2,0.2>}
finish{specular 0.5 roughness 0.015}
}

union{
cylinder{<0,0,0>,<0,0.5,0>,0.02}
cylinder{<0,0,0>,<0,0.7,0>,0.02 rotate z*70 rotate y*-20 translate<0,0.5,0>}

rotate z*40
rotate y*40
translate<-1.25,0.1,0.1>
pigment{rgb<0.2,0.2,0.2>}
finish{specular 0.5 roughness 0.015}
}
}

//--- declaring flying bee:
// #declare bee=union{object{beewings} object{beekoerper}}  

//--- declaring sitting bee:
// #declare bee_sitting=union{object{beesitting_wing} object{beekoerper}}

//---------------------------------------------
//  Object declaration
//---------------------------------------------

#declare objet_toplace = 
object {
  union {
    object{beewings} 
    object{beekoerper}
  }  
 scale LOCAL_OBJECT_SCALE // additonnal scale to fit whit the terraform's scales
}

//---------------------------------------------
//  trace object include file
//---------------------------------------------

#if (RENDER_TRACE_OBJECT)
  #include "trace_objects.inc"
#end





