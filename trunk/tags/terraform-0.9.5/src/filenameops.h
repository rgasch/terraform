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


#ifndef __FILENAMEOPS_H__
#define __FILENAMEOPS_H__

enum TFileType
{
  FILE_UNKNOWN,
  FILE_AUTO,
  FILE_NATIVE,
  FILE_TGA,
  FILE_POV,
  FILE_BMP,
  FILE_BMP_BW,
  FILE_GTOPO,
  FILE_GRD,
  FILE_PGM,
  FILE_PG8,
  FILE_PPM,
  FILE_MAT,
  FILE_OCT,
  FILE_AC,
  FILE_GIF,
  FILE_ICO,
  FILE_JPG,
  FILE_PNG,
  FILE_RAS,
  FILE_TIF,
  FILE_XBM,
  FILE_XPM,
  FILE_VRML,
  FILE_TERRAGEN,
  FILE_XYZ,
  FILE_DXF,
  FILE_BNA,
  FILE_BT,
  FILE_DTED,
  FILE_E00,
  FILE_INI
};
typedef enum TFileType TFileType;

gchar     *filename_age                                 (const gchar *name);
gchar     *filename_new_extension                       (const gchar *name,
                                                         const gchar *extension);
gchar     *filename_get_base                            (const gchar *name);
gchar     *filename_get_without_extension               (const gchar *name);
gchar     *filename_get_without_extension_and_dot       (const gchar *name);
gchar     *filename_get_base_without_extension          (const gchar *name);
gchar     *filename_get_base_without_extension_and_dot  (const gchar *name);
gchar     *filename_get_friendly                        (const gchar *name);
gchar     *filename_get_extension                       (const gchar *name);
TFileType  filename_determine_type                      (const gchar *name);
gchar     *filename_povray_safe                         (const gchar *name);
gchar     *filename_create_tempdir                      (void);

#endif /* __FILENAMEOPS_H__ */
