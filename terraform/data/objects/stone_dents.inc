// Copyright (c) 2002 Raymond Ostertag based on Gille Tran's work
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

#local LOCAL_STONE_COLORATION_1 = <1.0, 1.0, 1.0> * 0.8;
#local LOCAL_STONE_COLORATION_2 = <1.0, 1.0, 1.0> * 0.5;

//---------------------------------------------
// Based on a Gille Tran script
// http://www.oyonale.com/
//---------------------------------------------

// ----------------------------------------
// Max_gradient
// This parameter may need to be changed if some isosurfaces look strange (or don't appear at all)
// ----------------------------------------
#local mxg=8; 

#local isoScale=4;      // scale of pigment modifier (higher values=smaller scale)
#local isoStrength=0.4;     // strength of pigment modifie (how much the shape is deformed)
#local isoThreshold=0.5;  // threshold (radius of the sphere without deformation)
#local isoBound=2.5;        // bounding sphere (should be larger than isoThreshold)
#local isoTurb=0;       // turbulence of the pigment modifier
#local isoPig=pigment {dents turbulence isoTurb color_map{[0  White][1  Black]}}
#local isoFunc=function{ pigment {isoPig}}
#local isoSphere_dents=isosurface{
        function{x*x+y*y+z*z -isoFunc(x*isoScale,y*isoScale,z*isoScale).gray*isoStrength}
        contained_by {box{-isoBound,isoBound}}
        max_gradient mxg
 
         threshold isoThreshold
}

#local StoneTex=
  texture{
    pigment{
      onion 
      color_map{
        [0 color rgb LOCAL_STONE_COLORATION_1]
        [1 color rgb LOCAL_STONE_COLORATION_2]
      }
    } 
    finish{
      ambient 0.3 
      specular 0.1 
      roughness 0.04
    }
  }

#declare stone_dents_obj = 
  object{
    isoSphere_dents 
    texture{
      StoneTex
    }
  }//end of stone_obj

#macro stone_dents ()
   object { 
     stone_dents_obj 
   }
#end //macro










