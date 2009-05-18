// Copyright (c) 1999, 2000 David A. Bartold
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

// Declare a TF_LANDSCAPE_TEXTURE before #including this file.
// TF_LANDSCAPE_TEXTURE (if it's a texture_map) should be defined as such:
//   0.0 = seafloor level
//   1.0 = highest point

// Create river:

  object
  {
    RIVER_HEIGHT_FIELD

    #if (TF_WATER_APPLY_ON_RIVER)
      material {
        TF_WATER_MATERIAL
      }
    #else
      texture
      {
        RIVER_LANDSCAPE_TEXTURE
      }
    #end

    scale <TF_X_SCALE, TF_Y_SCALE, TF_Z_SCALE>

    // Center land in the x and z axes:
    //translate -0.5 * <TF_X_SCALE, 0, TF_Z_SCALE>
  }

