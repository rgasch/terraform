//----------------------------------------------------
// Originally Created with Truevision, version 0.3.10
// Adapted for Terraform 0.9.0
//----------------------------------------------------
// Copyright (c) Raymond Ostertag
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//----------------------------------------------------

//---------------------------------------------
//  Declaration of the following :
// Textures : BTEX_Rock, BTEX_Rock_stratum, BTEX_Grass, BTEX_Snow, BTEX_Grasstex,
//---------------------------------------------

//---------------------------------------------
//  Parameters
//---------------------------------------------

#local BTEX_STRATUM_RATIO = 1.5 ;  // color variation for stratum
#local BTEX_STRATUM_ANGLE = 60 ;  // angle for stratum

#local BTEX_SAND_FREQUENCY = 100000 ;

#local LOCAL_SCALE_PIGMENT = 25 ; //global scale for the textures
#local LOCAL_SCALE_NORMAL = 1; // global scale for the bump mapping

#declare BTEX_SCALE = <1/TF_X_SCALE, 1/TF_Y_SCALE, 1/TF_Z_SCALE> * LOCAL_SCALE_PIGMENT ;
#local BTEX_BUMPYSCALE = <1/TF_X_SCALE, 1/TF_Y_SCALE, 1/TF_Z_SCALE> * LOCAL_SCALE_NORMAL ;
#local BTEX_BUMPDEPTH = 0.5 ; // global scale for the depth of the bump mapping

//---------------------------------------------
//  Basic Pigments Normal Finish
//---------------------------------------------

#declare BTEX_P_Rock =
pigment {
	granite
	color_map {
		[ 0.02  color BTEX_ROCK_COLOR * 1.3 ]
		[ 0.20  color BTEX_ROCK_COLOR * 1.3 ]
		[ 0.85  color BTEX_ROCK_COLOR * 1.0 ]
		[ 1.00  color BTEX_ROCK_COLOR * 0.5 ]
	}
	scale 0.05 * BTEX_SCALE
}

#declare BTEX_P_Rock_Stratum =
pigment {
	granite
	color_map {
		[ 0.02  color BTEX_ROCK_COLOR * 1.3 / BTEX_STRATUM_RATIO ]
		[ 0.20  color BTEX_ROCK_COLOR * 1.3 / BTEX_STRATUM_RATIO ]
		[ 0.85  color BTEX_ROCK_COLOR * 1.0 / BTEX_STRATUM_RATIO ]
		[ 1.00  color BTEX_ROCK_COLOR * 0.5 / BTEX_STRATUM_RATIO ]
	}
	scale 0.05 * BTEX_SCALE
}

#declare BTEX_P_Grass =
pigment {
	wrinkles
	color_map {
		[ 0.000000  color BTEX_GRASS_COLOR ]
		[ 1.000000  color BTEX_GRASS_COLOR *  ( 1 - ( BTEX_GRASS_GRAIN / 5)) ]
	}
   turbulence 0.5
   octaves 20
   lambda 5
   omega 1.0
}

#declare BTEX_P_Snow =
pigment {
	leopard
	color_map {
		[ 0.000000  color BTEX_SNOW_COLOR ]
		[ 1.000000  color BTEX_SNOW_COLOR *  ( 1 - ( BTEX_SNOW_GRAIN / 5)) ]
	}
   turbulence 0.5
   octaves 20
   lambda 5
   omega 1.0
}

#declare BTEX_P_Sand =
    pigment {
      granite
        color_map {
          [ 0.02  color BTEX_SAND_COLOR * 1.3 ]
          [ 0.20  color BTEX_SAND_COLOR * 1.3 ]
          [ 0.85  color BTEX_SAND_COLOR * 1.0 ]
          [ 1.00  color BTEX_SAND_COLOR * 0.5 ]
        }
        scale 0.05 * LOCAL_SCALE_PIGMENT
    }

#declare BTEX_N_Rock =
		normal {
			granite
			bump_size 0.5 * BTEX_ROCK_GRAIN
			translate 0
			scale 0.05 * BTEX_BUMPYSCALE
			rotate 0
		}

#declare BTEX_N_Grass = 
		normal {
			wrinkles
			bump_size 0.6 * (1 + BTEX_GRASS_GRAIN )
			scale 0.05 * BTEX_BUMPYSCALE
		}

#declare BTEX_N_Snow = 
		normal {
			leopard
			bump_size 0.6 * (1 + BTEX_SNOW_GRAIN )
			scale 0.05 * BTEX_BUMPYSCALE
		}

#declare BTEX_N_Sand = 
    normal
    {
      average
      normal_map
      {
        [1.0 waves BTEX_SAND_GRAIN  frequency BTEX_SAND_FREQUENCY*1.0 phase 720.0 * clock]
        [1.0 waves BTEX_SAND_GRAIN  frequency BTEX_SAND_FREQUENCY*0.4 phase 720.0 * clock]
        [1.0 waves BTEX_SAND_GRAIN  frequency BTEX_SAND_FREQUENCY*0.1 phase 720.0 * clock]
      }
    }

#declare BTEX_F_Rock = 
		finish {
			ambient rgb 0.1
			diffuse 0.6 
			brilliance 1.0
			phong 0.0 
			phong_size 40
			specular 0.0 
			roughness 0.15 
			metallic 0.0 
			reflection 0
			reflection_exponent 1.0 
		}

#declare BTEX_F_Grass = 
		finish {
			ambient 0.1
			diffuse 0.6
			brilliance 1.0
			phong 0.0
			phong_size 40
			specular 0.0
			roughness 0.5
			metallic 0.0
			reflection rgb <0.0, 0.0, 0.0>
			reflection_exponent 1.0
		}

#declare BTEX_F_Snow = 
		finish {
			ambient 0.15
			diffuse 0.7
			brilliance 1.0
			phong 0.0
			phong_size 40
			specular 0.0
			roughness 0.25
			metallic 0.0
			reflection <0.15, 0.15, 0.25>
			reflection_exponent 1.0
		}

#declare BTEX_F_Ice = 
		finish {
			ambient 0.15
			diffuse 0.7
			brilliance 1.0
			phong 0.0
			phong_size 40
			specular 0.0
			roughness 0.0
			metallic 0.0
			reflection <0.25, 0.25, 0.30>
			reflection_exponent 1.0
		}

#declare BTEX_F_Sand = 
		finish {
			ambient rgb 0.1
			diffuse 0.6 
			brilliance 1.0
			phong 0.0 
			phong_size 40
			specular 0.0 
			roughness 0.15 
			metallic 0.0 
			reflection 0
			reflection_exponent 1.0 
		}

//---------------------------------------------
//  Main Basic Textures
//---------------------------------------------

#declare BTEX_Rock = 
  texture {
    pigment {
      BTEX_P_Rock
    }
    normal {
      BTEX_N_Rock
    }
    finish {
      BTEX_F_Rock
    }
  }

#declare BTEX_Rock_stratum =
  texture {
    pigment {
      gradient y
      pigment_map {
        [0    BTEX_P_Rock ]
        [1.00 BTEX_P_Rock_Stratum ]
      }
      scale 0.025 * BTEX_STRATUM_SIZE
      rotate <0, 0, BTEX_STRATUM_ANGLE>
    }
    normal {
      BTEX_N_Rock
    }
    finish {
      BTEX_F_Rock
    }
  }

#declare BTEX_Grass = 
	texture {
		pigment {
			BTEX_P_Grass
		}
		normal {
			BTEX_N_Grass
		}
		finish {
			BTEX_F_Grass
		}
	}

#declare BTEX_Snow = 
	texture {
		pigment {
			BTEX_P_Snow
		}
		normal {
			BTEX_N_Snow
		}
		finish {
			BTEX_F_Snow
		}
	}

#declare BTEX_Sand =
  texture {
    pigment {
      BTEX_P_Sand
    }
    normal {
      BTEX_N_Sand
    }
    finish {
      BTEX_F_Sand
    }
  }

#declare BTEX_Ice = 
  texture {
    pigment {
      color BTEX_SNOW_COLOR
    }
    finish {
    BTEX_F_Ice
    }
  }

//---------------------------------------------
//  Grasstex
//---------------------------------------------

   #local LOCAL_SCALE = 0.001;

   #include "grasstex.inc"    // See grastex.inc for more informations
   #declare camera_location = TF_CAMERA_LOCATION ;
   #declare gt_height         = 1.00;
   #declare gt_turb           = 0.5;
   #declare gt_samples        = 20;  //increase this to have more density
   #declare gt_tweak          = 0.01; //increase this to have bigger blades
   #declare gt_groundcolors   =
   color_map {
      [0.0, color BTEX_GRASS_COLOR  * ( 1 - (BTEX_GRASS_GRAIN / 5)) ]
      [0.5, color BTEX_GRASS_COLOR  * ( 1 - (BTEX_GRASS_GRAIN / 5)) ]
      [1.0, color BTEX_GRASS_COLOR  * ( 1 - (BTEX_GRASS_GRAIN / 5)) ]
   }

// *********************************************

#local TEXGRASS_BG_TEXTURE =
      texture {
         gt_ground()
         normal {
           bumps 0.5 
           scale 0.01
         }
      }

#local TEXGRASS_FG_TEXTURE =
      texture {
         #local gt_color   = BTEX_GRASS_COLOR ;
         #local gt_offset  = 0.0;
         #local gt_seed    = 0;
         gt_grass ()
         normal {
           bumps 0.5 
           scale 2 
           warp {turbulence 2} 
           scale 1
         }
         scale LOCAL_SCALE/TF_SCALE
      }

#local TEXGRASS_WHITEFLOWER_TEXTURE =
      texture {
         #local gt_flowercolor = <1,1,1>;
         #local gt_seed = 0;
         gt_flowers ()
         normal {
           bumps 0.5 
           scale 2 
           warp {turbulence 2} 
           scale 1
         }
         scale LOCAL_SCALE/TF_SCALE
      }

#local TEXGRASS_YELLOWFLOWER_TEXTURE =
      texture {
         #local gt_flowercolor = <1,1,0>;
         #local gt_seed = 10000;
         gt_flowers ()
         normal {
           bumps 0.5 
           scale 2 
           warp {turbulence 2} 
           scale 1
         }
         scale LOCAL_SCALE/TF_SCALE
      }

#declare BTEX_Grasstex =  
//    The very first layer should be the ground layer:
      texture {
        TEXGRASS_BG_TEXTURE
      }
      texture {
        TEXGRASS_FG_TEXTURE
      }
      texture {
        TEXGRASS_WHITEFLOWER_TEXTURE
      }
      texture {
        TEXGRASS_YELLOWFLOWER_TEXTURE
      }
