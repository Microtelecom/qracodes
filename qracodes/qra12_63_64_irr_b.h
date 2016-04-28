// qra12_63_64_irr_b.h
// Code tables and defines for the Q-ary irregular RA code (12,63) over GF(64)
// Code Name: qra12_63_64_irr_b
// Systematic symbols repetition factors: 333344455567
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

#ifndef _qra12_64_64_irr_b_h
#define _qra12_64_64_irr_b_h


#define qra_K       12 // number of information symbols
#define qra_N       63 // codeword length in symbols
#define qra_m        6 // bits/symbol
#define qra_M       64 // Symbol alphabet cardinality
#define qra_NC      51 // number of check symbols (N-K)

// Defines used by the message passing decoder --------

#define qra_V       63 // number of variables in the code graph (N)
#define qra_C      115 // number of factors in the code graph (N +(N-K)+1)
#define qra_NMSG   217 // number of msgs in the code graph
#define qra_MAXVDEG    8 // maximum variable degree (intrinsic check included)
#define qra_MAXCDEG    3 // maximum factor degree
#define qra_R     0.19048 // code rate (K/N)

#ifdef __cplusplus
extern "C" {
#endif

// Tables used by the encoder -------------------------
extern const unsigned int qra_acc_input_idx[qra_NC+1];
extern const unsigned int qra_acc_input_wlog[qra_NC+1];
extern const unsigned int qra_log[qra_M];
extern const unsigned int qra_exp[qra_M-1];
// Tables used by the decoder -------------------------
extern const unsigned int qra_msgw[qra_NMSG];
extern const unsigned int qra_vdeg[qra_V];
extern const unsigned int qra_cdeg[qra_C];
extern const unsigned int qra_v2cmidx[qra_V*qra_MAXVDEG];
extern const unsigned int qra_c2vmidx[qra_C*qra_MAXCDEG];
extern const unsigned int qra_pmat[qra_M*qra_M];

#ifdef __cplusplus
}
#endif

#endif // _qra12_64_64_irr_b_h
