
//---------------------------------------------
// Terrain Template for Terraform
// Save this file as earth_myterrain.inc in
// the directory ~/.terraform/earth_textures/
//---------------------------------------------

//---------------------------------------------
//  Standard Parameters 
// (see defaults.inc )
//---------------------------------------------
// TF_LANDSCAPE_DISTRIBUTION
// in the dialog box : TEXTURE SPREADING
// TF_LANDSCAPE_TURBULENCE
// in the dialog box : TURBULENCE
// TF_WATER_BOTTOMCOLOR
// in the dialog box : no

//---------------------------------------------
// Basic Textures
// Textures : BTEX_Rock, BTEX_Rock_stratum, BTEX_Grass, BTEX_Snow, BTEX_Grasstex,
//---------------------------------------------
#include "basic_textures.inc"  


//---------------------------------------------
//  Additionnal Parameters 
// (put here your own parameters prefixed by LOCAL_)
//---------------------------------------------
#local LOCAL_FILTER = 0.5 ;

//---------------------------------------------
//  Terrain texture declaration
//---------------------------------------------

#local BTEX_P_Grass =
pigment {
			wrinkles
			color_map {
				[ 0.000000  color BTEX_GRASS_COLOR * 1.05 ]
				[ 1.000000  color BTEX_GRASS_COLOR * 1.05 * ( 1 - ( BTEX_GRASS_GRAIN / 5)) ]
			}
   turbulence 0.5
   octaves 20
   lambda 5
   omega 1.0
}  

#local BTEX_P_Grass2 =
pigment {
			wrinkles
			color_map {
				[ 0.000000  color BTEX_GRASS_COLOR * 0.3 ]
				[ 1.000000  color BTEX_GRASS_COLOR * 0.3 * ( 1 - ( BTEX_GRASS_GRAIN / 5)) ]
			}
   turbulence 0.5
   octaves 20
   lambda 5
   omega 1.0
}  


#local BTEX_P_Sand =
pigment {
			granite
			color_map {
				[ 0.02  color BTEX_SAND_COLOR * 1.3 ]
				[ 0.20  color BTEX_SAND_COLOR * 1.3 ]
				[ 0.85  color BTEX_SAND_COLOR * 1.0 ]
				[ 1.00  color BTEX_SAND_COLOR * 0.5 ]
			}
}

#local LOCAL_BEACH_TEXTURE =
  texture {
    BTEX_Rock
  }
  texture {
    pigment {
      slope {
        y, 0.5, 1.0            
        altitude -y, 1.0, 0.0        
      }
      pigment_map {
        [0.00 color BTEX_SAND_COLOR filter LOCAL_FILTER transmit 1] // vertical haut
        [0.95 color BTEX_SAND_COLOR filter LOCAL_FILTER transmit 1] 
        [0.95 BTEX_P_Sand ] 
        [1.00 BTEX_P_Sand ] // horizontal bas
      }
	     
      turbulence TF_LANDSCAPE_TURBULENCE
      octaves 1
      lambda 5
      omega TF_LANDSCAPE_DISTRIBUTION
      
    }
    finish {
      BTEX_F_Sand
    }
    normal {
      BTEX_N_Sand
    }
  }

#local LOCAL_TERRAIN_TEXTURE =
  texture {
    BTEX_Rock
  }
  texture {
    pigment {
      slope {
        y, 0.5, 1.0             
        altitude y, 0.0, 1.0        
      }
      pigment_map {
        [0.00 BTEX_P_Grass2 ]
        [0.05 BTEX_P_Grass2 ]
        [0.50 BTEX_P_Grass ] 
        [0.51 color BTEX_GRASS_COLOR filter LOCAL_FILTER transmit 1]
        [1.00 color BTEX_GRASS_COLOR filter LOCAL_FILTER transmit 1] // horizontal haut 
      }
      turbulence TF_LANDSCAPE_TURBULENCE
      octaves 1
      lambda 5
      omega TF_LANDSCAPE_DISTRIBUTION
    }
    finish {
      BTEX_F_Grass
    }
    normal {
      BTEX_N_Grass
    }
  }

// finally we apply this texture but only from the water level
// the texture under the water level is covered by a gradient
//
#declare TF_LANDSCAPE_TEXTURE =
texture {	
  gradient y
  texture_map {
    [0	             pigment { color TF_WATER_BOTTOMCOLOR } ]  //bottom of the sea
    [TF_WATER_LEVEL	BTEX_Sand ]  // water level
    [TF_WATER_LEVEL+0.015 LOCAL_BEACH_TEXTURE ]  
    [TF_WATER_LEVEL+0.020 LOCAL_TERRAIN_TEXTURE ]
    [1 LOCAL_TERRAIN_TEXTURE ] // summit
  }
}


