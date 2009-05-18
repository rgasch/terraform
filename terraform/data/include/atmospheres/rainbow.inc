// Copyright (c) 1999, 2000 Robert Gasch
// Copyright (c) 2002 Raymond Ostertag
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

//// Rainbow
//
// This a file to include if TF_HAVE_GROUND_FOG is TRUE

//// Global declarations for rainbow

#local LOCAL_RAINBOW_DISTANCE = 10 * vlength( TF_SCALE );
#local LOCAL_RAINBOW_X = 0.0 * TF_X_SCALE;
#local LOCAL_RAINBOW_Z = 0.0 * TF_Z_SCALE;
#local LOCAL_RAINBOW_WIDTH = 1.0;
#local LOCAL_RAINBOW_SIZE = 0.15;

// rainbow colours
#local LOCAL_RAINBOW_violet1 = color rgb<1.0, 0.5, 1.0>;
#local LOCAL_RAINBOW_violet2 = color rgb<1.0, 0.5, 1.0>;
#local LOCAL_RAINBOW_indigo = color rgb<0.5, 0.5, 1.0>;
#local LOCAL_RAINBOW_blue = color rgb<0.2, 0.2, 1.0>;
#local LOCAL_RAINBOW_cyan = color rgb<0.2, 1.0, 1.0>;
#local LOCAL_RAINBOW_green = color rgb<0.2, 1.0, 0.2>;
#local LOCAL_RAINBOW_yellow = color rgb<1.0, 1.0, 0.2>;
#local LOCAL_RAINBOW_orange = color rgb<1.0, 0.5, 0.2>;
#local LOCAL_RAINBOW_red1 = color rgb<1.0, 0.2, 0.2>;
#local LOCAL_RAINBOW_red2 = color rgb<1.0, 0.2, 0.2>;

rainbow {
	direction <TF_CAMERA_LOOK_AT.x + LOCAL_RAINBOW_X, 0.0, TF_CAMERA_LOOK_AT.z + LOCAL_RAINBOW_Z>
	angle LOCAL_RAINBOW_SIZE * 200
	width 0.002 * vlength( TF_SCALE ) * LOCAL_RAINBOW_WIDTH
	jitter 0.01
	distance LOCAL_RAINBOW_DISTANCE
   color_map {
		[0.000 color LOCAL_RAINBOW_violet1]
		[0.100 color LOCAL_RAINBOW_violet2]
		[0.214 color LOCAL_RAINBOW_indigo]
		[0.328 color LOCAL_RAINBOW_blue]
		[0.442 color LOCAL_RAINBOW_cyan]
		[0.556 color LOCAL_RAINBOW_green]
		[0.670 color LOCAL_RAINBOW_yellow]
		[0.784 color LOCAL_RAINBOW_orange]
		[0.900 color LOCAL_RAINBOW_red1]
	}
   up y
	arc_angle 180
	falloff_angle 60
}





