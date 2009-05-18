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


#ifndef __SMARTEXEC_H__
#define __SMARTEXEC_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*FinishedFunc) (gpointer user_data);

const char *povray_probe (void);

void        execute_command (gchar        *command,
                             gchar        *error_message,
                             FinishedFunc  func,
                             gpointer      user_data);

#ifdef __cplusplus
}
#endif

#endif /* __SMARTEXEC_H__ */
