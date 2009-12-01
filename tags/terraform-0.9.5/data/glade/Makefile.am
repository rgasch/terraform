datadir = @datadir@/terraform/glade

data_DATA = \
	about_window.glade \
	basins_window.glade \
	chooser_window.glade \
	close_terraform_dialog.glade \
	close_terrain_dialog.glade \
	craters_window.glade \
	digital_filter_window.glade \
	erode_flowmap_window.glade \
	erode_window.glade \
	error_log_filesel.glade \
	execution_error_window.glade \
	export_view_window.glade \
	faulting_window.glade \
	feather_window.glade \
	file_options_window.glade \
	fill_window.glade \
	find_povray_window.glade \
	flowmap_window.glade \
	fold_window.glade \
	gaussian_hill_window.glade \
	info_window.glade \
	installer_window.glade \
	join_window.glade \
	level_connector_window.glade \
	main_window.glade \
	merge_window.glade \
	mirror_window.glade \
	move_window.glade \
	native_render_window.glade \
	open_file.glade \
	options_window.glade \
	perlin_noise_window.glade \
	place_objects_window.glade \
	povray_render_options_window.glade \
	preview_popup.glade \
	print_preview_window.glade \
	prune_window.glade \
	radial_scale_window.glade \
	rasterize_window.glade \
	rescale_placed_objects_window.glade \
	river_all_window.glade \
	river_window.glade \
	rotate_window.glade \
	roughen_window.glade \
	save_as.glade \
	scale_window.glade \
	scene_options_window.glade \
	select_by_height_window.glade \
	smooth_window.glade \
	spectral_window.glade \
	spherical_map_window.glade \
	subdivision_window.glade \
	terrace_window.glade \
	terraform.gladep \
	terrain_options_window.glade \
	terrain_window.glade \
	tile_window.glade \
	trace_objects_window.glade \
	transform_window.glade \
	undo_tree_window.glade \
	warp_window.glade \
	add.xpm \
	arrow.xpm \
	camera.xpm \
	circle.xpm \
	crop.xpm \
	die.xpm \
	eye.xpm \
	rectangle.xpm \
	replace.xpm \
	seed.xpm \
	subtract.xpm \
	terraform_logo.xpm \
	zoom.xpm \
	gnome-question.png \
	gnome-warning.png
 
install_home: 
	echo "$(HOME)/.terraform"; 
	if [ ! -d "$(HOME)/.terraform" ]; then \
	  mkdir "$(HOME)/.terraform" ; \
	fi;
	if [ ! -d "$(HOME)/.terraform/glade" ]; then \
	  mkdir "$(HOME)/.terraform/glade" ; \
	fi;
	cp -f *.glade *.gladep *.xpm *.png "$(HOME)/.terraform/glade";
