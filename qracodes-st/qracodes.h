// qracodes.h
// Q-ary RA codes encoding/decoding functions
// 
// (c) 2016 - Nico Palermo, IV3NWV - Microtelecom Srl, Italy
// ------------------------------------------------------------------------------
// This file is part of the qracodes project, a Forward Error Control
// encoding/decoding package based on Q-ary RA (Repeat and Accumulate) LDPC codes.
//
//    qracodes is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//    qracodes is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with qra_codes source distribution.  
//    If not, see <http://www.gnu.org/licenses/>.

#ifndef _qracodes_h_
#define _qracodes_h_

typedef unsigned int uint;

#ifdef __cplusplus
extern "C" {
#endif

int  qra_encode(uint *y, const uint *x);
void qra_mfskbesselmetric(float *pix, float EsNoMetric);
int  qra_extrinsic(float *pex, const float *pix, int maxiter);
void qra_mapdecode(uint *xdec, float *pex, const float *pix);

#ifdef __cplusplus
}
#endif

#endif // _qracodes_h_
