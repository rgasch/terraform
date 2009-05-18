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

/* Functions in this file are Copyright (c) 1998, 2000 David A. Bartold */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <gtk/gtk.h>
#include "interface.h"
#include "support2.h"
#include "smartexec.h"


/*
 *  regexp_match: perform a regular expression check of PAT on STR and return
 *      1 if there's a match, 0 if not
 */
static int
regexp_match (const gchar *str, const gchar *pat)
{
  int g;

  for (;;)
  {
    if (*str == '\0')
      return *pat == '\0';

    switch (*pat)
    {
      case '\0':
        return 0;

      case '?':
        str++;
        pat++;
        break;

      case '\\':
        if (pat [1] == '\0' || *str != pat [1])
          return 0;

        str++;
        pat += 2;
        break;
        
      case '[':
        g = 1;
        while (pat [g] != '\0' &&
               *str != pat [g] &&
               (pat [g] != ']' || g == 1))
          g++;

        if (*str == pat [g])
        {
          while (pat [g] != '\0' && (pat [g] != ']' || g == 1))
            g++;

          if (pat [g] == '\0')
            return 1;
          else
          {
            str++;
            pat += g + 1;
            break;
          }
        }
        else
          return 0;

      case '*':
        for (g = 0; str [g] != '\0'; g++)
          if (regexp_match (&str [g], &pat [1]))
            return 1;

        return pat [1] == '\0';

      default:
        if (*pat != *str)
          return 0;

        str++;
        pat++;
        break;
    }
  }

  return *pat == '\0' && *str == '\0';
}


/*
 * proc_open: like unix's popen (<cmd>, "r"), except return a stdio stream
 *     of stdout + stderr data combined and optionally return pid
 */
static int
proc_open (const gchar *cmd, pid_t *pid)
{
  pid_t  p;
  int    io[2];

  if (pipe (io))
    return -1;

  p = fork ();
  if (p == -1)
    return -1;

  if (p == 0)
    {
      int status;

      dup2 (io[1], 1);
      dup2 (io[1], 2);
      close (io[0]);
      close (io[1]);
      status = system (cmd);
      printf ("\n\nReturned status: (%i)", status);
      fflush (stdout);
      _exit (0);
    }
  else
    {
      close (io[1]);

      if (pid != NULL)
        *pid = p;

      return io[0];
    }
}


/*
 *  ee_try: run command CMD and return result of it's output compared to
 *      the regular expression REG.
 */
static int
ee_try (const gchar *cmd, const gchar *reg)
{
  int   out;
  pid_t pid;
  gchar block[1024];
  gint  num;

  out = proc_open (cmd, &pid);

  if (out == -1)
    return 0;

  num = read (out, block, 1023);
  close (out);
  block [num] = '\0';

  waitpid (pid, NULL, 0);

  return regexp_match (block, reg);
}


/*
 *  ee_is_ldd_available: make sure ldd exists
 */
static int
ee_is_ldd_available (void)
{
  return ee_try ("which ldd", "/*");
}


/*
 *  ee_is_x_capable: verify program is compiled for X11
 */
static int
ee_is_x_capable (const gchar *cmd)
{
  gchar *buf;
  int result;

  buf = g_strdup_printf ("ldd `which %s`", cmd);
  result = ee_try (buf, "*libX11*");
  g_free (buf);

  return result;
}


/*
 *  povray_try: verify if povray executable is acceptable
 */
static int
povray_try (const gchar *cmd, const gchar *reg)
{
  return ee_try (cmd, reg) && ee_is_x_capable (cmd);
}


/*
 *  povray_probe: return static string of executable for X POV-Ray 3.1,
 *      return NULL on error
 */
const gchar *
povray_probe (void)
{
  /* Verify ldd exists. */
  if (!ee_is_ldd_available ())
    return NULL;

  /* Try to find an X version of POV-Ray. */
  if (povray_try ("x-povray", "*Version 3.1*"))
    return "x-povray";

  if (povray_try ("povray-x", "*Version 3.1*"))
    return "povray-x";

  if (povray_try ("povray", "*Version 3.1*"))
    return "povray";

  if (povray_try ("megapov", "*WinMegaPov*"))
    return "megapov";

  return NULL;
}

typedef struct ExecData ExecData;
struct ExecData
{
  int           input;
  gchar        *text;
  gchar        *error_message;
  FinishedFunc  finish_func;
  gpointer      user_data;
  gint          pid;
  gint          input_id;
};

static ExecData *
exec_data_new (int           in,
               gchar        *error_msg,
               FinishedFunc  func,
               gpointer      user_data,
               int           pid)
{
  ExecData *data;

  data = g_new0 (ExecData, 1);
  data->input = in;
  data->text = g_strdup ("");
  data->error_message = g_strdup (error_msg);
  data->finish_func = func;
  data->user_data = user_data;
  data->pid = pid;

  return data;
}

static void
exec_data_free (ExecData *data)
{
  close (data->input);
  g_free (data->error_message);
  g_free (data->text);
  g_free (data);
}

static void
execute_input (gpointer          user_data,
               gint              source_fd,
               GdkInputCondition condition)
{
  ExecData *data = (ExecData*) user_data;
  gchar     buf[1025];
  gint      length;

  fcntl (data->input, F_SETFL, O_NONBLOCK);
  length = read (data->input, buf, 1024);
  fcntl (data->input, F_SETFL, 0);
  if (length <= 0)
    {
      if (length == 0 && errno != EAGAIN)
        {
          gchar *open_paren;
          gint   status;

          open_paren = strrchr (data->text, '(');
          status = -1;
          if (open_paren != NULL)
          if (sscanf (&open_paren[1], "%i", &status) != 1)
            status = -1;

          /* POV-Ray returns 512 on an aborted trace.
             Don't popup an error dialog box. */
          if (status != 0 && status != 512)
            {
              GtkWidget *window;

              window = create_execution_error_window ();

              set_text (window, "error_label", data->error_message);
              set_text (window, "error_text", data->text);
              gtk_widget_show (window);
            }

          if (data->finish_func != NULL)
            data->finish_func (data->user_data);

          waitpid (data->pid, NULL, 0);

          gdk_input_remove (data->input_id);
          close (data->input);
          exec_data_free (data);
        }
    }
  else
    {
      buf[length] = '\0';
      data->text =
        g_renew (gchar, data->text, length + strlen (data->text) + 1);
      strcat (data->text, buf);
    }
}

void
execute_command (gchar        *command,
                 gchar        *error_message,
                 FinishedFunc  func,
                 gpointer      user_data)
{
  int         in;
  ExecData   *data;
  int         pid;
  GIOChannel *gio_in;

  in = proc_open (command, &pid);
  data = exec_data_new (in, error_message, func, user_data, pid);

  g_free (data->text);
  data->text = g_strdup_printf ("Executing the command: '%s'\n\n", command);

  gio_in = g_io_channel_unix_new (in);
  data->input_id = g_io_add_watch (gio_in, G_IO_IN, (GIOFunc) execute_input, (gpointer) data);
}
