/*  BitSequenceBuilder.h
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * Francisco Claude <fclaude@cs.uwaterloo.ca>
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

#ifndef BITSEQUENCEBUILDER_H
#define BITSEQUENCEBUILDER_H

#include <utils/libcdsBasics.h>
#include <bitsequence/BitSequence.h>
#include <utils/BitString.h>

using namespace cds_utils;

namespace cds_static
{
	/** Base class for BitSequence builders, it defines the build function
	 * that takes only a bitmap. The parameters for construction are can
	 * be set in any way by the builder, but none are received when
	 * the actual building takes place.
	 *
	 * @author Francisco Claude
	 */
	class BitSequenceBuilder
	{
		public:
			BitSequenceBuilder() { userCount=0; }
			virtual ~BitSequenceBuilder() {}
			virtual void use() { userCount++; }
			virtual void unuse() {
				userCount--;
//				if (userCount==0) delete this;
			 }
			virtual BitSequence * build(uint * bitseq, size_t len) const = 0;
			virtual BitSequence * build(const BitString & bs) const = 0;

		protected:
			int userCount;
	};
};

#include <bitsequence/BitSequenceBuilderRG.h>
#include <bitsequence/BitSequenceBuilderRRR.h>
#include <bitsequence/BitSequenceBuilderSDArray.h>
#include <bitsequence/BitSequenceBuilderDArray.h>
#include <bitsequence/BitSequenceBuilderPlain.h>
#include <bitsequence/BitSequenceBuilderRPSC.h>
#include <bitsequence/BitSequenceBuilderRPSN.h>
#include <bitsequence/BitSequenceBuilderDelta.h>
#endif
