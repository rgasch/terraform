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
//

//////////////// Colors description to be used as a regular earth sky /////////

// Noon:
  #declare noon_sun_color   = <1.00, 0.95, 0.80>;
  #declare noon_sky_color_1 = <0.98, 0.98, 1.00> * TF_SKY_COLORATION; // Low in sky
  #declare noon_sky_color_2 = <0.75, 0.75, 0.90> * TF_SKY_COLORATION; // Mid of sky
  #declare noon_sky_color_3 = <0.30, 0.40, 0.60> * TF_SKY_COLORATION; // High in sky
  #declare noon_light_color = <1.00, 0.95, 0.80> * TF_SKY_COLORATION;

// Sunrise/Sunset:
  #declare srise_sun_color   = <1.00, 0.70, 0.33>;
  #declare srise_sky_color_1 = <1.00, 0.70, 0.40> * TF_SKY_COLORATION; // Low in sky
  #declare srise_sky_color_2 = <0.85, 0.85, 0.95> * TF_SKY_COLORATION; // Mid of sky
  #declare srise_sky_color_3 = <0.70, 0.70, 0.95> * TF_SKY_COLORATION; // High in sky
  #declare srise_light_color = <1.00, 0.80, 0.70> * TF_SKY_COLORATION;

// Night time:
// The sun isn't visible then, but the reason why the sun's color at night
// is reddish is so that we can blend the color for sunrise/sunset.
  #declare night_sun_color   = <1.0000, 0.6000, 0.2330>;
  #declare night_sky_color_1 = <0.0010, 0.0010, 0.0010> * TF_SKY_COLORATION; // Low in sky
  #declare night_sky_color_2 = <0.0005, 0.0005, 0.0005> * TF_SKY_COLORATION; // Mid of sky
  #declare night_sky_color_3 = <0.0005, 0.0005, 0.0005> * TF_SKY_COLORATION; // High in sky
  #declare night_light_color = <0.0000, 0.0000, 0.0000> * TF_SKY_COLORATION;
  #declare moon_color        = <0.5000, 0.5000, 0.5000>;


