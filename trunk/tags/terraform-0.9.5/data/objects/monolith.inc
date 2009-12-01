
#declare monolith_obj = box
{
  <-0.375, 0.0, -0.125>,
  <0.375, 1.0, 0.125>

  texture
  {
    pigment { color rgb <0.0, 0.0, 0.0> }
    finish { phong 0.8 reflection 0.35 }
  }
}

#macro monolith ()
   object { monolith_obj } 
#end
