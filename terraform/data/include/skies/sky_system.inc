// Copyright (c) 2003 Raymond Ostertag
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
//

// -----------------------
// Main Sky system file. 
// -----------------------

#if (TF_LIGHTS_SYSTEM=0) //dynamic
   #declare TF_SHOW_CELEST_OBJECTS_LIGHTS = true ; // use Lights of sun and moon
#else
   #if (TF_LIGHTS_SYSTEM=1) //static
      #include TF_LIGHTS_DESCRIPTION
      #declare TF_SHOW_CELEST_OBJECTS_LIGHTS = false ; // never use Lights of sun and moon
   #else
      #declare TF_SHOW_CELEST_OBJECTS_LIGTHS = true ; // you should'nt go there
   #end 
#end

#if (TF_SKY_SYSTEM=0) //regular sky
   #include "earth_regular_sky.inc"
#else
   #if (TF_SKY_SYSTEM=1) //mapped sky
      #include "earth_regular_sky.inc"
      #include "mapped_sky.inc"
    #else
      #include "earth_regular_sky.inc" // you should'nt go there
   #end 
#end

