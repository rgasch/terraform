/* Terraform - (C) 1997-2002 Robert Gasch (r.gasch@chello.nl)
 *  - http://terraform.sourceforge.net
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <gnome.h>


void
on_main_file_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_new_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_perlin_noise_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_spectral_synthesis_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_subdivision_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_random_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_open_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_join_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_merge_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_warp_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_gamma_preview_realize               (GtkWidget       *widget,
                                        gpointer         user_data);

GtkWidget*
create_gamma_preview (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

void
on_main_options_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_print_settings_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_exit_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_help_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_about_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_save_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_save_as_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_gimp_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_open_gl_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_close_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_clone_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_undo_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_connect_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_craters_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_digital_filter_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_fill_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_fold_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_gaussian_hill_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_radial_scale_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_mirror_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_move_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_rotate_changed                      (GtkToggleButton *toggle,
                                        GtkWidget       *widget);

void
on_terrain_rotate_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_roughen_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_smooth_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_terrace_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_tile_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_transform_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_invert_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_erode_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_flowmap_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_scale_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_auto_rotate_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
on_main_spectral_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_main_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

GtkWidget*
create_lc_custom (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

GtkWidget*
create_render_custom (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

void
on_close_yes_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_terrain_view_destroy_event          (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_terrain_view_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_terrain_window_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_generic_new_seed_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
terrain_toggle_activate                (GtkMenuShell    *menushell,
                                        gpointer         user_data);

void
on_load_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_option_ok_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_option_povray_browse_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_povray_yes_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_povray_no_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_terrain_render_povray_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_native_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_export_povray_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about_info_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_generic_cancel_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_option_download_povray_clicked      (GtkButton       *button,
                                        gpointer         user_data);

void
on_generic_use_preview_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_save_as_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data);


void
on_use_preview_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data);


gboolean
on_render_preview_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_render_preview_button_release_event (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_generic_preview_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_generic_preview_motion_notify_event (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

void
on_mirror_changed                      (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_generic_toggled                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_generic_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_preview_popup_selection_done        (GtkMenuShell    *menushell,
                                        gpointer         user_data);

void
on_undo_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_tree_selection_changed              (GtkTree         *tree,
                                        gpointer         user_data);

void
on_execution_error_details_click       (GtkButton       *button,
                                        gpointer         user_data);

void
on_perlin_toggled                      (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_terrain_print_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_erode_save_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_merge_ok_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_merge_cancel_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_merge_list_select_row               (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_merge_list_unselect_row             (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_merge_top_select_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_merge_bottom_select_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_native_save_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_scale_ok_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_scale_toggled                       (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_terrain_print_preview_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_select_invert_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_select_all_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_select_none_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_select_feather_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_contents_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_by_height_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_level_connector_count_changed       (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_place_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_generic_radio_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_place_ok_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_tutorial_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_users_guide_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_sub_ok_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_sub_apply_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_ss_ok_clicked                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_ss_apply_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_pn_ok_clicked                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_pn_apply_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_ss_generate_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_ss_cancel_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_pn_generate_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_pn_cancel_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_sub_generate_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_sub_cancel_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

GtkWidget*
create_terrain_preview (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);


GtkWidget*
create_transfer_function (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

void
on_transfer_function_realize           (GtkWidget       *widget,
                                        gpointer         user_data);

GtkWidget*
create_native_image (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

void
on_toolbar_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_export_view_ok_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_terrain_export_view_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

GtkWidget*
create_choose_terrain (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

void
on_randomize_clicked                   (GtkButton       *button,
		                        gpointer         user_data);

GtkWidget*
create_randomize (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

void
on_chooser_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

GtkWidget*
create_terrain_aspect_frame (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

void
on_fault_option_selected               (GtkMenuShell *menu_shell,
		                        gpointer data);

void
on_fault_generate_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_fault_cancel_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_main_faulting_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_main_faulting_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
on_terrain_info_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_info_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_save_yes_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_close_yes_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_generic_cancel_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_save_terrain_yes_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_close_terrain_yes_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_join_top_select_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_join_bottom_select_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_join_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_join_cancel_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_warp_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_warp_cancel_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_warp_top_select_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_warp_bottom_select_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_main_join_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_merge_list_select_row               (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_merge_list_unselect_row             (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_join_list_select_row                (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_join_list_unselect_row              (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_warp_list_select_row                (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_warp_list_unselect_row              (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_save_terrain_no_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_save_all_terrains_yes_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_save_all_terrains_no_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_generic_cancel_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_erode_flowmap_ok_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_erode_flowmap_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_erode_flowmap_top_select_clicked    (GtkButton       *button,
		                        gpointer         user_data);

void
on_erode_flowmap_bottom_select_clicked (GtkButton       *button,
		                        gpointer         user_data);

void
on_rasterize_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);



void
on_warp_mode_toggled                   (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_save_error_log_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_save_error_log_ok_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_terrain_mosaic_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_x_mosaicsize_changed                (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_y_mosaicsize_changed                (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_spherical_map_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_undo_apply_clicked                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_tile_assemble_terrains_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_join_reverse_toggled                (GtkButton       *button,
                                        gpointer         user_data);

void
on_terrain_export_gimp_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_view_export_file_activate   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_view_export_gimp_activate   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_terrain_options_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_render_options_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_scene_options_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_options_ok_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_terrain_options_apply_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_scene_options_ok_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_scene_options_apply_clicked         (GtkButton       *button,
                                        gpointer         user_data);

void
on_terrain_options_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_options_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
on_use_antialiasing_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_custom_size_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_use_jitter_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_povray_render_options_ok_clicked    (GtkButton       *button,
                                        gpointer         user_data);

void
on_prune_ok_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_terrain_river_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

GtkWidget*
create_terrain_preview_camera          (gchar           *widget_name, 
                                        gchar           *string1, 
                                        gchar           *string2,
                                        gint             int1, 
                                        gint             int2);

void
on_observe_sealevel_checkbutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_povray_render_options_apply_clicked (GtkButton       *button,
                                        gpointer         user_data);

void
on_terrain_all_rivers_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_redraw_rivers_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_remove_all_rivers_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_method_toggled                      (GtkToggleButton *togglebutton,
                                        gpointer         user_data);


void
on_render_clouds_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_scene_options_cancel_clicked        (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_scene_options_window_destroy_event  (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_scene_options_window_delete_event   (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_scene_options_window_destroy2       (GtkObject       *object,
                                        gpointer         user_data);

gboolean
on_scene_options_delete_event          (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_terrain_fill_basins_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_rasterize_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_roughness_map_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_spherical_map_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_place_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_prune_placed_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_rescale_placed_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_remove_all_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_terrain_remove_selected_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
on_checkbutton_file_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_checkbutton_partial_render_toggled  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_checkbutton_percentage_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_add_single_object1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_remove_single_object1_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_trace_ok_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_single_trace_object1_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_render_more_background_clicked      (GtkButton       *button,
                                        gpointer         user_data);

void
on_use_fog_toggled                      (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_use_ground_fog_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data);
void
on_render_stars_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_render_celest_objects_toggled       (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_render_scene_clouds_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_render_camera_light_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_render_filled_sea_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_render_camera_presets_apply_clicked (GtkButton       *button,
                                        gpointer         user_data);

void
on_render_texture_waterborder_ice_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_render_texture_waterborder_sand_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_render_texture_waterborder_toggled  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_render_texture_stratum_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_render_texture_constructor_toggled  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_render_texture_presets_apply_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_render_radiosity_toggled        (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_render_scene_options_help_clicked   (GtkButton       *button,
                                        gpointer         user_data);


GtkWidget*
create_main_list (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

GtkWidget*
create_undo_tree (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

GtkWidget*
create_merge_list (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

GtkWidget*
create_join_list (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

GtkWidget*
create_erode_list (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);

GtkWidget*
create_warp_list (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2);
