datadir = @datadir@/terraform/include/atmospheres

EXTRA_DIST = earth_fog.inc \
             earth_haze_fast.inc \
             earth_haze_slow.inc \
             fog_ground.inc \
             rainbow.inc 

data_DATA = earth_fog.inc \
            earth_haze_fast.inc \
            earth_haze_slow.inc \
            fog_ground.inc \
            rainbow.inc 

install_home: 
	if [ ! -d "$(HOME)/.terraform" ]; then \
	  mkdir "$(HOME)/.terraform" ; \
	fi;
	if [ ! -d "$(HOME)/.terraform/include" ]; then \
	  mkdir "$(HOME)/.terraform/include" ; \
	fi;
	if [ ! -d "$(HOME)/.terraform/include/atmospheres" ]; then \
	  mkdir "$(HOME)/.terraform/include/atmospheres" ; \
	fi;
	cp -f *.inc "$(HOME)/.terraform/include/atmospheres";

