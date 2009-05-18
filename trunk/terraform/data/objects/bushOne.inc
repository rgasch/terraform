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

#local TF_BUSHONE_SCALE = 0.2;

#local LOCAL_BushStemText =   // BushStemText is the texture of the bush stem
texture {
  pigment { color <0,1,0> }
}

#local LOCAL_BushText =  // BushText is the leaf texture
texture {
  pigment { color <0,1,0> }
}

//---------------------------------------------
//  Object ans Macro declaration
//---------------------------------------------

#include "foliage_inc.txt"

#declare bush1_obj = 
  object{
    bushOne( LOCAL_BushStemText, LOCAL_BushText)
    translate -y * 0.2
    scale TF_BUSHONE_SCALE
  }//end 

#macro bushOne ()
   object { 
     bush1_obj  
   }
#end //macro



