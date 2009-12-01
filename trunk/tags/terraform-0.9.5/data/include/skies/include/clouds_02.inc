// Copyright (c) 2002 Raymond Ostertag
// Generated with Truevision
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

#declare TF_clouds =
	texture {
		pigment {
			boxed
			color_map {
				[ 0.0 rgb 1.0 filter 1 ]
				[ 1.0 rgb 1.0 filter 0 ]
			}
		 turbulence <1.5, 3.5,1.5>
		 octaves 15
		 lambda 2.5
		 omega 0.5
		}
      finish
      {
        ambient 0.15
      }
	}

