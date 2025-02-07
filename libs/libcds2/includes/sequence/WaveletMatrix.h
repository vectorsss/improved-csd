/* WaveletTreeNoptrs.h
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * WaveletTreeNoptrs definition
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
 *
 */

#ifndef _WVTREE_MATRIX_H
#define _WVTREE_MATRIX_H

#include <iostream>
#include <cassert>
#include <utils/libcdsBasics.h>
#include <bitsequence/BitSequence.h>
#include <bitsequence/BitSequenceBuilder.h>
#include <sequence/Sequence.h>
#include <mapper/Mapper.h>

using namespace std;

namespace cds_static
{

	class WaveletMatrix : public Sequence
	{
		public:

			/** Builds a Wavelet Tree for the string
			 * pointed by symbols assuming its length
			 * equals n and uses bmb to build the bitsequence
			 * @param bmb builder for the bitmaps in each level.
			 * @param am alphabet mapper (we need all symbols to be used).
			 * */
			WaveletMatrix(uint * symbols, size_t n, BitSequenceBuilder * bmb, Mapper * am, bool deleteSymbols = false);
			WaveletMatrix(uint * symbols, size_t n, vector<BitSequenceBuilder*> &bsbs, Mapper * am, bool deleteSymbols = false);
			WaveletMatrix(const Array &symbols2, BitSequenceBuilder * bmb, Mapper * am);
			WaveletMatrix(uint * symbols, size_t n, BitSequenceBuilder * bmb, BitSequenceBuilder * bmb2, Mapper * am, bool deleteSymbols = false);

			//
			/** Destroys the Wavelet Tree */
			virtual ~WaveletMatrix();

			virtual size_t rank(uint symbol, size_t pos) const;
			virtual size_t select(uint symbol, size_t j) const;
			virtual uint access(size_t pos) const;
			virtual ulong extract(ulong i, ulong j, vector<uint> &res) const;
			virtual size_t getSize() const;
			virtual size_t rangeCount(size_t xs, size_t xe, uint ys, uint ye) ;
			virtual void range(int i1, int i2, int j1, int j2, vector<pair<int,int> > *res);
			virtual void save(ofstream & fp) const;
			static WaveletMatrix * load(ifstream & fp);

		protected:
			WaveletMatrix();

			Mapper * am;

			BitSequence **bitstring;

			/** Length of the string. */
			size_t n;

			/** Height of the Wavelet Tree. */
			uint height, max_v;
			uint *OCC;
			size_t *C;

			/** Obtains the maximum value from the string
			 * symbols of length n */
			uint max_value(uint *symbols, size_t n);

			/** How many bits are needed to represent val */
			uint bits(uint val);

			/** Returns true if val has its ind-th bit set
			 * to one. */
			bool is_set(uint val, uint ind) const;

			/** Sets the ind-th bit in val */
			uint set(uint val, uint ind) const;

			/** Recursive function for building the Wavelet Tree. */
			void build_level(uint **bm, uint *symbols, size_t length, uint *occs);
			size_t rngCount(size_t xs, size_t xe, uint ys, uint ye, uint current, uint lefty, uint righty, int level);
			uint trackUp(uint pos, int l);
			size_t rng(int xs, int xe, int ys, int ye, uint current, int level, uint lefty, uint righty,vector<pair<int,int> > *res,bool addRes);
			ulong extract(ulong i, ulong j, uint level, uint code, vector<uint> &res) const;
	};
};
#endif
