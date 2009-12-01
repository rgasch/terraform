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

#local TF_PALMTREE1_SCALE = 0.4;

#local LOCAL_Wide = 2;  // Wide is how many bushes along +X axis
#local LOCAL_Deep = 2;  // Deep is how many bushes along +Z axis 
#local LOCAL_Dense = 6; // Dense is how close each bush is to one another

#local LOCAL_TrunkText =  // the tree's trunk texture
texture {
  pigment {
    wood
    turbulence 0.6
    color_map {
        [0.0 rgb <0.93, 0.71, 0.53>]
        [0.8 rgb <0.98, 0.81, 0.60>]
        [0.8 rgb <0.60, 0.33, 0.27>]
        [1.0 rgb <0.70, 0.60, 0.23>]
    }
  }
}

#local LOCAL_LeafText =  // what the texture is of the trees leaves
texture {
  pigment { color <0.4,0.6,0.1> }
}

#local LOCAL_LeafEdge = // what the texture is of the leaves edge ** required
texture {
  pigment { color <0.4,0.6,0.1> }
}

//---------------------------------------------
//  Object ans Macro declaration
//---------------------------------------------

#include "foliage_inc.txt"

#declare palmtree_patch_obj = 
  object{
    palmTrees(LOCAL_Wide, LOCAL_Deep, LOCAL_Dense, LOCAL_TrunkText, LOCAL_LeafText, LOCAL_LeafEdge )
    scale TF_PALMTREE1_SCALE
  }//end 

#macro tree_palm_patch ()
   object { 
     palmtree_patch_obj  
   }
#end //macro







