/*****************************************************************************
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
     scale<.1, .1, .1>
    }
   }
   hollow
}

#local Tann1 = object{
 cone{<0,0,0>,1,<0,1,0>,0 open
  texture{
   pigment{
    image_map{ 
      gif "tree_4.gif" 
      interpolate 2.0 
      transmit 0 1.0
      filter 236 0.5 
      filter 239 0.5 
    }
    rotate<90,0,0>translate<0.5,0.5,0.5>scale<2,2,2>
   }
  }
 }
 hollow
}

#declare Tree = object {
 union{
  object{Trunk scale<1,0.9,1>}
  object{Tann1 scale<6,2,6>rotate<0,0,0>translate<0,1.25,0>}
  object{Tann1 scale<5.5,2.2,5.5>rotate<0,120,0>translate<0,2,0>} #
  object{Tann1 scale<5,2.2,5>rotate<0,60,0>translate<0,3,0>} #
  object{Tann1 scale<4,2.2,4>rotate<0,180,0>translate<0,4,0>}
  object{Tann1 scale<3,2.2,3>rotate<0,300,0>translate<0,5,0>}
  object{Tann1 scale<2,2.2,2>rotate<0,240,0>translate<0,6,0>}
  object{Tann1 scale<2,2.2,2>rotate<0,45,0>translate<0,7,0>}
  object{Tann1 scale<1.5,1,1.5> rotate<0,90,0>translate<0,8,0>}
 }
}

#macro tree_4 ()
  object { Tree scale <0.1, 0.1, 0.1> }
#end 

