
#declare treeTrunkTexture = texture 
{
  finish
    {
    roughness 1.0
    }
  pigment
    {
    wrinkles
    turbulence 1
    octaves 8
    omega 0.7
    lambda 2
    color_map
      {
        [0.0 color Tan ]
        [0.4 color DarkBrown]
        [0.8 color Copper]
        [1.001 color Brass]
      }
    }
}

#declare treeBranchTexture = texture 
{
  finish
    {
    roughness 1.0
    }
  pigment
    {
    wrinkles
    turbulence 2
    octaves 8
    omega 0.7
    lambda 2
    frequency 2
    color_map
      {
        [0.0 color ForestGreen ]
        [0.4 transmit 1]
        [0.8 color DarkBrown]
        [1.001 transmit 1]
      }
    }
}


#declare GreenTree = union 
{
  #declare greenTreeCount = 0;
  #declare treeHeight = 10;
  #declare branchIntervalCount = 50;
  #declare branchProb = 0.85;
  #declare trunkThickness = treeHeight/30;
  #declare branchSeed = seed(2725);
  #declare branchDroop = 5; // in degrees
  #declare counter = 1 + treeHeight/branchIntervalCount;
  #declare maxLen = treeHeight/1.1;

  cone 
    {
    <0,0,0>, trunkThickness, <0,treeHeight,0>, 0.1
    texture {treeTrunkTexture}
    }

  #while (counter < treeHeight)

    #declare cb=0;
    #declare nb=3;
    #while (cb < nb)

      #if (rand(branchSeed) < branchProb)

        #declare greenTreeCount = greenTreeCount + 1;
        #declare branchAngle = rand(branchSeed) * 3.14159 * 2;
        #declare workingLen = maxLen / max(counter,1);

        cone 
          {
          <0,counter,0>, trunkThickness/max(counter,1)
          <workingLen*sin(branchAngle)*1.5,
            counter-workingLen*sin(radians(branchDroop)),
            workingLen*cos(branchAngle)*1.5 >,
          trunkThickness 
	  texture { treeBranchTexture }
	  }
       #end //if

       #declare cb = cb + 1;
     #end // while 

     #declare counter = counter + treeHeight/branchIntervalCount;
   #end // while

  #debug concat("green tree 1: ", str(greenTreeCount,5,0)," branches\n")
}

#macro tree_3 ()
  object { GreenTree scale <0.1, 0.1, 0.1> } 
#end

