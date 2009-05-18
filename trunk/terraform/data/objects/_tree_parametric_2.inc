//=========================================
// Leaf & Tree examples for the MakeTree macro
// (c) 1999 Gilles Tran tran@inapg.inra.fr
//=========================================
// Distribution made in Terraform by Raymond Ostertag wih permission of Gilles Tran <gitran@wanadoo.fr>
//=========================================
// Big top tree example
//-----------------------------------------
// This file calls the MakeTree and MakeLeaf macros
// and defines the textures and parameters
// Set the [dofile] parameter to "true"
// when you want to create the final file
//-----------------------------------------

//---------------------------------------------
// Parameters 
//---------------------------------------------

#ifndef (TF_TRACE_OBJECT_X)
   #local LOCAL_EYE_POINT = < 0.5, 10, 0.05 >; 
#else
   #local LOCAL_EYE_POINT = < TF_TRACE_OBJECT_X, TF_TRACE_OBJECT_Y, TF_TRACE_OBJECT_Z > ; 
#end
// coordinates are like in the camera dialog 
// X=0,Z=0 and X=1,Z=1 are the bounding of the terrain
// if X and Z coordinates are in the bounding then the object is placed on the terrain (Y must be greater than the height of the terrain)
// if X or Z coordinates are out of the bounding then the object is placed on the water

#ifndef (TF_TRACE_OBJECT_RELATIV)
   #local LOCAL_CAMERA_RELATIV = false;
#else
   #local LOCAL_CAMERA_RELATIV = TF_TRACE_OBJECT_RELATIV;
#end
// if this boolean is true the coordinate are relativ to the camera location instead of the origin of the HF
// if the eye_point is over the terrain the object is placed on the terrain, if it's over the water then the object is placed on the water

#ifndef (TF_TRACE_OBJECT_CONSERVE)
   #local LOCAL_CONSERVE_ELEVATION = false;
#else
   #local LOCAL_CONSERVE_ELEVATION = TF_TRACE_OBJECT_CONSERVE;
#end
// if this boolean is true the object is placed in the air (for a plane) or in the water (for a submarine) at the coordinates of the eye_point. 

#ifndef (TF_TRACE_OBJECT_FLOAT)
   #local LOCAL_OBJECT_FLOAT = false;
#else
   #local LOCAL_OBJECT_FLOAT = TF_TRACE_OBJECT_FLOAT;
#end
// if this boolean is false the object can be on earth but under the water.

#ifndef (TF_TRACE_OBJECT_SCALE)
   #local LOCAL_OBJECT_SCALE = 0.035;
#else
   #local LOCAL_OBJECT_SCALE = TF_TRACE_OBJECT_SCALE;
#end

#local TF_RECURSION_LEVEL = 4;
// recursion level of the tree

//=========================================
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
#local ftname="gttree2.inc" // file name for tree
#local fvname="gtfoliage2.inc" // file name for foliage
#local ffname="gtleaf2.inc" // file name for leaf
#local txtTree=texture{txtTree_6} // Bark texture

//-----------------------------------------
// Random streams
// one stream for branches and another one for leaves
// so that the leafed tree has the same structure as the leafless one
//-----------------------------------------
#local rsd=211;      // random seed
//#local rsd=212;      // random seed
//#local rsd=213;      // random seed
#local rd=seed(rsd);  // random stream for branches
#local rdl=seed(rsd); // separate random stream for leaves

//-----------------------------------------
// Tree structure parameters        
// test with low level0 and nseg0 (3 or 4)
// High (>=6) recursion levels [level0] gives more complex trees
// High (>=6) segment numbers [nseg0] gives smoother trees
//-----------------------------------------
#local nseg0=10;       // initial number of branch segments (decreases of one at each level)
#local nb=7;          // max number of branches per level
#local dotop=false;   // if true, generates an extra branch on top of trunk (sometimes necessary to give more verticality)

#local lb0=35;        // initial branch length
#local rb0=2;         // initial branch radius
#local ab0=95;        // initial branch angle (x angle between the trunk and the first branch)
#local qlb=0.6;       // branch length decrease ratio (1=constant length)
#local qrb=0.4;       // branch radius decrease ratio (1=constant radius)
#local qab=0.7;       // branch angle decrease ratio (1=constant angle)
#local stdax=10;      // stdev of x angle (angle x = ax+(0.5-rand)*stdax)
#local stday=10;      // stdev of y angle (angle y = ay+(0.5-rand)*stday)

#local branchproba=1; // probability of branch apparition 
#local jb=0.5;        // secondary branches start after this ratio of branch length

#local fgnarl=0.4;    // gnarledness factor - keep it low <0.8
#local stdlseg=0;     // stddev of branch segment length (0...1) (adds more randomness to branch length)

#local twigproba=0.5; // probability to have a twig on a trunk segment

#local v0=<0,1,0>;    // initial direction - change to give an initial orientation
#local pos0=<0,0,0>;  // initial trunk position (no need to change this one)

//-----------------------------------------
// constraints parameters
//-----------------------------------------
#local vpush=<0,0.2,0>;// direction of push (wind, gravity...) <0,-1,0> = gravity ; <0,1,0> = antigravity
#local fpush=1;       // force of push
#local aboveground=4; // constrains the branches above this level 
#local belowsky=140;  // constrains the branches below this level 

//-----------------------------------------
// root parameters
//-----------------------------------------
#local rootproba=1;   // probability of root 0=no root ; 1=all [nb] roots
#local nroot=5;      // number of main roots;
#local vroot=<1,-0.1,0>; // initial direction of root 
#local yroot=<0,1.5,0>;   // initial position of root above ground

//-----------------------------------------
// leaf position parameters
//-----------------------------------------
#local leafproba=1;   // probability of leaf 0=no leaf ; 1=leaf on each segment
#local leaflevel=4;   // level where the leaves start to appear
#local alz0=40;       // max z angle for leaf
#local alx0=-20;      // start x angle for leaf
#local stdalx=40;     // std x angle for leaf
#local stdlsize=0.5;     // stddev of leaf size 0=constant size; size = leafsize*(1+stdlsize*rand)

//-----------------------------------------
// leaf structure parameters
//-----------------------------------------
#local txtLeaf=texture{txtLeaf_4} // Leaf texture
#local lsize=0.4;     // leaf size
#local seg=10;        // nb of leaf segments and stalk segments : increase to smooth
#local ll=5;          // leaf length
#local wl=1;          // leaf width 
#local fl=0.5;        // depth of leaf fold
#local lpow=2;        // modifies the leaf shape : lpow=3 makes heart-shaped leaf
#local al=100;        // leaf bending angle : the leaf bends backward until this angle
#local apow=1;        // curve power, how slow the leaf bends
#local ndents=0;      // dents in the leaf (8 = "oak" leaf). May require a high seg >20
#local nlobes=7;      // number of lobes (individual leaves)
#local alobes=280;    // angle made by all the lobes
#local qlobes=0.6;    // size of the farest lobe (0.9 = size will be 0.9*leaf length)                               
#local ls=3;          // stalk length (0=no stalk)
#local ws=0.1;        // width of stalk
#local as=10;         // stalk bending angle : the stalk bends forward
//-----------------------------------------
// end of parameters
//=========================================

//---------------------------------------------
//  Object declaration
//---------------------------------------------

#declare objet_toplace = 
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
        scale LOCAL_OBJECT_SCALE
  }//end 

//---------------------------------------------
//  trace object include file
//---------------------------------------------
#if (RENDER_TRACE_OBJECT)
  #include "trace_objects.inc"
#end






