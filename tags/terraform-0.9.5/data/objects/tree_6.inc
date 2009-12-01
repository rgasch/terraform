/*****************************************************************************
    The Grantrae was designed by Stig Bachmann Nielsen.
      <http://www.isa.dknet.dk/~stigbn/ray/raytrace.html>
    The Genesis Toolkit is freeware and copyright by Joerg Schrammel
      <http://www.rhein-ruhr.de/~schrammel/index.htm>
 ****************************************************************************/


#local Trunk = object{
   cone{<0,0,0>,0.3,<0,10,0>,0}
   texture{
    pigment{ color rgb<0.501, 0.313, 0.184>}
    normal{
     bumps
     bump_size -2
     scale< .1, .1, .1>
    }
   }
   hollow
}

#local Gran = object{
 cone{<0,0,0>,1,<0,1,0>,0 open
  texture{
   pigment{
    image_map{ gif "tree_6.gif" interpolate 2.0 transmit 0 1.0 }
    rotate<90,0,0>
    translate<0.5,0.5,0.5>
    scale<2,2,2>
   }
  }
 }
 hollow
}

#declare GranTree = object{
 union{
  object{Trunk } 
  object{Gran scale<3.1,2.9,3.1> rotate<0,180,0>  translate <0,1,0>}
  object{Gran scale<2,1.7,2>     rotate<0,120,0>  translate <0,1.9,0>}
  object{Gran scale<3.1,2.9,3.1> rotate<0,90,0>   translate <0,2.5,0>}
  object{Gran scale<2.5,2.4,2.5> rotate<0,0,0>    translate <0,4,0>}
  object{Gran scale<2.1,1.9,2.1> rotate<0,120,0>  translate <0,5.3,0>}
  object{Gran scale<1.7,1.5,1.7> rotate<0,180,0>  translate <0,6.5,0>}
  object{Gran scale<1.3,1,1.3>   rotate<0,90,0>   translate <0,7.5,0>}
 }
}


#macro tree_6 ()
  object { GranTree scale <0.1, 0.1, 0.1> }
#end
