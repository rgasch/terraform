datadir = @datadir@/terraform/objects

EXTRA_DIST = fern.inc monolith.inc \
             tree_1.inc tree_2.inc tree_3.inc \
             tree_4.inc tree_5.inc tree_6.inc \
             grass_patch_small.inc grass_patch_big.inc \
             stone_agate.inc stone_dents.inc stone_peeble.inc stone_wrinkles.inc \
	          fern.gif tree_4.gif tree_5.gif tree_6.gif \
	          bushes.inc bushOne.inc flowerPatch.inc foliage_inc.txt \
	          grass_flowers.inc grass_flowers_inc1.txt grass_flowers_inc2.txt \
             grass_multiflowers.inc grass_prairie.inc grass_prairie_inc.txt \
             sevenBushes.inc tree_7.inc tree7_inc.txt tree_8.inc \
             tree_8_inc.txt tree_palm_1.inc tree_palm_1_inc.txt \
             tree_palm_patch.inc tree_parametric_1.inc tree_parametric_2.inc \
	          tree_parametric_3.inc tree_parametric_4.inc tree_parametric_5.inc \
	          tree_parametric_6.inc  tree_parametric_7.inc \
	          tree_parametric_macro_inc.txt tree_parametric_texture_inc.txt _bee.inc \
	          _boat.inc _butterfly.inc _butterfly_wing.gif _example.inc \
	          _grass_multiflowers.inc _grass_prairie.inc  _needlebranch.inc \
	          _tree_8.inc _tree_parametric_1.inc _tree_parametric_2.inc\
	          _tree_parametric_3.inc _tree_parametric_4.inc _tree_parametric_5.inc \
	          _tree_parametric_6.inc  _tree_parametric_7.inc 

data_DATA =  fern.inc monolith.inc \
             tree_1.inc tree_2.inc tree_3.inc \
             tree_4.inc tree_5.inc tree_6.inc \
             grass_patch_small.inc grass_patch_big.inc \
             stone_agate.inc stone_dents.inc stone_peeble.inc stone_wrinkles.inc \
	          fern.gif tree_4.gif tree_5.gif tree_6.gif \
	          bushes.inc bushOne.inc flowerPatch.inc foliage_inc.txt \
	          grass_flowers.inc grass_flowers_inc1.txt grass_flowers_inc2.txt \
             grass_multiflowers.inc grass_prairie.inc grass_prairie_inc.txt \
             sevenBushes.inc tree_7.inc tree7_inc.txt tree_8.inc \
             tree_8_inc.txt tree_palm_1.inc tree_palm_1_inc.txt \
             tree_palm_patch.inc tree_parametric_1.inc tree_parametric_2.inc \
	          tree_parametric_3.inc tree_parametric_4.inc tree_parametric_5.inc \
	          tree_parametric_6.inc  tree_parametric_7.inc \
	          tree_parametric_macro_inc.txt tree_parametric_texture_inc.txt _bee.inc \
	          _boat.inc _butterfly.inc _butterfly_wing.gif _example.inc \
	          _grass_multiflowers.inc _grass_prairie.inc  _needlebranch.inc \
	          _tree_8.inc _tree_parametric_1.inc _tree_parametric_2.inc\
	          _tree_parametric_3.inc _tree_parametric_4.inc _tree_parametric_5.inc \
	          _tree_parametric_6.inc  _tree_parametric_7.inc 
 
install_home: 
	echo "$(HOME)/.terraform"; 
	if [ ! -d "$(HOME)/.terraform" ]; then \
	  mkdir "$(HOME)/.terraform" ; \
	fi;
	if [ ! -d "$(HOME)/.terraform/objects" ]; then \
	  mkdir "$(HOME)/.terraform/objects" ; \
	fi;
	cp -f *.inc *.gif *.txt "$(HOME)/.terraform/objects";

