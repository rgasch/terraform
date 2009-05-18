datadir = @datadir@/terraform/include/earth_textures

EXTRA_DIST = earth_canyon_landscape.inc \
             earth_desert_landscape.inc \
             earth_green_landscape.inc \
             earth_mountain_landscape.inc \
             earth_sand.inc \
             earth_grass.inc \
             earth_atoll.inc

data_DATA =  earth_canyon_landscape.inc \
             earth_desert_landscape.inc \
             earth_green_landscape.inc \
             earth_mountain_landscape.inc \
             earth_sand.inc \
             earth_grass.inc \
             earth_atoll.inc

install_home: 
	echo "$(HOME)/.terraform"; 
	if [ ! -d "$(HOME)/.terraform" ]; then \
	  mkdir "$(HOME)/.terraform" ; \
	fi;
	if [ ! -d "$(HOME)/.terraform/include" ]; then \
	  mkdir "$(HOME)/.terraform/include" ; \
	fi;
	if [ ! -d "$(HOME)/.terraform/include/earth_textures" ]; then \
	  mkdir "$(HOME)/.terraform/include/earth_textures" ; \
	fi;
	cp -f *.inc "$(HOME)/.terraform/include/earth_textures";

