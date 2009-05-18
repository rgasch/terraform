// This file was downloaded from http://www.povworld.org/objects/cat/
// Because this collection is free it needs people to help it work. 
// Visit and help them. Thank you.

// Persistence of Vision Ray Tracer Scene Description File
// File: ?.pov
// Vers: 3.1
// Desc: Basic Scene Example
// Date: mm/dd/yy
// Auth: Mick Hazelgrove
//

// Adapted for Terraform by Raymond Ostertag

//---------------------------------------------
// Parameters 
//---------------------------------------------
#ifndef (TF_TRACE_OBJECT_X)
   #local LOCAL_EYE_POINT = < 0.5, 2.5, -0.05 >; 
#else
   #local LOCAL_EYE_POINT = < TF_TRACE_OBJECT_X, TF_TRACE_OBJECT_Y, TF_TRACE_OBJECT_Z > ; 
#end
// coordinates are like in the camera dialog 
// X=0,Z=0 and X=1,Z=1 are the bounding of the terrain
// if X and Z coordinates are in the bounding then the object is placed on the terrain (Y must be greater than the height of the terrain)
// if X or Z coordinates are out of the bounding then the object is placed on the water

#ifndef (TF_TRACE_OBJECT_RELATIV)
   #local LOCAL_CAMERA_RELATIV = false;
#else
   #local LOCAL_CAMERA_RELATIV = TF_TRACE_OBJECT_RELATIV;
#end
// if this boolean is true the coordinate are relativ to the camera location instead of the origin of the HF
// if the eye_point is over the terrain the object is placed on the terrain, if it's over the water then the object is placed on the water

#ifndef (TF_TRACE_OBJECT_CONSERVE)
   #local LOCAL_CONSERVE_ELEVATION = false;
#else
   #local LOCAL_CONSERVE_ELEVATION = TF_TRACE_OBJECT_CONSERVE;
#end
// if this boolean is true the object is placed in the air (for a plane) or in the water (for a submarine) at the coordinates of the eye_point. 

#ifndef (TF_TRACE_OBJECT_FLOAT)
   #local LOCAL_OBJECT_FLOAT = true;
#else
   #local LOCAL_OBJECT_FLOAT = TF_TRACE_OBJECT_FLOAT;
#end
// if this boolean is false the object can be on earth but under the water.

#ifndef (TF_TRACE_OBJECT_SCALE)
   #local LOCAL_OBJECT_SCALE = 0.2;
#else
   #local LOCAL_OBJECT_SCALE = TF_TRACE_OBJECT_SCALE;
#end
// additonnal scale to fit whit the terraform's scales

#local LOCAL_OBJECT_FLOATLEVEL = 1.5;
// increase or decrease the floating level

//------------------------------------------------------------------- 

#local B_Tex = texture {
    pigment {
        wood turbulence 0.0425
        scale 0.2
        color_map {
            [0.05 color rgbt <0.55294, 0.21176, 0.00000, 0.0>]
            [0.15 color rgbt <0.32549, 0.13725, 0.00000, 0.0>]
            [0.25 color rgbt <0.55294, 0.21176, 0.00000, 0.0>]
            [0.35 color rgbt <0.32549, 0.11765, 0.00000, 0.0>]
            [0.48 color rgbt <0.55294, 0.21176, 0.00000, 0.0>]
            [0.55 color rgbt <0.29412, 0.13725, 0.01176, 0.0>]
            [0.65 color rgbt <0.55294, 0.21176, 0.00000, 0.0>]
            [0.78 color rgbt <0.32549, 0.13725, 0.00000, 0.0>]
            [0.85 color rgbt <0.55294, 0.21176, 0.00000, 0.0>]
            [0.96 color rgbt <0.28627, 0.13725, 0.00000, 0.0>]
            [1.00 color rgbt <0.54510, 0.17647, 0.03529, 0.0>]
        }
        translate <.015, 0, 0>
        rotate y * 90
    }
    scale 1
}

#local Prow = union{

#local Count = 0;
#local Radius = 1;
#local Angle = 0;
#local SphRad = .01;

#while(Count<140)

sphere{0,SphRad 
        translate<Count/25,pow(Count,2)/750,0>
        rotate z*Angle
        }
        
#local Count = Count +1;
#local Angle = Angle + 5;
#local SphRad = SphRad + 0.03;
#local Radius =  Radius +Radius;

#end        
rotate x*180
}



#local Prow2 = union{

#local Count = 0;
#local Radius = 100;
#local Angle = 280;
#local SphRad = 4.2;

#while(Count<40)

sphere{0,SphRad 
        translate <0,Radius,0>
        rotate z*Angle
        }
        
#local Count = Count +1;
#local Angle = Angle + 1;
#local SphRad = SphRad + 0.03;


#end        
rotate x*180
}


#local FProw = union{
        object{Prow
                scale 0.025
                rotate z*35 
                
        }

        object{Prow2
                scale 0.025 
                translate<-1.8495,0.15,0>        
        }
        object {torus{0.1,0.075}
                translate<0.6,-0.175,0>
        }
        translate <4.85,1,0>       
}
#local Post = difference{
cylinder{z*-0.05,z*0.05,2.1}
cylinder{z*-0.2,z*0.2,1.95}
scale<2.5,1.25,1>
clipped_by{sphere{0,20 inverse translate<1,19.1,0>}}
}
 
#local Bsphere = difference{
        sphere{0,2}
        sphere{0,1.95}
        }
  
#local R1 = seed(1000);
        
#local BtShape = union{
 object{Bsphere
        scale <2.5,1.25,1>
        clipped_by{sphere{0,20 inverse translate<1,18,0>}}
        texture{B_Tex translate 1}
        }
        
   object{Bsphere
        scale <2.5,1.25,1>
        clipped_by{sphere{0,20 inverse translate<1,18.2,0>}}
        clipped_by{sphere{0,20 translate<1,18,0>}}
        texture{B_Tex translate 2}
        scale 1.01
        }       
       
   object{Bsphere
        scale <2.5,1.25,1>
        clipped_by{sphere{0,20 inverse translate<1,18.4,0>}}
        clipped_by{sphere{0,20 translate<1,18.2,0>}}
        texture{B_Tex translate 3}
        scale 1.02
        }
        
              
   object{Bsphere
        scale <2.5,1.25,1>
        clipped_by{sphere{0,20 inverse translate<1,18.6,0>}}
        clipped_by{sphere{0,20 translate<1,18.4,0>}}
        texture{B_Tex translate 4}
        scale 1.03
        }       
         
   object{Bsphere
        scale <2.5,1.25,1>
        clipped_by{sphere{0,20 inverse translate<1,18.8,0>}}
        clipped_by{sphere{0,20 translate<1,18.6,0>}} 
        texture{B_Tex translate 5}
        scale 1.04
        }
      
  object{Bsphere
        scale <2.5,1.25,1>
        clipped_by{sphere{0,20 inverse translate<1,19,0>}}
        clipped_by{sphere{0,20 translate<1,18.8,0>}}
        texture{B_Tex translate 6}
        scale 1.05
        }                    
 }
 
#local boat = union{        
 object{BtShape clipped_by{plane{z,-0.4 }}translate<0,0,.4>}
 object{BtShape clipped_by{plane{z,0.4 inverse }}translate<0,0,-.4>}
 object{FProw texture{B_Tex translate 7}scale 0.75 rotate z*18 translate <1.4,-1.25,0>}
 object{Post texture{B_Tex translate 8}}
 //rotate z*10
}


//---------------------------------------------
//  Object declaration
//---------------------------------------------

#declare objet_toplace = 
object {
  boat 
  translate LOCAL_OBJECT_FLOATLEVEL
  scale LOCAL_OBJECT_SCALE
}

//---------------------------------------------
//  trace object include file
//---------------------------------------------

#if (RENDER_TRACE_OBJECT)
  #include "trace_objects.inc"
#end














