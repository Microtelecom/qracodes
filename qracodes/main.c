// main.c 
// Word Error Rate test for the Q-ary RA code (12,63) over GF(64)
// 
// (c) 2016 - Nico Palermo, IV3NWV - Microtelecom Srl, Italy
// ------------------------------------------------------------------------------
// This file is part of the qracodes project, a Forward Error Control
// encoding/decoding package based on Q-ary RA (Repeat and Accumulate) LDPC codes.
//
// Files in the package:
//    main.c		 - this file
//    normrnd.c/.h   - random gaussian number generator and header
//    npfwht.c/.h    - Fast Walsh-Hadamard Transforms
//    pdmath.c/.h    - Elementary math on probability distributions
//    qra12_63_64_irr_b.c/.h - Tables and defines for a P(12,63) IRA code over GF(64)
//    qracodes.c/.h  - QRA (Q-ary Repeat & Accumulate) codes encoding/decoding functions
//
// -------------------------------------------------------------------------------
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

#if _WIN32 // note the underscore: without it, it's not msdn official!
	// Windows (x64 and x86)
	#include <windows.h>   // required only for GetTickCount(...)
#endif

#if __linux__
#include <unistd.h>
#include <time.h>

unsigned GetTickCount() {
    struct timespec ts;
    unsigned theTick = 0U;
    clock_gettime( CLOCK_REALTIME, &ts );
    theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
}
#endif

#if __APPLE__
#endif

#include <stdio.h>

#ifdef FTZ_ENABLE	   // shouldn't be required anymore - kept here just as a remind in the case we need it
				       // round off errors in the qra_extrinsics fixed with biasing
#include <xmmintrin.h> // used to set the Flush to Zero flag in the FPU
#endif

#include "normrnd.h"		   // gaussian number generators
#include "qra12_63_64_irr_b.h" // tables and defines for the P(12,63) Q-ary RA code over GF(64)
#include "qracodes.h"		   // encoding/decoding functions

#define NSAMPLES (qra_N*qra_M) // samples per message (codeword length * codeword alphabet size)

#define CHANNEL_AWGN     0
#define CHANNEL_RAYLEIGH 1

int wer_test(int channel_type, float EbNodB, int desirederrs, FILE *fout)
{
	int k;

//	uint x[qra_K]= {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
	uint x[qra_K]= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,};
	uint y[qra_N],ydec[qra_N];

	float rp[qra_N*qra_M];		// received samples (real component)
	float rq[qra_N*qra_M];		// received samples (imag component)
	float chp[qra_N];			// channel gains (real component)
	float chq[qra_N];			// channel gains (imag component)
	float r[qra_N*qra_M];		// r = sqrt(rp^2+rq^2)

	float No = 1.0f;				// noise spectral density
	float sigma   = (float)sqrt(No/2.0f);	// std dev of noise I/Q components
	float sigmach = (float)sqrt(1/2.0f);	// std dev of channel I/Q gains
	float R = 1.0f*qra_K/qra_N;		// code rate

	float EbNo = (float)pow(10,EbNodB/10);
	float EsNo = 1.0f*qra_m*R*EbNo;
	float Es = EsNo*No;
	float A = (float)sqrt(Es);

	// Eb/No value for which we optimize the bessel metric
	const float EbNodBMetric = 2.8f;
	const float EbNoMetric   = (float)pow(10,EbNodBMetric/10);
	const float EsNoMetric   = 1.0f*qra_m*R*EbNoMetric;

	float ex[qra_N*qra_M];	// extrinsic information from the MP algorithm
	float avgt;				// average time per codeword encoding/channel sim/decoding
	int nt=0;				// transmitted codewords
	int nerrs  = 0;			// total number of errors 
	int nerrsu = 0;			// number of undetected errors
	int nep    = -1;

	unsigned int cini,cend; // time tick counters

	int rc;

	// encode the input
	qra_encode(y,x);


	cini = GetTickCount();

	while (1) {

		// simulate the channel
		normrnd_s(rp,NSAMPLES,0,sigma);
		normrnd_s(rq,NSAMPLES,0,sigma);

		if (channel_type == CHANNEL_AWGN) {
			for (k=0;k<qra_N;k++) 
				rp[k*qra_M+y[k]]+=A;
			}
		else if (channel_type == CHANNEL_RAYLEIGH) {
			normrnd_s(chp,qra_N,0,sigmach);
			normrnd_s(chq,qra_N,0,sigmach);
			for (k=0;k<qra_N;k++) {
				rp[k*qra_M+y[k]]+=A*chp[k];
				rq[k*qra_M+y[k]]+=A*chq[k];
				}
			}
		else
			return -1;	// unknown channel type

		// compute the squares of the amplitudes of the received samples
		for (k=0;k<NSAMPLES;k++) 
			r[k] = rp[k]*rp[k] + rq[k]*rq[k];

		// compute the intrinsic symbols probabilities 
		qra_mfskbesselmetric(r,EsNoMetric);

		// compute the extrinsic symbols probabilities with the message-passing algorithm
		// stop if extrinsic information does not converges to 1 within 50 iterations
		rc = qra_extrinsic(ex,r,50);

		if (rc>=0) { // the MP algorithm converged to Iex~1 in rc iterations

			// decode the codeword
			qra_mapdecode(ydec,ex,r);

			// look for undetected errors
			for (k=0;k<qra_K;k++) 
				if (ydec[k]!=x[k]) {	// decoded symbol differs from the transmitted one
					nerrsu += 1;
					nerrs  += 1;
					break;
					}
			}	
		else // failed to converge to a solution within the given number of iterations
			nerrs++;

		// print a dot every 100 errors 
		if (nerrs!=nep && (nerrs%100)==0) {
			printf("."); fflush (stdout);
			nep = nerrs;
		}

		nt = nt+1;
		if (nerrs==desirederrs) 
			break;		// desired number of errors reached. exit and report the results

		}

	cend = GetTickCount();
	printf("\n"); fflush (stdout);

	avgt = 1.0f*(cend-cini)/nt; // average time per decode(ms)
	printf("Elapsed time=%5.1fs Avg time per word=%6.2fms ntx=%6d errs=%5d errsu=%3d pe=%.2e\n",0.001f*(cend-cini),avgt, nt, nerrs, nerrsu, 1.0f*nerrs/nt);
	fflush (stdout);
	fprintf(fout,"%01d %.2f %d %d %d %.2f\n",channel_type, EbNodB,nt,nerrs,nerrsu, avgt);

	return 0;
}

void wer_test_awgn(void)
{
	int k,nn;
	FILE *fout;

	float EbNodB[]   = { 0.8f, 1.3f, 1.8f, 2.3f, 2.8f, 3.3f, 3.8f, 4.3f, 4.8f};
	int   nerrstgt[] = { 5000, 5000, 5000, 5000, 5000, 5000, 2000, 500, 100};

	nn = sizeof(EbNodB)/sizeof(float);

	printf("\nTesting the code over the AWGN channel\n");
	fout = fopen("wer-awgn.txt","w");

	for (k=0;k<nn;k++) {
		printf("\nTesting at Eb/No=%4.1f dB...\n",EbNodB[k]);
		wer_test(CHANNEL_AWGN, EbNodB[k], nerrstgt[k], fout);
		}

	fclose(fout);

}

void wer_test_rayleigh(void)
{
	int k,nn;
	FILE *fout;

	float EbNodB[]   = { 1.1f, 1.6f, 2.1f, 2.6f, 3.1f, 3.6f, 4.1f, 4.6f, 5.1f, 5.6f, 6.1f, 6.6f};
	int   nerrstgt[] = { 5000, 5000, 5000, 5000, 5000, 5000, 2000, 2000, 1000, 1000, 200,  50};

	nn = sizeof(EbNodB)/sizeof(float);

	printf("\nTesting the code over the Rayleigh channel\n");
	fout = fopen("wer-rayleigh.txt","w");

	for (k=0;k<nn;k++) {
		printf("\nTesting at Eb/No=%4.1f dB...\n",EbNodB[k]);
		wer_test(CHANNEL_RAYLEIGH, EbNodB[k], nerrstgt[k], fout);
		}

	fclose(fout);

}

//#define TEST_WER_AWGN
//#define TEST_WER_RAYLEIGH

#if !defined TEST_WER_AWGN && !defined TEST_WER_RAYLEIGH
#error Please define one among TEST_WER_AWGN and TEST_WER_RAYLEIGH
#endif

int main(int argc, char* argv[])
{
#ifdef FTZ_ENABLE
	// set the Flush-to-Zero flag in the FPU
#if _WIN32	
	int ftzmode;
	_MM_GET_FLUSH_ZERO_MODE(ftzmode);
#elif __linux__
	int ftzmode =
	_MM_GET_FLUSH_ZERO_MODE();
#else
#error Platform does not support _MM_GET_FLUSH_ZERO_MODE
#endif
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
#endif

#ifdef TEST_WER_AWGN
	wer_test_awgn();
#endif

#ifdef TEST_WER_RAYLEIGH
	wer_test_rayleigh();
#endif

#ifdef FTZ_ENABLE
	// restore flush_zero_mode
	_MM_SET_FLUSH_ZERO_MODE(ftzmode);
#endif

	return 0;
}

