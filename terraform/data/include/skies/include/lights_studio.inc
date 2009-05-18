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


////////////////// STATIC (STUDIO) MODEL /////////////////////

// Turn off realistic light sources to give a "studio model" look.

// Main light
light_source
{
  <-0.5, 4.0, -0.5> * <TF_X_SCALE, TF_Y_SCALE, TF_Z_SCALE>
  color  <1.0, 1.0, 1.0> * TF_LIGHTS_LUMINOSITY * 0.55
}

// Smooth contrast light
light_source
{
  <1.5, 4.0, -0.5> * <TF_X_SCALE, TF_Y_SCALE, TF_Z_SCALE>
  color  <1.0, 1.0, 1.0> *TF_LIGHTS_LUMINOSITY * 0.35
  shadowless
}

// Ambient light
light_source
{
  <0.5, 4.0, 0.5> * <TF_X_SCALE, TF_Y_SCALE, TF_Z_SCALE>
  color  <1.0, 1.0, 1.0> * TF_LIGHTS_LUMINOSITY * 0.10
  shadowless
}

///////////////////// Camera Light ////////////////////////////////////////////

#if (TF_HAVE_CAMERA_LIGHT)

  light_source
  {
    TF_CAMERA_LOCATION
    color ( <1.0, 1.0, 1.0>* TF_CAMERA_LIGHT_LUMINOSITY )
    translate < 0, 1.0, 0 >
  }

#end // TF_HAVE_CAMERA_LIGHT


