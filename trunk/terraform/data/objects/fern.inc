/*****************************************************************************
 This taken from: 
    The Genesis Toolkit is freeware and copyright by Joerg Schrammel
      <http://www.rhein-ruhr.de/~schrammel/index.htm>
 ****************************************************************************/


#local FB = triangle{ <-0.125,0,0>,<0,0.5,0>,<0.125,0,0>
 texture{
  pigment{
     image_map{
       gif "fern.gif"
       interpolate 2.0
       filter 0 1.0
       filter 3 0.5
       filter 8 0.5
     }
     translate<0.5,0,0>
     scale<0.2,0.5,0.5>
  }
 }
 hollow
}

#declare FernObj = object {
 union{
  object{FB rotate<20,0,0>rotate<0,70,0>}
  object{FB rotate<20,0,0>rotate<0,20,0>}
  object{FB rotate<20,0,0>rotate<0,-50,0>}
  object{FB rotate<50,0,0>rotate<0,170,0>}
  object{FB rotate<50,0,0>rotate<0,120,0>}
  object{FB rotate<50,0,0>rotate<0,-150,0>}
 }
}

#macro fern () 
  object { FernObj } 
#end

