/*
 * Implementation of "Improved Compressed String Dictionaries" (CIKM'19)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef _UTILS_H
#define _UTILS_H

#include <string.h>
#include <sys/time.h>
#include <sys/stat.h> 
#include <sys/types.h> 

#include <includes/utils/libcdsBasics.h>
using namespace cds_utils;

#define GET_TIME_DIVIDER ((double) 1.0 )	//getTime deals with "seconds" as time unit.
#define GET_TIME_UNIT "sec"
#define MSEC_TIME_DIVIDER  ((double) 1000.0)	//1 sec = 1000 msec
#define MSEC_TIME_UNIT 	  "msec"
#define SEC_TIME_DIVIDER  ((double) 1.0	)	//1 sec = 1 sec
#define SEC_TIME_UNIT 	  "sec"
#define MCSEC_TIME_DIVIDER  ((double)1000000)	//1 microsec = 0.000001 sec
#define MCSEC_TIME_UNIT 	  "microsec"

static const size_t NORESULT = 0;

static const uint32_t MMPFCPLAIN    = 91; 		// Manber-Myers Plain Front-Coding plain
static const uint32_t MMPFCPLAINLLCP    = 911; 		// Manber-Myers Plain Front-Coding plain + one lcp: llcp
static const uint32_t MMPFCPLAINRLCP    = 912; 		// Manber-Myers Plain Front-Coding plain + one lcp: rlcp
static const uint32_t MMPFCbSD    = 92; 		// Manber-Myers Plain Front-Coding plain (libcds bSD)
static const uint32_t MMPFCbSDLLCP    = 921; 		// Manber-Myers Plain Front-Coding plain (libcds bSD) + one lcp: llcp
static const uint32_t MMPFCbSDRLCP    = 922; 		// Manber-Myers Plain Front-Coding plain (libcds bSD) + one lcp: rlcp
static const uint32_t MMRPFCbSD    = 93; 		// Manber-Myers Plain Front-Coding (libcds bSD) (with Repair and VByte for strings)
static const uint32_t MMRPFCbSDLLCP    = 931; 		// Manber-Myers Plain Front-Coding (libcds bSD) (with Repair and VByte for strings) + one lcp: llcp
static const uint32_t MMRPFCbSDRLCP    = 932; 		// Manber-Myers Plain Front-Coding (libcds bSD) (with Repair and VByte for strings) + one lcp: rlcp
// static const uint32_t MMRPFCbSDDIFF    = 94; 	// Manber-Myers Plain Front-Coding (libcds bSD) (with Repair and VByte for strings) + lcps diferenciales
static const uint32_t MMRPFCbSDDAC    = 95; 	// Manber-Myers Plain Front-Coding (libcds bSD) (with Repair and VByte for strings) + lcps con DAC
static const uint32_t MMRPFCbSDDACLLCP    = 951; 	// Manber-Myers Plain Front-Coding (libcds bSD) (with Repair and VByte for strings) + one lcp: llcp con DAC
static const uint32_t MMRPFCbSDDACRLCP    = 952; 	// Manber-Myers Plain Front-Coding (libcds bSD) (with Repair and VByte for strings) + one lcp: rlcp con DAC
static const uint32_t MMRPFCbSDDAC2    = 96; 	// Manber-Myers Plain Front-Coding (libcds bSD) (with Repair and DAC for strings) + lcps con DAC
static const uint32_t MMRPFCbSDDAC2LLCP = 961; 	// Manber-Myers Plain Front-Coding (libcds bSD) (with Repair and DAC for strings) +  one lcp: llcp con DAC
static const uint32_t MMRPFCbSDDAC2RLCP = 962; 	// Manber-Myers Plain Front-Coding (libcds bSD) (with Repair and DAC for strings) +  one lcp: rlcp con DAC
static const uint32_t MMRPFCDACVLS    = 97; 	// Manber-Myers Plain Front-Coding (with Repair and DAC_VLS for strings)
static const uint32_t MMRPFCDACVLSLLCP    = 971; 	// Manber-Myers Plain Front-Coding (with Repair and DAC_VLS for strings) + one lcp: llcp
static const uint32_t MMRPFCDACVLSRLCP    = 972; 	// Manber-Myers Plain Front-Coding (with Repair and DAC_VLS for strings) + one lcp: llcp
static const uint32_t MMRPFCDAC2VLS    = 98; 	// Manber-Myers Plain Front-Coding (with Repair and DAC_VLS for strings) + lcps con DAC
static const uint32_t MMRPFCDAC2VLSLLCP    = 981; 	// Manber-Myers Plain Front-Coding (with Repair and DAC_VLS for strings) + one lcp: llcp con DAC
static const uint32_t MMRPFCDAC2VLSRLCP    = 982; 	// Manber-Myers Plain Front-Coding (with Repair and DAC_VLS for strings) + one lcp: llcp con DAC


inline uint
encodeVB2(uint c, uchar *r)
{
	unsigned int i= 0;

	while (c>127)
	{
		r[i] = (unsigned char)(c&127);
		i++;
		c>>=7;
	}

	r[i] = (unsigned char)(c|0x80);
	i++;

	return i;
}
	
inline uint
decodeVB2(uint *c, uchar *r)
{
	*c = 0;
	int i = 0;
	int shift = 0;

	while ( !(r[i] & 0x80) )
	{
		*c |= (r[i] & 127) << shift;
		i++;
		shift+=7;
	}

	*c |= (r[i] & 127) << shift;
	i++;

	return i;
}


inline ushort
mask(uint k) { return (65535u >> (16u-k)); }

inline size_t
Reallocate(uchar** array, size_t len)
{
	size_t llen = len*2;
	uchar *xarr = new uchar[llen];
	memcpy(xarr, *array, len);
	delete [] *array;

	// This initialization is due to Valgrind warnings
	for (uint i=len; i<llen; i++) xarr[i] = 0;

	*array = xarr;

	return llen;
}

inline size_t
Reallocate(int** array, size_t len)
{
	size_t llen = len*2;
	int *xarr = new int[llen];
	memcpy(xarr, *array, len*sizeof(int));
	delete [] *array;

	// This initialization is due to valgrind warnings
	for (uint i=len; i<llen; i++) xarr[i] = 0;

	*array = xarr;

	return llen;
}

inline int 
longestCommonPrefix(const uchar* str1, const uchar* str2, uint length, uint *lcp)
{	
	uint ptr = 0;

	for (;ptr<length; ptr++)
	{
		if (str1[ptr] != str2[ptr])
		{
			*lcp += ptr;
			return (str1[ptr] - str2[ptr]);;
		}
	}

	*lcp += ptr;

	return 0;
}

inline double
getTime(void)
{
	struct rusage buffer;
	struct timeval tp;
	struct timezone tzp;

	getrusage( RUSAGE_SELF, &buffer );
	gettimeofday( &tp, &tzp );

	return (double) buffer.ru_utime.tv_sec + 1.0e-6 * buffer.ru_utime.tv_usec;
}


#endif  /* _UTILS_H */

