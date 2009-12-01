// Copyright (c) 2002 Raymond Ostertag based on Mick's work
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


//---------------------------------------------
//  Additonnal Parameters 
//---------------------------------------------

#declare LOCAL_GRASS_SCALE = <1.5, 0.7, 1.5> * 0.01;
#declare LOCAL_GRASS_PATCHSIZE = 2; //increasing this value make the render very slow
#declare LOCAL_GRASS_COLORATION = <1.0, 1.0, 1.0> * 1.0;

//---------------------------------------------
// Based on Mick' grass texture
//---------------------------------------------

#declare GrassTex = texture{
    pigment {
    granite
    color_map {
      [0.0  color rgb <0.70, 0.80, 0.40> * LOCAL_GRASS_COLORATION ]
      [0.25 color rgb <0.80, 0.99, 0.60> * LOCAL_GRASS_COLORATION ]
      [0.5  color rgb <0.90, 0.95, 0.75> * LOCAL_GRASS_COLORATION ]
      [0.75 color rgb <0.95, 0.99, 0.90> * LOCAL_GRASS_COLORATION ]
    }
    scale 0.00125
    }
  }
    
#declare R = seed(3243);
 
#declare blade = mesh {//creates a single blade of grass

   #declare No_Segs = 10;
   #declare C = 0;
   #declare Seg_length = .35;
   #declare Rot = 90-(15+(rand(R)*20));//controls the bend of the grass
   #declare Ox = 0;
   #declare Oy =0;
   #declare Nx = Seg_length;
   #declare Ny = Seg_length;
   #declare Z= 0.15;  
  
   #while (C<No_Segs)

                #declare xdist = sin(radians(90-Rot))*Seg_length;
                #declare ydist = cos(radians(90-Rot))*Seg_length;
                   #declare No_Segs = 10+(rand(R)*10); 
                    #declare Nx = Ox+xdist; 
                    #declare Ny = Oy+ydist;
                    
                    
               #if (C<(No_Segs-1))
                 triangle{<Ox,Oy,Z> <Ox,Oy,-Z> <Nx,Ny,Z> }
                 triangle{<Ox,Oy,-Z> <Nx,Ny,Z> <Nx,Ny,-Z>}
               #else
                 triangle{<Ox,Oy,Z> <Ox,Oy,-Z> <Nx,Ny,0>}
               #end           
                  #declare Rot = Rot-pow((rand(R)),2);
                  #declare Ox = Nx;
                  #declare Oy = Ny;
                  
                #declare Z=Z-0.012;
                #declare Seg_length = Seg_length +.01;   
                #declare C=C+1;
   #end
   //translate y*.05
   texture{GrassTex}
}//end mesh    

#declare Hor = -5*LOCAL_GRASS_PATCHSIZE;
#declare Ver = -5*LOCAL_GRASS_PATCHSIZE;
  
#declare grass_patch_obj = union{// creates a patch of grass

    #while (Hor < 5*LOCAL_GRASS_PATCHSIZE)
       #while (Ver < 5*LOCAL_GRASS_PATCHSIZE)
        object{blade rotate y*-((Ver*20)*rand(R)*120)
          scale y *1+rand(R)
          translate<Hor/3,0,Ver/3>
        }
        #declare Ver = Ver +1;
        #end
      #declare Ver= -5*LOCAL_GRASS_PATCHSIZE;
      #declare Hor = Hor+1;   
    #end               

}//end of union

#macro grass_patch_small ()
   object { 
     grass_patch_obj 
     scale LOCAL_GRASS_SCALE
   }
   
#end