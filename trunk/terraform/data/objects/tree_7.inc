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

#local TF_TREE7_SCALE = 2.5;

//---------------------------------------------
//  Object ans Macro declaration
//---------------------------------------------

#include "tree7_inc.txt"

#declare tree_7_obj = 
  object{
    tree7
    scale TF_TREE7_SCALE
  }//end 

#macro tree_7 ()
   object { 
     tree_7_obj
   }
#end //macro







