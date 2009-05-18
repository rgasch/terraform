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
 *
 * This base64 decoder/encoder is pulled from dhttpd:
 * Copyright (C) 1997  David A. Bartold
 */

#include <stdio.h>
#include "base64.h"

gint
base64_encoded_length (gint decoded_length)
{
  return decoded_length * 4 / 3 + 5;
}

void
base64_encode (guchar *in,
               gint    in_length,
               guchar *out)
{
  gint  in_pos;
  gint  num, a, b, c, d;
  guchar encode[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
                    "0123456789+/";

  in_pos = 0;
  c = 0;
  d = 0;
  while (in_pos < in_length)
    {
      num = 1;
      a = in[in_pos];

      in_pos++;
      if (in_pos < in_length)
        {
          b = in[in_pos];
          num++;
        }
      else
        b = 0;

      in_pos++;
      if (in_pos < in_length)
        {
          c = in[in_pos];
          num++;
        }

      in_pos++;
      if (in_pos < in_length)
        {
          d = in[in_pos];
          num++;
        }

      *out++ = encode[a >> 2];
      *out++ = encode[(a & 0x3) << 4 | (b >> 4)];
      *out++ = (num >= 2) ? encode[(b & 0xf) << 2 | (c >> 6)] : '=';
      *out++ = (num >= 3) ? encode[c & 0x3f] : '=';
    }

  *out = '\0';
}

static gboolean
base64_is_valid_char (guchar c)
{
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
         || (c >= '0' && c <= '9') || c == '+' || c == '/';

}

static gint
base64_char_to_int (guchar c)
{
  if (c >= 'A' && c <= 'Z')
    return c - 'A';
  else if (c >= 'a' && c <= 'z')
    return c - 'a' + 26;
  else if (c >= '0' && c <= '9')
    return c - '0' + 52;
  else if (c == '+')
    return 62;
  else if (c == '/')
    return 63;

  return -1;
}

gint
base64_decode (guchar *in,
               guchar *out,
               gint    out_length)
{
  gint a, b, c, d;
  gint num, pos;

  pos = 0;
  while (base64_is_valid_char (*in))
    {
      num = 1;

      a = base64_char_to_int (*in++);

      b = base64_char_to_int (*in);
      if (*in && *in != '=')
        in++, num++;

      c = base64_char_to_int (*in);
      if (*in && *in != '=')
        in++, num++;

      d = base64_char_to_int (*in);
      if (*in && *in != '=')
        in++, num++;

      if (num >= 2)
        {
          if (pos == out_length)
            return -1;

          out[pos++] = (a << 2) | (b >> 4);
        }

      if (num >= 3)
        {
          if (pos == out_length)
            return -1;

          out[pos++] = (b << 4) | (c >> 2);
        }

      if (num >= 4)
        {
          if (pos == out_length)
            return -1;

          out[pos++] = (c << 6) | d;
        }
    }

  return pos;
}
