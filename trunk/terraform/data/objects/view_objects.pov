// POV file to view the prev-supplied objets 
// written by RNG
//


// Include files
#include "colors.inc"
#include "skies.inc"
#include "textures.inc"


camera
{
	location	<0,  0.5, -7.5>	// <X Y Z>
	direction	2*z            	// which way are we looking <X Y Z>
	up		y              	// which way is +up <X Y Z>
	right		4*x         	// which way is +right <X Y Z> and aspect ratio
	look_at		<0, 0.05, 0> 	// point center of view at this point <X Y Z>
}

light_source { <-30, 30, -10> color red 0.75 green 0.5 blue 0.3 }
light_source { <-30, 21, 5>   color red 0.25 green 0.1 blue 0.0 }

sky_sphere { S_Cloud2 }

plane { y,-25
   pigment {
         checker  
         color Gold 
         color Firebrick
         scale 10 
	 }
   /*
   normal {
         waves 1
         frequency 10
         scale .1 
	 }
   */
   finish {
       ambient 0.8
       diffuse 0.8
       /* reflection 0.1 */
      }
}


// The heightfield object is in the X-Z plane, centered on the origin, 
// and extends +/- 0.5 units in the X and Z directions
//#include "tree_macro_1.inc"
#include "monolith.inc" 
object { monolith () translate <-6, 0, 0>}

#include "tree_1.inc"
object { tree_1 () translate <-4, 0, 0>}

#include "tree_2.inc"
object { tree_2 () translate <-2, 0, 0>}

#include "tree_3.inc" 
object { tree_3 () translate <0, 0, 0>}

#include "tree_4.inc" 
object { tree_4 () translate <2, 0, 0>}

#include "tree_5.inc" 
object { tree_5 () translate <4, 0, 0>}

#include "tree_6.inc" 
object { tree_6 () translate <6, 0, 0>}

