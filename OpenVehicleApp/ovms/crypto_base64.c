//
//  crypto_base64.c
//  ovms

/*********************************************************************
 MODULE NAME:    b64.c
 
 AUTHOR:         Bob Trower 08/04/01
 
 PROJECT:        Crypt Data Packaging
 
 COPYRIGHT:      Copyright (c) Trantor Standard Systems Inc., 2001
 
 NOTE:           This source code may be used as you wish, subject to
 the MIT license.  See the LICENCE section below.
 
 LICENCE:        Copyright (c) 2001 Bob Trower, Trantor Standard Systems Inc.
 
 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated
 documentation files (the "Software"), to deal in the
 Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so,
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall
 be included in all copies or substantial portions of the
 Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
 VERSION HISTORY:
 Bob Trower 08/04/01 -- Create Version 0.00.00B
 *********************************************************************/

#include <string.h>
#include <stdlib.h>
#include "crypto_base64.h"

// Translation Table as described in RFC1113
const unsigned char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Translation Table to decode (created by author)
const unsigned char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";


void encodeblock( unsigned char in[3], unsigned char out[4], int len )
{
  out[0] = cb64[ in[0] >> 2 ];
  out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
  out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
  out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

void base64encode(uint8_t *inputData, int inputLen, uint8_t *outputData)
{
  unsigned char in[3], out[4];
  int len = 0;
  int k;
  for (k=0;k<inputLen;k++)
    {
    in[len++] = inputData[k];
    if (len==3)
      {
      // Block is full
      encodeblock(in, out, 3);
      for (len=0;len<4;len++) *outputData++ = out[len];
      len = 0;
      }
    }
  if (len>0)
    {
    for (k=len;k<3;k++) in[k]=0;
    encodeblock(in, out, len);
    for (len=0;len<4;len++) *outputData++ = out[len];
    }
  *outputData = 0;
}

void decodeblock( unsigned char in[4], unsigned char out[3] )
{
  out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
  out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
  out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}

int base64decode(uint8_t *inputData, uint8_t *outputData)
{
  unsigned char in[4], out[3];
  unsigned char v;
  int i, len;
  int written = 0;
  
  while( *inputData != 0 )
    {
    for( len = 0, i = 0; (i < 4) && (*inputData != 0); i++ )
      {
      v = 0;
      while( (*inputData != 0) && (v == 0) )
        {
        v = (unsigned char) *inputData++;
        v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
        if( v )
          {
          v = (unsigned char) ((v == '$') ? 0 : v - 61);
          }
        }
      if( *inputData != 0 )
        {
        len++;
        if( v )
          {
          in[ i ] = (unsigned char) (v - 1);
          }
        }
      else
        {
        in[i] = 0;
        }
      }
    if( len )
      {
      decodeblock( in, out );
      for( i = 0; i < len - 1; i++ )
        {
        *outputData++ = out[i];
        written++;
        }
      }
    }
  *outputData = 0;
  return written;
}