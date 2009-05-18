#include "woods.inc"
#include "textures.inc"

//-------//-------//-------//-------//-------//-------//-------//
// meshcone.inc
//
// Macro for one "cone" made from triangles.
//
// By: Paul T. Dawson

#macro MeshCone ( BaseVector, LENGTH, DIAM1, DIAM2, TiltVector, Tex )

  // If INCREMENT is lower, there will be more triangles!
  #local INCREMENT = 20;

  // Go around the edges of the cone, do one section at a time.
  #local A = 0; 
  #while ( A < 360 )

    // Starting locations.
    #local Corner1 = vrotate ( <DIAM1,0,0>, <0,A+INCREMENT,0> );
    #local Corner2 = vrotate ( <DIAM2,LENGTH,0>, <0,A+INCREMENT,0> );
    #local Corner3 = vrotate ( <DIAM2,LENGTH,0>, <0,A,0> );
    #local Corner4 = vrotate ( <DIAM1,0,0>, <0,A,0> );

    // Rotate everything.
    #local Corner1 = vrotate ( Corner1, TiltVector );
    #local Corner2 = vrotate ( Corner2, TiltVector );
    #local Corner3 = vrotate ( Corner3, TiltVector );
    #local Corner4 = vrotate ( Corner4, TiltVector );

    // Translate everything.
    #local Corner1 = Corner1 + BaseVector;
    #local Corner2 = Corner2 + BaseVector;
    #local Corner3 = Corner3 + BaseVector;
    #local Corner4 = Corner4 + BaseVector;

    // Make the triangles.
    triangle { Corner1 Corner2 Corner3 texture {Tex} }
    triangle { Corner1 Corner3 Corner4 texture {Tex} }
                
    #declare COUNT_TRIANGLES = COUNT_TRIANGLES + 2;

    #local A = A + INCREMENT; 
  #end

  // Calculate the top center of the cone.
  #declare Cone_End = <0,LENGTH,0>;
  #declare Cone_End = vrotate ( Cone_End, TiltVector );
  #declare Cone_End = Cone_End + BaseVector;

#end


//-------//-------//-------//-------//-------//-------//-------//
// meshtree.inc
//
// Macro for one "tree" made from triangles.
//
// By: Paul T. Dawson

#macro MeshTree ( 
  Number_Of_Large_Branches,
  Number_Of_Medium_Branches,
  Number_Of_Small_Branches,
  Number_Of_Leaves,
        
  Branch_Minimum_Angle,
  Branch_Maximum_Angle,

  Tree_Trunk_Size,
  Large_Branch_Size_Min,
  Large_Branch_Size_Max,
  Medium_Branch_Size_Min,
  Medium_Branch_Size_Max,
  Small_Branch_Size_Min,
  Small_Branch_Size_Max,
        
  T_Tree_1,
  T_Tree_2,

  Seed_1
)


//-------//-------//-------//-------//-------//-------//-------//
// Start the mesh right here.
mesh {
  // Don't change these!
  // They are calculated from your variables.
  #declare R1 = seed(Seed_1);

  #declare Bmin = Branch_Minimum_Angle;
  #declare Bmax = Branch_Maximum_Angle - Branch_Minimum_Angle;

  #declare Large_Branch_Size_Range =
                Large_Branch_Size_Max - Large_Branch_Size_Min;

  #declare Medium_Branch_Size_Range =
                Medium_Branch_Size_Max - Medium_Branch_Size_Min;

  #declare Small_Branch_Size_Range =
                Small_Branch_Size_Max - Small_Branch_Size_Min;

  #declare COUNT_TRIANGLES = 0;

  // Start with the trunk.
  #declare Trunk_Start = < 0, 0, 0 >;
  #declare Trunk_Length = Tree_Trunk_Size;
  #declare Trunk_Angle = <0,0,0>;

  MeshCone ( Trunk_Start, Trunk_Length, 0.6, 0.5, Trunk_Angle, T_Tree_1 )

  #declare Trunk_End = Cone_End;


  // Right here, you know where Trunk_End (the top of the trunk) is,
  // so start putting large branches onto that point.
  #declare Large_Loop = 1;
    #while ( Large_Loop <= Number_Of_Large_Branches )

      #declare Large_Start = Trunk_End;
      #declare Large_Length = ( rand(R1) * Large_Branch_Size_Range ) +
                Large_Branch_Size_Min;
      #declare Large_Tilt = (rand(R1)*Bmax)+Bmin;
      #declare Large_Rotate = ( 360 / Number_Of_Large_Branches ) * Large_Loop;
      #declare Large_Angle = < Large_Tilt, Large_Rotate, 0 >;
        
      MeshCone ( Large_Start, Large_Length, 0.4, 0.2, Large_Angle, T_Tree_1 )

      // The outer end of this branch.
      #declare Large_End = Cone_End;

      // Now you know where Large_End (the outer end of one branch) is,
      // so start putting medium branches onto that point.
      #declare Medium_Loop = 1;
        #while ( Medium_Loop <= Number_Of_Medium_Branches )

          #declare Medium_Start = Large_End;
          #declare Medium_Length = ( rand(R1) * Medium_Branch_Size_Range ) +
                Medium_Branch_Size_Min;
          #declare Medium_Tilt = (rand(R1)*Bmax)+Bmin;
          #declare Medium_Rotate = ( 360 / Number_Of_Medium_Branches ) * Medium_Loop;
          #declare Medium_Angle = < Medium_Tilt, Medium_Rotate, 0 >;

          MeshCone ( Medium_Start, Medium_Length, 0.2, 0.1, Medium_Angle, T_Tree_1 )

          // The outer end of this branch.
          #declare Medium_End = Cone_End;


         // Right here, you know where Medium_End (the outer end of 1 branch) is,
         // so start putting small branches onto that point.
          #declare Small_Loop = 1;

            #while ( Small_Loop <= Number_Of_Small_Branches )
              #declare Small_Start = Medium_End;
              #declare Small_Length = ( rand(R1) * Small_Branch_Size_Range ) +
                Small_Branch_Size_Min;
              #declare Small_Tilt = (rand(R1)*Bmax)+Bmin;
              #declare Small_Rotate = ( 360 / Number_Of_Small_Branches ) * Small_Loop;
              #declare Small_Angle = < Small_Tilt, Small_Rotate, 0 >;
        
              MeshCone ( Small_Start, Small_Length, 0.1, 0.05, Small_Angle, T_Tree_1 )

              // The outer end of this branch.
              #declare Small_End = Cone_End;


             // Now you have Small_End (the outer end of one small branch),
             // so go ahead and put some leaves on that point.
             #declare Leaf_Loop = 1;
               #while ( Leaf_Loop <= Number_Of_Leaves )

                 #declare L1 = Small_End + < 0, 0.1, 0 >;
                 #declare L2 = Small_End - < 0, 0.1, 0 >;
                 #declare L3 = Small_End + <(rand(R1)-0.5)*2,(rand(R1)-0.5)*2,(rand(R1)-0.5)*2>;
                 #declare LEAF_ROT = < rand(R1)*30, rand(R1)*360,0>;
                 #declare L4 = vrotate ( < -0.10, -0.25, 0 >, LEAF_ROT ) + L3;
                 #declare L5 = vrotate ( <  0.10, -0.25, 0 >, LEAF_ROT ) + L3;
                 #declare L6 = vrotate ( <  0.00, -1.00, 0 >, LEAF_ROT ) + L3;

                 triangle { L1 L2 L3 texture {T_Tree_2} }
                 triangle { L3 L4 L5 texture {T_Tree_2} }
                 triangle { L4 L5 L6 texture {T_Tree_2} }
        
                 #declare COUNT_TRIANGLES = COUNT_TRIANGLES + 3;


                // End all of the loops.
                 #declare Leaf_Loop   = Leaf_Loop   + 1; 
	       #end

             #declare Small_Loop  = Small_Loop  + 1; 
	   #end

         #declare Medium_Loop = Medium_Loop + 1; 
       #end

     #declare Large_Loop  = Large_Loop  + 1; 
   #end
   // End the mesh, and the macro!
}

#debug concat ( "Triangles in one tree = ",
                str(COUNT_TRIANGLES,0,0) )

#end


// Build one tree.
#declare Entire_Tree = MeshTree (
        8,    // Number_Of_Large_Branches
        8,    // Number_Of_Medium_Branches
        8,    // Number_Of_Small_Branches
        12,   // Number_Of_Leaves
                
        20,   // Branch_Minimum_Angle
        140,  // Branch_Maximum_Angle

        10,   // Tree_Trunk_Size
        4,    // Large_Branch_Size_Min
        5,    // Large_Branch_Size_Max
        2,    // Medium_Branch_Size_Min
        3,    // Medium_Branch_Size_Max
        1,    // Small_Branch_Size_Min
        2,    // Small_Branch_Size_Max

        texture { T_Wood32 } ,    // T_Tree_1
        texture { Jade },         // T_Tree_2
        
        9432  // Seed_1
        )

// tree macro 
#macro tree_1 ()
  object { Entire_Tree scale <0.06, 0.06, 0.06> }
#end

