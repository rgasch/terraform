#declare RIVER_LANDSCAPE_TEXTURE = 
texture
{
    pigment
    {
      color rgb TF_WATER_COLOR transmit TF_WATER_CLARITY
    }

    finish
    {
      ambient 0.15
      brilliance 0.9
      specular 0.4
      reflection TF_WATER_REFLECTION
      diffuse    0.3
      roughness  0.01
      metallic 0.0
    }

    scale 10.0
    translate <0, -10, 0>

    scale (1.0 - TF_WATER_LEVEL) / (1.0 - 0.24)
    translate <0.0, 1.0, 0.0>

    normal
    {
      average
      normal_map
      {
        [1.0 ripples 0.20  frequency 1300.0 phase 720.0 * clock]
        [1.0 ripples 0.30  frequency  343.0 phase 720.0 * clock]
      }
    }
}
