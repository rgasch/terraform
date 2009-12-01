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
// ----------------------------------------

//---------------------------------------------
//  Additonnal Parameters 
//---------------------------------------------

#local TF_OBJECTNAME_SCALE = 0.008; // scale applied to the object

// --------------------------------------
// Grass texture
// --------------------------------------

#local White = 1;
#local Tan = color red 0.858824 green 0.576471 blue 0.439216;
#local Blanc1=rgb<0.90,0.81,0.81>;
#local Green1=rgb<230,170,50>/255;
#local Green2=rgb<110,160,8>/255;

#local pigSpotted =pigment {
spotted
    color_map {
        [0.0, 0.2   color Tan*0.1 color Tan*0.4]
        [0.2, 0.3   color Tan*0.4 color Green1]
        [0.3, 1.01   color Green1 color Green1*1.2]
    }
}

#local pigEarth =pigment {
    spotted
    color_map {
        [0 color Tan*0.3]
        [0.6 color Tan*0.3]
        [0.6 color Green1*0.4]
        [1   color Green1*0.4]
    }
}
#local pigGreen=pigment{Green2*1.3}
#local txtGrass=texture {                  

                pigment {
                        gradient y
                        turbulence 0.2
                                pigment_map {                                
                                    [0.0 pigEarth]
                                    [0.3 pigGreen]
                                    [0.9 pigSpotted]
                                    [1.00 pigEarth]
                                }
                        }

        finish{ambient 0.01 diffuse 0.7 specular 0.2 roughness 0.015}
        scale <0.001,1,0.001>
 
} 
// --------------------------------------
// Patch parameters
// --------------------------------------
// Final number of triangles = nBlade x nBlade x segBlade x 2 (or 4 if dofold = true)
// --------------------------------------
#local lPatch=50;               // size of patch
//#local nBlade=4;              // number of blades per line (there will be nBlade x nBlade blades)
#local nBlade=10;               // number of blades per line (there will be nBlade x nBlade blades)
#local ryBlade = 0;             // initial y rotation of blade
#local segBlade= 15;            // number of blade segments
#local lBlade = 25;             // length of blade
#local wBlade = 1;              // width of blade at start
#local wBladeEnd = 0.3;         // width of blade at the end
#local doSmooth=false;          // true makes smooth triangles
#local startBend = <0,1,0.3>;   // bending of blade at start (<0,1,0>=no bending)
#local vBend = <0,-0.5,0>;      // force bending the blade (<0,1,1> = 45°)
#local pwBend = 3;              // bending power (how slowly the curve bends)
#local rd = 459;                // seed
#local stdposBlade = 1;         // standard deviation of blade position 0..1
#local stdrotBlade = 360;       // standard deviation of rotation
#local stdBlade = 1.2;          // standard deviation of blade scale;
#local stdBend = 2;             // standard deviation of blade bending;
#local dofold = true;           // true creates a central fold in the blade (twice more triangles)
#local dofile = false;          // true creates a mesh file
#local fname = "fgrass1.inc"    // name of the mesh file to create

// --------------------------------------
// Prairie parameters
// --------------------------------------
#local nxPrairie=1;             // number of patches for the first line
#local addPatches=1;            // number of patches to add at each line
#local nzPrairie=1;             // number of lines of patches
#local rd=seed(779);            // random seed
#local stdscale=1;              // stddev of scale
#local stdrotate=1;             // stddev of rotation
#local doTest=false;            // replaces the patch with a sphere

//---------------------------------------------
//  Object ans Macro declaration
//---------------------------------------------

#include "grass_prairie_inc.txt"

#declare objectname_obj = 
  #local objectPatch=object{MakeGrassPatch(lPatch,nBlade,ryBlade,segBlade,lBlade,wBlade,wBladeEnd,doSmooth,startBend,vBend,pwBend,rd,stdposBlade,stdrotBlade,stdBlade,stdBend,dofold,dofile,fname)}
  object{
    MakePrairie(lPatch,nxPrairie,addPatches,nzPrairie,objectPatch,rd,stdscale,stdrotate,doTest) 
    texture{txtGrass scale 40}
    scale TF_OBJECTNAME_SCALE
  }//end 


#macro grass_prairie ()
   object { 
     objectname_obj
   }
#end //macro




