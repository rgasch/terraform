// the original terraform tree include file

#declare T_Bark = texture
{
  wood
  ramp_wave
  rotate x*90
  turbulence 0.2
  omega 0.4
  lambda 0.1

  texture_map
  {
    [0.0 pigment { rgb<0.2, 0.1, 0.05> } finish { roughness 0.8 specular 0.1} normal { bumps 0.8 scale 0.1 } ]
    [0.3 pigment { rgb<0.7, 0.5, 0.4> } finish { roughness 0.2 specular 0.2} normal { bumps 0.5 scale 0.1 } ]
    [1.0 pigment { rgb<0.5, 0.3, 0.2> } finish { roughness 0.5 specular 0.1} normal { bumps 0.3 scale 0.1 } ]
  }

  scale 0.1*(x+z) + 0.5*y
}

#declare T_Leaf = texture
{
  pigment
  {
    gradient x
    ramp_wave
    turbulence 0.8
    scale 0.5

    pigment_map
    {
      [0.0 color rgb <0.2, 0.5, 0.1>]
      [1.0 color rgb <0.1, 0.3, 0.02>]
    }
  }

  finish
  {
    roughness 0.7
    specular 0.2
  }

  normal
  {
    bumps 0.2
    scale 0.05
  }
}

#declare Leaf = union
{
  object {
    bicubic_patch { type 1 flatness 0.0100 u_steps 3 v_steps 3,
      <-0.119204, -3.027334, 0.000000>, <-0.061428, -3.016098, 0.000000>, <0.000576, -3.008185, 0.000000>, <0.038313, -3.007641, 0.000000>,
      <-0.238233, -2.009632, 0.222238>, <-0.151241, -1.972848, 0.000000>, <-0.072061, -1.960797, 0.018520>, <-0.011361, -1.966233, 0.009260>,
      <-0.392896, -1.894214, 0.312221>, <-0.316911, -1.785598, 0.000000>, <-0.170824, -1.670329, 0.000000>, <-0.033515, -1.675770, 0.000000>,
      <-1.462000, -0.831614, 0.341309>, <-0.994705, -0.569658, 0.166678>, <-0.554896, -0.454494, 0.231498>, <-0.091166, -0.447439, 0.180190>
    }
  }
  object {
    bicubic_patch { type 1 flatness 0.0100 u_steps 3 v_steps 3,
      <-1.462000, -0.831614, 0.341309>, <-0.994705, -0.569658, 0.166678>, <-0.554896, -0.454494, 0.231498>, <-0.091166, -0.447439, 0.180190>,
      <-2.531105, 0.230986, 0.370396>, <-1.672499, 0.646282, 0.333357>, <-0.938967, 0.761341, 0.462996>, <-0.148816, 0.780891, 0.360379>,
      <-2.669057, 1.751447, 0.163283>, <-1.758298, 1.901119, 0.242403>, <-0.944441, 2.000000, 0.388916>, <-0.260026, 1.996405, 0.362808>,
      <-0.666441, 3.450801, 0.000000>, <-0.594561, 3.424284, 0.000000>, <-0.655270, 3.424284, 0.000000>, <-0.517096, 3.437542, 0.000000>
    }
  }
  object {
    bicubic_patch { type 1 flatness 0.0100 u_steps 3 v_steps 3,
      <0.038313, -3.007641, 0.000000>, <0.076050, -3.007097, 0.000000>, <0.114767, -3.006735, 0.000000>, <0.156023, -3.004047, 0.000000>,
      <-0.011361, -1.966233, 0.009260>, <0.049340, -1.971669, 0.000000>, <0.126759, -1.989037, 0.000000>, <0.223836, -2.016147, 0.000000>,
      <-0.033515, -1.675770, 0.000000>, <0.103794, -1.681211, 0.000000>, <0.214447, -1.756154, 0.000000>, <0.282709, -1.897244, 0.000000>,
      <-0.091166, -0.447439, 0.180190>, <0.372564, -0.440385, 0.128882>, <0.786537, -0.675997, 0.199691>, <1.164742, -0.973546, 0.235690>
    }
  }
  object {
    bicubic_patch { type 1 flatness 0.0100 u_steps 3 v_steps 3,
      <-0.091166, -0.447439, 0.180190>, <0.372564, -0.440385, 0.128882>, <0.786537, -0.675997, 0.199691>, <1.164742, -0.973546, 0.235690>,
      <-0.148816, 0.780891, 0.360379>, <0.641335, 0.800441, 0.257763>, <1.358628, 0.404159, 0.399381>, <2.046775, -0.049848, 0.471380>,
      <-0.260026, 1.996405, 0.362808>, <0.424388, 1.992810, 0.336700>, <1.309806, 1.748407, 0.389070>, <2.106980, 1.365449, 0.438779>,
      <-0.517096, 3.437542, 0.000000>, <-0.378921, 3.450801, 0.000000>, <-0.413113, 3.371248, 0.000000>, <0.992952, 2.573777, 0.135891>
    }
  }

  texture
  {
    T_Leaf
  }

  translate  <-0.251852, 3.068687, 0.0>
  scale <1/6.0, 1/3.0, 1.8>
}

#declare Twig = union
{
  cone
  {
    <0, 0, 0>, 0.3
    <0, 3, 0>, 0.0

    texture
    {
      T_Bark
    }
  }

  object
  {
    Leaf

    rotate -92.0*z
    rotate 5*x
    translate 0.35*y
  }

  object
  {
    Leaf

    rotate -95.0*z
    rotate 12*x
    translate 1.43*y
  }

  object
  {
    Leaf

    rotate -93.0*z
    rotate 7*x
    translate 2.33*y
  }

  object
  {
    Leaf

    rotate 89.0*z
    rotate 4*x
    translate 0.58*y
  }

  object
  {
    Leaf

    rotate 87.0*z
    rotate 9*x
    translate 1.61*y
  }

  object
  {
    Leaf

    rotate 94.0*z
    rotate 11*x
    translate 2.18*y
  }

  object
  {
    Leaf

    scale <2,0.5,2>
    translate 3*y
  }

  scale <1/0.3, 1/3.0, 1/0.3>
}


#declare tree_seed = seed (0);

#macro tree_calc (value, factor, _jitter)
  (value * factor * (1.0 + (rand (tree_seed) - 0.5) * _jitter))
#end

#macro tree_branches (split_depth, _depth, split, split_percent,
                      thick, thick_factor, thick_jitter,
                      branch_factor, branch_jitter,
                      _angle,
                      _size, size_factor, size_jitter,
                      swirl_angle)
  #if (split_depth > 0)
    #if (rand (tree_seed) <= split_percent)
      object {
        tree (_depth - 1, split, split_percent,
              tree_calc (thick, thick_factor, thick_jitter),
              thick_factor, thick_jitter,
              branch_factor, branch_jitter,
              _angle,
              tree_calc (_size, size_factor, size_jitter),
              size_factor, size_jitter,
              swirl_angle)

        rotate x * _angle
        rotate y * (split_depth * 360.0 / split + swirl_angle)
        translate y * tree_calc (_size, branch_factor, branch_jitter)
      }
    #end

    tree_branches (split_depth - 1, _depth, split, split_percent,
                   thick, thick_factor, thick_jitter,
                   branch_factor, branch_jitter,
                   _angle,
                   _size, size_factor, size_jitter,
                   swirl_angle)
  #end
#end

#macro tree (_depth, split, split_percent,
             thick, thick_factor, thick_jitter,
             branch_factor, branch_jitter,
             _angle,
             _size, size_factor, size_jitter,
             swirl_angle)
  #if (_depth > 0)
    union {
      tree_branches (split,
                     _depth, split, split_percent,
                     thick, thick_factor, thick_jitter,
                     branch_factor, branch_jitter,
                     _angle,
                     _size, size_factor, size_jitter,
                     swirl_angle)

      object {
        cone { <0, 0, 0>, thick <0, _size, 0>, thick * thick_factor }
        texture { T_Bark }
      }
    }
  #else
    object { Twig scale <thick, _size, thick> }
  #end
#end

#macro tree_2 ()
  tree (4 /* Increase this to 6 to get a fuller tree,
             but be aware of the memory usage! */,
    3, 0.9, 0.6, 0.7, 0.0, 0.95, 0.0, 30.0, 3.0, 0.9, 0.2, 15.0)
    scale <0.1, 0.1, 0.1> 
#end

