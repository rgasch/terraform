//=========================================
// Leaf & Tree examples for the MakeTree macro
// (c) 1999 Gilles Tran tran@inapg.inra.fr
//=========================================
// Distribution made in Terraform by Raymond Ostertag wih permission of Gilles Tran <gitran@wanadoo.fr>
//=========================================
// Classic tree example
//-----------------------------------------
// This file calls the MakeTree and MakeLeaf macros
// and defines the textures and parameters
// Set the [dofile] parameter to "true"
// when you want to create the final file
//-----------------------------------------
#local Tan = color red 0.858824 green 0.576471 blue 0.439216;
#local Clear   = rgbf 1;
#local White   = rgb 1;

//=========================================
#include "tree_parametric_texture_inc.txt"
#include "tree_parametric_macro_inc.txt"

//=========================================
// Tree macro Parameters
//-----------------------------------------
// These parameters must be declared before calling the tree macro
//-----------------------------------------
#local dofile=false;    // true creates a tree file ; false otherwise
#local dotexture=true;  // true creates a textured tree (with the texture following the branch); false otherwise
#local ftname="gttree1.inc" // file name for tree
#local fvname="gtfoliage1.inc" // file name for foliage
#local ffname="gtleaf1.inc" // file name for leaf
#local txtTree=texture{txtTree_3} // Bark texture

//-----------------------------------------
// Random streams
// one stream for branches and another one for leaves
// so that the leafed tree has the same structure as the leafless one
//-----------------------------------------
#local rsd=1431;      // random seed
#local rsd=1432;      // random seed
#local rsd=1433;      // random seed
#local rd=seed(rsd);  // random stream for branches
#local rdl=seed(rsd); // separate random stream for leaves

//-----------------------------------------
// Tree structure parameters        
// test with low level0 and nseg0 (3 or 4)
// High (>=6) recursion levels [level0] gives more complex trees
// High (>=6) segment numbers [nseg0] gives smoother trees
//-----------------------------------------
#local nseg0=12;       // initial number of branch segments (decreases of one at each level)
#local nb=3;          // max number of branches per level
#local dotop=true;   // if true, generates an extra branch on top of trunk (sometimes necessary to give more verticality)

#local lb0=25;        // initial branch length
#local rb0=1.4;         // initial branch radius
#local ab0=35;        // initial branch angle (x angle between the trunk and the first branch)
#local qlb=0.67;       // branch length decrease ratio (1=constant length)
#local qrb=0.59;       // branch radius decrease ratio (1=constant radius)
#local qab=0.88;       // branch angle decrease ratio (1=constant angle)
#local stdax=10;      // stdev of x angle (angle x = ax+(0.5-rand)*stdax)
#local stday=10;      // stdev of y angle (angle y = ay+(0.5-rand)*stday)

#local branchproba=1; // probability of branch apparition 
#local jb=0.7;        // secondary branches start after this ratio of branch length

#local fgnarl=0.25;    // gnarledness factor - keep it low <0.8
#local stdlseg=0.5;     // stddev of branch segment length (0...1) (adds more randomness to branch length)

#local twigproba=0.59; // probability to have a twig on a trunk segment

#local v0=<0,1,0>;    // initial direction - change to give an initial orientation
#local pos0=<0,0,0>;  // initial trunk position (no need to change this one)

//-----------------------------------------
// constraints parameters
//-----------------------------------------
#local vpush=<0,-1,0>;// direction of push (wind, gravity...) <0,-1,0> = gravity ; <0,1,0> = antigravity
#local fpush=0.2;       // force of push
#local aboveground=0; // constrains the branches above this level 
#local belowsky=140;  // constrains the branches below this level 

//-----------------------------------------
// root parameters
//-----------------------------------------
#local rootproba=1;   // probability of root 0=no root ; 1=all [nb] roots
#local nroot=5;      // number of roots;
#local vroot=<1,0,0>; // initial direction of root 
#local yroot=<0,0.5,0>;   // initial position of root above ground

//-----------------------------------------
// leaf position parameters
//-----------------------------------------
#local leafproba=0.8;   // probability of leaf 0=no leaf ; 1=leaf on each segment
#local leaflevel=4;   // level where the leaves start to appear
#local alz0=100;       // max z angle for leaf
#local alx0=-10;      // start x angle for leaf
#local stdalx=40;     // std x angle for leaf
#local stdlsize=0.1;     // stddev of leaf size 0=constant size; size = leafsize*(1+stdlsize*rand)

//-----------------------------------------
// leaf structure parameters
//-----------------------------------------
#local txtLeaf=texture{txtLeaf_0} // Leaf texture
#local lsize=0.3;     // leaf size
#local seg=10;        // nb of leaf segments and stalk segments : increase to smooth
#local ll=5;          // leaf length
#local wl=1.2;          // leaf width 
#local fl=0.5;        // depth of leaf fold
#local lpow=1;        // modifies the leaf shape : lpow=3 makes heart-shaped leaf
#local al=100;        // leaf bending angle : the leaf bends backward until this angle
#local apow=1;        // curve power, how slow the leaf bends
#local ndents=0;      // dents in the leaf (8 = "oak" leaf). May require a high seg >20
#local nlobes=3;      // number of lobes (individual leaves)
#local alobes=110;    // angle made by all the lobes
#local qlobes=0.7;    // size of the farest lobe (0.9 = size will be 0.9*leaf length)                               
#local ls=3;          // stalk length (0=no stalk)
#local ws=0.12;        // width of stalk
#local as=10;         // stalk bending angle : the stalk bends forward
//-----------------------------------------
// end of parameters
//=========================================

//=========================================
// Make the tree now !
//-----------------------------------------

//---------------------------------------------
//  Additonnal Parameters 
//---------------------------------------------

#local TF_OBJECTNAME_SCALE = 0.035;
#local TF_RECURSION_LEVEL = 6;

//---------------------------------------------
//  Object ans Macro declaration
//---------------------------------------------

#declare tree_parametric1_obj = 
  object{
        #if (leafproba>0)
        #local Leaf=object{MakeLeaf(lsize,seg,ll,wl,fl,lpow,al,apow,
                 ndents,nlobes,alobes,qlobes,ls,ws,as,dofile,ffname)
                 #if (dotexture=false)  // no texture applied to tree segments, so the leaf texture must be used for the individual leaf
                        texture{txtLeaf}
                 #end
        } 
        
        #end              
        MakeTree()
        
        #if (dotexture = true) // texture is already applied to tree so that we can safely apply the leaf texture to the leaves
                texture {txtLeaf}
        #else

                texture {txtTree} // apply tree texture  regardless of the tree structure
        #end
        scale TF_OBJECTNAME_SCALE
  }//end 

#macro tree_parametric_1 ()
   object { 
     tree_parametric1_obj
   }
#end //macro



