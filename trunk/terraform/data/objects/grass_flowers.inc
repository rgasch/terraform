// This file was downloaded from http://www.povworld.org/objects/cat/
// Because this collection is free it needs people to help it work. 
// Visit and help them. Thank you.

// Grass Plugin for POVRay
// by Gordon (gbentley@birdcameron.com.au)

// Adapted for Terraform by Raymond Ostertag

// The grass.inc plugin file takes several (read lots) parameters as described below and creates a mesh object when #included. 
// You can create MANY copies of this mesh object as POVRay's memory usage is very efficient for this type of object. 
// I have created over 9000 copies of a Grass mesh and traced them in a few hours in 64 Meg.
// This plugin uses the coordinate system used by Moray. This means that X is right, Y is away from you,and the Z axis is up. 
// I did it this way as it makes it easier to add objects in Moray without converting coordinates. I also just prefer this coordinate system!
// The grass consists of "clumps" of blades with optional flowers. Each flower consists of a stem and a flower proper. 
// The flower proper consists of one or more rows of petals, a centre section (stamens, pistils, etc), and a set of sepals. 
// The parameters are grouped to reflect this structure. These parameters are defined below with sample values.

//---------------------------------------------
//  Additonnal Parameters 
//---------------------------------------------

#declare TF_GRASS_FLOWERS_SCALE = 0.8;
#declare TF_GRASS_FLOWERS_DENSITY = 3;

// Grass Parameters

#local PatShape = 0; // Set to 0 for a rectangle, 1 for a circle.
#local PatRad = 0.5; // Set this variable to the required radius if you are creating a circular patch
#local XStart=-0.5; // Set these for a rectangle.
#local YStart=-0.5; // Set these for a rectangle.
#local XEnd=0.5; // Set these for a rectangle.
#local YEnd=0.5; // Set these for a rectangle.
#local ZPos=0; // Vertical position of ground. This is the base of all plants.
#local XStep=0.3 / TF_GRASS_FLOWERS_DENSITY; // Average gap between clumps in the X direction
#local XStepRand=0.25; // Fractional variation of position in X direction
#local YStep=0.3 / TF_GRASS_FLOWERS_DENSITY; // Average gap between clumps in the Y direction  
#local YStepRand=0.25; // Fractional variation of position in Y direction
#local MeanBladeHeight=0.2; // Average length of a grass blade.
#local HeightVar=0.25; // Fractional variation of length. 0.5 means vary the length by plus or minus 50%.
#local MeanBladeWidth=0.01; // Average width of a single blade.
#local WidthVar=0.25; // As for length.
#local BladeTaper = 0.7;
#local MinAngle=4; // Minimum angle between blade and vertical
#local MaxAngle=30; // Maximum angle between blade and vertical
#local MinCurve=30; // Minimum total angle of curvature of blade
#local MaxCurve=90; // Maximum total angle of curvature of blade
#local MaxBladeSegs=24; // Maximum number of segments in blade. There are a maximum of twice this number of triangles plus one in a grass blade.
#local MinBladeSegs=18; // Minimum number of segments in blade.
#local MaxBlades=8; // Maximum number of blades in a clump
#local MinBlades=4; // Minimum number of blades in a clump
#declare R1 = seed(32); // R1 is a random seed used throughout the plugin.
#local ReedDebug = 0;
#local FlowerProb = 0.2; // Probability of flower(s) in this clump

// Stem Parameters
#local MeanStemHeight=0.25; // Average height of flower stem
#local StemHeightVar=0.25; // As for blade length above
#local MeanStemWidth=0.005; // Pretty obvious by now!
#local StemWidthVar=0.25; // As for blade length above
#local StemTaper = 0.7; // Fraction by which to reduce diameter of stem along length
#local MinStemAngle=4; // Angles work the same as for blades above.
#local MaxStemAngle=30;
#local MinStemCurve=4;
#local MaxStemCurve=30;
#local StemCurvePower = 0.8; // Sets the location of the curvature on the stem. 
                             // 1 makes the stem curve evenly along its length, 
                             // <1 makes the curvature move toward the top, and, 
                             // >1 moves the curve down. Play with it!
#local MaxStemSegs=9; // Max and min segments as for blades. Number of trianges is 6 time this number.
#local MinStemSegs=6;
#local MinStems = 1; // Minimum number of flower stems to draw
#local MaxStems = 3; // Maximum stems

// Flower Parameters

#local PetNum = 9; // Number of petals in each row
#local PetLen = 0.025; // Total length of petal
#local PetBendLen = 0.012; // Length at which to bend petal. Should be less than PetLen
#local PetWidL = 0.004; // Left half width of petal at bend point
#local PetWidR = 0.004; // Right half width of petal at bend point. These two parameters let you create asymetric petals
#local PetWidB = 0.004; // Width at base of petal
#local PetWidT = 0.003; // Width at top of petal
#local PetAng = 80; // Angle of petal from flower axis. 90 degrees means sticking straight out.
#local PetKink = -15; // Angle to bend petal at PetBendLen. Positive means bend down at tip
#local PetArch = -15; // Side to side bow of petal. Positive means bend down at edges
#local PetTwist = 10; // Angle to rotate petal about its own axis. Think of a propeller!
#local PetCentRad = 0.007; // Radius of centre piece of petal.
#local PetCentHght = 0.002; // Height of centre piece
#local SepalHght = 0.003; // Height of sepals below the flower.
#local SepalRad = 0.007; // Radius of sepals (The green bits around the base of the flower)
#local FlowerSizeVar = 0.1;
#local PetTipLen = 0.001; // Length of tip triangle of petal. This puts a point on the petal if it is larger.
#local PetTipKink = 5; // Angle to offset tip triangle. Positive is down.
#local PetRows = 1; // Number of rows of petals to draw.
#local PetRowScale = 1; // Amount to scale each successive row
#local PetRowOff = 0; // Amount to move each row along flower axis. Positive is up
#local PetRowRot = 0; // Amount to rotate each successive row around flower axis
#local PetRowAngOff = 0; // Amount to change PetAng each successive row. Negative means towards the vertical

// As well as the parameters, you need to declare several textures for the various parts of the plants.
// Grass_Text - is the texture used for the grass blades themselves and the stems.
// PetalText0 - is used for the first row of petals.
// PetalText1 - is used for any subsequent rows of petals. I did this because I wanted to draw daffodils!
// PetCentreText - is used for the flower centre section.
// SepalText - is used for the sepals. Usually this is the same as the grass blades and stems.

// All the parameters might seem overwhelming at first, but persist and you will find that they are fairly obvious to use after a few goes. 
// Define the grass, then the stems, then the flower and you should be OK.

// -----------------------------------------------------------
// Patch declaration
// -----------------------------------------------------------

#local Grass_Text = texture{
         pigment{ gradient y
                  turbulence 0.4
                  colour_map{
                             [0.0 colour rgb <0.10,0.55,0.06>]
                             [0.98 colour rgb <0.10,0.45,0.09>]
                             [1.0 colour rgb <0.4,0.21,0.04>]
                             }
                 scale <0.0001,0.0001,0.15>
                 }
         //normal {wrinkles 0.3 scale <0.0001,0.0001,1000>}
         finish{ ambient 0.2 diffuse 0.6 phong 0.4 phong_size 60}
}
#local SepalText = texture{ Grass_Text }

#local PetalText0 = texture{
         pigment{ gradient y
                  turbulence 0.4
                  colour_map{
                             [0.0 colour rgb <0.90,0.95,0.18>]
                             [0.98 colour rgb <0.90,0.9,0.08>]
                             [1.0 colour rgb <0.85,0.55,0.08>]
                             }
                 scale <0.0001,0.0001,0.15>
                 }
//         normal {wrinkles 0.3 scale <0.0001,0.0001,1000>}
         finish{ ambient 0.3 diffuse 0.7 phong 0.4 phong_size 10}
}
#local PetalText1 = texture{
         pigment{ gradient y
                  turbulence 0.4
                  colour_map{
                             [0.0 colour rgb <0.90,0.95,0.18>]
                             [0.98 colour rgb <0.90,0.9,0.08>]
                             [1.0 colour rgb <0.85,0.55,0.08>]
                             }
                 scale <0.0001,0.0001,0.15>
                 }
//         normal {wrinkles 0.3 scale <0.0001,0.0001,1000>}
         finish{ ambient 0.3 diffuse 0.7 phong 0.4 phong_size 10}
}
#local PetCentreText = texture{
         pigment{  colour rgb <0.05,0.15,0.08> }
//         normal {wrinkles 0.3 scale <0.0001,0.0001,1000>}
         finish{ ambient 0.2 diffuse 0.7 phong 0.6 phong_size 10}
}

#local NumTri = 0; //Initialized here. The plugin sends counts of triangles to the debug stream so you can see how many there are.
#declare GrassPatch0 = #include "grass_flowers_inc1.txt"

//---------------------------------------------
//  Object ans Macro declaration
//---------------------------------------------

#macro grass_flowers ()
     object{ 
             GrassPatch0
             rotate <0,0,rand(R1)*360>
             //translate <XOff, YOff, 0>
             rotate -x *90
             scale TF_GRASS_FLOWERS_SCALE
     }
#end //macro

