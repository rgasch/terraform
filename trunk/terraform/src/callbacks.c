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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <gnome.h>
#include <libgnome/gnome-help.h>
#if 0
#include <libgnomeprint/gnome-print.h>
#include <libgnomeprint/gnome-print-master.h>
#include <libgnomeprintui/gnome-printer-dialog.h>
#include <libgnomeprintui/gnome-print-preview.h>
#endif

#include "callbacks.h"
#include "filenameops.h"
#include "filelist.h"
#include "filters.h"
#include "filterwindow.h"
#include "genfault.h"
#include "genperlin.h"
#include "genspectral.h"
#include "gensubdiv.h"
#include "genrandom.h"
#include "interface.h"
#include "mainwindow.h"
#include "place.h"
#include "optionwindow.h"
#include "smartexec.h"
#include "selection.h"
#include "statistics.h"
#include "support.h"
#include "support2.h"
#include "reader.h"
#include "taspectframe.h"
#include "terrainwindow.h"
#include "tterrainview.h"
#include "tterrain.h"
#include "trandom.h"
#include "tundo.h"
#include "tworld.h"
#include "writer.h"
#include "exportview.h"

#define CLIP(x,m,M) MIN (MAX (x, m), M)

typedef struct
{
  gchar     *description;
  gchar     *pattern;
  TFileType  type;
} FileTypeInfo;

FileTypeInfo save_as_file_type_info[] =
{
  { "Automatic (*)", "*", FILE_AUTO },
  { "", "", FILE_UNKNOWN },
  { "Terraform (*.terraform)", "*.terraform", FILE_NATIVE },
  { "Autocad (*.ac)", "*.ac", FILE_AC },
  { "Autocad DXF (*.dxf)", "*.dxf", FILE_DXF },
  { "Binary Terrain (*.bt)", "*.bt", FILE_BT },
  { "Bitmap (*.bmp)", "*.bmp", FILE_BMP },
  { "Boundary Named ASCII (*.bna)", "*.bna", FILE_BNA},
  { "Greyscale Bitmap (*.bmp)", "*.bmp", FILE_BMP_BW },
  { "Gridded (*.grd)", "*.grd", FILE_GRD},
  { "Matlab (*.mat)", "*.mat", FILE_MAT },
  { "Oct (*.oct)", "*.oct", FILE_OCT },
  { "Pgm Binary (*.pgm)", "*.pgm", FILE_PGM },
  { "Pg8 Ascii (*.pg8)", "*.pg8", FILE_PG8 },
  { "Targa (*.tga)", "*.tga", FILE_TGA },
  { "Terragen (*.ter)", "*.ter", FILE_TERRAGEN },
  { "XYZ ASCII (*.xyz)", "*.xyz", FILE_XYZ },
  { NULL, NULL, FILE_UNKNOWN }
};

FileTypeInfo export_view_file_type_info[] =
{
#ifdef HAVE_LIBPNG
  { "Network Graphics (*.png)", "*.png", FILE_PNG },
  { "VRML (*.vrml)", "*.vrml", FILE_VRML },
#endif /* HAVE_LIBPNG */
  { "Automatic (*)", "*", FILE_AUTO },
  { "", "", FILE_UNKNOWN },
  { NULL, NULL, FILE_UNKNOWN }
};

FileTypeInfo open_file_type_info[] =
{
  { "Automatic (*)", "*", FILE_AUTO },
  { "", "", FILE_UNKNOWN },
  { "Terraform (*.terraform)", "*.terraform", FILE_NATIVE },
  { "Autocad DXF (*.dxf)", "*.dxf", FILE_DXF },
  { "Binary Terrain (*.bt)", "*.bt", FILE_BT},
  { "Bitmap (*.bmp)", "*.bmp", FILE_BMP },
  { "Boundary Named ASCII (*.bna)", "*.bna", FILE_BNA},
  { "Gridded (*.grd)", "*.grd", FILE_GRD},
  { "Gtopo (*.dem)", "*.dem", FILE_GTOPO },
  { "Matlab (*.mat)", "*.mat", FILE_MAT },
  { "Oct (*.oct)", "*.oct", FILE_OCT },
  { "Pgm Binary (*.pgm)", "*.pgm", FILE_PGM },
  { "Pg8 Ascii (*.pg8)", "*.pg8", FILE_PG8 },
  { "Targa (*.tga)", "*.tga", FILE_TGA },
  { "Terragen (*.ter)", "*.ter", FILE_TERRAGEN },
  { "XYZ ASCII (*.xyz)", "*.xyz", FILE_XYZ },
  { "Arc/Info e00 grid (*.e00)", "*.e00", FILE_E00 },

#ifdef HAVE_GDKPIXBUF
  { "Jpeg (*.jpg)", "*.jpg", FILE_JPG },
  { "Network Graphics (*.png)", "*.png", FILE_PNG },
  { "RAS (*.ras)", "*.ras", FILE_RAS },
  { "Tiff (*.tif)", "*.tif", FILE_TIF },
  { "X Bitmap (*.xbm)", "*.xbm", FILE_XBM },
  { "X Pixmap (*.xpm)", "*.xpm", FILE_XPM },
#endif /* HAVE_GDKPIXBUF */

  { NULL, NULL, FILE_UNKNOWN }
};


/*
 * create_file_options_menu:
 *   Create a GtkMenu widget given a FileTypeInfo array.
 */

static GtkWidget *
create_file_options_menu (FileTypeInfo *info)
{
  GtkWidget *menu;
  GtkWidget *item;
  int        i;

  menu = gtk_menu_new ();

  for (i = 0; info[i].description != NULL; i++)
    {
      if (info[i].description[0] == '\0')
        item = gtk_menu_item_new ();
      else
        item = gtk_menu_item_new_with_label (info[i].description);

      gtk_container_add (GTK_CONTAINER (menu), item);
      gtk_widget_show (item);
    }

  return menu;
}


static void
on_file_type_selected (GtkWidget *menu,
                       gpointer   user_data)
{
  GtkWidget    *file_sel;
  /* const */ gchar  *pattern;
  FileTypeInfo *info;

  file_sel = lookup_toplevel (menu);
  info = gtk_object_get_data (GTK_OBJECT (file_sel), "data_file_type_info");
  pattern = info[get_option (file_sel, "type_menu")].pattern;

  gtk_file_selection_complete (GTK_FILE_SELECTION (file_sel), pattern);
}


static void
add_file_type_menu (GtkWidget    *filesel,
                    FileTypeInfo *info)
{
  GtkWidget *frame;
  GtkWidget *file_options_window;
  GtkWidget *file_options_menu;
  GtkWidget *menu;

  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);

  /* Cannibalize the "file_options" object from the file_option_window... */
  file_options_window = create_file_options_window ();
  gtk_widget_reparent (lookup_widget (file_options_window, "file_options"),
                       frame);
  file_options_menu = lookup_widget (file_options_window, "type_menu");

  menu = create_file_options_menu (info);
  gtk_signal_connect (GTK_OBJECT (menu),
                      "deactivate",
                      GTK_SIGNAL_FUNC (on_file_type_selected),
                      NULL);

  gtk_option_menu_set_menu (GTK_OPTION_MENU (file_options_menu), menu);
  gtk_widget_destroy (file_options_window);

  /* ... and pack it into the file selection dialog box */
  gtk_box_pack_end (GTK_BOX (GTK_FILE_SELECTION (filesel)->main_vbox),
                    frame, FALSE, FALSE, 5);

  gtk_object_set_data (GTK_OBJECT (filesel), "type_menu", file_options_menu);
  gtk_object_set_data (GTK_OBJECT (filesel), "data_file_type_info", info);

  gtk_widget_show_all (frame);
}


void
on_main_file_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_main_new_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


static void 
on_file_save_error                     (gchar *fname)
{
  GtkWidget *error_dialog;
  gchar      buf[256];

  if (fname)
    snprintf (buf, 256, _("An error occurred while saving the file:\n%s."), 
              fname);
  else
    snprintf (buf, 256, "%s", 
              _("An error occurred while saving the file.")); 

  error_dialog = gnome_error_dialog (buf);
  gtk_widget_show (error_dialog);
}


static gchar *perlin_first[] =
{
  "first", "first_frequency", "first_amplitude",
  "first_iterations", "first_filter"
};

static gchar *perlin_second[] =
{
  "second", "second_frequency", "second_amplitude",
  "second_iterations", "second_filter"
};

static gchar *perlin_third[] =
{
  "third", "third_frequency", "third_amplitude",
  "third_iterations", "third_filter"
};

static gchar *perlin_fourth[] =
{
  "fourth", "fourth_frequency", "fourth_amplitude",
  "fourth_iterations", "fourth_filter"
};

static gchar **perlin_rows[] =
{
  perlin_first, perlin_second, perlin_third, perlin_fourth
};

typedef struct PerlinData PerlinData;

struct PerlinData
{
  gboolean enabled[4];
  gfloat   frequency[4];
  gfloat   amplitude[4];
  gint     iterations[4];
  gint     filter[4];
  gint     size;
  gboolean new_seed;
  gint     seed;
};


static void
perlin_pack (GtkWidget  *perlin_window,
             PerlinData *data)
{
  gint i;

  for (i = 0; i < 4; i++)
    {
      data->enabled[i] = get_boolean (perlin_window, perlin_rows[i][0]);
      data->frequency[i] = get_float (perlin_window, perlin_rows[i][1]);
      data->amplitude[i] = get_float (perlin_window, perlin_rows[i][2]);
      data->iterations[i] = get_int (perlin_window, perlin_rows[i][3]);
      data->filter[i] = get_option (perlin_window, perlin_rows[i][4]);
    }

  data->size = get_int (perlin_window, "pn_size");
  data->new_seed = get_boolean (perlin_window, "new_seed");
  data->seed = get_int (perlin_window, "seed");
}


static void
perlin_unpack (GtkWidget  *perlin_window,
               PerlinData *data)
{
  gint i;

  for (i = 0; i < 4; i++)
    {
      set_boolean (perlin_window, perlin_rows[i][0], data->enabled[i]);
      set_float (perlin_window, perlin_rows[i][1], data->frequency[i]);
      set_float (perlin_window, perlin_rows[i][2], data->amplitude[i]);
      set_int (perlin_window, perlin_rows[i][3], data->iterations[i]);
      set_option (perlin_window, perlin_rows[i][4], data->filter[i]);
    }

  set_int (perlin_window, "pn_size", data->size);
  set_boolean (perlin_window, "new_seed", data->new_seed);
  set_int (perlin_window, "seed", data->seed);
}


void
on_main_perlin_noise_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget  *perlin_noise;
  GtkWidget  *main_window;
  MainState  *main_state;
  PerlinData *data;

  perlin_noise = create_perlin_noise_window ();
  main_window = lookup_toplevel (GTK_WIDGET (menuitem));
  main_state = lookup_data (main_window, "data_state");
  set_int (perlin_noise, "pn_size", main_state->default_size);

  data = gtk_object_get_data (GTK_OBJECT (main_window),
                              "data/generator/perlin");
  if (data != NULL)
    perlin_unpack (perlin_noise, data);

  gtk_object_set_data (GTK_OBJECT (perlin_noise), "data_parent", main_window);
  gtk_widget_show (perlin_noise);
}


void
on_perlin_toggled                      (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  gint i, j;

  for (i = 0; i < 4; i++)
    {
      gboolean on;

      on = get_boolean (GTK_WIDGET (togglebutton), perlin_rows[i][0]);

      for (j = 1; j < 5; j++)
        gtk_widget_set_sensitive (lookup_widget (GTK_WIDGET (togglebutton),
                                  perlin_rows[i][j]), on);
    }
}


typedef struct SpectralData SpectralData;

struct SpectralData
{
  gint     size;
  gfloat   dimensions;
  gboolean invert;
  gboolean new_seed;
  gint     seed;
};


static void
spectral_pack (GtkWidget    *spectral_window,
               SpectralData *data)
{
  data->size = get_int (spectral_window, "ss_size");
  data->dimensions = get_float (spectral_window, "ss_fractal_dimensions"); 
  data->invert = get_boolean (spectral_window, "ss_invert_trig");
  data->new_seed = get_boolean (spectral_window, "new_seed");
  data->seed = get_int (spectral_window, "seed");
}


static void
spectral_unpack (GtkWidget    *spectral_window,
                 SpectralData *data)
{
  set_int (spectral_window, "ss_size", data->size);
  set_float (spectral_window, "ss_fractal_dimensions", data->dimensions); 
  set_boolean (spectral_window, "ss_invert_trig", data->invert);
  set_boolean (spectral_window, "new_seed", data->new_seed);
  set_int (spectral_window, "seed", data->seed);
}


void
on_main_spectral_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget    *spectral;
  GtkWidget    *main_window;
  MainState    *main_state;
  SpectralData *data;

  spectral = create_spectral_window ();
  main_window = lookup_toplevel (GTK_WIDGET (menuitem));
  main_state = lookup_data (main_window, "data_state");
  set_int (spectral, "ss_size", main_state->default_size);

  data = gtk_object_get_data (GTK_OBJECT (main_window),
                              "data/generator/spectral");
  if (data != NULL)
    spectral_unpack (spectral, data);

  gtk_object_set_data (GTK_OBJECT (spectral), "data_parent", main_window);
  gtk_widget_show (spectral);
}


typedef struct SubdivisionData SubdivisionData;

struct SubdivisionData
{
  gint     method;
  gint     size;
  gfloat   scale;
  gboolean new_seed;
  gint     seed;
};


static void
subdivision_pack (GtkWidget       *subdivision_window,
                  SubdivisionData *data)
{
  data->method = get_option (subdivision_window, "sub_method");
  data->size = get_int (subdivision_window, "sub_size");
  data->scale = get_float (subdivision_window, "sub_scale_factor"); 
  data->new_seed = get_boolean (subdivision_window, "new_seed");
  data->seed = get_int (subdivision_window, "seed");
}


static void
subdivision_unpack (GtkWidget       *subdivision_window,
                    SubdivisionData *data)
{
  set_option (subdivision_window, "sub_method", data->method);
  set_int (subdivision_window, "sub_size", data->size);
  set_float (subdivision_window, "sub_scale_factor", data->scale); 
  set_boolean (subdivision_window, "new_seed", data->new_seed);
  set_int (subdivision_window, "seed", data->seed);
}


void
on_main_subdivision_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget       *subdivision;
  GtkWidget       *main_window;
  MainState       *main_state;
  SubdivisionData *data;

  subdivision = create_subdivision_window ();
  main_window = lookup_toplevel (GTK_WIDGET (menuitem));
  main_state = lookup_data (main_window, "data_state");
  set_int (subdivision, "sub_size", main_state->default_size);

  data = gtk_object_get_data (GTK_OBJECT (main_window),
                              "data/generator/subdivision");
  if (data != NULL)
    subdivision_unpack (subdivision, data);

  gtk_object_set_data (GTK_OBJECT (subdivision), "data_parent", main_window);
  gtk_widget_show (subdivision);
}


typedef struct FaultingData FaultingData;

struct FaultingData
{
  gint     method;
  gint     size;
  gint     iterations;
  gfloat   scale;
  gint     cycles;
  gboolean constant_size;
  gboolean new_seed;
  gint     seed;
};

static void
faulting_pack (GtkWidget    *faulting_window,
               FaultingData *data)
{
  data->method = get_option (faulting_window, "fault_method");
  data->size = get_int (faulting_window, "fault_size");
  data->iterations = get_float (faulting_window, "fault_iterations"); 
  data->scale = get_float (faulting_window, "fault_scale_factor"); 
  data->cycles = get_float (faulting_window, "fault_cycles"); 
  data->constant_size = get_boolean (faulting_window, "constant_size"); 
  data->new_seed = get_boolean (faulting_window, "new_seed");
  data->seed = get_int (faulting_window, "seed");
}


static void
faulting_unpack (GtkWidget    *faulting_window,
                 FaultingData *data)
{
  set_option (faulting_window, "fault_method", data->method);
  set_int (faulting_window, "fault_size", data->size);
  set_float (faulting_window, "fault_iterations", data->iterations); 
  set_float (faulting_window, "fault_scale_factor", data->scale); 
  set_float (faulting_window, "fault_cycles", data->cycles); 
  set_boolean (faulting_window, "constant_size", data->constant_size); 
  set_boolean (faulting_window, "new_seed", data->new_seed);
  set_int (faulting_window, "seed", data->seed);
}


void
on_main_faulting_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  FaultingData *data;
  GtkWidget    *faulting;
  GtkWidget    *main_window;
  GtkWidget    *option_menu;
  MainState    *main_state;

  faulting = create_faulting_window ();
  main_window = lookup_toplevel (GTK_WIDGET (menuitem));
  main_state = lookup_data (main_window, "data_state");
  set_int (faulting, "fault_size", main_state->default_size);

  data = gtk_object_get_data (GTK_OBJECT (main_window),
                              "data/generator/faulting");
  if (data != NULL)
    faulting_unpack (faulting, data);

  option_menu = lookup_widget (faulting, "fault_method");
  gtk_signal_connect (GTK_OBJECT (GTK_OPTION_MENU (option_menu)->menu),
                      "deactivate", GTK_SIGNAL_FUNC (on_fault_option_selected),
		      NULL);

  gtk_object_set_data (GTK_OBJECT (faulting), "data_parent", main_window);
  gtk_widget_show (faulting);
}


void
on_main_random_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  TTerrain  *terrain;
  GtkWidget *terrain_window;
  GtkWidget *main_window;
  MainState *main_state;

  main_window = lookup_toplevel (GTK_WIDGET (menuitem));
  main_state = lookup_data (main_window, "data_state");

  terrain = t_terrain_generate_random (main_state->default_size, main_state->genfactor, main_state->gentypes);
  terrain_window = terrain_window_new (main_window, terrain);
  terrain_window_save_state (terrain_window, _("Generate Random"));
}


void
on_main_open_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *open_file;
  GtkWidget *main_window;

  main_window = lookup_toplevel (GTK_WIDGET (menuitem));
  open_file = create_open_file ();
  add_file_type_menu (open_file, open_file_type_info);
  gtk_object_set_data (GTK_OBJECT (open_file), "data_parent", main_window);
  gtk_widget_show (open_file);
}


static void
join_apply                             (GtkWidget       *join_window,
                                        gboolean         preview)
{
  TTerrain  *terrain_1;
  TTerrain  *terrain_2;
  TTerrain  *terrain;
  GtkWidget *main_window;
  gfloat     distance;
  gboolean   reverse_direction;
  gboolean   reverse_axis;

  if (preview)
    {
      GtkWidget *view;

      if (get_boolean (join_window, "use_preview") == FALSE)
        return;

      view = lookup_widget (join_window, "top_thumbnail");
      terrain_1 = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

      view = lookup_widget (join_window, "bottom_thumbnail");
      terrain_2 = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
    }
  else
    {
      terrain_1 = gtk_object_get_data (GTK_OBJECT (join_window),
                                       "data_top_t_terrain");
      terrain_2 = gtk_object_get_data (GTK_OBJECT (join_window),
                                       "data_bottom_t_terrain");
    }

  distance = get_float (join_window, "distance");
  reverse_direction = get_boolean (join_window, "reverse_direction");
  reverse_axis = get_boolean (join_window, "reverse_axis");

  terrain = t_terrain_join (terrain_1, terrain_2, distance, 
                            reverse_direction, !reverse_axis);
  t_terrain_normalize (terrain, FALSE);

  if (preview)
    {
      TTerrainView *view;

      view = T_TERRAIN_VIEW (lookup_widget (join_window, "preview"));
      t_terrain_view_set_terrain (view, terrain);
    }
  else
    {
      GtkWidget *terrain_window;

      main_window = lookup_widget (join_window, "data_parent");
      terrain_window = terrain_window_new (main_window, terrain);
      terrain_window_save_state (terrain_window, _("Join"));
    }
}

static void
join_apply_preview                     (GtkWidget       *join_window,
                                        TTerrain        *terrain)
{
  join_apply (join_window, TRUE);
}


static void
on_join_adjustment_changed             (GtkAdjustment   *adjustment,
                                        gpointer         join_window)
{
  join_apply (join_window, TRUE);
}


void
on_main_join_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget     *view;
  TTerrain      *terrain;
  GtkAdjustment *adjustment;
  GtkWidget     *terrain_window;
  GtkWidget     *join_window;
  GtkWidget     *main_window;
  GtkWidget     *area;
  GtkWidget     *frame;
  gchar         *text;

  main_window = lookup_toplevel (GTK_WIDGET (menuitem));

  join_window = create_join_window ();

  gtk_object_set_data (GTK_OBJECT (join_window), "data_filter_apply_func",
                       &join_apply_preview);

  gtk_object_set_data (GTK_OBJECT (join_window), "data_parent", main_window);

  area = lookup_widget (join_window, "top_thumbnail");
  terrain_window = main_window_get_terrain_window (main_window, 0);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (join_window),
                            "data_top_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  frame = lookup_widget (join_window, "top_frame");
  text = t_terrain_get_title (terrain);
  gtk_frame_set_label (GTK_FRAME (frame), text);
  g_free (text);

  terrain = t_terrain_clone_preview (terrain);
  area = lookup_widget (join_window, "top_thumbnail");
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (area), terrain);

  terrain_window = main_window_get_terrain_window (main_window, 1);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (join_window),
                            "data_bottom_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  frame = lookup_widget (join_window, "bottom_frame");
  text = t_terrain_get_title (terrain);
  gtk_frame_set_label (GTK_FRAME (frame), text);
  g_free (text);

  terrain = t_terrain_clone_preview (terrain);
  area = lookup_widget (join_window, "bottom_thumbnail");
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (area), terrain);

  adjustment =
    GTK_RANGE (lookup_widget (join_window, "distance"))->adjustment;
  gtk_signal_connect (GTK_OBJECT (adjustment), "value_changed",
                      (GtkSignalFunc) &on_join_adjustment_changed,
                      join_window);

  area = lookup_widget (join_window, "preview");

  main_window_copy_objects (main_window,
    GTK_CLIST (lookup_widget (join_window, "join_list")));

  join_apply (join_window, TRUE);
  gtk_widget_show (join_window);
}


static void
warp_apply                             (GtkWidget       *warp_window,
                                        gboolean         preview)
{
  TTerrain  *terrain_1;
  TTerrain  *terrain_2;
  TTerrain  *terrain;
  GtkWidget *main_window;
  gfloat     factor;
  gfloat     center_x;
  gfloat     center_y;
  gboolean   mode;

  if (preview)
    {
      GtkWidget *view;

      if (get_boolean (warp_window, "use_preview") == FALSE)
        return;

      view = lookup_widget (warp_window, "top_thumbnail");
      terrain_1 = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

      view = lookup_widget (warp_window, "bottom_thumbnail");
      terrain_2 = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
    }
  else
    {
      terrain_1 = gtk_object_get_data (GTK_OBJECT (warp_window),
                                       "data_top_t_terrain");
      terrain_2 = gtk_object_get_data (GTK_OBJECT (warp_window),
                                       "data_bottom_t_terrain");
    }

  factor = get_float (warp_window, "factor");
  center_x = get_float (warp_window, "center_x");
  center_y = get_float (warp_window, "center_y");
  mode = get_boolean (warp_window, "mode");

  terrain = t_terrain_warp (terrain_1, terrain_2, center_x, center_y, 
		            mode, factor);
  t_terrain_normalize (terrain, FALSE);

  if (preview)
    {
      TTerrainView *view;

      view = T_TERRAIN_VIEW (lookup_widget (warp_window, "preview"));
      t_terrain_view_set_terrain (view, terrain);
    }
  else
    {
      GtkWidget *terrain_window;

      main_window = lookup_widget (warp_window, "data_parent");
      terrain_window = terrain_window_new (main_window, terrain);
      terrain_window_save_state (terrain_window, _("Warp"));
    }
}


static void
warp_apply_preview                     (GtkWidget       *warp_window,
                                        TTerrain        *terrain)
{
  warp_apply (warp_window, TRUE);
}


static void
on_warp_adjustment_changed             (GtkAdjustment   *adjustment,
                                        gpointer         warp_window)
{
  warp_apply (warp_window, TRUE);
}


void
on_warp_mode_toggled                   (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *warp_window;

  warp_window = lookup_toplevel (GTK_WIDGET (togglebutton));
  warp_apply (warp_window, TRUE);
}


void
on_main_warp_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget     *view;
  TTerrain      *terrain;
  GtkAdjustment *adjustment;
  GtkWidget     *terrain_window;
  GtkWidget     *warp_window;
  GtkWidget     *main_window;
  GtkWidget     *area;
  GtkWidget     *frame;
  gchar         *text;

  main_window = lookup_toplevel (GTK_WIDGET (menuitem));

  warp_window = create_warp_window ();
  filter_window_add_crosshairs (warp_window, "top_thumbnail",
                                "center_x", "center_y", NULL);

  gtk_object_set_data (GTK_OBJECT (warp_window), "data_filter_apply_func",
                       &warp_apply_preview);

  gtk_object_set_data (GTK_OBJECT (warp_window), "data_parent", main_window);

  area = lookup_widget (warp_window, "top_thumbnail");
  terrain_window = main_window_get_terrain_window (main_window, 0);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (warp_window),
                            "data_top_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  frame = lookup_widget (warp_window, "top_frame");
  text = t_terrain_get_title (terrain);
  gtk_frame_set_label (GTK_FRAME (frame), text);
  g_free (text);

  terrain = t_terrain_clone_preview (terrain);
  area = lookup_widget (warp_window, "top_thumbnail");
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (area), terrain);

  terrain_window = main_window_get_terrain_window (main_window, 1);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (warp_window),
                            "data_bottom_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  frame = lookup_widget (warp_window, "bottom_frame");
  text = t_terrain_get_title (terrain);
  gtk_frame_set_label (GTK_FRAME (frame), text);
  g_free (text);

  terrain = t_terrain_clone_preview (terrain);
  area = lookup_widget (warp_window, "bottom_thumbnail");
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (area), terrain);

  adjustment =
    GTK_RANGE (lookup_widget (warp_window, "factor"))->adjustment;
  gtk_signal_connect (GTK_OBJECT (adjustment), "value_changed",
                      (GtkSignalFunc) &on_warp_adjustment_changed,
                      warp_window);
  adjustment =
    GTK_RANGE (lookup_widget (warp_window, "center_x"))->adjustment;
  gtk_signal_connect (GTK_OBJECT (adjustment), "value_changed",
                      (GtkSignalFunc) &on_warp_adjustment_changed,
                      warp_window);
  adjustment =
    GTK_RANGE (lookup_widget (warp_window, "center_y"))->adjustment;
  gtk_signal_connect (GTK_OBJECT (adjustment), "value_changed",
                      (GtkSignalFunc) &on_warp_adjustment_changed,
                      warp_window);

  area = lookup_widget (warp_window, "preview");

  main_window_copy_objects (main_window,
    GTK_CLIST (lookup_widget (warp_window, "warp_list")));

  warp_apply (warp_window, TRUE);
  gtk_widget_show (warp_window);
}


static void
merge_apply                            (GtkWidget       *merge_window,
                                        gboolean         preview)
{
  TTerrain  *terrain_1;
  TTerrain  *terrain_2;
  TTerrain  *terrain;
  GtkWidget *main_window;
  gint       operator;
  gfloat     weight_1;
  gfloat     weight_2;

  if (preview)
    {
      GtkWidget *view;

      if (get_boolean (merge_window, "use_preview") == FALSE)
        return;

      view = lookup_widget (merge_window, "top_thumbnail");
      terrain_1 = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

      view = lookup_widget (merge_window, "bottom_thumbnail");
      terrain_2 = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
    }
  else
    {
      terrain_1 = gtk_object_get_data (GTK_OBJECT (merge_window),
                                       "data_top_t_terrain");
      terrain_2 = gtk_object_get_data (GTK_OBJECT (merge_window),
                                       "data_bottom_t_terrain");
    }

  operator = get_option (merge_window, "merge_op");
  weight_1 = get_float (merge_window, "top_amount");
  weight_2 = get_float (merge_window, "bottom_amount");

  terrain =
    t_terrain_merge (terrain_1, terrain_2, weight_1, weight_2, operator);
  t_terrain_normalize (terrain, FALSE);

  if (preview)
    {
      TTerrainView *view;

      view = T_TERRAIN_VIEW (lookup_widget (merge_window, "preview"));

      t_terrain_view_set_terrain (view, terrain);
    }
  else
    {
      GtkWidget *terrain_window;

      main_window = lookup_widget (merge_window, "data_parent");
      terrain_window = terrain_window_new (main_window, terrain);
      terrain_window_save_state (terrain_window, _("Merge"));
    }
}


static void
merge_apply_preview                    (GtkWidget       *merge_window,
                                        TTerrain        *terrain)
{
  merge_apply (merge_window, TRUE);
}


static void
on_merge_adjustment_changed            (GtkAdjustment   *adjustment,
                                        gpointer         merge_window)
{
  merge_apply (merge_window, TRUE);
}


void
on_main_merge_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget     *view;
  TTerrain      *terrain;
  GtkAdjustment *adjustment;
  GtkWidget     *terrain_window;
  GtkWidget     *merge_window;
  GtkWidget     *main_window;
  GtkWidget     *area;
  GtkWidget     *option_menu;
  GtkWidget     *frame;
  gchar         *text;

  main_window = lookup_toplevel (GTK_WIDGET (menuitem));

  merge_window = create_merge_window ();

  option_menu = lookup_widget (merge_window, "merge_op");
  gtk_signal_connect (GTK_OBJECT (GTK_OPTION_MENU (option_menu)->menu),
                      "deactivate",
                      GTK_SIGNAL_FUNC (on_merge_adjustment_changed),
                      merge_window);

  gtk_object_set_data (GTK_OBJECT (merge_window), "data_filter_apply_func",
                       &merge_apply_preview);

  gtk_object_set_data (GTK_OBJECT (merge_window), "data_parent", main_window);

  area = lookup_widget (merge_window, "top_thumbnail");
  terrain_window = main_window_get_terrain_window (main_window, 0);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (merge_window),
                            "data_top_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  frame = lookup_widget (merge_window, "top_frame");
  text = t_terrain_get_title (terrain);
  gtk_frame_set_label (GTK_FRAME (frame), text);
  g_free (text);

  terrain = t_terrain_clone_preview (terrain);
  area = lookup_widget (merge_window, "top_thumbnail");
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (area), terrain);

  terrain_window = main_window_get_terrain_window (main_window, 1);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (merge_window),
                            "data_bottom_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  frame = lookup_widget (merge_window, "bottom_frame");
  text = t_terrain_get_title (terrain);
  gtk_frame_set_label (GTK_FRAME (frame), text);
  g_free (text);

  terrain = t_terrain_clone_preview (terrain);
  area = lookup_widget (merge_window, "bottom_thumbnail");
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (area), terrain);

  adjustment =
    GTK_RANGE (lookup_widget (merge_window, "top_amount"))->adjustment;
  gtk_signal_connect (GTK_OBJECT (adjustment), "value_changed",
                      (GtkSignalFunc) &on_merge_adjustment_changed,
                      merge_window);

  adjustment =
    GTK_RANGE (lookup_widget (merge_window, "bottom_amount"))->adjustment;
  gtk_signal_connect (GTK_OBJECT (adjustment), "value_changed",
                      (GtkSignalFunc) &on_merge_adjustment_changed,
                      merge_window);


  area = lookup_widget (merge_window, "preview");

  main_window_copy_objects (main_window,
    GTK_CLIST (lookup_widget (merge_window, "merge_list")));

  merge_apply (merge_window, TRUE);
  gtk_widget_show (merge_window);
}


static void
gamma_preview_update (GtkWidget *option_window)
{
  GtkWidget *canvas;
  TPixbuf   *pixbuf;
  gfloat     gamma;
  gint       value;

  canvas = lookup_widget (option_window, "gamma_preview");
  pixbuf = t_canvas_get_pixbuf (T_CANVAS (canvas));
  gamma = get_float (option_window, "gamma");

  value = rint (pow (0.5, 1.0 / gamma) * 255.0);
  t_pixbuf_set_fore (pixbuf, value, value, value);
  t_pixbuf_fill_rect (pixbuf, 0, 0, 49, 49);
  gtk_widget_queue_draw (canvas);
}


void
on_gamma_preview_realize               (GtkWidget       *widget,
                                        gpointer         user_data)
{
  TPixbuf *pixbuf;
  gint     x, y;

  pixbuf = t_canvas_get_pixbuf (T_CANVAS (widget));
  for (y = 0; y < 50; y++)
    for (x = 50; x < 100; x++)
      if (((x >> 1) + (y >> 1)) & 1)
        t_pixbuf_draw_pel (pixbuf, x, y);

  gamma_preview_update (lookup_toplevel (widget));
}


GtkWidget*
create_gamma_preview (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
  GtkWidget *widget;

  widget = t_canvas_new ();
  gtk_widget_set_size_request (widget, 100, 50);
  gtk_widget_show_all (widget);

  return widget;
}


void
on_main_options_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  static GtkWidget *download_povray;
  GtkWidget *main_window;
  GtkWidget *options;
  GtkWidget *gamma;
  GtkWidget *vbox_hyperlink;

  main_window = lookup_toplevel (GTK_WIDGET (menuitem));
  options = create_options_window ();

  vbox_hyperlink = lookup_widget (options, "vbox43");
  download_povray = gnome_href_new ("http://www.povray.org/download/",
				    _("Download POV-Ray..."));
  gtk_box_pack_start (GTK_BOX (vbox_hyperlink),
		      download_povray, FALSE, FALSE, 0);

  gtk_object_set_data (GTK_OBJECT (options), "data_parent", main_window);
  main_window_state_unpack_options (main_window, options);

  gamma = lookup_widget (options, "gamma");
  gtk_signal_connect_object (
            GTK_OBJECT (GTK_RANGE (gamma)->adjustment),
            "value_changed",
            GTK_SIGNAL_FUNC (&gamma_preview_update),
            GTK_OBJECT (options));

  gtk_widget_show (download_povray);
  gtk_widget_show (options);
}


void
on_main_print_settings_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
}


void
on_main_exit_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  main_window_exit_sequence (lookup_toplevel (GTK_WIDGET (menuitem)));
}


void
on_main_help_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_main_about_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  static GtkWidget *homepage, *development_site;
  GtkWidget *hbox_hyperlinks;
  GtkWidget *about_window = create_about_window ();
  gchar     *version, *label_text;

  hbox_hyperlinks = lookup_widget (about_window, "hbox_hyperlinks");

  homepage = gnome_href_new ("http://terraform.sourceforge.net/",
			     _("Terraform homepage"));
  development_site = gnome_href_new ("http://www.sourceforge.net/projects/terraform/",
				     _("Terraform development site"));

  gtk_box_pack_start (GTK_BOX (hbox_hyperlinks),
		      homepage, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox_hyperlinks),
		      development_site, FALSE, FALSE, 0);

  version = g_strdup(VERSION);
  label_text = g_strdup_printf (_("Development Version: %s\nLicensed under the GPL"), version);
  set_text (about_window, "about_terraform_label", label_text);

  gtk_widget_show (homepage);
  gtk_widget_show (development_site);
  gtk_widget_show (about_window);

  g_free (version);
  g_free (label_text);
}


void
on_terrain_save_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;
  gint       status;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  status = t_terrain_save (terrain, FILE_NATIVE, NULL);

  if (status != 0)
    on_file_save_error (terrain->filename);
}


void
on_terrain_save_as_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *save_as;
  GtkWidget *terrain_window;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  save_as = create_save_as ();
  add_file_type_menu (save_as, save_as_file_type_info);
  gtk_object_set_data (GTK_OBJECT (save_as), "data_parent", terrain_window);
  gtk_widget_show (save_as);
}


typedef GList Cleanup;

/* cleanup(): Treat gpointer as a GList of g_malloc'd gchar*
   file/directory names.  g_free each string afterwards.
   This function is used to delete temporary files. */

static void
cleanup (gpointer user_data)
{
  Cleanup *list = user_data;

  list = g_list_first (list);
  while (list != NULL)
    {
      Cleanup *next;

      unlink ((gchar*) list->data);
      rmdir ((gchar*) list->data);
      g_free (list->data);

      next = list->next;
      g_list_free_1 (list);
      list = next;
    }

  g_list_free (list);
}


void
on_terrain_export_gimp_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *view;
  GtkWidget *terrain_window;
  TTerrain  *terrain;
  gchar     *tmpdir;
  gchar     *tmp, *base, *bmp;
  gchar     *command;
  GList     *cleanup_list;
  gint       rc;

  cleanup_list = NULL;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  /* Create a safe temp directory */
  tmpdir = filename_create_tempdir ();
  if (tmpdir == NULL)
    {
      on_file_save_error (terrain->filename);
      return;
    }
  cleanup_list = g_list_prepend (cleanup_list, tmpdir);

  base = filename_get_base (terrain->filename);
  tmp = g_strdup_printf ("%s/%s", tmpdir, base);
  g_free (base);

  bmp = filename_new_extension (tmp, "bmp");
  cleanup_list = g_list_prepend (cleanup_list, bmp);
  g_free (tmp);

  rc = t_terrain_save (terrain, FILE_BMP_BW, bmp);
  if (rc)
    {
      cleanup (cleanup_list);

      on_file_save_error (terrain->filename);
      return;
    }

  command = g_strdup_printf ("gimp %s", bmp);


  execute_command (command, _("There was an error executing The GIMP"), cleanup,
                   cleanup_list);
  g_free (command);
}


void
on_terrain_open_gl_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_terrain_close_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  terrain_window_exit_sequence (terrain_window);
}


void
on_terrain_clone_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *view;
  TTerrain  *terrain;
  GtkWidget *terrain_window;
  GtkWidget *main_window;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  main_window = gtk_object_get_data (GTK_OBJECT (terrain_window),
                                     "data_parent");

  terrain = t_terrain_clone (terrain);
  g_free (terrain->filename);
  terrain->filename = main_window_filename_new (main_window);
  terrain_window = terrain_window_new (main_window, terrain);
  terrain_window_save_state (terrain_window, _("Clone"));
}


static void
undo_apply                             (GtkWidget       *undo_window,
                                        TTerrain        *terrain)
{
  GtkWidget *tree;
  GList     *selection;
  gboolean   apply_preview;
  GtkWidget *view;

  tree = lookup_widget (undo_window, "tree");
  view = lookup_widget (undo_window, "preview");
  apply_preview = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view)) == terrain;

  selection = GTK_TREE_SELECTION_OLD (GTK_TREE (tree));
  if (selection != NULL)
    {
      TUndoState *undo_state;

      undo_state =
        gtk_object_get_data (GTK_OBJECT (selection->data), "data_state");

      if (apply_preview)
        {
          TTerrain *undo_terrain, *preview_terrain;

          if (undo_state != NULL)
            {
              GtkObject *object;

              object = t_terrain_new (0, 0);
              undo_terrain = T_TERRAIN (object);
              t_terrain_ref (undo_terrain);
              t_undo_state_revert (undo_state, undo_terrain);
              preview_terrain = t_terrain_clone_preview (undo_terrain);
              t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), preview_terrain);
              t_terrain_unref (undo_terrain);
            }
          else
            t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), NULL);
        }
      else
        {
          GtkWidget *view;
          TUndo     *undo;
          TTerrain  *main_terrain;
          GtkWidget *terrain_window;

          terrain_window = gtk_object_get_data (GTK_OBJECT (undo_window),
                                             "data_parent");
          undo = gtk_object_get_data (GTK_OBJECT (terrain_window),
                                      "data_t_undo");
          view = lookup_widget (terrain_window, "terrain");
          main_terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

          if (terrain == main_terrain)
            t_undo_revert_state (undo, undo_state, terrain);
          else
            t_undo_state_revert (undo_state, terrain);
        }
    }
  else
    t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), NULL);
}


void
on_terrain_undo_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window;
  GtkWidget *modal;
  TUndo     *undo;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  undo = gtk_object_get_data (GTK_OBJECT (terrain_window), "data_t_undo");

  modal = gtk_object_get_data (GTK_OBJECT (terrain_window), "data_modal");
  if (modal == NULL)
    {
      GtkWidget *undo_window, *tree;
      GtkWidget *view;
      TTerrain  *terrain;
      TTerrain  *terrain_preview;
      GtkWidget *item;

      gtk_widget_set_sensitive (terrain_window, FALSE);
      undo_window = create_undo_tree_window ();
      gtk_object_set_data (GTK_OBJECT (undo_window),
                           "data_filter_apply_func", &undo_apply);

      view = lookup_widget (terrain_window, "terrain");
      terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

      gtk_object_set_data (GTK_OBJECT (terrain_window), "data_modal",
                           undo_window);
      gtk_object_set_data_full (GTK_OBJECT (undo_window), "data_parent",
                                terrain_window, terrain_window_modal_destroy);
      terrain_preview = t_terrain_clone_preview (terrain);
      view = lookup_widget (undo_window, "preview");
      t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), terrain_preview);
      terrain_window_set_title (terrain_window, undo_window);

      tree = lookup_widget (undo_window, "tree");
      item = t_undo_unpack_tree (undo, GTK_TREE (tree));
      filter_window_update_preview (undo_window);
      gtk_widget_show (undo_window);
      gtk_tree_select_child (GTK_TREE (tree), item);
    }
}


static void
level_connector_apply                  (GtkWidget       *window,
                                        TTerrain        *terrain)
{
  gint count;

  count = get_int (window, "count");
  t_terrain_connect (terrain, count);
}


void
on_terrain_connect_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_level_connector_window,
                     &level_connector_apply,
                     hscales,
                     TRUE);
}


static void
craters_auto_desensitize (GtkWidget *crater_window,
                          GtkWidget *adjust)
{
  gint count;

  count = get_int (crater_window, "count");

  gtk_widget_set_sensitive (lookup_widget (crater_window, "center_x"),
                            count == 1);
  gtk_widget_set_sensitive (lookup_widget (crater_window, "center_x_label"),
                            count == 1);
  gtk_widget_set_sensitive (lookup_widget (crater_window, "center_y"),
                            count == 1);
  gtk_widget_set_sensitive (lookup_widget (crater_window, "center_y_label"),
                            count == 1);
}


static void
craters_apply                          (GtkWidget       *crater_window,
                                        TTerrain        *terrain)
{
  gint       count;
  gfloat     radius, height, coverage, center_x, center_y;
  gboolean   wrap;
  gint       seed;

  center_x = get_float (crater_window, "center_x");
  center_y = get_float (crater_window, "center_y");
  count = get_int (crater_window, "count");
  radius = get_float (crater_window, "radius");
  height = get_float (crater_window, "height");
  coverage = get_float (crater_window, "coverage");
  wrap = get_boolean (crater_window, "wrap");
  if (get_boolean (crater_window, "new_seed"))
    seed = -1;
  else
    seed = get_int (crater_window, "seed");

  t_terrain_craters (terrain, count, wrap, height, radius, coverage,
                     seed, center_x, center_y);
  t_terrain_normalize (terrain, TRUE);
}


static GtkWidget *
create_craters_window_advanced (TTerrain *terrain)
{
  gchar     *count_scale[] = { "count", NULL };
  GtkWidget *craters_window;

  craters_window = create_craters_window ();
  hscale_callbacks (craters_window, count_scale,
                    (GtkSignalFunc) &craters_auto_desensitize);
  filter_window_add_crosshairs (craters_window, "preview",
                                "center_x", "center_y", "use_preview");

  return craters_window;
}


void
on_terrain_craters_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = {
    "center_x", "center_y", "count", "radius", "height", "coverage", NULL
  };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_craters_window_advanced,
                     &craters_apply,
                     hscales,
                     TRUE);
}


static void
digital_filter_apply                   (GtkWidget       *filter_window,
                                        TTerrain        *terrain)
{
  const gint FLEN = 5;
  const gint FSIZE = FLEN*FLEN;
  gint       i; 
  gfloat     filter[FSIZE]; 
  gfloat     elv_min;
  gfloat     elv_max;
  gchar      name[32];

  /* get all filter values */
  for (i=0; i<FSIZE; i++)
    {
    sprintf (name, "filter_val_%d", i+1);
    filter[i] = get_float (filter_window, name);
    }

  elv_min = get_float (filter_window, "min_elevation");
  elv_max = get_float (filter_window, "max_elevation");

  t_terrain_digital_filter (terrain, filter, FLEN, elv_min, elv_max);
}


void
on_terrain_digital_filter_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar     *hscales[] = { "min_elevation", "max_elevation", NULL };
  gchar     *spinbuttons[]={ "filter_val_1", "filter_val_2", "filter_val_3",
                             "filter_val_4", "filter_val_5", "filter_val_6",
                             "filter_val_7", "filter_val_8", "filter_val_9",
                             "filter_val_10", 
                             "filter_val_11", "filter_val_12", "filter_val_13",
                             "filter_val_14", "filter_val_15", "filter_val_16",
                             "filter_val_17", "filter_val_18", "filter_val_19",
                             "filter_val_20", 
                             "filter_val_21", "filter_val_22", "filter_val_23",
                             "filter_val_24", "filter_val_25", NULL };

  filter_window_new_extended (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_digital_filter_window,
                     &digital_filter_apply,
                     hscales,
		     spinbuttons,
		     NULL,
                     TRUE);
}


static void
fill_apply                             (GtkWidget       *fill_window,
                                        TTerrain        *terrain)
{
  gfloat fill_elevation, fill_tightness;

  fill_elevation = get_float (fill_window, "fill_elevation");
  fill_tightness = get_float (fill_window, "fill_tightness");
  t_terrain_fill (terrain, fill_elevation, fill_tightness);
}


void
on_terrain_fill_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { "fill_elevation", "fill_tightness", NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_fill_window,
                     &fill_apply,
                     hscales,
                     TRUE);
}


static void
fold_apply (GtkWidget *fold_window,
            TTerrain  *terrain)
{
  gfloat    fold_offset;
  gint      margin;

  fold_offset = get_float (fold_window, "fold_offset");
  margin = fold_offset * MIN (terrain->width, terrain->height);

  t_terrain_fold (terrain, margin);
}


void
on_terrain_fold_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { "fold_offset", NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_fold_window,
                     &fold_apply,
                     hscales,
                     TRUE);
}


static void
gaussian_hill_apply (GtkWidget     *hill_window,
                     TTerrain      *terrain)
{
  gfloat    center_x, center_y;
  gfloat    radius, scale_factor, radius_factor;
  gfloat    smoothing_factor, delta_scale_factor;

  center_x = get_float (hill_window, "center_x");
  center_y = get_float (hill_window, "center_y");
  radius = get_float (hill_window, "hill_radius");
  scale_factor = get_float (hill_window, "hill_scale_factor");
  radius_factor = get_float (hill_window, "hill_radius_factor");
  smoothing_factor = get_float (hill_window, "hill_smoothing_factor");
  delta_scale_factor = get_float (hill_window, "hill_delta_scale_factor");

  t_terrain_gaussian_hill (terrain, center_x, center_y,
                           radius, radius_factor,
                           scale_factor, smoothing_factor,
                           delta_scale_factor);
  t_terrain_normalize (terrain, TRUE);
}


static GtkWidget *
create_gaussian_hill_window_advanced   (TTerrain        *terrain)
{
  GtkWidget *window;

  window = create_gaussian_hill_window ();
  filter_window_add_crosshairs (window, "preview",
                                "center_x", "center_y", "use_preview");

  return window;
}


void
on_terrain_gaussian_hill_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = {
    "center_x", "center_y", "hill_radius", "hill_scale_factor",
    "hill_radius_factor", "hill_smoothing_factor", "hill_delta_scale_factor",
    NULL
  };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_gaussian_hill_window_advanced,
                     &gaussian_hill_apply,
                     hscales,
                     TRUE);
}


static void
radial_scale_apply                (GtkWidget *radial_window,
                                   TTerrain  *terrain)
{
  gfloat    center_x, center_y;
  gfloat    start_dist, end_dist;
  gfloat    scale_factor, smooth_factor;
  gint      frequency;

  center_x = get_float (radial_window, "center_x");
  center_y = get_float (radial_window, "center_y");
  start_dist = get_float (radial_window, "scale_start_distance");
  end_dist = get_float (radial_window, "scale_end_distance");
  scale_factor = get_float (radial_window, "scale_factor");
  smooth_factor = get_float (radial_window, "smoothing_factor");
  frequency = get_int (radial_window, "frequency");
  if (get_boolean (radial_window, "invert"))
    scale_factor = 1.0 / scale_factor;

  t_terrain_radial_scale (terrain, center_x, center_y,
                          scale_factor, start_dist, end_dist, 
			  1-smooth_factor, frequency);
  t_terrain_normalize (terrain, FALSE);
}


static GtkWidget *
create_radial_scale_window_advanced    (TTerrain        *terrain)
{
  GtkWidget *window;

  window = create_radial_scale_window ();
  filter_window_add_crosshairs (window, "preview",
                                "center_x", "center_y", "use_preview");

  return window;
}


void
on_terrain_radial_scale_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = {
    "center_x", "center_y", "scale_start_distance", "scale_end_distance",
    "scale_factor", "smoothing_factor", "frequency", NULL
  };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_radial_scale_window_advanced,
                     &radial_scale_apply,
                     hscales,
                     TRUE);
}


static void
rasterize_apply                   (GtkWidget *rasterize_window,
                                   TTerrain  *terrain)
{
  gint       x_size, y_size;
  gfloat     factor;
  gboolean   do_adjust_size;

  do_adjust_size = get_boolean (rasterize_window, "rasterize_do_adjust_size");
  x_size = get_int (rasterize_window, "rasterize_xsize");
  y_size = get_int (rasterize_window, "rasterize_ysize");
  
  factor = get_float (rasterize_window, "rasterize_factor");

  if (get_boolean (rasterize_window, "use_preview"))
    if (do_adjust_size)
      {
      GtkWidget *terrain_window = gtk_object_get_data (
		                        GTK_OBJECT (rasterize_window),
                                        "data_parent");
      GtkWidget *view = lookup_widget (terrain_window, "terrain"); 
      TTerrain  *preview_terrain = terrain;
      TTerrain  *terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view)); 
      gint       org_x = terrain->width;
      gint       org_y = terrain->height;
      gint       new_x = preview_terrain->width;
      gint       new_y = preview_terrain->height;
      gfloat     f = sqrt ((gfloat)(new_x+new_y) / (gfloat)(org_x+org_y));

      x_size = (gint)(x_size*f);
      if (x_size == 0)
        x_size = 1;
      
      y_size = (gint)(y_size*f);
      if (y_size == 0)
	y_size = 1;

      terrain = preview_terrain;
      }
    
  t_terrain_rasterize (terrain, x_size, y_size, factor);
}


void
on_terrain_rasterize_activate                  (GtkMenuItem     *menuitem,
                                                gpointer         user_data)
{
  gchar *checkboxes[] = { "rasterize_do_adjust_size", NULL };
  gchar *hscales[] = { "rasterize_xsize", "rasterize_ysize", "rasterize_factor", NULL };

  filter_window_new_extended (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_rasterize_window,
                     &rasterize_apply,
                     hscales,
		     NULL,
		     checkboxes,
                     TRUE);
}


static gchar *mirror_options[] = {
  "mirror_horizontal", "mirror_vertical",
  "mirror_diagonal_1", "mirror_diagonal_2", NULL
};


static void
mirror_apply                           (GtkWidget       *mirror_window,
                                        TTerrain        *terrain)
{
  gint       direction;
  TCanvas   *canvas;
  GtkWidget *terrain_window;
  GtkWidget *view;

  direction = get_radio_menu (mirror_window, mirror_options);
  t_terrain_mirror (terrain, direction);

  /* we've done the terrain, we now need to up update the canvas objects */
  if (!terrain->objects->len)
    return;

  terrain_window = lookup_widget (mirror_window, "data_parent");
  view = lookup_widget (terrain_window, "terrain");
  t_terrain_view_set_terrain (T_TERRAIN_VIEW(view), terrain);

  canvas = T_CANVAS (view);
  if (!canvas->object_array)
    return;

  t_canvas_clear_objects (canvas);
  t_terrain_view_set_terrain (T_TERRAIN_VIEW(view), terrain);
}


static GtkWidget *
create_mirror_window_advanced          (TTerrain        *terrain)
{
  GtkWidget *mirror_window;

  mirror_window = create_mirror_window ();

  if (terrain->width != terrain->height)
    {
      gtk_widget_set_sensitive (lookup_widget (mirror_window,
                                "mirror_diagonal_1"), FALSE);
      gtk_widget_set_sensitive (lookup_widget (mirror_window,
                                "mirror_diagonal_2"), FALSE);
    }

  return mirror_window;
}


void
on_terrain_mirror_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_mirror_window_advanced,
                     &mirror_apply,
                     hscales,
                     TRUE);
}


static void
move_apply                             (GtkWidget       *mirror_window,
                                        TTerrain        *terrain)
{
  gfloat x_offset, y_offset;

  x_offset = get_float (mirror_window, "x_offset");
  y_offset = get_float (mirror_window, "y_offset");

  t_terrain_move (terrain, x_offset, y_offset);
}


void
on_terrain_move_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { "x_offset", "y_offset", NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_move_window,
                     &move_apply,
                     hscales,
                     TRUE);
}


static gchar *rotate_options[] = {
  "rotate_90", "rotate_180", "rotate_270", NULL
};


static void
rotate_apply                           (GtkWidget       *rotate_window,
                                        TTerrain        *terrain)
{
  gint       angle;
  TCanvas   *canvas;
  GtkWidget *terrain_window;
  GtkWidget *view;

  angle = get_radio_menu (rotate_window, rotate_options);
  t_terrain_rotate (terrain, angle);

  /* we've done the terrain, we now need to up update the canvas objects */
  if (!terrain->objects->len)
    return;

  terrain_window = lookup_widget (rotate_window, "data_parent");
  view = lookup_widget (terrain_window, "terrain");
  t_terrain_view_set_terrain (T_TERRAIN_VIEW(view), terrain);

  canvas = T_CANVAS (view);
  if (!canvas->object_array)
    return;

  t_canvas_clear_objects (canvas);
  t_terrain_view_set_terrain (T_TERRAIN_VIEW(view), terrain);
}


void
on_terrain_rotate_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_rotate_window,
                     &rotate_apply,
                     hscales,
                     TRUE);
}


static void
fill_basins_apply                      (GtkWidget       *fill_basins_window,
                                        TTerrain        *terrain)
{
  gint      iterations;
  gboolean  big_grid;

  iterations = get_int (fill_basins_window, "iterations");
  big_grid = get_boolean (fill_basins_window, "big_grid");

  t_terrain_fill_basins (terrain, iterations, big_grid);
}


void
on_terrain_fill_basins_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { "iterations", NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_basins_window,
                     &fill_basins_apply,
                     hscales,
                     TRUE);
}



static void
roughen_apply                          (GtkWidget       *roughen_window,
                                        TTerrain        *terrain)
{
  gfloat    factor;
  gboolean  big_grid;

  factor = get_float (roughen_window, "roughen_factor");
  big_grid = get_boolean (roughen_window, "roughen_big");

  t_terrain_roughen_smooth (terrain, TRUE, big_grid, factor);
}


void
on_terrain_roughen_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { "roughen_factor", NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_roughen_window,
                     &roughen_apply,
                     hscales,
                     TRUE);
}


static void
smooth_apply                           (GtkWidget       *smooth_window,
                                        TTerrain        *terrain)
{
  gfloat    factor;
  gboolean  big_grid;

  factor = get_float (smooth_window, "smooth_factor");
  big_grid = get_boolean (smooth_window, "smooth_big");

  t_terrain_roughen_smooth (terrain, FALSE, big_grid, factor);
}


void
on_terrain_smooth_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { "smooth_factor", NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_smooth_window,
                     &smooth_apply,
                     hscales,
                     TRUE);
}


static void
terrace_apply                          (GtkWidget       *terrace_window,
                                        TTerrain        *terrain)
{
  gint      level_count;
  gfloat    factor;
  gboolean  adjust_sealevel;

  level_count = get_int (terrace_window, "terrace_levels");
  factor = get_float (terrace_window, "terrace_tightness");
  adjust_sealevel = get_boolean (terrace_window, "terrace_adjust_waterlevel");

  t_terrain_terrace (terrain, level_count, factor, adjust_sealevel);
}


void
on_terrain_terrace_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { "terrace_levels", "terrace_tightness", NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_terrace_window,
                     &terrace_apply,
                     hscales,
                     TRUE);
}


void
on_tile_assemble_terrains_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *widget;
  gboolean   value; 

  value = gtk_toggle_button_get_active (togglebutton);

  widget = lookup_widget (GTK_WIDGET (togglebutton), "assemble_terrains_x");
  gtk_widget_set_sensitive (widget, value);
  widget = lookup_widget (GTK_WIDGET (togglebutton), "assemble_terrains_y");
  gtk_widget_set_sensitive (widget, value);
}


static void
tile_apply                             (GtkWidget       *tile_window,
                                        TTerrain        *terrain)
{
  TTerrain *preview_terrain;
  gfloat    tile_offset;

  preview_terrain =
    gtk_object_get_data (GTK_OBJECT (tile_window), "data_t_terrain");

  tile_offset = get_float (tile_window, "tile_offset");
  t_terrain_tiler (terrain, tile_offset);

  if (preview_terrain == terrain)
    t_terrain_tile (terrain, 2);

  /* if we're dealing with the terrain window, check the assembly paramters */
  //if (preview_terrain != terrain)
  if (terrain->width != 100 || terrain->height != 100)
    if (get_boolean (tile_window, "assemble_terrains"))
      {
      TTerrain *tnew = NULL;
      gint num_x = get_int (tile_window, "assemble_terrains_x");
      gint num_y = get_int (tile_window, "assemble_terrains_y");

      if (num_x > 1 || num_y > 1)
        tnew = t_terrain_tile_new (terrain, num_x, num_y);

      if (tnew != NULL)
        {
        GtkWidget *main_window;
        GtkWidget *terrain_window;
        GtkWidget *new_terrain_window;

        terrain_window = gtk_object_get_data (GTK_OBJECT (tile_window), 
			                      "data_parent");
        main_window = lookup_widget (terrain_window, "data_parent");

        new_terrain_window = terrain_window_new (main_window, tnew);
        terrain_window_save_state (new_terrain_window, _("Tile"));
        }
      }
}


void
on_terrain_tile_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { "tile_offset", NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_tile_window,
                     &tile_apply,
                     hscales,
                     TRUE);
}


static void
transform_apply                        (GtkWidget       *transform_window,
                                        TTerrain        *terrain)
{
  gfloat     sea_threshold, sea_depth, sea_dropoff;
  gfloat     above_power, below_power;
  GtkWidget *view;

  sea_threshold = get_float (transform_window, "sea_threshold");
  sea_depth = get_float (transform_window, "sea_depth");
  sea_dropoff = get_float (transform_window, "sea_dropoff");
  above_power = get_float (transform_window, "above_power");
  below_power = get_float (transform_window, "below_power");

  view = lookup_widget (transform_window, "preview");
  if (t_terrain_view_get_terrain (T_TERRAIN_VIEW (view)) == terrain)
    {
      GtkWidget *canvas;
      TPixbuf   *pixbuf;

      canvas = lookup_widget (transform_window, "transfer_function");
      pixbuf = t_canvas_get_pixbuf (T_CANVAS (canvas));
      t_terrain_draw_transform (pixbuf, sea_threshold, sea_depth, sea_dropoff,
                                above_power, below_power);

      gtk_widget_queue_draw (canvas);
    }
  terrain->sealevel = sea_threshold;
  t_terrain_transform (terrain, sea_threshold, sea_depth, sea_dropoff,
                                above_power, below_power);
  terrain->sealevel = sea_depth;
}


static GtkWidget *
create_transform_window_advanced       (TTerrain        *terrain)
{
  GtkWidget *widget;

  widget = create_transform_window ();

  set_float (widget, "sea_threshold", terrain->sealevel);
  set_float (widget, "sea_depth", terrain->sealevel);

  return widget;
}


void
on_terrain_transform_activate          (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = {
    "sea_threshold", "sea_depth", "sea_dropoff",
    "above_power", "below_power", NULL
  };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_transform_window_advanced,
                     &transform_apply,
                     hscales,
                     TRUE);
}


void
on_terrain_invert_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *view;
  TTerrain  *terrain;
  GtkWidget *terrain_window;
  GtkWidget *main_window;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  main_window = gtk_object_get_data (GTK_OBJECT (terrain_window),
                                     "data_parent");

  t_terrain_invert (terrain); 
  terrain_window_save_state (terrain_window, _("Invert"));

  t_terrain_heightfield_modified (terrain);
}


static void
erode_apply                            (GtkWidget       *terrain_window,
                                        TTerrain        *terrain)
{
  gint      iterations;
  gint      max_flow_age;
  gint      age_flowmap_times;
  gfloat    threshold;
  gboolean  trim;
  gchar    *filename_anim;
  gchar    *filename_flow;
  gint      frame_count;
  gboolean  single_flow_direction;
  gboolean  do_observe_sealevel;

  iterations = get_int (terrain_window, "erode_iterations");
  max_flow_age = get_int (terrain_window, "max_flowmap_age");
  age_flowmap_times = get_int (terrain_window, "age_flowmap_times");
  threshold = get_float (terrain_window, "erode_threshold");
  trim = get_boolean (terrain_window, "erode_trim");

  if (get_boolean (terrain_window, "erode_save_flowmap"))
    filename_flow = get_text (terrain_window, "erode_save_flowmap_name");
  else
    filename_flow = NULL;

  if (get_boolean (terrain_window, "erode_save_anim"))
    filename_anim = get_text (terrain_window, "erode_save_anim_name");
  else
    filename_anim = NULL;

  frame_count = get_int (terrain_window, "erode_frame_count");
  single_flow_direction = get_boolean (terrain_window, "erode_single");
  do_observe_sealevel = get_boolean (terrain_window, "erode_sealevel");

  t_terrain_erode (terrain, iterations, max_flow_age, age_flowmap_times, 
                   threshold, filename_anim, filename_flow, frame_count, 
                   trim, single_flow_direction, !do_observe_sealevel, NULL);
}


void
on_terrain_erode_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_erode_window,
                     &erode_apply,
                     hscales,
                     TRUE);
}


static void
flowmap_apply                          (GtkWidget       *flowmap_window,
                                        TTerrain        *terrain)
{
  TTerrain *flowmap;
  GtkWidget *main_window;
  GtkWidget *terrain_window;
  gboolean  single_flow_direction;
  gboolean  do_observe_sealevel;

  terrain_window = gtk_object_get_data (GTK_OBJECT (flowmap_window),
                                     "data_parent");
  main_window = gtk_object_get_data (GTK_OBJECT (terrain_window),
                                     "data_parent");

  single_flow_direction = get_boolean (flowmap_window, "flowmap_single");
  do_observe_sealevel = get_boolean (flowmap_window, "flowmap_sealevel");

  flowmap = t_terrain_flowmap (terrain, single_flow_direction,
                               !do_observe_sealevel, 1.0);

  terrain_window = terrain_window_new (main_window, flowmap);
  terrain_window_save_state (terrain_window, _("Flowmap"));
  t_terrain_heightfield_modified (flowmap);
}


void
on_terrain_flowmap_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_flowmap_window,
                     &flowmap_apply,
                     hscales,
                     TRUE);
}


void
on_terrain_scale_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *scale_window;
  GtkWidget *terrain_window;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  gtk_widget_set_sensitive (terrain_window, FALSE);
  scale_window = create_scale_window ();
  gtk_object_set_data_full (GTK_OBJECT (scale_window), "data_parent",
                            terrain_window, terrain_window_modal_destroy);
  gtk_object_set_data (GTK_OBJECT (terrain_window), "data_modal",
                       scale_window);

  gtk_widget_show (scale_window);
}


void
on_terrain_auto_rotate_activate        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


gboolean
on_main_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  return main_window_exit_sequence (lookup_toplevel (GTK_WIDGET (widget)));
}


GtkWidget*
create_lc_custom (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
  GtkWidget *widget;

  widget = gtk_label_new ("Foo");
  gtk_widget_show_all (widget);
  return widget;
}


gboolean
on_terrain_window_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  return terrain_window_exit_sequence (widget);
}


void
on_generic_new_seed_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *widget;
  gboolean   value;

  value = !gtk_toggle_button_get_active (togglebutton);

  widget = lookup_widget (GTK_WIDGET (togglebutton), "seed_label");
  gtk_widget_set_sensitive (widget, value);
  widget = lookup_widget (GTK_WIDGET (togglebutton), "seed");
  gtk_widget_set_sensitive (widget, value);
}


void
on_info_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget  *info_window;

  info_window = lookup_toplevel (GTK_WIDGET (button));
  gtk_widget_destroy (info_window);
}


void
on_terrain_info_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window;
  GtkWidget *modal;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));

  modal = gtk_object_get_data (GTK_OBJECT (terrain_window), "data_modal");
  if (modal == NULL)
    {
      GtkWidget *view;
      TTerrain  *terrain;
      TTerrain  *terrain_preview;
      GtkWidget *info_window;
      gint       height;
      gint       width;
      gint       size;
      gfloat     average;
      gfloat     variance;
      gfloat     skewness;
      gchar      buf[80];

      gtk_widget_set_sensitive (terrain_window, FALSE);
      info_window = create_info_window ();

      view = lookup_widget (terrain_window, "terrain");
      terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

      terrain_preview = t_terrain_clone_histogram (terrain, 0.85);

      view = lookup_widget (info_window, "preview");
      t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), terrain_preview);

      width = terrain->width;
      height = terrain->height;
      size = width * height;
      skewness = t_terrain_calculate_skewness (terrain, &average, &variance);

      sprintf (buf, "%d", size);
      set_text (info_window, "label_points", buf);
      sprintf (buf, "%d", width);
      set_text (info_window, "label_width", buf);
      sprintf (buf, "%d", height);
      set_text (info_window, "label_height", buf);
      sprintf (buf, "%f", average);
      set_text (info_window, "label_average", buf);
      sprintf (buf, "%f", variance);
      set_text (info_window, "label_variance", buf);
      sprintf (buf, "%f", skewness);
      set_text (info_window, "label_skewness", buf);
      sprintf (buf, "%f", t_terrain_calculate_dimension(terrain));
      set_text (info_window, "label_dimension", buf);

      gtk_object_set_data_full (GTK_OBJECT (info_window),
                                "data_parent", terrain_window,
                                terrain_window_modal_destroy);
      gtk_object_set_data (GTK_OBJECT (terrain_window),
                           "data_modal", info_window);

      gtk_widget_show (GTK_WIDGET (info_window));
    }
}


void
terrain_toggle_activate                (GtkMenuShell    *menushell,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window;

  terrain_window = lookup_toplevel (GTK_WIDGET (menushell));
  if (GTK_WIDGET_VISIBLE (terrain_window))
    terrain_window_pack_menu (terrain_window);
}


void
on_load_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget    *open_file;
  GtkWidget    *main_window;
  TTerrain     *terrain;
  const gchar  *filename;
  TFileType     type;
  FileTypeInfo *info;

  open_file = lookup_toplevel (GTK_WIDGET (button));
  main_window = gtk_object_get_data (GTK_OBJECT (open_file), "data_parent");
  filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (open_file));
  info = gtk_object_get_data (GTK_OBJECT (open_file), "data_file_type_info");
  type = info[get_option (open_file, "type_menu")].type;
  terrain = t_terrain_load (filename, type);
  if (terrain != NULL)
    {
      GtkWidget *terrain_window;

      terrain_window = terrain_window_new (main_window, terrain);
      terrain_window_save_state (terrain_window, _("Load"));
    }
  else
    {
      GtkWidget *error_dialog;
      gchar msg[256];

      snprintf (msg, 256, _("An error occurred while opening the file:\n%s."),
		filename);
      error_dialog = gnome_error_dialog (msg);
      gtk_widget_show (error_dialog);
    }

  gtk_widget_destroy (open_file);
}


void
on_option_ok_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *options_window;
  GtkWidget *main_window;
  gchar     *file;
  gchar     *home_dir;
  MainState *main_state;

  options_window = lookup_toplevel (GTK_WIDGET (button));
  main_window = gtk_object_get_data (GTK_OBJECT (options_window), "data_parent");
  main_window_state_pack_options (main_window, options_window);
  main_state = gtk_object_get_data (GTK_OBJECT (main_window), "data_state");

  home_dir = getenv ("HOME");
  if (home_dir != NULL)
    { 
      gchar *terraform_dir= g_strdup_printf("%s/.terraform", home_dir);
      struct stat statdir;

      if (stat (terraform_dir, &statdir) != -1 && S_ISDIR (statdir.st_mode))
        {
          file = g_strdup_printf ("%s/terraformrc", terraform_dir);
          main_window_state_save (main_state, file);
          g_free (file);
        }
      else   /* terraform_dir does not exist */
        {
          gint rc = mkdir(terraform_dir, S_IRWXU );
          if (rc==0) /* terraform_dir can be created */
            {
              file = g_strdup_printf ("%s/terraformrc", terraform_dir);
              main_window_state_save(main_state, file);
              g_free (file);
            }
          else if (rc == -1)
            g_print("~/.terraform/ cannot be created\n");
        }

      g_free (terraform_dir);
    }

  gtk_widget_destroy (options_window);
}


void
on_option_povray_browse_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *find_window;
  GtkWidget *options_window;

  options_window = lookup_toplevel (GTK_WIDGET (button));
  find_window = create_find_povray_window ();
  gtk_object_set_data (GTK_OBJECT (find_window), "data_parent", options_window);
  gtk_widget_show (find_window);
}


void
on_povray_yes_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget   *find_window;
  GtkWidget   *options_window;
  const gchar *povray;

  find_window = lookup_toplevel (GTK_WIDGET (button));
  options_window = gtk_object_get_data (GTK_OBJECT (find_window),
                                        "data_parent");

  povray = povray_probe ();
  if (povray != NULL)
    {
      GtkWidget *text;
      gint       pos;

      text = lookup_widget (options_window, "option_povray");
      gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
      pos = 0;
      gtk_editable_insert_text (GTK_EDITABLE (text), povray,
                                strlen (povray), &pos);
    }

  gtk_widget_destroy (find_window);
}


void
on_povray_no_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *find_window;

  find_window = lookup_toplevel (GTK_WIDGET (button));
  gtk_widget_destroy (find_window);
}


static void
render_povray                          (TTerrain  *terrain, 
					MainState *main_state)
{
  gchar *tmpdir;
  gchar *location, *tmp, *base, *ini, *pov, *hf, *river, *safe;
  gchar *cmd;
  GList *cleanup_list;
  gint   rc;
  gchar  previous_dir[1024];

  cleanup_list = NULL;

  /* Create a safe temp directory */
  tmpdir = filename_create_tempdir ();
  if (tmpdir == NULL)
    {
      on_file_save_error (terrain->filename);
      return;
    }
  cleanup_list = g_list_prepend (cleanup_list, tmpdir);

  /* Create path to file in tmp */
  base = filename_get_base (terrain->filename);
  location = g_strdup_printf ("%s/%s", tmpdir, base);
  g_free (base);

  /* Remove characters POV-Ray doesn't like */
  safe = filename_povray_safe (location);
  g_free (location);

  ini = filename_new_extension (safe, "ini");
  cleanup_list = g_list_prepend (cleanup_list, ini);
  pov = filename_new_extension (safe, "pov");
  cleanup_list = g_list_prepend (cleanup_list, pov);

  tmp = filename_get_without_extension_and_dot (safe);
  g_free (safe);

  hf = g_strdup_printf ("%s_hf.tga", tmp);
  cleanup_list = g_list_prepend (cleanup_list, hf);
  river = g_strdup_printf ("%s_river.tga", tmp);
  cleanup_list = g_list_prepend (cleanup_list, river);
  g_free (tmp);

  rc = t_terrain_save (terrain, FILE_INI, ini);
  if (rc)
    {
      cleanup (cleanup_list);
      on_file_save_error (terrain->filename);
    }

  cmd = g_strdup_printf ("%s %s", main_state->povray_executable, ini);

  getcwd (previous_dir, 1024);
  chdir (tmpdir);
  execute_command (cmd, _("There was an error executing POV-Ray"), cleanup,
                   cleanup_list);
  chdir (previous_dir);

  g_free (cmd);
}


void
on_terrain_render_povray_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  MainState *main_state;
  GtkWidget *terrain_window;
  GtkWidget *main_window;
  GtkWidget *view;
  TTerrain  *terrain;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  main_window =
    gtk_object_get_data (GTK_OBJECT (terrain_window), "data_parent");
  main_state =
    gtk_object_get_data (GTK_OBJECT (main_window), "data_state");

  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  render_povray (terrain, main_state);
}


void
on_terrain_native_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window;
  GtkWidget *main_window;
  MainState *main_state;
  GtkWidget *native_window;
  GtkWidget *view;
  TTerrain  *terrain;
  GtkWidget *canvas;
  TPixbuf   *pixbuf;
  SkyValues  sky;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  main_window =
    gtk_object_get_data (GTK_OBJECT (terrain_window), "data_parent");
  main_state = gtk_object_get_data (GTK_OBJECT (main_window), "data_state");

  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  native_window = create_native_render_window ();
  canvas = lookup_widget (native_window, "image");
  pixbuf = t_canvas_get_pixbuf (T_CANVAS (canvas));
  gtk_drawing_area_size (GTK_DRAWING_AREA (canvas),
                         terrain->render_options.image_width,
                         terrain->render_options.image_height);

  gtk_widget_show (native_window);
  while (gtk_events_pending ())
    gtk_main_iteration ();

/*
  t_terrain_render (terrain, pixbuf,
                    EARTH_RADIUS, EARTH_CLOUD_HEIGHT, LENS_ANGLE,
                    0.0, 0.0, EARTH_RADIUS + 0.2);
*/

  sky_values_init (&sky, main_state->gamma);
  sky_render (terrain, pixbuf, &sky);
}


void
on_terrain_export_povray_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;
  gchar     *safe, *ini;
  gint       rc;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  safe = filename_povray_safe (terrain->filename);
  ini = filename_new_extension (terrain->filename, "ini");
  g_free (safe);

  rc = t_terrain_save (terrain, FILE_INI, ini);
  g_free (ini);

  if (rc)
    {
      on_file_save_error (terrain->filename);
      return;
    }
}


void
on_about_info_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *about_text;
  GtkWidget *about_frame;
  GtkWidget *copyright_label;
  gboolean   about_text_visible;

  about_text = lookup_widget (GTK_WIDGET (button), "about_text");
  about_frame = lookup_widget (GTK_WIDGET (button), "about_frame");
  copyright_label = GTK_BIN (button)->child;

  about_text_visible = GTK_WIDGET_VISIBLE (about_text);
  if (about_text_visible)
    {
      gtk_widget_hide (about_text);
      gtk_widget_show (about_frame);
      gtk_label_set_text (GTK_LABEL (copyright_label), _("Copyright Info"));
    }
  else
    {
      gtk_widget_hide (about_frame);
      gtk_widget_show (about_text);
      gtk_label_set_text (GTK_LABEL (copyright_label), _("Show Logo"));
    }
}


void
on_generic_cancel_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_destroy (lookup_toplevel (GTK_WIDGET (button)));
}


void
on_option_download_povray_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_generic_use_preview_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *filter_window;
  GtkWidget *view;

  filter_window = lookup_toplevel (GTK_WIDGET (togglebutton));
  view = lookup_widget (filter_window, "preview");

  if (get_boolean (GTK_WIDGET (filter_window), "use_preview"))
    {
      filter_window_update_preview (filter_window);
    }
  else
    {
      TTerrain *reference;

      reference =
        gtk_object_get_data (GTK_OBJECT (filter_window),
                             "data_t_terrain_reference");

      t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), reference);
    }
}


void
on_save_as_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget    *save_as;
  GtkWidget    *terrain_window;
  GtkWidget    *view;
  TTerrain     *terrain;
  const gchar  *filename;
  gint          status;
  TFileType     type;
  FileTypeInfo *info;

  save_as = lookup_toplevel (GTK_WIDGET (button));
  terrain_window = gtk_object_get_data (GTK_OBJECT (save_as), "data_parent");
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (save_as));
  info = gtk_object_get_data (GTK_OBJECT (save_as), "data_file_type_info");
  type = info[get_option (save_as, "type_menu")].type;
  status = t_terrain_save (terrain, type, filename);

  gtk_widget_destroy (save_as);

  if (status)
    on_file_save_error (terrain->filename);
}


void
on_use_preview_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{

}


gboolean
on_render_preview_button_press_event   (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  TTerrain     *terrain;
  TTerrainView *view;
  TPixbuf      *pixbuf;
  GtkWidget    *render_options;
  GdkPixbuf    *pixmap;
  GError       *gerror = NULL;
  gfloat        elevation_offset;
  gfloat        y;
  gchar         pixmap_file[1000];


  render_options = lookup_toplevel (widget);
  view = T_TERRAIN_VIEW (lookup_widget (render_options, "preview"));
  terrain = view->terrain;
  pixbuf = T_CANVAS (view)->pixbuf;

  elevation_offset = get_float (widget, "render_elevation");
  y = terrain->heightfield[((gint) event->y) * terrain->width + 
	                   ((gint) event->x)];

  if (event->button == 1)
    {
      /* Left button. */
      if (get_boolean (widget, "render_observe_sealevel"))
        y = MAX (y, terrain->sealevel);

      set_float (widget, "render_camera_x", ((gfloat) event->x) / pixbuf->width);
      set_float (widget, "render_camera_y", y + elevation_offset);
      set_float (widget, "render_camera_z", 1-((gfloat) event->y) / pixbuf->height);
     
      // FIXME, new code 
      sprintf (pixmap_file, "%s/glade/camera.xpm", TERRAFORM_DATA_DIR);
      pixmap = gdk_pixbuf_new_from_file (pixmap_file, &gerror);
    }
  else
  if (event->button == 3)
    {
      /* Right button. */
      if (get_boolean (widget, "render_observe_sealevel"))
        y = MAX (y, terrain->sealevel);

      set_float (widget, "render_lookat_x", ((gfloat) event->x) / pixbuf->width);
      set_float (widget, "render_lookat_y", y + elevation_offset);
      set_float (widget, "render_lookat_z", 1-((gfloat) event->y) / pixbuf->height);

      // FIXME, new code 
      sprintf (pixmap_file, "%s/glade/eye.xpm", TERRAFORM_DATA_DIR);
      pixmap = gdk_pixbuf_new_from_file (pixmap_file, &gerror);
    }
  /* else if (event->button == 2) 
     { 
       on_generic_preview_button_press_event (widget, event, user_data);
       } */


  // FIXME, new code 
  if (pixmap) {
      t_terrain_add_object (terrain, terrain->width, terrain->height, event->x, event->y, 0.0, 0, 0, 0, "camera");
  }

  return FALSE;
}


gboolean
on_render_preview_button_release_event (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  return FALSE;
}


static void
preview_popup_destroy (gpointer user_data)
{
  gtk_object_remove_data (GTK_OBJECT (user_data), "data_popup");
}


static gchar *preview_view_radio[] = {
  "2d_plane", "2d_contour",
  "3d_wire", "3d_height",
  "3d_light", NULL
};


static gchar *preview_colormap_radio[] = {
  "land", "desert",
  "grayscale", "heat", NULL
};


static void
t_terrain_view_pack_preview_menu (TTerrainView *view,
                                  GtkWidget    *menu)
{
  GtkWidget     *filter_window;
  TViewModel     model;
  TColormapType  colormap;
  gboolean       refresh;

  refresh = FALSE;

  filter_window = gtk_object_get_data (GTK_OBJECT (menu), "data_parent");
  model = get_radio_menu (menu, preview_view_radio);
  t_terrain_view_set_model (T_TERRAIN_VIEW (view), model);

  colormap = get_radio_menu (menu, preview_colormap_radio);
  t_terrain_view_set_colormap (T_TERRAIN_VIEW (view), colormap);

  filter_window_update_preview (filter_window);
}


static void
t_terrain_view_unpack_preview_menu (TTerrainView *view,
                                    GtkWidget    *menu)
{
  set_radio_menu (menu, preview_view_radio, view->model);
  set_radio_menu (menu, preview_colormap_radio, view->colormap);
}


gboolean
on_generic_preview_button_press_event  (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  GtkWidget *window;

  window = lookup_toplevel (widget);

  if (event->button == 3)
    {
      GtkWidget *preview_popup;

      preview_popup = gtk_object_get_data (GTK_OBJECT (window), "data_popup");
      if (preview_popup == NULL)
        {
          GtkWidget *view;

          preview_popup = create_preview_popup ();

          gtk_object_set_data_full (
            GTK_OBJECT (preview_popup), "data_parent", window,
            (GtkDestroyNotify) preview_popup_destroy);

          gtk_object_set_data (
            GTK_OBJECT (window), "data_popup", preview_popup);

          view = lookup_widget (window, "preview");
          t_terrain_view_unpack_preview_menu (T_TERRAIN_VIEW (view),
                                              preview_popup);

          gtk_menu_popup (GTK_MENU (preview_popup), NULL, NULL, NULL, NULL, 3,
                          event->time);
        }
    }

  return FALSE;
}


gboolean
on_generic_preview_motion_notify_event (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data)
{
  return FALSE;
}


void
on_generic_toggled                     (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  filter_window_update_preview (lookup_toplevel (GTK_WIDGET (togglebutton)));
}


void
on_generic_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
  filter_window_perform (lookup_toplevel (GTK_WIDGET (button)));
}


void
on_preview_popup_selection_done        (GtkMenuShell    *menushell,
                                        gpointer         user_data)
{
  GtkWidget *filter_window;
  GtkWidget *view;

  filter_window = gtk_object_get_data (GTK_OBJECT (menushell), "data_parent");
  view = lookup_widget (filter_window, "preview");
  t_terrain_view_pack_preview_menu (T_TERRAIN_VIEW (view),
                                    GTK_WIDGET (menushell));
  gtk_object_sink (GTK_OBJECT (menushell));
}


void
on_undo_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *view;
  TTerrain  *terrain;
  GtkWidget *undo_window;
  GtkWidget *terrain_window;
  GtkWidget *main_window;

  undo_window = lookup_toplevel (GTK_WIDGET (button));
  terrain_window =
    gtk_object_get_data (GTK_OBJECT (undo_window), "data_parent");

  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  main_window = gtk_object_get_data (GTK_OBJECT (terrain_window),
                                     "data_parent");

  undo_apply (undo_window, terrain);

  t_terrain_heightfield_modified (terrain);

  gtk_widget_destroy (undo_window);
}


void
on_tree_selection_changed              (GtkTree         *tree,
                                        gpointer         user_data)
{
  GtkWidget *undo_window;
  GtkTree   *root_tree;
  GList     *selection;
  gboolean   sensitive;

  undo_window = lookup_toplevel (GTK_WIDGET (tree));
  root_tree = GTK_TREE (lookup_widget (undo_window, "tree"));

  selection = GTK_TREE_SELECTION_OLD (root_tree);
  sensitive =
    selection != NULL &&
    gtk_object_get_data (GTK_OBJECT (selection->data), "data_state") != NULL;

  gtk_widget_set_sensitive (lookup_widget (GTK_WIDGET (undo_window), "undo_ok"),
                            sensitive);
  gtk_widget_set_sensitive (lookup_widget (GTK_WIDGET (undo_window), "undo_apply"),
			    sensitive);

  filter_window_update_preview (undo_window);
}


void
on_execution_error_details_click       (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *error_log, *save_error_log_button;
  GtkWidget *execution_error_window;
  gboolean   visible;

  error_log = lookup_widget (GTK_WIDGET (button), "error_log");
  save_error_log_button = lookup_widget (GTK_WIDGET (button), "save_error_log");
  execution_error_window = lookup_widget (GTK_WIDGET (button), "execution_error_window");
  visible = GTK_WIDGET_VISIBLE (error_log);
  if (visible){
    gtk_widget_hide (error_log);
    gtk_widget_hide (save_error_log_button);
    gtk_window_set_policy (GTK_WINDOW (execution_error_window), FALSE, FALSE, FALSE);
    }
  else{
    gtk_widget_show (error_log);
    gtk_widget_show (save_error_log_button);
    gtk_window_set_policy (GTK_WINDOW (execution_error_window), FALSE, TRUE, FALSE);
    }

  if (visible)
    set_text (GTK_WIDGET (button), "details", _("Show Details"));
  else
    set_text (GTK_WIDGET (button), "details", _("Hide Details"));
}


void
on_terrain_print_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
#if 0
  GnomePrinter      *printer;
  GnomePrintContext *context;
  GtkWidget         *terrain_window;
  GtkWidget         *view;
  TTerrain          *terrain;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  printer = gnome_printer_dialog_new_modal ();

  if (printer != NULL)
    {
      context = gnome_print_context_new (printer);
      gnome_print_beginpage (context, "Terrain");
      t_terrain_print_contour_map (terrain, context);
      gnome_print_showpage (context);
      gnome_print_context_close (context);
      gtk_object_unref (GTK_OBJECT (context));
      gtk_object_unref (GTK_OBJECT (printer));
    }
#endif
}

void
on_erode_save_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *widget;
  gboolean enabled;

  widget = GTK_WIDGET (togglebutton);

  enabled = get_boolean (widget, "erode_save_flowmap");
  gtk_widget_set_sensitive (lookup_widget (widget, "erode_save_flowmap_name"), 
                            enabled);

  enabled = get_boolean (widget, "erode_save_anim");
  gtk_widget_set_sensitive (lookup_widget (widget, "erode_save_anim_name"), 
                            enabled);
  gtk_widget_set_sensitive (lookup_widget (widget, "erode_frame_count_label"),
                            enabled);
  gtk_widget_set_sensitive (lookup_widget (widget, "erode_frame_count"),
                            enabled);
}


void
on_join_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *join_window;

  join_window = lookup_toplevel (GTK_WIDGET (button));
  join_apply (join_window, FALSE);
  gtk_widget_destroy (join_window);
}


void
on_join_cancel_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_destroy (lookup_toplevel (GTK_WIDGET (button)));
}


/* need forward declarations to use merge methods here */
void on_merge_list_select_row (GtkCList *clist, gint row, gint column, 
		               GdkEvent *event, gpointer user_data);
void on_merge_list_select_row (GtkCList *clist, gint row, gint column, 
		               GdkEvent *event, gpointer user_data);


void
on_join_list_select_row                (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  on_merge_list_select_row (clist, row, column, event, user_data);
}


void
on_join_list_unselect_row              (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  on_merge_list_unselect_row (clist, row, column, event, user_data);
}

void
on_join_reverse_toggled                (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *join_window;

  join_window = lookup_toplevel (GTK_WIDGET (button));
  join_apply (join_window, TRUE);
  gtk_widget_queue_resize (join_window);
}


void
on_join_top_select_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *main_window;
  GtkWidget *terrain_window;
  GtkWidget *join_window;
  GtkWidget *frame;
  GtkWidget *view;
  TTerrain  *terrain;
  gint       row;
  gchar     *text;

  join_window = lookup_toplevel (GTK_WIDGET (button));
  row = *((gint*) gtk_object_get_data (GTK_OBJECT (join_window), "data_row"));

  main_window = lookup_widget (join_window, "data_parent");
  terrain_window = main_window_get_terrain_window (main_window, row);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (join_window),
                            "data_top_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  view = lookup_widget (join_window, "top_thumbnail");

  frame = lookup_widget (join_window, "top_frame");
  text = t_terrain_get_title (terrain);
  gtk_frame_set_label (GTK_FRAME (frame), text);
  g_free (text);

  terrain = t_terrain_clone_preview (terrain);
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), terrain);

  join_apply (join_window, TRUE);
  gtk_widget_queue_resize (join_window);
}


void
on_join_bottom_select_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *main_window;
  GtkWidget *terrain_window;
  GtkWidget *join_window;
  GtkWidget *frame;
  GtkWidget *view;
  TTerrain  *terrain;
  gint       row;
  gchar     *text;

  join_window = lookup_toplevel (GTK_WIDGET (button));
  row = *((gint*) gtk_object_get_data (GTK_OBJECT (join_window), "data_row"));

  main_window = lookup_widget (join_window, "data_parent");
  terrain_window = main_window_get_terrain_window (main_window, row);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (join_window),
                            "data_bottom_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  view = lookup_widget (join_window, "bottom_thumbnail");

  frame = lookup_widget (join_window, "bottom_frame");
  text = t_terrain_get_title (terrain);
  gtk_frame_set_label (GTK_FRAME (frame), text);
  g_free (text);

  terrain = t_terrain_clone_preview (terrain);
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), terrain);

  join_apply (join_window, TRUE);
  gtk_widget_queue_resize (join_window);
}


void
on_warp_list_select_row                (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  on_merge_list_select_row (clist, row, column, event, user_data);
}


void
on_warp_list_unselect_row              (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  on_merge_list_unselect_row (clist, row, column, event, user_data);
}


void
on_warp_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *warp_window;

  warp_window = lookup_toplevel (GTK_WIDGET (button));
  warp_apply (warp_window, FALSE);
  gtk_widget_destroy (warp_window);
}


void
on_warp_cancel_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_destroy (lookup_toplevel (GTK_WIDGET (button)));
}


void
on_warp_top_select_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *main_window;
  GtkWidget *terrain_window;
  GtkWidget *warp_window;
  GtkWidget *frame;
  GtkWidget *view;
  TTerrain  *terrain;
  gint       row;
  gchar     *text;

  warp_window = lookup_toplevel (GTK_WIDGET (button));
  row = *((gint*) gtk_object_get_data (GTK_OBJECT (warp_window), "data_row"));

  main_window = lookup_widget (warp_window, "data_parent");
  terrain_window = main_window_get_terrain_window (main_window, row);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (warp_window),
                            "data_top_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  view = lookup_widget (warp_window, "top_thumbnail");

  frame = lookup_widget (warp_window, "top_frame");
  text = t_terrain_get_title (terrain);
  gtk_frame_set_label (GTK_FRAME (frame), text);
  g_free (text);

  terrain = t_terrain_clone_preview (terrain);
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), terrain);

  warp_apply (warp_window, TRUE);
}


void
on_warp_bottom_select_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *main_window;
  GtkWidget *terrain_window;
  GtkWidget *warp_window;
  GtkWidget *frame;
  GtkWidget *view;
  TTerrain  *terrain;
  gint       row;
  gchar     *text;

  warp_window = lookup_toplevel (GTK_WIDGET (button));
  row = *((gint*) gtk_object_get_data (GTK_OBJECT (warp_window), "data_row"));

  main_window = lookup_widget (warp_window, "data_parent");
  terrain_window = main_window_get_terrain_window (main_window, row);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (warp_window),
                            "data_bottom_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  view = lookup_widget (warp_window, "bottom_thumbnail");

  frame = lookup_widget (warp_window, "bottom_frame");
  text = t_terrain_get_title (terrain);
  gtk_frame_set_label (GTK_FRAME (frame), text);
  g_free (text);

  terrain = t_terrain_clone_preview (terrain);
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), terrain);

  warp_apply (warp_window, TRUE);
}


void
on_merge_ok_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *merge_window;

  merge_window = lookup_toplevel (GTK_WIDGET (button));
  merge_apply (merge_window, FALSE);
  gtk_widget_destroy (merge_window);
}


void
on_merge_cancel_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_destroy (lookup_toplevel (GTK_WIDGET (button)));
}


void
on_merge_list_select_row               (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  GtkWidget *widget;
  GtkWidget *merge_window;
  gint      *row_number;

  row_number = g_new (gint, 1);
  *row_number = row;
  merge_window = lookup_toplevel (GTK_WIDGET (clist));
  gtk_object_set_data_full (GTK_OBJECT (merge_window), "data_row",
                            row_number, (GtkDestroyNotify) g_free);

  widget = lookup_widget (GTK_WIDGET (clist), "top_select");
  gtk_widget_set_sensitive (widget, TRUE);
  widget = lookup_widget (GTK_WIDGET (clist), "bottom_select");
  gtk_widget_set_sensitive (widget, TRUE);
}


void
on_merge_list_unselect_row             (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  GtkWidget *widget;

  widget = lookup_widget (GTK_WIDGET (clist), "top_select");
  gtk_widget_set_sensitive (widget, FALSE);
  widget = lookup_widget (GTK_WIDGET (clist), "bottom_select");
  gtk_widget_set_sensitive (widget, FALSE);
}


void
on_merge_top_select_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *main_window;
  GtkWidget *terrain_window;
  GtkWidget *merge_window;
  GtkWidget *frame;
  GtkWidget *view;
  TTerrain  *terrain;
  gint       row;
  gchar     *text;

  merge_window = lookup_toplevel (GTK_WIDGET (button));
  row = *((gint*) gtk_object_get_data (GTK_OBJECT (merge_window), "data_row"));

  main_window = lookup_widget (merge_window, "data_parent");
  terrain_window = main_window_get_terrain_window (main_window, row);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (merge_window),
                            "data_top_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  view = lookup_widget (merge_window, "top_thumbnail");

  frame = lookup_widget (merge_window, "top_frame");
  text = t_terrain_get_title (terrain);
  gtk_frame_set_label (GTK_FRAME (frame), text);
  g_free (text);

  terrain = t_terrain_clone_preview (terrain);
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), terrain);

  merge_apply (merge_window, TRUE);
  gtk_widget_queue_resize (merge_window);
}


void
on_merge_bottom_select_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *main_window;
  GtkWidget *terrain_window;
  GtkWidget *merge_window;
  GtkWidget *frame;
  GtkWidget *view;
  TTerrain  *terrain;
  gint       row;
  gchar     *text;

  merge_window = lookup_toplevel (GTK_WIDGET (button));
  row = *((gint*) gtk_object_get_data (GTK_OBJECT (merge_window), "data_row"));

  main_window = lookup_widget (merge_window, "data_parent");
  terrain_window = main_window_get_terrain_window (main_window, row);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (merge_window),
                            "data_bottom_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  view = lookup_widget (merge_window, "bottom_thumbnail");

  frame = lookup_widget (merge_window, "bottom_frame");
  text = t_terrain_get_title (terrain);
  gtk_frame_set_label (GTK_FRAME (frame), text);
  g_free (text);

  terrain = t_terrain_clone_preview (terrain);
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), terrain);

  merge_apply (merge_window, TRUE);
  gtk_widget_queue_resize (merge_window);
}


void
on_native_save_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_scale_ok_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *view;
  TTerrain  *terrain;
  GtkWidget *terrain_window;
  GtkWidget *scale_window;
  gboolean   double_size;

  scale_window = lookup_toplevel (GTK_WIDGET (button));
  terrain_window = lookup_widget (scale_window, "data_parent");
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  double_size = get_boolean (GTK_WIDGET (button), "double_size");

  if (double_size)
    {
      gfloat local_scaling;
      gfloat global_scaling;

      local_scaling = get_float (scale_window, "local_scaling");
      global_scaling = get_float (scale_window, "global_scaling");
      t_terrain_double (terrain, local_scaling, global_scaling);
    }
  else
    t_terrain_half (terrain);

  terrain_window_save_state (terrain_window, _("Scale"));
  t_terrain_heightfield_modified (terrain);

  gtk_widget_destroy (scale_window);
}

void
on_scale_toggled                       (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  gboolean double_size;

  double_size = get_boolean (GTK_WIDGET (togglebutton), "double_size");
  gtk_widget_set_sensitive (
    lookup_widget (GTK_WIDGET (togglebutton),
                   "local_label"), double_size);
  gtk_widget_set_sensitive (
    lookup_widget (GTK_WIDGET (togglebutton),
                   "local_scaling"), double_size);
  gtk_widget_set_sensitive (
    lookup_widget (GTK_WIDGET (togglebutton),
                   "global_label"), double_size);
  gtk_widget_set_sensitive (
    lookup_widget (GTK_WIDGET (togglebutton),
                   "global_scaling"), double_size);
}


void
on_terrain_print_preview_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
#if 0
  GnomePrintContext *context;
  GtkWidget         *preview_window;
  GtkWidget         *terrain_window;
  GtkWidget         *canvas;
  GtkWidget         *view;
  TTerrain          *terrain;

  preview_window = create_print_preview_window ();

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  canvas = lookup_widget (preview_window, "preview_canvas");

  context = gnome_print_preview_new (GNOME_CANVAS (canvas), "US-Letter");
  
  gnome_print_beginpage (context, "Terrain");
  t_terrain_print_contour_map (terrain, context);
  gnome_print_showpage (context);
  gnome_print_context_close (context);

  gtk_widget_show (preview_window);
#endif
}

void
on_select_invert_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window;
  TTerrain  *terrain;
  GtkWidget *view;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_select_invert (terrain);
  t_terrain_selection_modified (terrain);
}


void
on_select_all_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_select_all (terrain);
  t_terrain_selection_modified (terrain);
}


void
on_select_none_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  if (terrain->selection != NULL)
    {
      t_terrain_select_none (terrain);
      t_terrain_selection_modified (terrain);
    }
}


static void
feather_apply                          (GtkWidget       *window,
                                        TTerrain        *terrain)
{
  gint radius;

  radius = get_int (window, "radius");
  t_terrain_select_feather (terrain, radius);
}


void
on_select_feather_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_feather_window,
                     &feather_apply,
                     hscales,
                     FALSE);
}


static void
select_by_height_apply                 (GtkWidget       *window,
                                        TTerrain        *terrain)
{
  gfloat floor, ceil;
  TTerrainView *view;
  GtkWidget    *terrain_window;

  floor = get_float (window, "floor");
  ceil = get_float (window, "ceiling");

  terrain_window = lookup_widget (window, "data_parent");
  view = T_TERRAIN_VIEW (lookup_widget (terrain_window, "terrain"));
  t_terrain_select_by_height (terrain, floor, ceil, view->compose_op);
}


void
on_by_height_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { "floor", "ceiling", NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_select_by_height_window,
                     &select_by_height_apply,
                     hscales,
                     FALSE);
}


void
on_level_connector_count_changed       (GtkEditable     *editable,
                                        gpointer         user_data)
{
  filter_window_update_preview (lookup_toplevel (GTK_WIDGET (editable)));
}


typedef struct PlaceObjectsData PlaceObjectsData;

struct PlaceObjectsData
{
  gchar    *object_type;
  gfloat    elevation_min;
  gfloat    elevation_max;
  gfloat    density;
  gfloat    variance;
  gfloat    scale_x, scale_y, scale_z;
  gfloat    variance_x, variance_y, variance_z;
  gboolean  uniform_scaling;
  gboolean  vary_direction;
  gboolean  decrease_density_bottom; 
  gboolean  decrease_density_top;
  gboolean  vary_bottom_edge;
  gboolean  vary_top_edge;
  gboolean  new_seed;
  gint      seed;
};


static void
place_objects_data_free (PlaceObjectsData *data)
{
  g_free (data->object_type);
  g_free (data);
}


static void
place_objects_pack (GtkWidget        *window,
                    PlaceObjectsData *data)
{
  GList *object_names;
  gchar *object_type;

  g_free (data->object_type);
  object_names = gtk_object_get_data (GTK_OBJECT (window), "data_objects");
  object_type = g_list_nth_data (object_names, get_option (window, "object"));
  data->object_type = g_strdup (object_type);
  data->elevation_min = get_float (window, "elevation_min");
  data->elevation_max = get_float (window, "elevation_max");
  data->density = get_float (window, "density");
  data->variance = get_float (window, "variance");
  data->scale_x = get_float (window, "scale_x");
  data->scale_y = get_float (window, "scale_y");
  data->scale_z = get_float (window, "scale_z");
  data->variance_x = get_float (window, "variance_x");
  data->variance_y = get_float (window, "variance_y");
  data->variance_z = get_float (window, "variance_z");
  data->vary_direction = get_boolean (window, "vary_direction");
  data->uniform_scaling = get_boolean (window, "uniform_scaling");
  data->decrease_density_bottom = get_boolean (window, "decrease_density_bottom");
  data->decrease_density_top = get_boolean (window, "decrease_density_top");
  data->vary_bottom_edge = get_boolean (window, "vary_bottom_edge");
  data->vary_top_edge = get_boolean (window, "vary_top_edge");
  data->new_seed = get_boolean (window, "new_seed");
  data->seed = get_int (window, "seed");
}


static void
place_objects_unpack (GtkWidget  *window,
                      PlaceObjectsData *data)
{
  GList *object_names;
  GList *iter;
  int    i;

  object_names = gtk_object_get_data (GTK_OBJECT (window), "data_objects");
  for (iter = g_list_first (object_names), i = 0;
       iter != NULL;
       iter = g_list_next (iter), i++)
    {
      if (!strcmp ((gchar*) iter->data, data->object_type))
        set_option (window, "object", i);
    }
  set_float (window, "elevation_min", data->elevation_min);
  set_float (window, "elevation_max", data->elevation_max);
  set_float (window, "density", data->density);
  set_float (window, "variance", data->variance);
  set_float (window, "scale_x", data->scale_x);
  set_float (window, "scale_y", data->scale_y);
  set_float (window, "scale_z", data->scale_z);
  set_float (window, "variance_x", data->variance_x);
  set_float (window, "variance_y", data->variance_y);
  set_float (window, "variance_z", data->variance_z);
  set_boolean (window, "vary_direction", data->vary_direction);
  set_boolean (window, "uniform_scaling", data->uniform_scaling);
  set_boolean (window, "decrease_density_bottom", data->decrease_density_bottom);
  set_boolean (window, "decrease_density_top", data->decrease_density_top);
  set_boolean (window, "vary_bottom_edge", data->vary_bottom_edge);
  set_boolean (window, "vary_top_edge", data->vary_top_edge);
  set_boolean (window, "new_seed", data->new_seed);
  set_int (window, "seed", data->seed);
}


static void
place_objects_apply                    (GtkWidget       *window,
                                        TTerrain        *terrain)
{
  GtkWidget        *terrain_window;
  PlaceObjectsData *data;

  data = g_new0 (PlaceObjectsData, 1);
  place_objects_pack (window, data);

  if (data->new_seed)
    data->seed = new_seed ();

  terrain_window = lookup_widget (window, "data_parent");
  gtk_object_set_data_full (GTK_OBJECT (terrain_window),
                            "data/objects/place", data,
                            (GtkDestroyNotify) &place_objects_data_free);

  t_terrain_place_objects (terrain, data->object_type,
    data->elevation_min, data->elevation_max, data->density, data->variance,
    data->scale_x, data->scale_y, data->scale_z,
    data->variance_x, data->variance_y, data->variance_z,
    data->vary_direction, data->uniform_scaling, data->decrease_density_bottom, 
    data->decrease_density_top, data->vary_bottom_edge, data->vary_top_edge,
    data->seed);

  terrain_window_update_menu (terrain_window);
}


static GtkWidget *
create_place_window_advanced           (TTerrain        *terrain,
                                        GtkWidget       *terrain_window)
{
  gchar            *text;
  gfloat            pov_x, pov_y, pov_z;
  GtkWidget        *place_window;
  GtkWidget        *object_menu;
  GList            *list;
  PlaceObjectsData *data;

  place_window = create_place_objects_window ();

  list = object_list_new ();
  object_menu = gtk_menu_new ();
  if (list == NULL)
    list = g_list_append (list, g_strdup (_("[No Objects Installed]")));

  gtk_object_set_data_full (GTK_OBJECT (place_window),
                            "data_objects", list,
                            (GtkDestroyNotify) file_list_free);

  t_terrain_get_povray_size (terrain, &pov_x, &pov_y, &pov_z);
  text = g_strdup_printf (_("The terrain size is X=%.2f Y=%.2f Z=%.2f"), pov_x, pov_y, pov_z);
  set_text (place_window, "terrain_info", text);
  g_free (text);

  set_float (place_window, "elevation_min", terrain->sealevel);
  set_float (place_window, "elevation_max", (1.0 + terrain->sealevel) * 0.5);

  for (; list != NULL; list = list->next)
    {
      GtkWidget *object_menuitem;
      guchar    *friendly_name;

      friendly_name = filename_get_friendly ((gchar*) list->data);
      object_menuitem = gtk_menu_item_new_with_label (friendly_name);
      g_free (friendly_name);

      gtk_widget_show (object_menuitem);
      gtk_menu_append (GTK_MENU (object_menu), object_menuitem);
    }

  gtk_option_menu_set_menu (
    GTK_OPTION_MENU (lookup_widget (place_window, "object")),
    object_menu);

  data = lookup_data (terrain_window, "data/objects/place");
  if (data != NULL)
    place_objects_unpack (place_window, data);

  return place_window;
}


static void
rescale_placed_objects_apply           (GtkWidget       *window,
                                        TTerrain        *terrain)
{
  gfloat    scale_x, scale_y, scale_z;
  gfloat    variance_x, variance_y, variance_z;
  gboolean  uniform_scaling;
  gint      seed;

  scale_x = get_float (window, "scale_x");
  scale_y = get_float (window, "scale_y");
  scale_z = get_float (window, "scale_z");
  variance_x = get_float (window, "variance_x");
  variance_y = get_float (window, "variance_y");
  variance_z = get_float (window, "variance_z");
  uniform_scaling = get_boolean (window, "proportional");

  if (get_boolean (window, "new_seed"))
    seed = -1;
  else
    seed = get_int (window, "seed");

  t_terrain_rescale_placed_objects (terrain, scale_x, scale_y, scale_z, 
                                    variance_x, variance_y, variance_z,
                                    uniform_scaling, seed);
}


void
on_terrain_place_activate                      (GtkMenuItem     *menuitem,
                                                gpointer         user_data)
{
  /*
   * Even though the place objects window contains hscales, it does not have
   * a preview.  Therefore, we don't bother adding update signal
   * callbacks to the hscales.
   */

  gchar *hscales[] = { NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_place_window_advanced,
                     &place_objects_apply,
                     hscales,
                     FALSE);
}


void
on_terrain_remove_all_activate                 (GtkMenuItem     *menuitem,
                                                gpointer         user_data)
{
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_remove_objects (terrain);
  terrain_window_update_menu (terrain_window);
}


void
on_terrain_remove_selected_activate            (GtkMenuItem     *menuitem,
                                                gpointer         user_data)
{
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  /* need to base this on view since changes must propagate to tcanvas */
  t_terrain_view_delete_selected_objects (T_TERRAIN_VIEW (view));
  terrain_window_update_menu (terrain_window);
}


void
on_terrain_rescale_placed_activate             (GtkMenuItem     *menuitem,
                                                gpointer         user_data)
{
  /*
   * Even though the place objects window contains hscales, it does not have
   * a preview.  Therefore, we don't bother adding update signal
   * callbacks to the hscales.
   */

  gchar *hscales[] = { NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_rescale_placed_objects_window,
                     &rescale_placed_objects_apply,
                     hscales,
                     FALSE);
}


void
on_generic_radio_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  if (gtk_toggle_button_get_active (togglebutton))
    filter_window_update_preview (lookup_toplevel (GTK_WIDGET (togglebutton)));
}


void
on_place_ok_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *filter_window;
  GtkWidget *terrain_window;

  filter_window = lookup_toplevel (GTK_WIDGET (button));
  terrain_window = lookup_widget (filter_window, "data_parent");
  filter_window_perform (filter_window);
}


static void
goto_page (gchar *page)
{
  gchar *path;
  GError *error = NULL;
  gboolean result;

  path = g_strdup_printf (GNOME_DATA_DIR "/gnome/help/terraform/C/%s\n", page);
  result = gnome_help_display_uri (path, &error);
  g_free (path);

  if (result != TRUE)
    {
      fprintf (stderr, "Could not display help page %s: %s\n", page, error->message);
      g_error_free (error); 
      error = NULL;
    }
}


void
on_tutorial_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  goto_page ("tutorial.html");
}


void
on_users_guide_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  goto_page ("index.html");
}


void
on_sub_generate_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
  TTerrain        *terrain;
  GtkWidget       *subdivision_window, *main_window;
  SubdivisionData *data;

  subdivision_window = lookup_toplevel (GTK_WIDGET (button));
  main_window = lookup_widget (GTK_WIDGET (subdivision_window), "data_parent");

  if (get_boolean (subdivision_window, "new_seed"))
    set_int (subdivision_window, "seed", new_seed ());

  data = g_new0 (SubdivisionData, 1);
  subdivision_pack (subdivision_window, data);
  gtk_object_set_data_full (GTK_OBJECT (main_window),
                            "data/generator/subdivision", data,
                            (GtkDestroyNotify) g_free);

  terrain = t_terrain_generate_subdiv (data->method, data->size,
                                       data->scale, data->seed);

  if (terrain != NULL)
    {
      GtkWidget *terrain_window;

      terrain_window = gtk_object_get_data (GTK_OBJECT (subdivision_window),
                                            "data_terrain_window");

      if (terrain_window == NULL)
        {
          terrain_window = terrain_window_new (main_window, terrain);
          set_text (subdivision_window, "generate", _("Regenerate"));
          set_text (subdivision_window, "cancel", _("Finish"));
          gtk_object_set_data (GTK_OBJECT (subdivision_window),
                               "data_terrain_window", terrain_window);
        }
      else
        terrain_window_set_terrain (terrain_window, terrain);

      terrain_window_save_state (terrain_window, _("Subdivision Synthesis"));
    }
}


void
on_sub_cancel_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *subdivision_window;

  subdivision_window = lookup_toplevel (GTK_WIDGET (button));

  gtk_widget_destroy (subdivision_window);
}


void
on_fault_option_selected              (GtkMenuShell *menu_shell,
		                       gpointer data)
{
  GtkWidget *active_item;
  GtkWidget *faulting_window;
  gint item_index;
  gboolean cycles_sensitive = FALSE;
  gboolean constant_size_sensitive = FALSE;
	      
  faulting_window = lookup_toplevel (GTK_WIDGET (menu_shell));
  active_item = gtk_menu_get_active (GTK_MENU (menu_shell));
  item_index = g_list_index (menu_shell->children, active_item);
  if (item_index==4)
    constant_size_sensitive = TRUE;
  if (item_index==4 || item_index==5)
    cycles_sensitive = TRUE;

  gtk_widget_set_sensitive (lookup_widget (GTK_WIDGET(faulting_window),"fault_cycles_label"), cycles_sensitive);
  gtk_widget_set_sensitive (lookup_widget (GTK_WIDGET(faulting_window),"fault_cycles"), cycles_sensitive);
  gtk_widget_set_sensitive (lookup_widget (GTK_WIDGET(faulting_window),"constant_size_label"), constant_size_sensitive);
  gtk_widget_set_sensitive (lookup_widget (GTK_WIDGET(faulting_window),"constant_size"), constant_size_sensitive);
}


void
on_fault_generate_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
  TTerrain     *terrain;
  GtkWidget    *faulting_window, *main_window;
  FaultingData *data;

  faulting_window = lookup_toplevel (GTK_WIDGET (button));
  main_window = lookup_widget (GTK_WIDGET (faulting_window), "data_parent");

  if (get_boolean (faulting_window, "new_seed"))
    set_int (faulting_window, "seed", new_seed ());

  data = g_new0 (FaultingData, 1);
  faulting_pack (faulting_window, data);
  gtk_object_set_data_full (GTK_OBJECT (main_window),
                            "data/generator/faulting", data,
                            (GtkDestroyNotify) g_free);

  terrain =
    t_terrain_generate_fault (data->method, data->size, data->iterations,
                              data->seed, data->scale, data->cycles,
                              data->constant_size);

  if (terrain != NULL)
    {
      GtkWidget *terrain_window;

      terrain_window = gtk_object_get_data (GTK_OBJECT (faulting_window),
                                            "data_terrain_window");

      if (terrain_window == NULL)
        {
          terrain_window = terrain_window_new (main_window, terrain);
          set_text (faulting_window, "generate", _("Regenerate"));
          set_text (faulting_window, "cancel", _("Finish"));
          gtk_object_set_data (GTK_OBJECT (faulting_window),
                               "data_terrain_window", terrain_window);
        }
      else
        terrain_window_set_terrain (terrain_window, terrain);

      terrain_window_save_state (terrain_window, _("Faulting Synthesis"));
    }
}


void
on_fault_cancel_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *faulting_window;

  faulting_window = lookup_toplevel (GTK_WIDGET (button));

  gtk_widget_destroy (faulting_window);
}


void
on_ss_generate_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
  TTerrain     *terrain;
  GtkWidget    *spectral_window, *main_window;
  SpectralData *data;

  spectral_window = lookup_toplevel (GTK_WIDGET (button));
  main_window = lookup_widget (GTK_WIDGET (spectral_window), "data_parent");

  if (get_boolean (spectral_window, "new_seed"))
    set_int (spectral_window, "seed", new_seed ());

  data = g_new0 (SpectralData, 1);
  spectral_pack (spectral_window, data);
  gtk_object_set_data_full (GTK_OBJECT (main_window),
                            "data/generator/spectral", data,
                            (GtkDestroyNotify) g_free);

  terrain = t_terrain_generate_spectral (data->size,
                                         3.0 - data->dimensions,
                                         data->seed, data->invert);

  if (terrain != NULL)
    {
      GtkWidget *terrain_window;

      terrain_window = gtk_object_get_data (GTK_OBJECT (spectral_window),
                                            "data_terrain_window");

      if (terrain_window == NULL)
        {
          terrain_window = terrain_window_new (main_window, terrain);
          set_text (spectral_window, "generate", _("Regenerate"));
          set_text (spectral_window, "cancel", _("Finish"));
          gtk_object_set_data (GTK_OBJECT (spectral_window),
                               "data_terrain_window", terrain_window);
        }
      else
        terrain_window_set_terrain (terrain_window, terrain);

      terrain_window_save_state (terrain_window, _("Spectral Synthesis"));
    }
}


void
on_ss_cancel_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *spectral_window;

  spectral_window = lookup_toplevel (GTK_WIDGET (button));

  gtk_widget_destroy (spectral_window);
}


void
on_pn_generate_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
  TTerrain   *terrain;
  GtkWidget  *widget;
  GtkWidget  *pn_window;
  GtkWidget  *main_window;
  PerlinData *data;

  widget = GTK_WIDGET (button);
  pn_window = lookup_toplevel (GTK_WIDGET (button));
  main_window = lookup_widget (GTK_WIDGET (pn_window), "data_parent");

  if (get_boolean (pn_window, "new_seed"))
    set_int (pn_window, "seed", new_seed ());

  data = g_new0 (PerlinData, 1);
  perlin_pack (pn_window, data);
  gtk_object_set_data_full (GTK_OBJECT (main_window),
                            "data/generator/perlin", data,
                            (GtkDestroyNotify) g_free);

  terrain =
    t_terrain_generate_perlin (data->size, data->size, data->seed,
                               data->enabled, data->frequency, data->amplitude,
                               data->iterations, data->filter);

  if (terrain != NULL)
    {
      GtkWidget *terrain_window;

      terrain_window =
        gtk_object_get_data (GTK_OBJECT (pn_window), "data_terrain_window");

      if (terrain_window == NULL)
        {
          terrain_window = terrain_window_new (main_window, terrain);
          set_text (pn_window, "generate", _("Regenerate"));
          set_text (pn_window, "cancel", _("Finish"));
          gtk_object_set_data (GTK_OBJECT (pn_window),
                               "data_terrain_window", terrain_window);
        }
      else
        terrain_window_set_terrain (terrain_window, terrain);

      terrain_window_save_state (terrain_window, _("Perlin Noise"));
    }
}



void
on_pn_cancel_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *perlin_window;

  perlin_window = lookup_toplevel (GTK_WIDGET (button));

  gtk_widget_destroy (perlin_window);
}


GtkWidget*
create_terrain_preview (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
  GtkWidget *view;

  view = t_terrain_view_new ();
  t_terrain_view_set_size (T_TERRAIN_VIEW (view), 100);
  gtk_widget_set_events (view, GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON1_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
  gtk_widget_show_all (view);

  return view;
}


GtkWidget*
create_transfer_function (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
  GtkWidget *widget;

  widget = t_canvas_new ();
  gtk_widget_show_all (widget);

  return widget;
}

void
on_transfer_function_realize           (GtkWidget       *widget,
                                        gpointer         user_data)
{
  GtkWidget *canvas;
  TPixbuf   *pixbuf;

  canvas = lookup_widget (widget, "transfer_function");
  pixbuf = t_canvas_get_pixbuf (T_CANVAS (canvas));
  t_terrain_draw_transform (pixbuf,
                            get_float (widget, "sea_threshold"),
                            get_float (widget, "sea_depth"),
                            get_float (widget, "sea_dropoff"),
                            get_float (widget, "above_power"),
                            get_float (widget, "below_power"));
}


GtkWidget*
create_native_image (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
  GtkWidget *widget;

  widget = t_canvas_new ();
  gtk_widget_show_all (widget);

  return widget;
}


static gchar *tool_group[] = {
  "move", "rectangular_select", "elliptical_select", "square_zoom_select", "crop_new", "square_seed", NULL
};


static gchar *operator_group[] = {
  "replace", "add", "subtract", NULL
};


void
on_toolbar_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget  *terrain_window;
  TToolMode   tool;
  TComposeOp  op;
  GtkWidget  *view;
  TTerrain   *terrain;

  terrain_window = lookup_toplevel (GTK_WIDGET (button));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  tool = get_radio_menu (terrain_window, tool_group);
  t_terrain_view_set_tool (T_TERRAIN_VIEW (view), tool);

  op = get_radio_menu (terrain_window, operator_group) + 1;

  /* when the zoom, crop or seed icons are clicked, the compose_op is reset 
   * to T_COMPOSE_REPLACE and the existing selection cleared */
  if (tool == T_TOOL_SELECT_SQUARE_ZOOM || 
      tool == T_TOOL_SELECT_CROP_NEW ||
      tool == T_TOOL_SELECT_SQUARE_SEED) 
    {
    GtkWidget *button = lookup_widget (terrain_window, "replace");

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), TRUE);
    t_terrain_select_none (terrain);
    t_canvas_set_selection (T_CANVAS (view), T_SELECTION_NONE, 0, 0, 0, 0);
    op = T_COMPOSE_REPLACE;

    t_terrain_selection_modified (terrain);
    }

  t_terrain_view_set_compose_op (T_TERRAIN_VIEW (view), op);
}


void
on_terrain_view_export_file_activate   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *export_view;
  GtkWidget *terrain_window;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  export_view = create_export_view_window ();
  add_file_type_menu (export_view, export_view_file_type_info);
  gtk_object_set_data (GTK_OBJECT (export_view), "data_parent", terrain_window);
  gtk_widget_show (export_view);
}


void
on_export_view_ok_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget    *export_view;
  GtkWidget    *terrain_window;
  GtkWidget    *view;
  const gchar  *filename;
  gint          status;
  TFileType     type;
  FileTypeInfo *info;

  export_view = lookup_toplevel (GTK_WIDGET (button));
  terrain_window = gtk_object_get_data (GTK_OBJECT (export_view), "data_parent");
  view = lookup_widget (terrain_window, "terrain");
  filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (export_view));
  info = gtk_object_get_data (GTK_OBJECT (export_view), "data_file_type_info");
  type = info[get_option (export_view, "type_menu")].type;
  status = t_terrain_view_export_view (T_TERRAIN_VIEW (view), type, filename);

  gtk_widget_destroy (export_view);

  if (status != 0)
    {
      GtkWidget *error_dialog;

      error_dialog = gnome_error_dialog (_("An error occurred while exporting the view."));

      gtk_widget_show (error_dialog);
    }
}


void
on_terrain_view_export_gimp_activate   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *view;
  GtkWidget *terrain_window;
  TTerrain  *terrain;
  gchar     *tmpdir;
  gchar     *tmp, *base, *png;
  gchar     *command;
  gint       rc;
  GList     *cleanup_list;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  cleanup_list = NULL;

  /* Create a safe temp directory */
  tmpdir = filename_create_tempdir ();
  if (tmpdir == NULL)
    {
      on_file_save_error (terrain->filename);
      return;
    }
  cleanup_list = g_list_prepend (cleanup_list, tmpdir);

  base = filename_get_base (terrain->filename);
  tmp = g_strdup_printf ("%s/%s", tmpdir, base);
  g_free (base);

  png = filename_new_extension (tmp, "png");
  cleanup_list = g_list_prepend (cleanup_list, png);
  g_free (tmp);

  rc = t_terrain_view_export_view (T_TERRAIN_VIEW (view), FILE_PNG, png);

  if (rc)
    {
      cleanup (cleanup_list);
      on_file_save_error (terrain->filename);
      return;
    }

  command = g_strdup_printf ("gimp %s", png);

  execute_command (command, _("There was an error executing The GIMP"), cleanup,
                   cleanup_list);

  g_free (command);
}


static void
on_choose_terrain_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *main_window;
  GtkWidget *view;
  TTerrain  *terrain;

  main_window = lookup_data (GTK_WIDGET (button), "data_parent");
  view = gtk_object_get_data (GTK_OBJECT (button), "view");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  if (terrain != NULL)
    {
      GtkWidget *terrain_window;

      terrain_window = terrain_window_new (main_window, terrain);
      terrain_window_save_state (terrain_window, _("Generate Random"));
    }
}


GtkWidget*
create_choose_terrain (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
  GtkWidget *button;
  GtkWidget *aspect;
  GtkWidget *view;

  button = gtk_button_new ();
  aspect = gtk_aspect_frame_new (NULL, 0.5, 0.5, 1.0, FALSE);
  gtk_frame_set_shadow_type (GTK_FRAME (aspect), GTK_SHADOW_IN);
  view = t_terrain_view_new ();
  gtk_container_add (GTK_CONTAINER (aspect), view);
  gtk_container_add (GTK_CONTAINER (button), aspect);
  gtk_widget_show_all (button);

  gtk_object_ref (GTK_OBJECT (view));
  gtk_object_set_data_full (GTK_OBJECT (button), "view", view,
                            (GtkDestroyNotify) gtk_object_unref);

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      (GtkSignalFunc) on_choose_terrain_clicked,
                      NULL);
  gtk_widget_set_size_request (button, 100, 100);
  gtk_widget_show_all (button);
  return button;
}


void
on_randomize_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
  MainState *main_state;
  GtkWidget *chooser_window;
  GtkWidget *main_window;
  gint       i;
  int        types;
  char *widgets[] = {
    "terrain1", "terrain2", "terrain3",
    "terrain4", "terrain5", "terrain6",
    "terrain7", "terrain8", "terrain9",
    NULL };


  chooser_window = lookup_toplevel (GTK_WIDGET (button));
  main_window = lookup_data (chooser_window, "data_parent");
  main_state = lookup_data (main_window, "data_state");

  gtk_object_ref (GTK_OBJECT (chooser_window));
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

  /* make sure the type bitmap holds a valid value */
  if (user_data != NULL)
    types = *((int*)user_data);
  else
    types = 0;

  for (i = 0; widgets[i] != NULL; i++)
    {
      TTerrain  *terrain;
      GtkWidget *widget;
      GtkWidget *view;

      while (gtk_events_pending ())
        gtk_main_iteration ();

      /* Break out of loop if window is closed. */
      if (!GTK_WIDGET_REALIZED (chooser_window))
        break;

      widget = lookup_widget (GTK_WIDGET (button), widgets[i]);
      view = gtk_object_get_data (GTK_OBJECT (widget), "view");

      terrain = t_terrain_generate_random (main_state->default_size, main_state->genfactor, main_state->gentypes);
      t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), terrain);
    }

  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);
  gtk_object_unref (GTK_OBJECT (chooser_window));
}


GtkWidget*
create_randomize (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
  GtkWidget *button;
  GtkWidget *hbox;
  GtkWidget *label;
  GtkWidget *die;

  button = gtk_button_new ();
  hbox = gtk_hbox_new (FALSE, 0);
  die = create_pixmap (button, TERRAFORM_DATA_DIR "/glade/die.xpm");
  label = gtk_label_new (_("Roll Again"));
  gtk_box_pack_start (GTK_BOX (hbox), die, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (button), hbox);
  gtk_widget_show_all (button);

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      (GtkSignalFunc) on_randomize_clicked,
                      NULL);
  gtk_widget_show_all (button);
  return button;
}


void
on_chooser_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *chooser, *main_window;
  GtkWidget *randomize;

  chooser = create_chooser_window ();
  main_window = lookup_toplevel (GTK_WIDGET (menuitem));
  gtk_object_set_data (GTK_OBJECT (chooser), "data_parent", main_window);
  gtk_widget_show (chooser);

  randomize = lookup_widget (chooser, "randomize");
  on_randomize_clicked (GTK_BUTTON (randomize), NULL);
}


GtkWidget*
create_terrain_aspect_frame (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
  GtkWidget *view;
  GtkWidget *aspect_frame;

  aspect_frame = t_aspect_frame_new ();

  view = t_terrain_view_new ();
  t_terrain_view_set_size (T_TERRAIN_VIEW (view), 300);
  t_terrain_view_set_tool (T_TERRAIN_VIEW (view), T_TOOL_MOVE);
  t_terrain_view_set_compose_op (T_TERRAIN_VIEW (view), T_COMPOSE_REPLACE);

  GTK_WIDGET_UNSET_FLAGS (view, GTK_CAN_FOCUS);
  GTK_WIDGET_UNSET_FLAGS (view, GTK_CAN_DEFAULT);
  gtk_widget_set_events (view, GDK_POINTER_MOTION_HINT_MASK |
                               GDK_BUTTON1_MOTION_MASK |
                               GDK_BUTTON_PRESS_MASK |
                               GDK_BUTTON_RELEASE_MASK |
                               GDK_KEY_PRESS_MASK |
                               GDK_KEY_RELEASE_MASK |
                               GDK_FOCUS_CHANGE_MASK |
                               GDK_SCROLL);

  gtk_frame_set_shadow_type (GTK_FRAME (aspect_frame), GTK_SHADOW_IN);
  gtk_container_add (GTK_CONTAINER (aspect_frame), view);
  gtk_widget_show_all (aspect_frame);

  return aspect_frame;
}


void
on_save_terrain_no_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *close_anyways_window, *terrain_window, *main_window;
  GtkWidget *modal;

  close_anyways_window = lookup_toplevel (GTK_WIDGET (button));
  terrain_window = gtk_object_get_data (GTK_OBJECT (close_anyways_window),
                                        "data_parent");
  main_window = gtk_object_get_data (GTK_OBJECT (terrain_window),
                                     "data_parent");

  modal = gtk_object_get_data (GTK_OBJECT (terrain_window), "data_modal");
  if (modal != NULL)
    gtk_widget_destroy (modal);

  main_window_remove_terrain_window (main_window, terrain_window);

  gtk_widget_destroy (close_anyways_window);
}


// on_save_terrain_yes_clicked contributed by Koos Jan Niesink
void
on_save_terrain_yes_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window;
  GtkWidget *view, *close_dialog;
  GtkWidget *main_window;
  GtkWidget *modal;
  TTerrain  *terrain;
  gint       status;
  
  //to save the tf-file
  close_dialog = lookup_toplevel (GTK_WIDGET (button));
  terrain_window = gtk_object_get_data (GTK_OBJECT (close_dialog),
                                        "data_parent");
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  status = t_terrain_save (terrain, FILE_NATIVE, NULL);

  if (status)
    on_file_save_error (terrain->filename);
    
  //to close the terrainwindow
  main_window = gtk_object_get_data (GTK_OBJECT (terrain_window),
                                     "data_parent");

  modal = gtk_object_get_data (GTK_OBJECT (terrain_window), "data_modal");
  if (modal != NULL)
    gtk_widget_destroy (modal);

  main_window_remove_terrain_window (main_window, terrain_window);

  gtk_widget_destroy (close_dialog); 
}


void
on_save_all_terrains_yes_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *exit_anyways_window, *main_window;
  GtkCList  *clist;
  gpointer   data;
  gint       row=0;

  exit_anyways_window = lookup_toplevel (GTK_WIDGET (button));
  main_window = gtk_object_get_data (GTK_OBJECT (exit_anyways_window),
                                        "data_parent");
  clist = GTK_CLIST (lookup_widget (main_window, "main_list"));
  for (row = 0; (data = gtk_clist_get_row_data (clist, row)) != NULL; row++)
    {
    GtkWidget *t_window = (GtkWidget*)data;
    GtkWidget *modal = gtk_object_get_data (GTK_OBJECT(t_window), "data_modal");
    GtkWidget *view = lookup_widget (t_window, "terrain");
    TTerrain  *terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

    if (terrain->modified)
      t_terrain_save (terrain, FILE_NATIVE, terrain->filename);

    if (modal != NULL)
      gtk_widget_destroy (modal);

    main_window_remove_terrain_window (main_window, t_window);
    }

  gtk_widget_destroy (exit_anyways_window);
  gtk_widget_destroy (main_window);
  gtk_main_quit ();
}


void
on_save_all_terrains_no_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *exit_anyways_window, *main_window;
  GtkCList  *clist;
  gpointer   data;
  gint       row=0;

  exit_anyways_window = lookup_toplevel (GTK_WIDGET (button));
  main_window = gtk_object_get_data (GTK_OBJECT (exit_anyways_window),
                                        "data_parent");
  clist = GTK_CLIST (lookup_widget (main_window, "main_list"));
  for (row = 0; (data = gtk_clist_get_row_data (clist, row)) != NULL; row++)
    {
    GtkWidget *t_window = (GtkWidget*)data;
    GtkWidget *modal = gtk_object_get_data (GTK_OBJECT(t_window), "data_modal");

    if (modal != NULL)
      gtk_widget_destroy (modal);

    main_window_remove_terrain_window (main_window, t_window);
    }

  gtk_widget_destroy (exit_anyways_window);
  gtk_widget_destroy (main_window);
  gtk_main_quit ();
}


static void
erode_flowmap_apply                           (GtkWidget       *erode_window,
                                               gboolean         preview)
{
  TTerrain  *terrain_1;
  TTerrain  *terrain_2;
  TTerrain  *terrain;
  GtkWidget *view;
  GtkWidget *main_window;
  GtkWidget *terrain_window;
  gint       iterations;
  gboolean   trim_local_peaks;

  view = lookup_widget (erode_window, "data_top_t_terrain");
  terrain_1 = gtk_object_get_data (GTK_OBJECT (erode_window),
                                       "data_top_t_terrain");
  terrain_2 = gtk_object_get_data (GTK_OBJECT (erode_window),
                                       "data_bottom_t_terrain");
  iterations = get_int (erode_window, "erode_iterations");
  trim_local_peaks = get_boolean (erode_window, "erode_trim");

  terrain = t_terrain_clone (terrain_1);
  t_terrain_erode_flowmap (terrain, terrain_2, 
		           iterations, trim_local_peaks);

  main_window = lookup_widget (erode_window, "data_parent");
  terrain_window = terrain_window_new (main_window, terrain);
  terrain_window_save_state (terrain_window, _("Erode"));
}


void
on_erode_flowmap_ok_clicked                    (GtkButton       *button,
                                                gpointer         user_data)
{
  GtkWidget *erode_window;
  
  erode_window = lookup_toplevel (GTK_WIDGET (button));
  erode_flowmap_apply (erode_window, 0);
  gtk_widget_destroy (erode_window);
}


void
on_erode_flowmap_top_select_clicked    (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *main_window;
  GtkWidget *terrain_window;
  GtkWidget *erode_window;
  GtkWidget *frame;
  GtkWidget *view;
  TTerrain  *terrain;
  gint       row;
  gchar     *text;

  erode_window = lookup_toplevel (GTK_WIDGET (button));
  row = *((gint*) gtk_object_get_data (GTK_OBJECT (erode_window), "data_row"));

  main_window = lookup_widget (erode_window, "data_parent");
  terrain_window = main_window_get_terrain_window (main_window, row);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (erode_window),
                            "data_top_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  view = lookup_widget (erode_window, "top_thumbnail");

  frame = lookup_widget (erode_window, "top_frame");
  text = t_terrain_get_title (terrain);
  gtk_frame_set_label (GTK_FRAME (frame), text);
  g_free (text);

  terrain = t_terrain_clone_preview (terrain);
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), terrain);
}


void
on_erode_flowmap_bottom_select_clicked (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *main_window;
  GtkWidget *terrain_window;
  GtkWidget *erode_window;
  GtkWidget *frame;
  GtkWidget *view;
  TTerrain  *terrain;
  gint       row;
  gchar     *text;

  erode_window = lookup_toplevel (GTK_WIDGET (button));
  row = *((gint*) gtk_object_get_data (GTK_OBJECT (erode_window), "data_row"));

  main_window = lookup_widget (erode_window, "data_parent");
  terrain_window = main_window_get_terrain_window (main_window, row);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (erode_window),
                            "data_bottom_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  view = lookup_widget (erode_window, "bottom_thumbnail");

  frame = lookup_widget (erode_window, "bottom_frame");
  text = t_terrain_get_title (terrain);
  gtk_frame_set_label (GTK_FRAME (frame), text);
  g_free (text);

  terrain = t_terrain_clone_preview (terrain);
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), terrain);
}


void
on_erode_flowmap_activate                  (GtkMenuItem     *menuitem,
                                            gpointer         user_data)
{
  GtkWidget *erode_window;
  GtkWidget     *main_window;
  GtkWidget     *terrain_window;
  GtkWidget     *area;
  GtkWidget     *frame;
  GtkWidget *view;
  TTerrain  *terrain;
  gchar         *text;

  main_window = lookup_toplevel (GTK_WIDGET (menuitem));
  erode_window = create_erode_flowmap_window ();

  gtk_object_set_data (GTK_OBJECT (erode_window), "data_parent", main_window);

  area = lookup_widget (erode_window, "top_thumbnail");
  terrain_window = main_window_get_terrain_window (main_window, 0);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (erode_window),
                            "data_top_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  frame = lookup_widget (erode_window, "top_frame");
  text = t_terrain_get_title (terrain);
  gtk_frame_set_label (GTK_FRAME (frame), text);
  g_free (text);

  terrain = t_terrain_clone_preview (terrain);
  area = lookup_widget (erode_window, "top_thumbnail");
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (area), terrain);

  area = lookup_widget (erode_window, "bottom_thumbnail");
  terrain_window = main_window_get_terrain_window (main_window, 1);
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  t_terrain_ref (terrain);
  gtk_object_set_data_full (GTK_OBJECT (erode_window),
                            "data_bottom_t_terrain", terrain,
                            (GtkDestroyNotify) t_terrain_unref);

  terrain = t_terrain_clone_preview (terrain);
  area = lookup_widget (erode_window, "bottom_thumbnail");
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (area), terrain);

  main_window_copy_objects (main_window,
    GTK_CLIST (lookup_widget (erode_window, "erode_list")));

  gtk_widget_show (erode_window);
}


void
on_save_error_log_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget	*save_filesel, *error_text;
	
	error_text = lookup_widget(GTK_WIDGET(button), "error_text");	
	save_filesel = create_error_log_filesel();
  	gtk_object_set_data (GTK_OBJECT (save_filesel), "error_text", error_text);
  	gtk_file_selection_set_filename (GTK_FILE_SELECTION (save_filesel), "error.log");
	gtk_widget_show (save_filesel);	
	
}


void
on_save_error_log_ok_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
	/* contributed by Koos Jan Niesink koos_jan@hotpop.com
	some code is from the example provided by glade 
	This code saves the povray error output to a file */
	
	GtkWidget *error_text, *save_filesel;
  	gchar *data;
	const gchar *filename;
	FILE *fp;
  	gint bytes_written, len;	
  
	save_filesel = lookup_toplevel (GTK_WIDGET (button));	
	filename = gtk_file_selection_get_filename (GTK_FILE_SELECTION (save_filesel));
	
	error_text = gtk_object_get_data(GTK_OBJECT (save_filesel), "error_text"); 
	data = gtk_editable_get_chars (GTK_EDITABLE (error_text), 0, -1);

  	fp = fopen (filename, "w");
  	if (fp == NULL)
    	{
      		g_free (data);
      		return;
    	}

  	len = strlen (data);
  	bytes_written = fwrite (data, sizeof (gchar), len, fp);
  	fclose (fp);

  	g_free (data);

  	if (len != bytes_written)
    	{
      		GtkWidget *error_dialog;
		
		error_dialog = gnome_error_dialog (_("An error occurred while writing the file."));
      		gtk_widget_show (error_dialog);

    	}
    	
	/* remove the file selection widget from the screen (and more) */
	gtk_widget_destroy(save_filesel);
}


static void
spherical_map_apply                  (GtkWidget       *window,
                                      TTerrain        *terrain)
{
  gfloat offset;

  offset = get_float (window, "offset");
  t_terrain_spherical (terrain, offset);
}


void
on_terrain_spherical_map_activate              (GtkMenuItem     *menuitem,
                                                gpointer         user_data)
{
  gchar *hscales[] = { "offset", NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_spherical_map_window,
                     &spherical_map_apply,
                     hscales,
                     TRUE);
}


void
on_undo_apply_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *view;
  TTerrain  *terrain;
  GtkWidget *undo_window;
  GtkWidget *terrain_window;
  GtkWidget *main_window;

  undo_window = lookup_toplevel (GTK_WIDGET (button));
  terrain_window =
    gtk_object_get_data (GTK_OBJECT (undo_window), "data_parent");

  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  main_window = gtk_object_get_data (GTK_OBJECT (terrain_window),
                                     "data_parent");

  undo_apply (undo_window, terrain);

  t_terrain_heightfield_modified (terrain);

}



void
on_terrain_terrain_options_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window, *view;
  GtkWidget *options_window; 
  TTerrain  *terrain;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  options_window = create_terrain_options_window ();

  gtk_object_set_data_full (GTK_OBJECT (options_window),
                            "data_parent", terrain_window,
                            terrain_window_modal_destroy);
  gtk_object_set_data (GTK_OBJECT (terrain_window),
                           "data_modal", options_window);

  t_terrain_unpack_terrain_options (terrain, options_window);

  gtk_widget_show (options_window);
}

static void
on_filetype_option_selected (GtkWidget *options_window,
			     gpointer data)
{
  gint option;
  
  option = get_option (options_window, "optionmenu_filetype");
  switch (option) {
  case 0:
    gtk_widget_set_sensitive (lookup_widget (options_window, "checkbutton_use_alpha"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "label_bits_per_colour"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "spinbutton_bits_per_colour"), TRUE);
    break;
  case 1:
    gtk_widget_set_sensitive (lookup_widget (options_window, "checkbutton_use_alpha"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "label_bits_per_colour"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "spinbutton_bits_per_colour"), FALSE);
    break;
  case 2: case 3:
    gtk_widget_set_sensitive (lookup_widget (options_window, "checkbutton_use_alpha"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "label_bits_per_colour"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "spinbutton_bits_per_colour"), FALSE);
    break;
  } 
}

static void
on_aa_type_option_selected (GtkWidget *options_window,
			    gpointer data)
{ 
  if ( get_option (options_window, "optionmenu_aa_type") == 1) /* method 2 has been chosen */
    {
      gtk_widget_set_sensitive (lookup_widget (options_window, "label_depth"), TRUE);
      gtk_widget_set_sensitive (lookup_widget (options_window, "aa_depth"), TRUE);
    }
  else /* method 1 has been chosen */
    {
      gtk_widget_set_sensitive (lookup_widget (options_window, "label_depth"), FALSE);
      gtk_widget_set_sensitive (lookup_widget (options_window, "aa_depth"), FALSE);
    }
}



void
on_terrain_render_options_activate     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window;
  GtkWidget *modal;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));

  modal = gtk_object_get_data (GTK_OBJECT (terrain_window), "data_modal");
  if (modal == NULL)
    {
      GList     *list;
      GtkWidget *view;
      TTerrain  *terrain;
      GtkWidget *options_window;
      GtkWidget *render_size_menu, *optionmenu_aa_type;
      GtkWidget *optionmenu_filetype;

      gtk_widget_set_sensitive (terrain_window, FALSE);
      options_window = create_povray_render_options_window ();

      view = lookup_widget (terrain_window, "terrain");
      terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

      gtk_object_set_data_full (GTK_OBJECT (options_window),
                                "data_parent", terrain_window,
                                terrain_window_modal_destroy);
      gtk_object_set_data (GTK_OBJECT (terrain_window),
                           "data_modal", options_window);

      /* populate render size list & menu */
      list = render_size_list_new ();
      render_size_menu = gtk_menu_new ();

      gtk_object_set_data_full (GTK_OBJECT (options_window),
				"data_render_size", list,
				(GtkDestroyNotify) file_list_free);

      for (; list != NULL; list = list->next)
	{
	  GtkWidget *render_size_menuitem;
	  gchar     *label;

	  label = g_strdup ((gchar*) list->data);
	  render_size_menuitem = gtk_menu_item_new_with_label (label);
	  g_free (label);

	  gtk_widget_show (render_size_menuitem);
	  gtk_menu_append (GTK_MENU (render_size_menu), render_size_menuitem);
	}

      gtk_option_menu_set_menu (
	  GTK_OPTION_MENU (lookup_widget (options_window, "optionmenu_render_size")), 
	  render_size_menu);

      optionmenu_filetype = lookup_widget (options_window, "optionmenu_filetype");
      gtk_signal_connect_object (GTK_OBJECT (GTK_OPTION_MENU (optionmenu_filetype)->menu),
				 "deactivate", GTK_SIGNAL_FUNC (on_filetype_option_selected),
				 GTK_OBJECT (options_window));

      optionmenu_aa_type = lookup_widget (options_window, "optionmenu_aa_type");
      gtk_signal_connect_object (GTK_OBJECT (GTK_OPTION_MENU (optionmenu_aa_type)->menu),
				 "deactivate", GTK_SIGNAL_FUNC (on_aa_type_option_selected),
				 GTK_OBJECT (options_window));

      /* unpack the TTerrain structure. */
      t_terrain_unpack_povray_options (terrain, options_window);

      gtk_widget_show (GTK_WIDGET (options_window));
    }
}

 /*
   * Added for sentivity by Raymond Ostertag
   */

static void
on_render_sky_system_selected (GtkWidget *options_window,
			     gpointer data)
{
  gint option;
  gboolean option_clouds;
  gboolean option_stars;
  gboolean option_celest_objects;
  
  option = get_option (options_window, "render_sky_system");
  option_clouds = get_boolean (options_window, "render_clouds");
  option_stars = get_boolean (options_window, "render_stars");
  option_celest_objects = get_boolean (options_window, "render_celest_objects");

  switch (option) {
  case 0: // Regular Sky
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_skycolor_type"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_skycolor_red"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_skycolor_green"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_skycolor_blue"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_clouds"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_cloud_type"), option_clouds);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_stars"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_star_type"), option_stars);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_background_image_file"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_bgimage_scale"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_bgimage_offset"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_bgimage_enlightment"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_celest_objects"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_sun_size"), option_celest_objects);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_sun_color_red"), option_celest_objects);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_sun_color_green"), option_celest_objects);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_sun_color_blue"), option_celest_objects);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_image_file"), option_celest_objects);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_rot_Y"), option_celest_objects);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_rot_Z"), option_celest_objects);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_map_enlightment"), option_celest_objects);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_size"),option_celest_objects );
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_color_red"), option_celest_objects);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_color_green"), option_celest_objects);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_color_blue"), option_celest_objects);
    break;
  case 1:  // Mapped Sky
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_skycolor_type"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_skycolor_red"),FALSE),     
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_skycolor_green"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_skycolor_blue"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_clouds"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_cloud_type"),FALSE),     
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_stars"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_star_type"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_background_image_file"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_bgimage_scale"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_bgimage_offset"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_bgimage_enlightment"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_celest_objects"), FALSE);    
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_sun_size"), FALSE);    
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_sun_color_red"), FALSE);    
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_sun_color_green"), FALSE);    
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_sun_color_blue"), FALSE);    
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_image_file"), FALSE);    
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_rot_Y"), FALSE);    
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_rot_Z"), FALSE);    
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_map_enlightment"), FALSE);    
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_size"), FALSE);    
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_color_red"), FALSE);    
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_color_green"), FALSE);    
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_color_blue"), FALSE);    
    break;
  } 
}

static void
on_render_light_system_selected (GtkWidget *options_window,
			     gpointer data)
{
  gint option;
  gboolean option_camera_light;
   
  option = get_option (options_window, "render_light_system");
  option_camera_light = get_boolean (options_window, "render_camera_light");

  switch (option) {
  case 0: // Dynamic Lights
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_time_of_day"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_time_offset"),TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_west_direction"),TRUE);

    gtk_widget_set_sensitive (lookup_widget (options_window, "render_light_type"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_camera_light"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_camera_light_luminosity"), FALSE);
    break;
  case 1:  // Static Lights
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_time_of_day"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_moon_time_offset"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_west_direction"), FALSE);

    gtk_widget_set_sensitive (lookup_widget (options_window, "render_light_type"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_camera_light"), TRUE);
    gtk_widget_set_sensitive (lookup_widget (options_window, "render_camera_light_luminosity"), option_camera_light);
     break;
  } 
}

 /* End by Raymond Ostertag */

#define SCENE_PREVIEW_SIZE 200

void
on_terrain_scene_options_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;
  TTerrain  *terrain_preview;
  GtkWidget *atm_menu;
  GtkWidget *cloud_menu;
  GtkWidget *options_window;
  GtkWidget *star_menu;
  GtkWidget *texture_menu;
  GtkWidget *water_menu;
  GtkWidget *light_menu;
  GtkWidget *scene_theme_menu;
  GtkWidget *skycolor_menu;
  GtkWidget *background_image_menu;
  GtkWidget *moon_image_menu;
  GList     *list;
  gint       tsize = SCENE_PREVIEW_SIZE;
  gint       maximum;
  gchar     *hscales[] = { "render_scale_x", 
                           "render_scale_z",
			   "render_y_scale_factor",
                           NULL };
  gchar     *spinbuttons[] = { "spinbutton_render_scale_x", 
                               "spinbutton_render_scale_z",
			       "spinbutton_render_y_scale_factor",
                               NULL };
  GtkWidget *optionmenu_render_sky_system;    
  GtkWidget *optionmenu_render_light_system;    
  GtkWidget *hbox_hyperlink;
  static GtkWidget *download_background_image;
  static GtkWidget *download_moon_image;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  options_window = create_scene_options_window ();

  //terrain_preview = t_terrain_clone_preview (terrain);
  maximum = MAX (terrain->width, terrain->height);
  terrain_preview = t_terrain_clone_resize (terrain,
              MAX (1, MIN(terrain->width * tsize / maximum, terrain->width)),
              MAX (1, MIN(terrain->height * tsize / maximum, terrain->height)),
              TRUE);
  view = lookup_widget (options_window, "preview");
  t_terrain_view_set_terrain (T_TERRAIN_VIEW (view), terrain_preview);

  gtk_object_set_data_full (GTK_OBJECT (options_window),
                            "data_parent", terrain_window,
                            terrain_window_modal_destroy);
  gtk_object_set_data (GTK_OBJECT (terrain_window),
                           "data_modal", options_window);

  /* populate texture list & menu */
  list = texture_list_new ();
  texture_menu = gtk_menu_new ();
  if (list == NULL)
    list = g_list_append (list, g_strdup (_("[No Terrain textures Installed]")));

  gtk_object_set_data_full (GTK_OBJECT (options_window),
                            "data_textures", list,
                            (GtkDestroyNotify) file_list_free);

  for (; list != NULL; list = list->next)
    {
    GtkWidget *texture_menuitem;
    guchar    *friendly_name;

    friendly_name = filename_get_friendly ((gchar*) list->data);
    texture_menuitem = gtk_menu_item_new_with_label (friendly_name);
    g_free (friendly_name);

    gtk_widget_show (texture_menuitem);
    gtk_menu_append (GTK_MENU (texture_menu), texture_menuitem);
    }

  gtk_option_menu_set_menu (
    GTK_OPTION_MENU (lookup_widget (options_window, "render_texture_type")),
    texture_menu);

  /* populate cloud list & menu */
  list = cloud_list_new ();
  cloud_menu = gtk_menu_new ();
  if (list == NULL)
    list = g_list_append (list, g_strdup (_("[No Clouds Installed]")));

  gtk_object_set_data_full (GTK_OBJECT (options_window),
                            "data_clouds", list,
                            (GtkDestroyNotify) file_list_free);

  for (; list != NULL; list = list->next)
    {
    GtkWidget *cloud_menuitem;
    guchar    *friendly_name;

    friendly_name = filename_get_friendly ((gchar*) list->data);
    cloud_menuitem = gtk_menu_item_new_with_label (friendly_name);
    g_free (friendly_name);

    gtk_widget_show (cloud_menuitem);
    gtk_menu_append (GTK_MENU (cloud_menu), cloud_menuitem);
    }

  gtk_option_menu_set_menu (
    GTK_OPTION_MENU (lookup_widget (options_window, "render_cloud_type")),
    cloud_menu);

  /* populate star list & menu */
  list = star_list_new ();
  star_menu = gtk_menu_new ();
  if (list == NULL)
    list = g_list_append (list, g_strdup (_("[No Stars Installed]")));

  gtk_object_set_data_full (GTK_OBJECT (options_window),
                            "data_stars", list,
                            (GtkDestroyNotify) file_list_free);

  for (; list != NULL; list = list->next)
    {
    GtkWidget *star_menuitem;
    guchar    *friendly_name;

    friendly_name = filename_get_friendly ((gchar*) list->data);
    star_menuitem = gtk_menu_item_new_with_label (friendly_name);
    g_free (friendly_name);

    gtk_widget_show (star_menuitem);
    gtk_menu_append (GTK_MENU (star_menu), star_menuitem);
    }

  gtk_option_menu_set_menu (
    GTK_OPTION_MENU (lookup_widget (options_window, "render_star_type")),
    star_menu);

  /* populate atmosphere list & menu */
  list = atmosphere_list_new ();
  atm_menu = gtk_menu_new ();
  if (list == NULL)
    list = g_list_append (list, g_strdup (_("[No Atmospheres Installed]")));

  gtk_object_set_data_full (GTK_OBJECT (options_window),
                            "data_atmospheres", list,
                            (GtkDestroyNotify) file_list_free);

  for (; list != NULL; list = list->next)
    {
    GtkWidget *atm_menuitem;
    guchar    *friendly_name;

    friendly_name = filename_get_friendly ((gchar*) list->data);
    atm_menuitem = gtk_menu_item_new_with_label (friendly_name);
    g_free (friendly_name);

    gtk_widget_show (atm_menuitem);
    gtk_menu_append (GTK_MENU (atm_menu), atm_menuitem);
    }

  gtk_option_menu_set_menu (
    GTK_OPTION_MENU (lookup_widget (options_window, "render_atmosphere_type")),
    atm_menu);

  /* populate water list & menu */
  list = water_list_new ();
  water_menu = gtk_menu_new ();
  if (list == NULL)
    list = g_list_append (list, g_strdup (_("[No Water Installed]")));

  gtk_object_set_data_full (GTK_OBJECT (options_window),
                            "data_water", list,
                            (GtkDestroyNotify) file_list_free);

  for (; list != NULL; list = list->next)
    {
    GtkWidget *water_menuitem;
    guchar    *friendly_name;

    friendly_name = filename_get_friendly ((gchar*) list->data);
    water_menuitem = gtk_menu_item_new_with_label (friendly_name);
    g_free (friendly_name);

    gtk_widget_show (water_menuitem);
    gtk_menu_append (GTK_MENU (water_menu), water_menuitem);
    }

  gtk_option_menu_set_menu (
    GTK_OPTION_MENU (lookup_widget (options_window, "render_water_type")),
    water_menu);

  /* populate lights list & menu */
  list = light_list_new ();
  light_menu = gtk_menu_new ();
  if (list == NULL)
    list = g_list_append (list, g_strdup (_("[No Lights Installed]")));

  gtk_object_set_data_full (GTK_OBJECT (options_window),
                            "data_lights", list,
                            (GtkDestroyNotify) file_list_free);

  for (; list != NULL; list = list->next)
    {
    GtkWidget *light_menuitem;
    guchar    *friendly_name;

    friendly_name = filename_get_friendly ((gchar*) list->data);
    light_menuitem = gtk_menu_item_new_with_label (friendly_name);
    g_free (friendly_name);

    gtk_widget_show (light_menuitem);
    gtk_menu_append (GTK_MENU (light_menu), light_menuitem);
    }

  gtk_option_menu_set_menu (
    GTK_OPTION_MENU (lookup_widget (options_window, "render_light_type")),
    light_menu);

  /* populate scene_theme list & menu */
  list = scene_theme_list_new ();
  scene_theme_menu = gtk_menu_new ();
  if (list == NULL)
    list = g_list_append (list, g_strdup (_("[No Scene themes Installed]")));

  gtk_object_set_data_full (GTK_OBJECT (options_window),
                            "data_scene_themes", list,
                            (GtkDestroyNotify) file_list_free);

  for (; list != NULL; list = list->next)
    {
    GtkWidget *scene_theme_menuitem;
    guchar    *friendly_name;

    friendly_name = filename_get_friendly ((gchar*) list->data);
    scene_theme_menuitem = gtk_menu_item_new_with_label (friendly_name);
    g_free (friendly_name);

    gtk_widget_show (scene_theme_menuitem);
    gtk_menu_append (GTK_MENU (scene_theme_menu), scene_theme_menuitem);
    }

  gtk_option_menu_set_menu (
    GTK_OPTION_MENU (lookup_widget (options_window, "render_scene_theme")),
    scene_theme_menu);

  /* populate skycolor list & menu */
  list = skycolor_list_new ();
  skycolor_menu = gtk_menu_new ();
  if (list == NULL)
    list = g_list_append (list, g_strdup (_("[No Skycolors Installed]")));

  gtk_object_set_data_full (GTK_OBJECT (options_window),
                            "data_skycolors", list,
                            (GtkDestroyNotify) file_list_free);

  for (; list != NULL; list = list->next)
    {
    GtkWidget *skycolor_menuitem;
    guchar    *friendly_name;

    friendly_name = filename_get_friendly ((gchar*) list->data);
    skycolor_menuitem = gtk_menu_item_new_with_label (friendly_name);
    g_free (friendly_name);

    gtk_widget_show (skycolor_menuitem);
    gtk_menu_append (GTK_MENU (skycolor_menu), skycolor_menuitem);
    }

  gtk_option_menu_set_menu (
    GTK_OPTION_MENU (lookup_widget (options_window, "render_skycolor_type")),
    skycolor_menu);

 /* populate background image list & menu */
  list = background_image_list_new ();
  background_image_menu = gtk_menu_new ();
  if (list == NULL)
    list = g_list_append (list, g_strdup (_("[No Background Image Installed]")));

  gtk_object_set_data_full (GTK_OBJECT (options_window),
                            "data_background_images", list,
                            (GtkDestroyNotify) file_list_free);

  for (; list != NULL; list = list->next)
    {
    GtkWidget *background_image_menuitem;
    guchar    *friendly_name;

    friendly_name = filename_get_friendly ((gchar*) list->data);
    background_image_menuitem = gtk_menu_item_new_with_label (friendly_name);
    g_free (friendly_name);

    gtk_widget_show (background_image_menuitem);
    gtk_menu_append (GTK_MENU (background_image_menu), background_image_menuitem);
    }

  gtk_option_menu_set_menu (
    GTK_OPTION_MENU (lookup_widget (options_window, "render_background_image_file")),
    background_image_menu);

 /* populate moon_image list & menu */
  list = moon_image_list_new ();
  moon_image_menu = gtk_menu_new ();
  if (list == NULL)
    list = g_list_append (list, g_strdup (_("[No Moon Image Installed]")));

  gtk_object_set_data_full (GTK_OBJECT (options_window),
                            "data_moon_images", list,
                            (GtkDestroyNotify) file_list_free);

  for (; list != NULL; list = list->next)
    {
    GtkWidget *moon_image_menuitem;
    guchar    *friendly_name;

    friendly_name = filename_get_friendly ((gchar*) list->data);
    moon_image_menuitem = gtk_menu_item_new_with_label (friendly_name);
    g_free (friendly_name);

    gtk_widget_show (moon_image_menuitem);
    gtk_menu_append (GTK_MENU (moon_image_menu), moon_image_menuitem);
    }

  gtk_option_menu_set_menu (
    GTK_OPTION_MENU (lookup_widget (options_window, "render_moon_image_file")),
    moon_image_menu);

  /* to make sure the gtkhscales and spinbuttons will react to their changes */

  /* to make sure the gtkhscales and spinbuttons will react to their changes */
  option_window_update_adjustments (options_window, hscales, spinbuttons);

  /* Now that the theme list is set up, unpack the TTerrain structure. */
  t_terrain_unpack_scene_options (terrain, options_window);

  /* connect signals for sensitivity */     
  optionmenu_render_sky_system = lookup_widget (options_window, "render_sky_system");
  gtk_signal_connect_object (GTK_OBJECT (GTK_OPTION_MENU (optionmenu_render_sky_system)->menu),
      "deactivate", GTK_SIGNAL_FUNC (on_render_sky_system_selected),
		GTK_OBJECT (options_window));

  optionmenu_render_light_system = lookup_widget (options_window, "render_light_system");
  gtk_signal_connect_object (GTK_OBJECT (GTK_OPTION_MENU (optionmenu_render_light_system)->menu),
      "deactivate", GTK_SIGNAL_FUNC (on_render_light_system_selected),
		GTK_OBJECT (options_window));

  /* add download buttons for background and moon images */
  hbox_hyperlink = lookup_widget (options_window, "hbox_background_image");
  download_background_image = gnome_href_new ("http://terraform.tuxfamily.org/ressource1.php?lang=en",
				    _("Download ..."));
  gtk_box_pack_start (GTK_BOX (hbox_hyperlink),
		      download_background_image, FALSE, FALSE, 0);
  gtk_widget_show (download_background_image);

  hbox_hyperlink = lookup_widget (options_window, "hbox_moon_image");
  download_moon_image = gnome_href_new ("http://terraform.tuxfamily.org/ressource2.php?lang=en",
				    _("Download ..."));
  gtk_box_pack_start (GTK_BOX (hbox_hyperlink),
		      download_moon_image, FALSE, FALSE, 0);
  gtk_widget_show (download_moon_image);

  /* make scene option menu insensitive */
  gtk_widget_set_sensitive (
                  lookup_widget (terrain_window, "scene_options"), FALSE);

  gtk_widget_show (GTK_WIDGET (options_window));
}

void
on_terrain_options_ok_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *options_window;
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;

  options_window = lookup_toplevel (GTK_WIDGET (button));
  terrain_window = gtk_object_get_data (GTK_OBJECT (options_window),
                                        "data_parent");

  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_pack_terrain_options (terrain, options_window); 

  t_terrain_heightfield_modified (terrain);

  gtk_widget_destroy (options_window);
}


void
on_terrain_options_apply_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
  MainState *main_state;
  GtkWidget *main_window;
  GtkWidget *options_window;
  GtkWidget *terrain_window;
  GtkWidget *view;
  GtkWidget *notebook;
  TTerrain  *terrain;

  options_window = lookup_toplevel (GTK_WIDGET (button));
  terrain_window = gtk_object_get_data (GTK_OBJECT (options_window),
                                        "data_parent");
  main_window =
    gtk_object_get_data (GTK_OBJECT (terrain_window), "data_parent");
  main_state =
    gtk_object_get_data (GTK_OBJECT (main_window), "data_state");

  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_pack_terrain_options (terrain, options_window);
  t_terrain_heightfield_modified (terrain);

  notebook = gtk_object_get_data (GTK_OBJECT (options_window), "notebook2");

  /* page 2 (wireframe & contour) isn't applicable for external rendering */
  /* if (get_boolean(options_window, "render_autorender")) 
    if (gtk_notebook_get_current_page (GTK_NOTEBOOK (notebook)) != 6) 
    render_povray (terrain, main_state); */
}


/* make sure scene options menu is sensitized again */
gboolean
on_scene_options_delete_event   (GtkWidget       *widget,
                                 GdkEvent        *event,
                                 gpointer         user_data)
{
  GtkWidget *options_window;
  GtkWidget *terrain_window;

  options_window = lookup_toplevel (GTK_WIDGET (widget));
  terrain_window = gtk_object_get_data (GTK_OBJECT (options_window),
                                        "data_parent");

  gtk_widget_set_sensitive (
                  lookup_widget (terrain_window, "scene_options"), TRUE);

  return FALSE;
}


void
on_scene_options_cancel_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *options_window;
  GtkWidget *terrain_window;

  options_window = lookup_toplevel (GTK_WIDGET (button));
  terrain_window = gtk_object_get_data (GTK_OBJECT (options_window),
                                        "data_parent");

  gtk_widget_set_sensitive (
                  lookup_widget (terrain_window, "scene_options"), TRUE);

  on_generic_cancel_clicked (button, NULL);
}



void
on_scene_options_ok_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *options_window;
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;

  options_window = lookup_toplevel (GTK_WIDGET (button));
  terrain_window = gtk_object_get_data (GTK_OBJECT (options_window),
                                        "data_parent");

  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_pack_scene_options (terrain, options_window); 
  t_terrain_heightfield_modified (terrain);

  gtk_widget_set_sensitive (
                  lookup_widget (terrain_window, "scene_options"), TRUE);

  gtk_widget_destroy (options_window);
}


void
on_scene_options_apply_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
  MainState *main_state;
  GtkWidget *main_window;
  GtkWidget *options_window;
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;

  options_window = lookup_toplevel (GTK_WIDGET (button));
  terrain_window = gtk_object_get_data (GTK_OBJECT (options_window),
                                        "data_parent");
  main_window =
    gtk_object_get_data (GTK_OBJECT (terrain_window), "data_parent");
  main_state =
    gtk_object_get_data (GTK_OBJECT (main_window), "data_state");

  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_pack_scene_options (terrain, options_window);
  t_terrain_heightfield_modified (terrain);

  if (get_boolean(options_window, "render_autorender")) 
    render_povray (terrain, main_state);
}

void
on_use_antialiasing_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *render_options;
  gboolean   aa_on; /* antialiasing switched on */

  render_options = lookup_toplevel (GTK_WIDGET (togglebutton));

  aa_on = get_boolean (GTK_WIDGET (togglebutton), "use_antialiasing");

  gtk_widget_set_sensitive (lookup_widget (render_options, "label_method"), aa_on);
  gtk_widget_set_sensitive (lookup_widget (render_options, "optionmenu_aa_type"), aa_on);
  gtk_widget_set_sensitive (lookup_widget (render_options, "label_threshold"), aa_on);
  gtk_widget_set_sensitive (lookup_widget (render_options, "aa_threshold"), aa_on);
  gtk_widget_set_sensitive (lookup_widget (render_options, "use_jitter"), aa_on);

  if(!aa_on)
    {
    gtk_widget_set_sensitive (lookup_widget (render_options, "label_amount"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (render_options, "jitter_amount"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (render_options, "label_depth"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (render_options, "aa_depth"), FALSE);
    }
  else
    {
      if (get_option (render_options, "optionmenu_aa_type") == 1)
	{
	  gtk_widget_set_sensitive (lookup_widget (render_options, "label_depth"), TRUE);
	  gtk_widget_set_sensitive (lookup_widget (render_options, "aa_depth"), TRUE);	  
	}
      if(get_boolean (GTK_WIDGET (render_options), "use_jitter"))
	{
	  gtk_widget_set_sensitive (lookup_widget (render_options, "label_amount"), TRUE);
	  gtk_widget_set_sensitive (lookup_widget (render_options, "jitter_amount"), TRUE);
	}
    }
}


void
on_custom_size_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *render_options;
  gboolean   custom_size;

  render_options = lookup_toplevel (GTK_WIDGET (togglebutton));

  custom_size = get_boolean (GTK_WIDGET (togglebutton), "custom_size");

  gtk_widget_set_sensitive (lookup_widget (render_options, "image_width"), custom_size);
  gtk_widget_set_sensitive (lookup_widget (render_options, "label_x"), custom_size);
  gtk_widget_set_sensitive (lookup_widget (render_options, "image_height"), custom_size);
}


void
on_use_jitter_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *render_options;
  gboolean  jitter_on; /* jitter switched on */

  render_options = lookup_toplevel (GTK_WIDGET (togglebutton));

  jitter_on = get_boolean (GTK_WIDGET (togglebutton), "use_jitter");

  gtk_widget_set_sensitive (lookup_widget (render_options, "label_amount"), jitter_on);
  gtk_widget_set_sensitive (lookup_widget (render_options, "jitter_amount"), jitter_on);
}


void
on_povray_render_options_ok_clicked    (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *options_window;
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;

  options_window = lookup_toplevel (GTK_WIDGET (button));
  terrain_window = gtk_object_get_data (GTK_OBJECT (options_window),
                                        "data_parent");

  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_pack_povray_options (terrain, options_window);

  t_terrain_heightfield_modified (terrain);

  gtk_widget_destroy (options_window);
}


void
on_terrain_prune_placed_activate               (GtkMenuItem     *menuitem,
                                                gpointer         user_data)
{
  GtkWidget *prune_window;
  GtkWidget *terrain_window;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  gtk_widget_set_sensitive (terrain_window, FALSE);
  prune_window = create_prune_window ();
  gtk_object_set_data_full (GTK_OBJECT (prune_window), "data_parent",
                            terrain_window, terrain_window_modal_destroy);
  gtk_object_set_data (GTK_OBJECT (terrain_window), "data_modal",
                       prune_window);

  gtk_widget_show (prune_window);
}


void
on_prune_ok_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *prune_window;
  GtkWidget *view;
  TTerrain  *terrain;
  GtkWidget *terrain_window;
  gfloat     factor;

  prune_window = lookup_toplevel (GTK_WIDGET (button));
  terrain_window = lookup_widget (prune_window, "data_parent");
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  factor = get_float (prune_window, "pruning_factor");

  /* need to base this on view since changes must propagate to tcanvas */
  t_terrain_view_prune_objects  (T_TERRAIN_VIEW (view), factor, -1);

  terrain_window_update_menu (terrain_window);

  gtk_widget_destroy (prune_window);
}

static void
all_rivers_apply (GtkWidget     *hill_window,
                  TTerrain      *terrain)
{
  GtkWidget *terrain_window;
  gfloat     power;
  gint       method;

  power = get_float (hill_window, "power");
  method = OWN_COUNTING;

  if(get_boolean (GTK_WIDGET (hill_window), "upslope_sfd"))
    method = UPSLOPE_SFD;
  else
  if(get_boolean (GTK_WIDGET (hill_window), "upslope_mfd"))
    method = UPSLOPE_MFD;

  t_terrain_all_rivers (terrain, power, method);
  t_terrain_normalize (terrain, TRUE);

  terrain_window = lookup_widget (GTK_WIDGET (hill_window), "data_parent");
  terrain_window_update_menu (terrain_window);
}


static void
river_apply (GtkWidget     *hill_window,
             TTerrain      *terrain)
{
  GtkWidget *terrain_window;
  gfloat     center_x, center_y;

  center_x = get_float (hill_window, "center_x");
  center_y = get_float (hill_window, "center_y");

  t_terrain_river (terrain, center_x, center_y);
  t_terrain_normalize (terrain, TRUE);

  terrain_window = lookup_widget (GTK_WIDGET (hill_window), "data_parent");
  terrain_window_update_menu (terrain_window);
}


static GtkWidget *
create_all_rivers_window_advanced   (TTerrain        *terrain)
{
  GtkWidget *window;

  window = create_river_all_window ();
  return window;
}


static GtkWidget *
create_river_window_advanced   (TTerrain        *terrain)
{
  GtkWidget *window;

  window = create_river_window ();
  filter_window_add_crosshairs (window, "preview",
                                "center_x", "center_y", "use_preview");

  return window;
}


void
on_terrain_river_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { "center_x", "center_y", NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_river_window_advanced,
                     &river_apply,
                     hscales,
                     TRUE);
}


GtkWidget*
create_terrain_preview_camera (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
  GtkWidget *view;

  view = t_terrain_view_new ();
  t_terrain_view_set_size (T_TERRAIN_VIEW (view), SCENE_PREVIEW_SIZE);
  gtk_widget_set_events (view, GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON1_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
  gtk_widget_show_all (view);

  return view;
}

void
on_observe_sealevel_checkbutton_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  gboolean observe;
  GtkWidget *render_options;

  render_options = lookup_toplevel (GTK_WIDGET (togglebutton));

  observe = get_boolean (GTK_WIDGET (togglebutton), "render_observe_sealevel");
  gtk_widget_set_sensitive (lookup_widget (render_options, "label_elev_offset"), observe);
  gtk_widget_set_sensitive (lookup_widget (render_options, "render_elevation"), observe);
}


void
on_povray_render_options_apply_clicked (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *options_window;
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;

  options_window = lookup_toplevel (GTK_WIDGET (button));
  terrain_window = gtk_object_get_data (GTK_OBJECT (options_window),
                                        "data_parent");

  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_pack_povray_options (terrain, options_window);

  t_terrain_heightfield_modified (terrain);
}


void
on_terrain_all_rivers_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { "power", NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_all_rivers_window_advanced,
                     &all_rivers_apply,
                     hscales,
                     TRUE);
}

void
on_terrain_redraw_rivers_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_redraw_rivers (terrain);
  terrain_window_update_menu (terrain_window);
}


void
on_terrain_remove_all_rivers_activate  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_remove_rivers (terrain);

  t_terrain_heightfield_modified (terrain);
  terrain_window_update_menu (terrain_window);
}


void
on_method_toggled                      (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget    *rivers_window;
  
  rivers_window = lookup_toplevel (GTK_WIDGET (togglebutton));
  filter_window_update_preview (rivers_window);
}


void
on_render_clouds_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  gboolean clouds;
  GtkWidget *render_options;

  render_options = lookup_toplevel (GTK_WIDGET (togglebutton));

  clouds = get_boolean (GTK_WIDGET (togglebutton), "render_clouds");
  gtk_widget_set_sensitive (lookup_widget (render_options, "labelasbdc"), clouds);
  gtk_widget_set_sensitive (lookup_widget (render_options, "render_cloud_type"), clouds);
}



void
on_terrain_roughness_map_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *view;
  TTerrain  *terrain;
  GtkWidget *terrain_window;
  GtkWidget *main_window;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));
  main_window = gtk_object_get_data (GTK_OBJECT (terrain_window),
                                     "data_parent");

  terrain = t_terrain_clone (terrain);
  t_terrain_rough_map (terrain); 
  g_free (terrain->filename);
  terrain->filename = main_window_filename_new (main_window);
  terrain_window = terrain_window_new (main_window, terrain);
  terrain_window_save_state (terrain_window, _("Roughness Map"));

  t_terrain_heightfield_modified (terrain);
}




void
on_checkbutton_file_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *render_options;
  gboolean   do_output_file;
  
  render_options = lookup_toplevel (GTK_WIDGET (togglebutton));

  do_output_file = get_boolean (GTK_WIDGET (togglebutton), "checkbutton_file");

  gtk_widget_set_sensitive (lookup_widget (render_options, "filename_label"), do_output_file);
  gtk_widget_set_sensitive (lookup_widget (render_options, "fileentry_povray_filename"), do_output_file);
  gtk_widget_set_sensitive (lookup_widget (render_options, "file_type"), do_output_file);
  gtk_widget_set_sensitive (lookup_widget (render_options, "optionmenu_filetype"), do_output_file);

  if (do_output_file == FALSE)
    {
      gtk_widget_set_sensitive (lookup_widget (render_options, "checkbutton_use_alpha"), do_output_file);
      gtk_widget_set_sensitive (lookup_widget (render_options, "label_bits_per_colour"), do_output_file);
      gtk_widget_set_sensitive (lookup_widget (render_options, "spinbutton_bits_per_colour"), do_output_file);
    }
  else if (do_output_file == TRUE && (get_option (render_options, "optionmenu_filetype") == 2 
				      ||get_option (render_options, "optionmenu_filetype") == 3 ))
    gtk_widget_set_sensitive (lookup_widget (render_options, "checkbutton_use_alpha"), do_output_file);
  else if (do_output_file == TRUE && get_option (render_options, "optionmenu_filetype") == 0)
    {
      gtk_widget_set_sensitive (lookup_widget (render_options, "checkbutton_use_alpha"), do_output_file);
      gtk_widget_set_sensitive (lookup_widget (render_options, "label_bits_per_colour"), do_output_file);
      gtk_widget_set_sensitive (lookup_widget (render_options, "spinbutton_bits_per_colour"), do_output_file);
    }
}


void
on_checkbutton_partial_render_toggled  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *render_options;
  gboolean   do_partial_render;

  render_options = lookup_toplevel (GTK_WIDGET (togglebutton));
  do_partial_render = get_boolean (GTK_WIDGET (togglebutton), "checkbutton_partial_render");

  gtk_widget_set_sensitive (lookup_widget (render_options, "label_start_col"), do_partial_render);
  gtk_widget_set_sensitive (lookup_widget (render_options, "label_end_col"), do_partial_render);
  gtk_widget_set_sensitive (lookup_widget (render_options, "label_start_row"), do_partial_render);
  gtk_widget_set_sensitive (lookup_widget (render_options, "label_end_row"), do_partial_render);
  gtk_widget_set_sensitive (lookup_widget (render_options, "spinbutton_start_col"), do_partial_render);
  gtk_widget_set_sensitive (lookup_widget (render_options, "spinbutton_end_col"), do_partial_render);
  gtk_widget_set_sensitive (lookup_widget (render_options, "spinbutton_start_row"), do_partial_render);
  gtk_widget_set_sensitive (lookup_widget (render_options, "spinbutton_end_row"), do_partial_render);
  gtk_widget_set_sensitive (lookup_widget (render_options, "checkbutton_percentage"), do_partial_render);
}


void
on_checkbutton_percentage_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *render_options;
  gboolean   percentage;

  render_options = lookup_toplevel (GTK_WIDGET (togglebutton));
  percentage = get_boolean (GTK_WIDGET (togglebutton), "checkbutton_percentage");
  
  if (percentage)
    {
      gtk_spin_button_set_digits (GTK_SPIN_BUTTON (lookup_widget (render_options, "spinbutton_start_col")), 2);
      gtk_spin_button_set_digits (GTK_SPIN_BUTTON (lookup_widget (render_options, "spinbutton_end_col")), 2);
      gtk_spin_button_set_digits (GTK_SPIN_BUTTON (lookup_widget (render_options, "spinbutton_start_row")), 2);
      gtk_spin_button_set_digits (GTK_SPIN_BUTTON (lookup_widget (render_options, "spinbutton_end_row")), 2);
    }
  else 
    {      
      gtk_spin_button_set_digits (GTK_SPIN_BUTTON (lookup_widget (render_options, "spinbutton_start_col")), 0);
      gtk_spin_button_set_digits (GTK_SPIN_BUTTON (lookup_widget (render_options, "spinbutton_end_col")), 0);
      gtk_spin_button_set_digits (GTK_SPIN_BUTTON (lookup_widget (render_options, "spinbutton_start_row")), 0);
      gtk_spin_button_set_digits (GTK_SPIN_BUTTON (lookup_widget (render_options, "spinbutton_end_row")), 0);
    }

}


/*
 * This part added for Trace Objects by Raymond Ostertag
 */


static GtkWidget *
create_trace_window_advanced           (TTerrain        *terrain,
                                        GtkWidget       *terrain_window)
{
  gchar            *text;
  gfloat            pov_x, pov_y, pov_z;
  GtkWidget        *trace_window;
  GtkWidget        *object_menu;
  GList            *list;
  GList            *iter;
  int    i;
  TTerrainOptRender * const ropt = &(terrain->render_options);

  trace_window = create_trace_objects_window ();

  list = trace_object_list_new ();
  object_menu = gtk_menu_new ();
  if (list == NULL)
    list = g_list_append (list, g_strdup (_("[No TRACE Objects Installed]")));

  gtk_object_set_data_full (GTK_OBJECT (trace_window),
                            "data_trace_objects", list,
                            (GtkDestroyNotify) file_list_free);

  t_terrain_get_povray_size (terrain, &pov_x, &pov_y, &pov_z);
  text = g_strdup_printf (_("The terrain size is X=%.2f Y=%.2f Z=%.2f"), pov_x, pov_y, pov_z);
  set_text (trace_window, "trace_terrain_info", text);
  g_free (text);

  for (; list != NULL; list = list->next)
    {
      GtkWidget *object_menuitem;
      guchar    *friendly_name;

      friendly_name = filename_get_friendly ((gchar*) list->data);
      object_menuitem = gtk_menu_item_new_with_label (friendly_name);
      g_free (friendly_name);

      gtk_widget_show (object_menuitem);
      gtk_menu_append (GTK_MENU (object_menu), object_menuitem);
    }

  gtk_option_menu_set_menu (
    GTK_OPTION_MENU (lookup_widget (trace_window, "trace_object_name")),
    object_menu);

  if (ropt->trace_object_file)
  {
    list = trace_object_list_new ();
    for (iter = g_list_first (list), i = 0;
         iter != NULL;
         iter = g_list_next (iter), i++)
      {
        if (!strcmp ((gchar*) iter->data, ropt->trace_object_file))
          set_option (trace_window, "trace_object_name", i);
      }
    set_float (trace_window, "trace_object_x", ropt->trace_object_x);
    set_float (trace_window, "trace_object_y", ropt->trace_object_y);
    set_float (trace_window, "trace_object_z", ropt->trace_object_z);
    set_boolean (trace_window, "trace_object_relativ", ropt->do_trace_object_relativ);
    set_boolean (trace_window, "trace_object_conserve", ropt->do_trace_object_conserve);
    set_boolean (trace_window, "trace_object_float", ropt->do_trace_object_float);  
    set_float (trace_window, "trace_object_scale", ropt->trace_object_scale);
  }

  return trace_window;
}


static void
trace_objects_apply                    ()
{
}

void
on_single_trace_object1_activate       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  gchar *hscales[] = { NULL };

  filter_window_new (GTK_WIDGET (menuitem),
                     (CreateWindowFunc) &create_trace_window_advanced,
                     &trace_objects_apply,
                     hscales,
                     FALSE);
}


void
on_trace_ok_clicked    (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *trace_window;
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;

  trace_window = lookup_toplevel (GTK_WIDGET (button));
  terrain_window = gtk_object_get_data (GTK_OBJECT (trace_window),
                                        "data_parent");

  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_pack_povray_trace (terrain, trace_window);

  t_terrain_heightfield_modified (terrain);

  gtk_widget_destroy (trace_window);
}


void
on_remove_single_object1_activate      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{  
  GtkWidget *terrain_window;
  GtkWidget *view;
  TTerrain  *terrain;

  terrain_window = lookup_toplevel (GTK_WIDGET (menuitem));
  view = lookup_widget (terrain_window, "terrain");
  terrain = t_terrain_view_get_terrain (T_TERRAIN_VIEW (view));

  t_terrain_remove_trace_objects (terrain);

  t_terrain_heightfield_modified (terrain);
}

/*
 * End of Trace Objects
 */

/*
 * This part added for Scene Options sensitivity by Raymond Ostertag
 */

void
on_use_fog_toggled                      (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *scene_options;
  gboolean   render_fog_on; 

  scene_options = lookup_toplevel (GTK_WIDGET (togglebutton));
  render_fog_on = get_boolean (GTK_WIDGET (togglebutton), "render_fog");

  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_fog_turbulence"), render_fog_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_fog_density"), render_fog_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_fog_blue"), render_fog_on);
}


void
on_use_ground_fog_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *scene_options;
  gboolean   render_ground_fog_on; 

  scene_options = lookup_toplevel (GTK_WIDGET (togglebutton));
  render_ground_fog_on = get_boolean (GTK_WIDGET (togglebutton), "render_ground_fog");

  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_fog_alt"), render_ground_fog_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_fog_offset"), render_ground_fog_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_ground_fog_density"), render_ground_fog_on);
}

void
on_render_stars_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *scene_options;
  gboolean   render_stars_on; 

  scene_options = lookup_toplevel (GTK_WIDGET (togglebutton));
  render_stars_on = get_boolean (GTK_WIDGET (togglebutton), "render_stars");

  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_star_type"), render_stars_on);
}


void
on_render_scene_clouds_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *scene_options;
  gboolean   render_clouds_on; 

  scene_options = lookup_toplevel (GTK_WIDGET (togglebutton));
  render_clouds_on = get_boolean (GTK_WIDGET (togglebutton), "render_clouds");

  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_cloud_type"), render_clouds_on);
}

void
on_render_celest_objects_toggled       (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *scene_options;
  gboolean   render_celest_objects_on; 

  scene_options = lookup_toplevel (GTK_WIDGET (togglebutton));
  render_celest_objects_on = get_boolean (GTK_WIDGET (togglebutton), "render_celest_objects");

  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_sun_size"), render_celest_objects_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_sun_color_red"), render_celest_objects_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_sun_color_green"), render_celest_objects_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_sun_color_blue"), render_celest_objects_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_moon_image_file"), render_celest_objects_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_moon_rot_Y"), render_celest_objects_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_moon_rot_Z"), render_celest_objects_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_moon_map_enlightment"), render_celest_objects_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_moon_size"), render_celest_objects_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_moon_color_red"), render_celest_objects_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_moon_color_green"), render_celest_objects_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_moon_color_blue"), render_celest_objects_on);
}


void
on_render_camera_light_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *scene_options;
  gboolean   render_camera_light_on;

  scene_options = lookup_toplevel (GTK_WIDGET (togglebutton));
  render_camera_light_on = get_boolean (GTK_WIDGET (togglebutton), "render_camera_light");

  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_camera_light_luminosity"), render_camera_light_on);
}


void
on_render_filled_sea_toggled           (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *scene_options;
  gboolean   render_sea_on; 

  scene_options = lookup_toplevel (GTK_WIDGET (togglebutton));
  render_sea_on = get_boolean (GTK_WIDGET (togglebutton), "render_filled_sea");

  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_water_type"), render_sea_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_water_apply_on_river"), render_sea_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_sealevel"), render_sea_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_clarity"), render_sea_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_water_reflection"), render_sea_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_water_color_red"), render_sea_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_water_color_green"), render_sea_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_water_color_blue"), render_sea_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_water_frequency"), render_sea_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_waves_height"), render_sea_on);
}

void
on_render_texture_waterborder_ice_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *scene_options;
  gboolean   render_ice_on; 
  
  scene_options = lookup_toplevel (GTK_WIDGET (togglebutton));
  render_ice_on = get_boolean (GTK_WIDGET (togglebutton), "render_texture_waterborder_ice");
  
  set_boolean (scene_options, "render_texture_waterborder_sand", !(render_ice_on));
}


void
on_render_texture_waterborder_sand_toggled
                                        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *scene_options;
  gboolean   render_sand_on; 

  scene_options = lookup_toplevel (GTK_WIDGET (togglebutton));
  render_sand_on = get_boolean (GTK_WIDGET (togglebutton), "render_texture_waterborder_sand");

  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_red"), render_sand_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_green"), render_sand_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_blue"), render_sand_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_grain"), render_sand_on);

  set_boolean (scene_options, "render_texture_waterborder_ice", !(render_sand_on));
}


void
on_render_texture_waterborder_toggled  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
 GtkWidget *scene_options;
  gboolean   render_waterborder_on; 
  gboolean   render_sand_on; 

  scene_options = lookup_toplevel (GTK_WIDGET (togglebutton));
  render_waterborder_on = get_boolean (GTK_WIDGET (togglebutton), "render_texture_waterborder");
  render_sand_on = get_boolean (GTK_WIDGET (togglebutton), "render_texture_waterborder_sand");

  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_size"), render_waterborder_on);   
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand"), render_waterborder_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_ice"), render_waterborder_on);
  if (render_waterborder_on)
  {
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_red"), render_sand_on);
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_green"), render_sand_on);
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_blue"), render_sand_on);
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_grain"), render_sand_on);
   }
  else
  {
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_red"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_green"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_blue"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_grain"), FALSE);
   }
}

void
on_render_texture_stratum_toggled      (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *scene_options;
  gboolean   render_stratum_on; 

  scene_options = lookup_toplevel (GTK_WIDGET (togglebutton));
  render_stratum_on = get_boolean (GTK_WIDGET (togglebutton), "render_texture_stratum");

 gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_stratum_size"), render_stratum_on);
}


void
on_render_texture_constructor_toggled  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *scene_options;
  gboolean   render_texture_constructor_on; 
  gboolean   render_waterborder_on; 
  gboolean   render_sand_on; 
  gboolean   render_stratum_on;

  scene_options = lookup_toplevel (GTK_WIDGET (togglebutton));
  render_texture_constructor_on = get_boolean (GTK_WIDGET (togglebutton), "render_texture_constructor");
  render_waterborder_on = get_boolean (GTK_WIDGET (togglebutton), "render_texture_waterborder");
  render_sand_on = get_boolean (GTK_WIDGET (togglebutton), "render_texture_waterborder_sand");
  render_stratum_on = get_boolean (GTK_WIDGET (togglebutton), "render_texture_stratum");

  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_method"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_presets"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_presets_apply"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_stratum"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_grasstex"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_ice"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_rock_amount"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_grass_amount"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_snow_amount"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_rock_red"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_rock_green"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_rock_blue"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_rock_grain"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_grass_red"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_grass_green"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_grass_blue"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_grass_grain"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_snow_red"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_snow_green"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_snow_blue"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_snow_grain"), render_texture_constructor_on);
  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_stratum_size"), render_texture_constructor_on);
  if (render_texture_constructor_on)
  {
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_stratum_size"), render_stratum_on);
  }
  else
  {
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_stratum_size"), FALSE);
  }
  if (render_texture_constructor_on)
  {
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_size"), render_waterborder_on);
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand"), render_waterborder_on);
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_ice"), render_waterborder_on);
    if (render_waterborder_on)
    {
      gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_red"), render_sand_on);
      gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_green"), render_sand_on);
      gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_blue"), render_sand_on);
      gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_grain"), render_sand_on);
    }
    else
    {
      gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_red"), FALSE);
      gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_green"), FALSE);
      gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_blue"), FALSE);
      gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_grain"), FALSE);
    }
  }
  else
  {
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_size"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_ice"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_red"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_green"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_blue"), FALSE);
    gtk_widget_set_sensitive (lookup_widget (scene_options, "render_texture_waterborder_sand_grain"), FALSE);
  }
}


void
on_render_radiosity_toggled            (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  GtkWidget *scene_options;
  gboolean   render_radiosity_on;

  scene_options = lookup_toplevel (GTK_WIDGET (togglebutton));
  render_radiosity_on = get_boolean (GTK_WIDGET (togglebutton), "render_radiosity");

  gtk_widget_set_sensitive (lookup_widget (scene_options, "render_radiosity_quality"), render_radiosity_on);
}


/*
 * End of Scene Options sensitivity
 */


/*
 * This part added for Camera Presets by Raymond Ostertag
 */

void
on_render_camera_presets_apply_clicked (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *scene_options;
  gint   render_camera_presets_option; 

  scene_options = lookup_toplevel (GTK_WIDGET (button));
  render_camera_presets_option = get_option (GTK_WIDGET (button), "render_camera_presets");

  switch (render_camera_presets_option) {
  case 0:
    set_float (scene_options, "render_camera_x", 0.5);
    set_float (scene_options, "render_camera_y", 2.5);
    set_float (scene_options, "render_camera_z", -1.25);
    set_float (scene_options, "render_lookat_x", 0.5);
    set_float (scene_options, "render_lookat_y", 0.0);
    set_float (scene_options, "render_lookat_z", 0.5);
  break;
  case 1:
    set_float (scene_options, "render_camera_x", 0.5);
    set_float (scene_options, "render_camera_y", 2.5);
    set_float (scene_options, "render_camera_z", -1.25);
    set_float (scene_options, "render_lookat_x", 0.5);
    set_float (scene_options, "render_lookat_y", 0.0);
    set_float (scene_options, "render_lookat_z", 100.0);
  break;
  case 2:
    set_float (scene_options, "render_camera_x", 0.5);
    set_float (scene_options, "render_camera_y", 1.0);
    set_float (scene_options, "render_camera_z", -0.3);
    set_float (scene_options, "render_lookat_x", 0.5);
    set_float (scene_options, "render_lookat_y", 0.0);
    set_float (scene_options, "render_lookat_z", 0.4);
  break;
  case 3:
    set_float (scene_options, "render_camera_x", 0.5);
    set_float (scene_options, "render_camera_y", 1.0);
    set_float (scene_options, "render_camera_z", -0.3);
    set_float (scene_options, "render_lookat_x", 0.5);
    set_float (scene_options, "render_lookat_y", 0.0);
    set_float (scene_options, "render_lookat_z", 100.0);
  break;
  case 4:
    set_float (scene_options, "render_camera_x", 0.5);
    set_float (scene_options, "render_camera_y", 1.0);
    set_float (scene_options, "render_camera_z", 0.0);
    set_float (scene_options, "render_lookat_x", 0.5);
    set_float (scene_options, "render_lookat_y", 0.0);
    set_float (scene_options, "render_lookat_z", 1.0);
  break;
  case 5:
    set_float (scene_options, "render_camera_x", 0.5);
    set_float (scene_options, "render_camera_y", 1.0);
    set_float (scene_options, "render_camera_z", 0.0);
    set_float (scene_options, "render_lookat_x", 0.5);
    set_float (scene_options, "render_lookat_y", 0.0);
    set_float (scene_options, "render_lookat_z", 0.58);
  break;
  }
}

/*
 * End of Camera Presets
 */

/*
 * This part added for Texture Constructor Presets by Raymond Ostertag
 */

void
on_render_texture_presets_apply_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *scene_options;
  gint   render_constructor_presets_option; 

  scene_options = lookup_toplevel (GTK_WIDGET (button));
  render_constructor_presets_option = get_option (GTK_WIDGET (button), "render_texture_presets");

  switch (render_constructor_presets_option) {  
  case 0: /*Mountain*/
    set_boolean (scene_options, "render_texture_stratum", FALSE);
    set_boolean (scene_options, "render_texture_grasstex", TRUE);
    set_boolean (scene_options, "render_texture_waterborder", TRUE);
    set_boolean (scene_options, "render_texture_waterborder_sand", FALSE);
    set_boolean (scene_options, "render_texture_waterborder_ice", TRUE);
    set_float (scene_options, "render_texture_rock_amount", 1.0);
    set_float (scene_options, "render_texture_grass_amount", 0.5);
    set_float (scene_options, "render_texture_snow_amount", 1.0);
    set_float (scene_options, "render_texture_rock_red", 0.6);
    set_float (scene_options, "render_texture_rock_green", 0.6);
    set_float (scene_options, "render_texture_rock_blue", 0.6);
    set_float (scene_options, "render_texture_rock_grain", 1.0);
    set_float (scene_options, "render_texture_grass_red", 0.85);
    set_float (scene_options, "render_texture_grass_green", 0.99);
    set_float (scene_options, "render_texture_grass_blue", 0.70);
    set_float (scene_options, "render_texture_grass_grain", 0.5);
    set_float (scene_options, "render_texture_snow_red", 0.95);
    set_float (scene_options, "render_texture_snow_green", 0.97);
    set_float (scene_options, "render_texture_snow_blue", 0.99);
    set_float (scene_options, "render_texture_snow_grain", 2.0);
    set_float (scene_options, "render_texture_stratum_size", 0.0);
    set_float (scene_options, "render_texture_waterborder_size", 0.8);
    set_float (scene_options, "render_texture_waterborder_sand_red", 0.0);
    set_float (scene_options, "render_texture_waterborder_sand_green", 0.0);
    set_float (scene_options, "render_texture_waterborder_sand_blue", 0.0);
    set_float (scene_options, "render_texture_waterborder_sand_grain", 0.0);
  break;
  case 1: /*Atoll*/
    set_boolean (scene_options, "render_texture_stratum", TRUE);
    set_boolean (scene_options, "render_texture_grasstex", TRUE);
    set_boolean (scene_options, "render_texture_waterborder", TRUE);
    set_boolean (scene_options, "render_texture_waterborder_sand", TRUE);
    set_boolean (scene_options, "render_texture_waterborder_ice", FALSE);
    set_float (scene_options, "render_texture_rock_amount", 1.0);
    set_float (scene_options, "render_texture_grass_amount", 1.0);
    set_float (scene_options, "render_texture_snow_amount", 0.0);
    set_float (scene_options, "render_texture_rock_red", 0.6);
    set_float (scene_options, "render_texture_rock_green", 0.6);
    set_float (scene_options, "render_texture_rock_blue", 0.6);
    set_float (scene_options, "render_texture_rock_grain", 2.0);
    set_float (scene_options, "render_texture_grass_red", 0.85);
    set_float (scene_options, "render_texture_grass_green", 0.90);
    set_float (scene_options, "render_texture_grass_blue", 0.70);
    set_float (scene_options, "render_texture_grass_grain", 2.0);
    set_float (scene_options, "render_texture_snow_red", 1.0);
    set_float (scene_options, "render_texture_snow_green", 1.0);
    set_float (scene_options, "render_texture_snow_blue", 1.0);
    set_float (scene_options, "render_texture_snow_grain", 1.0);
    set_float (scene_options, "render_texture_stratum_size", 10.0);
    set_float (scene_options, "render_texture_waterborder_size", 0.6);
    set_float (scene_options, "render_texture_waterborder_sand_red", 0.90);
    set_float (scene_options, "render_texture_waterborder_sand_green", 0.85);
    set_float (scene_options, "render_texture_waterborder_sand_blue", 0.70);
  break;
  case 2: /*Rock and Grass*/
    set_boolean (scene_options, "render_texture_stratum", TRUE);
    set_boolean (scene_options, "render_texture_grasstex", TRUE);
    set_boolean (scene_options, "render_texture_waterborder", FALSE);
    set_boolean (scene_options, "render_texture_waterborder_sand", TRUE);
    set_boolean (scene_options, "render_texture_waterborder_ice", FALSE);
    set_float (scene_options, "render_texture_rock_amount", 1.0);
    set_float (scene_options, "render_texture_grass_amount", 1.0);
    set_float (scene_options, "render_texture_snow_amount", 0.0);
    set_float (scene_options, "render_texture_rock_red", 0.6);
    set_float (scene_options, "render_texture_rock_green", 0.6);
    set_float (scene_options, "render_texture_rock_blue", 0.6);
    set_float (scene_options, "render_texture_rock_grain", 1.0);
    set_float (scene_options, "render_texture_grass_red", 0.85);
    set_float (scene_options, "render_texture_grass_green", 0.90);
    set_float (scene_options, "render_texture_grass_blue", 0.70);
    set_float (scene_options, "render_texture_grass_grain", 1.0);
    set_float (scene_options, "render_texture_snow_red", 1.0);
    set_float (scene_options, "render_texture_snow_green", 1.0);
    set_float (scene_options, "render_texture_snow_blue", 1.0);
    set_float (scene_options, "render_texture_snow_grain", 1.0);
    set_float (scene_options, "render_texture_stratum_size", 10.0);
    set_float (scene_options, "render_texture_waterborder_size", 0.6);
    set_float (scene_options, "render_texture_waterborder_sand_red", 0.90);
    set_float (scene_options, "render_texture_waterborder_sand_green", 0.85);
    set_float (scene_options, "render_texture_waterborder_sand_blue", 0.70);
    set_float (scene_options, "render_texture_waterborder_sand_grain", 2.0);
  break;
  case 3: /*Rock and Snow*/
    set_boolean (scene_options, "render_texture_stratum", TRUE);
    set_boolean (scene_options, "render_texture_grasstex", FALSE);
    set_boolean (scene_options, "render_texture_waterborder", FALSE);
    set_boolean (scene_options, "render_texture_waterborder_sand", FALSE);
    set_boolean (scene_options, "render_texture_waterborder_ice", TRUE);
    set_float (scene_options, "render_texture_rock_amount", 1.0);
    set_float (scene_options, "render_texture_grass_amount", 0.0);
    set_float (scene_options, "render_texture_snow_amount", 1.0);
    set_float (scene_options, "render_texture_rock_red", 0.6);
    set_float (scene_options, "render_texture_rock_green", 0.6);
    set_float (scene_options, "render_texture_rock_blue", 0.6);
    set_float (scene_options, "render_texture_rock_grain", 1.0);
    set_float (scene_options, "render_texture_grass_red", 0.85);
    set_float (scene_options, "render_texture_grass_green", 0.90);
    set_float (scene_options, "render_texture_grass_blue", 0.70);
    set_float (scene_options, "render_texture_grass_grain", 1.0);
    set_float (scene_options, "render_texture_snow_red", 1.0);
    set_float (scene_options, "render_texture_snow_green", 1.0);
    set_float (scene_options, "render_texture_snow_blue", 1.0);
    set_float (scene_options, "render_texture_snow_grain", 1.0);
    set_float (scene_options, "render_texture_stratum_size", 10.0);
    set_float (scene_options, "render_texture_waterborder_size", 0.6);
    set_float (scene_options, "render_texture_waterborder_sand_red", 0.90);
    set_float (scene_options, "render_texture_waterborder_sand_green", 0.85);
    set_float (scene_options, "render_texture_waterborder_sand_blue", 0.70);
    set_float (scene_options, "render_texture_waterborder_sand_grain", 2.0);
  break;
  case 4: /*Grass and Snow*/
    set_boolean (scene_options, "render_texture_stratum", FALSE);
    set_boolean (scene_options, "render_texture_grasstex", TRUE);
    set_boolean (scene_options, "render_texture_waterborder", FALSE);
    set_boolean (scene_options, "render_texture_waterborder_sand", TRUE);
    set_boolean (scene_options, "render_texture_waterborder_ice", FALSE);
    set_float (scene_options, "render_texture_rock_amount", 0.0);
    set_float (scene_options, "render_texture_grass_amount", 1.0);
    set_float (scene_options, "render_texture_snow_amount", 1.0);
    set_float (scene_options, "render_texture_rock_red", 0.6);
    set_float (scene_options, "render_texture_rock_green", 0.6);
    set_float (scene_options, "render_texture_rock_blue", 0.6);
    set_float (scene_options, "render_texture_rock_grain", 1.0);
    set_float (scene_options, "render_texture_grass_red", 0.85);
    set_float (scene_options, "render_texture_grass_green", 0.90);
    set_float (scene_options, "render_texture_grass_blue", 0.70);
    set_float (scene_options, "render_texture_grass_grain", 1.0);
    set_float (scene_options, "render_texture_snow_red", 1.0);
    set_float (scene_options, "render_texture_snow_green", 1.0);
    set_float (scene_options, "render_texture_snow_blue", 1.0);
    set_float (scene_options, "render_texture_snow_grain", 1.0);
    set_float (scene_options, "render_texture_stratum_size", 10.0);
    set_float (scene_options, "render_texture_waterborder_size", 0.6);
    set_float (scene_options, "render_texture_waterborder_sand_red", 0.90);
    set_float (scene_options, "render_texture_waterborder_sand_green", 0.85);
    set_float (scene_options, "render_texture_waterborder_sand_blue", 0.70);
    set_float (scene_options, "render_texture_waterborder_sand_grain", 2.0);
  break;
  }
}

/*
 * End of Texture Constructor Presets
 */


void
on_render_scene_options_help_clicked   (GtkButton       *button,
                                        gpointer         user_data)
{
  goto_page ("render.html"); 
}



GtkWidget*
create_main_list (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
  GtkWidget *main_list;
  gchar *titles[1];

  titles[0] = _("Terrain Object List");
  main_list = gtk_clist_new_with_titles (1, titles);
  gtk_clist_set_shadow_type (GTK_CLIST (main_list), GTK_SHADOW_IN);
  gtk_container_set_border_width (GTK_CONTAINER (main_list), 8);
  gtk_clist_set_column_width (GTK_CLIST (main_list), 0, 80);
  gtk_clist_column_titles_show (GTK_CLIST (main_list));

  gtk_widget_show_all (main_list);
  return main_list;
}


GtkWidget*
create_undo_tree (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
  GtkWidget *tree;

  tree = gtk_tree_new ();
  gtk_widget_set_name (tree, "tree");
  gtk_signal_connect (GTK_OBJECT (tree), "selection_changed",
                      GTK_SIGNAL_FUNC (on_tree_selection_changed),
                      NULL);

  gtk_widget_show_all (tree);
  return tree;
}


GtkWidget*
create_merge_list (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
  GtkWidget *merge_list;
  gchar *titles[1];

  titles[0] = _("Terrain Object List");
  merge_list = gtk_clist_new_with_titles (1, titles);

  gtk_widget_set_usize (merge_list, 200, -2);
  gtk_clist_set_column_width (GTK_CLIST (merge_list), 0, 80);
  gtk_clist_column_titles_show (GTK_CLIST (merge_list));
  gtk_signal_connect (GTK_OBJECT (merge_list), "select_row",
                      GTK_SIGNAL_FUNC (on_merge_list_select_row),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (merge_list), "unselect_row",
                      GTK_SIGNAL_FUNC (on_merge_list_unselect_row),
                      NULL);

  gtk_widget_show_all (merge_list);
  return merge_list;
}


GtkWidget*
create_join_list (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
  GtkWidget *join_list;
  gchar *titles[1];

  titles[0] = _("Terrain Object List");
  join_list = gtk_clist_new_with_titles (1, titles);
  gtk_widget_set_usize (join_list, 200, -2);
  gtk_clist_set_column_width (GTK_CLIST (join_list), 0, 80);
  gtk_clist_column_titles_show (GTK_CLIST (join_list));

  gtk_signal_connect (GTK_OBJECT (join_list), "select_row",
                      GTK_SIGNAL_FUNC (on_join_list_select_row),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (join_list), "unselect_row",
                      GTK_SIGNAL_FUNC (on_join_list_unselect_row),
                      NULL);

  gtk_widget_show_all (join_list);
  return join_list;
}


GtkWidget*
create_erode_list (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
  GtkWidget *erode_list;
  gchar *titles[1];
                                                                                
  titles[0] = _("Terrain Object List");
  erode_list = gtk_clist_new_with_titles (1, titles);

  gtk_widget_set_usize (erode_list, 200, -2);
  gtk_clist_set_column_width (GTK_CLIST (erode_list), 0, 80);
  gtk_clist_column_titles_show (GTK_CLIST (erode_list));

  gtk_signal_connect (GTK_OBJECT (erode_list), "select_row",
                      GTK_SIGNAL_FUNC (on_join_list_select_row),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (erode_list), "unselect_row",
                      GTK_SIGNAL_FUNC (on_join_list_unselect_row),
                      NULL);

  gtk_widget_show_all (erode_list);
  return erode_list;
}


GtkWidget*
create_warp_list (gchar *widget_name, gchar *string1, gchar *string2,
                gint int1, gint int2)
{
  GtkWidget *warp_list;
  gchar *titles[1];
                                                                                
  titles[0] = _("Terrain Object List");
  warp_list = gtk_clist_new_with_titles (1, titles);
                                                                                
  gtk_widget_set_usize (warp_list, 200, -2);
  gtk_clist_set_column_width (GTK_CLIST (warp_list), 0, 80);
  gtk_clist_column_titles_show (GTK_CLIST (warp_list));

  gtk_signal_connect (GTK_OBJECT (warp_list), "select_row",
                      GTK_SIGNAL_FUNC (on_warp_list_select_row),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (warp_list), "unselect_row",
                      GTK_SIGNAL_FUNC (on_warp_list_unselect_row),
                      NULL);

  gtk_widget_show_all (warp_list);
  return warp_list;
}

