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

#local TF_BUSHES_SCALE = 0.2;

#local LOCAL_BushText =  // BushText is what the texture is of the bush leaves
texture {
  pigment { color <0,1,0> }
}

#local LOCAL_Wide = 3;  // Wide is how many bushes along +X axis
#local LOCAL_Deep = 3;  // Deep is how many bushes along +Z axis 
#local LOCAL_Dense = 1; // Dense is how close each bush is to one another


//---------------------------------------------
//  Object ans Macro declaration
//---------------------------------------------

#include "foliage_inc.txt"

#declare bushes_obj = 
  object{
    bushes(LOCAL_Wide, LOCAL_Deep, LOCAL_Dense, LOCAL_BushText)
    scale TF_BUSHES_SCALE
  }//end 

#macro bushes ()
   object { 
     bushes_obj  
   }
#end //macro

