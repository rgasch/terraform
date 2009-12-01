datadir = @datadir@/terraform/include/water

EXTRA_DIST = earth_water.inc \
             earth_artic_water.inc \
             earth_mediawater.inc \
             earth_water_noise.inc \
             earth_water_ridged.inc \
             earth_water_waves.inc \
             earth_seawater.inc \
             earth_seawater2.inc

data_DATA = earth_water.inc \
             earth_artic_water.inc \
             earth_mediawater.inc \
             earth_water_noise.inc \
             earth_water_ridged.inc \
             earth_water_waves.inc \
             earth_seawater.inc \
             earth_seawater2.inc

install_home: 
	echo "$(HOME)/.terraform"; 
	if [ ! -d "$(HOME)/.terraform" ]; then \
	  mkdir "$(HOME)/.terraform" ; \
	fi;
	if [ ! -d "$(HOME)/.terraform/include" ]; then \
	  mkdir "$(HOME)/.terraform/include" ; \
	fi;
	if [ ! -d "$(HOME)/.terraform/include/water" ]; then \
	  mkdir "$(HOME)/.terraform/include/water" ; \
	fi;
	cp -f *.inc "$(HOME)/.terraform/include/water";

