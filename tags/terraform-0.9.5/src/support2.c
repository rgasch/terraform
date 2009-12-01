/*  Terraform - (C) 1997-2002 Robert Gasch (r.gasch@chello.nl)
 *   - http://terraform.sourceforge.net
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
 *
 *  This file Copyright (c) 2000-2002 David A. Bartold
 */

#include <stdio.h>
#include <string.h>
#include "support.h"
#include "support2.h"

GtkWidget *
lookup_toplevel (GtkWidget *widget)
{
  GtkWidget *parent;

  for (;;)
    {
      if (GTK_IS_MENU (widget))
        parent = gtk_menu_get_attach_widget (GTK_MENU (widget));
      else
        parent = widget->parent;
      if (parent == NULL)
        break;
      widget = parent;
    }

  return widget;
}

gpointer
lookup_data (GtkWidget *widget,
             gchar     *name)
{
  GtkWidget *toplevel;

  toplevel = lookup_toplevel (widget);
  return gtk_object_get_data (GTK_OBJECT (toplevel), name);
}

gfloat
get_float (GtkWidget *parent,
           gchar     *name)
{
  GtkWidget *widget = lookup_widget (parent, name);
  g_return_val_if_fail (widget != NULL, 0.0);

  if (GTK_IS_SPIN_BUTTON (widget))
    return gtk_spin_button_get_value_as_float (GTK_SPIN_BUTTON (widget));
  else if (GTK_IS_RANGE (widget))
    return gtk_range_get_adjustment (GTK_RANGE (widget))->value;
  else if (GTK_IS_EDITABLE (widget) || GTK_IS_TEXT_VIEW (widget))
    {
      gchar *str;
      gfloat value;

      str = get_text (parent, name);
      if (sscanf (str, "%f", &value) == 0)
        value = 0.0;
      g_free (str);

      return value;
    }

  g_assert_not_reached ();
  return 0.0;
}

gint
get_int (GtkWidget *parent,
         gchar     *name)
{
  GtkWidget *widget = lookup_widget (parent, name);
  g_return_val_if_fail (widget != NULL, 0);

  if (GTK_IS_SPIN_BUTTON (widget))
    return gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget));
  else if (GTK_IS_RANGE (widget))
    return (gint) (gtk_range_get_adjustment (GTK_RANGE (widget))->value + 0.5);
  else if (GTK_IS_EDITABLE (widget) || GTK_IS_TEXT_VIEW (widget))
    {
      gchar *str;
      gint   value;

      str = get_text (parent, name);
      if (sscanf (str, "%i", &value) == 0)
        value = 0;
      g_free (str);

      return value;
    }

  g_assert_not_reached ();
  return 0;
}

gboolean
get_boolean (GtkWidget *parent,
             gchar     *name)
{
  GtkWidget *widget = lookup_widget (parent, name);
  g_return_val_if_fail (widget != NULL, FALSE);

  if (GTK_IS_TOGGLE_BUTTON (widget))
    return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
  else if (GTK_IS_CHECK_MENU_ITEM (widget))
    return GTK_CHECK_MENU_ITEM (widget)->active;

  g_assert_not_reached ();
  return FALSE;
}

gint
get_option (GtkWidget *options,
            gchar     *name)
{
  GtkWidget *widget = lookup_widget (options, name);
  g_return_val_if_fail (widget != NULL, -1);

  if (GTK_IS_OPTION_MENU (widget))
    {
      GtkWidget *active_item;
      GtkMenu *menu = GTK_MENU (GTK_OPTION_MENU (widget)->menu);

      active_item = gtk_menu_get_active (GTK_MENU (menu));
      return g_list_index (GTK_MENU_SHELL (menu)->children, active_item);
    }

  g_assert_not_reached ();
  return 0;
}

gchar *
get_option_text (GtkWidget *options,
                 gchar     *name)
{
  GtkWidget *widget = lookup_widget (options, name);
  g_return_val_if_fail (widget != NULL, g_strdup (""));

  if (GTK_IS_OPTION_MENU (widget))
    {
      GtkWidget *active_item;
      GtkMenu *menu = GTK_MENU (GTK_OPTION_MENU (widget)->menu);

      active_item = gtk_menu_get_active (GTK_MENU (menu));
      return g_strdup ("(foo)");
    }

  g_assert_not_reached ();
  return g_strdup ("");
}

gint
get_radio_menu (GtkWidget *menu,
                gchar    **options)
{
  gint i;

  for (i = 0; options[i] != NULL; i++)
    if (get_boolean (menu, options[i]))
      return i;

  return -1;
}

gchar *
get_text (GtkWidget *widget,
          gchar     *name)
{
  GtkWidget *text;

  text = lookup_widget (widget, name);
  if (GTK_IS_EDITABLE (text))
    return g_strdup (gtk_editable_get_chars (GTK_EDITABLE (text), 0, -1));
  else if (GTK_IS_TEXT_VIEW (text))
    {
      GtkTextBuffer *buffer;
      GtkTextIter begin, end;

      buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
      gtk_text_buffer_get_start_iter (buffer, &begin);
      gtk_text_buffer_get_end_iter (buffer, &end);
      return gtk_text_buffer_get_text(buffer, &begin, &end, TRUE);
    }

  g_assert_not_reached ();
  return NULL;
}

void
set_float (GtkWidget *parent,
           gchar     *name,
           gfloat     value)
{
  GtkWidget *widget = lookup_widget (parent, name);
  g_return_if_fail (widget != NULL);

  if (GTK_IS_SPIN_BUTTON (widget))
    gtk_spin_button_set_value (GTK_SPIN_BUTTON (widget), value);
  else if (GTK_IS_RANGE (widget))
    {
      GtkAdjustment *adjustment;

      adjustment = gtk_range_get_adjustment (GTK_RANGE (widget));
      adjustment->value = value;
      gtk_adjustment_value_changed (adjustment);
    }
  else if (GTK_IS_EDITABLE (widget) || GTK_IS_TEXT_VIEW (widget))
    {
      gchar str[80];

      sprintf (str, "%f", value);
      set_text (parent, name, str);
    }
  else
    g_assert_not_reached ();
}

void
set_int (GtkWidget *parent,
         gchar     *name,
         gint       value)
{
  GtkWidget *widget = lookup_widget (parent, name);
  g_return_if_fail (widget != NULL);

  if (GTK_IS_SPIN_BUTTON (widget))
    gtk_spin_button_set_value (GTK_SPIN_BUTTON (widget), (gfloat) value);
  else if (GTK_IS_RANGE (widget))
    gtk_range_get_adjustment (GTK_RANGE (widget))->value = (gfloat) value;
  else if (GTK_IS_EDITABLE (widget) || GTK_IS_TEXT_VIEW (widget))
    {
      gchar str[80];

      sprintf (str, "%i", value);
      set_text (parent, name, str);
    }
  else
    g_assert_not_reached ();
}

void
set_boolean (GtkWidget *parent,
             gchar     *name,
             gboolean   value)
{
  GtkWidget *widget = lookup_widget (parent, name);
  g_return_if_fail (widget != NULL);

  if (GTK_IS_TOGGLE_BUTTON (widget))
    return gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), value);
  else if (GTK_IS_MENU_ITEM (widget))
    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (widget), value);
  else
    g_assert_not_reached ();
}

void
set_option (GtkWidget *parent,
            gchar     *name,
            gint       value)
{
  GtkWidget *widget = lookup_widget (parent, name);
  g_return_if_fail (widget != NULL);

  if (GTK_IS_OPTION_MENU (widget))
    gtk_option_menu_set_history (GTK_OPTION_MENU (widget), value);
  else
    g_assert_not_reached ();
}

void
set_radio_menu (GtkWidget *menu,
                gchar    **options,
                gint       value)
{
  set_boolean (menu, options[value], TRUE);
}

void
set_radio_toolbar (GtkWidget *toolbar,
                   gchar    **options,
                   GtkWidget *value)
{
  gint i;

  for (i = 0; options[i] != NULL; i++)
    {
      GtkWidget *widget;

      widget = lookup_widget (toolbar, options[i]);
      if (widget != value)
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), FALSE);
    }
}

void
set_text (GtkWidget *widget,
          gchar     *name,
          gchar     *value)
{
  GtkWidget *text;

  text = lookup_widget (widget, name);
  if (GTK_IS_LABEL (text))
    {
      gtk_label_set_text (GTK_LABEL (text), value);
    }
  else if (GTK_IS_BIN (text))
    {
      GtkWidget *child = GTK_BIN (text)->child;

      if (GTK_IS_LABEL (child))
        gtk_label_set_text (GTK_LABEL (child), value);
    }
  else if (GTK_IS_EDITABLE (text))
    {
      gint pos;

      gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
      pos = 0;
      gtk_editable_insert_text (GTK_EDITABLE (text), value,
                                strlen (value), &pos);
    }
  else if (GTK_IS_TEXT_VIEW (text))
    {
      GtkTextBuffer *buffer;
      buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));
      gtk_text_buffer_set_text (buffer, value, -1);
    }
  else
    g_assert_not_reached ();
}

/* Helper functions */
static void
widget_timer_destroy_handler (guint *timer)
{
  gtk_timeout_remove (*timer);
}

void
widget_remove_timer (GtkWidget *widget)
{
  guint *timer;

  timer = gtk_object_get_data (GTK_OBJECT (widget), "data_timer");
  if (timer != NULL)
    {
      gtk_timeout_remove (*timer);
      gtk_object_set_data (GTK_OBJECT (widget), "data_timer", NULL);
    }
}

/*
 * Create the widget's timeout.  There can only be one timeout per widget.
 * The timeout is removed using widget_remove_timer or when the widget is
 * destroyed.
 */

void
widget_set_timer (GtkWidget   *widget,
                  guint32      interval,
                  GtkFunction  function)
{
  guint *timer;

  widget_remove_timer (widget);

  timer = g_new (guint, 1);
  *timer = gtk_timeout_add (interval, function, widget);
  gtk_object_set_data_full (GTK_OBJECT (widget), "data_timer", timer,
                            (GtkDestroyNotify) widget_timer_destroy_handler);
}

void
hscale_callbacks (GtkWidget     *widget,
                  gchar        **hscales,
                  GtkSignalFunc  func)
{
  gint i;

  for (i = 0; hscales[i] != NULL; i++)
    {
      GtkWidget *hscale;

      hscale = lookup_widget (widget, hscales[i]);
      gtk_signal_connect_object (
        GTK_OBJECT (GTK_RANGE (hscale)->adjustment),
        "value_changed",
        func,
        GTK_OBJECT (widget));
    }
}
