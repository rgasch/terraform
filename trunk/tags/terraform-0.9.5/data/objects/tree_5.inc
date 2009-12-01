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
     scale< .1, .1, .1>
    }
   }
   hollow
}

#local Tann2 = object{
  cone{
   <0,0.5,0>,4,<0,4.5,0>,0 open
   texture{
    pigment{
     image_map{
      gif "tree_5.gif"
      map_type 2
      interpolate 2.0
      filter  0 1.0     filter  1 0.9     filter  2 0.8     filter  3 0.7
      filter  4 0.6     filter  5 0.5     filter  6 0.4     filter  7 0.3
      filter  8 0.3     filter  9 0.3     filter 10 0.2     filter 11 0.2
      filter 12 0.2     filter 13 0.1     filter 14 0.1     filter 15 0.0
     }
     scale<1,1,1>
    }
   }
  }
}

#declare Tree = object {
 union{
  object{Trunk scale<1,0.9,1>}
  object{Tann2 scale<1,2,1>}
 }
}

#macro tree_5 ()
  object { Tree scale <0.1, 0.1, 0.1> }
#end
