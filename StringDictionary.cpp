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

#include "StringDictionary.h"

StringDictionary*
StringDictionary::load(ifstream & fp, uint opt)
{
	size_t r = loadValue<uint32_t>(fp);
	fp.seekg(0, fp.beg);

	switch(r)
	{

		case MMPFCPLAIN:		return StringDictionaryMMPFCPLAIN::load(fp);
		case MMPFCPLAINLLCP:		return StringDictionaryMMPFCPLAINLlcp::load(fp);
		case MMPFCPLAINRLCP:		return StringDictionaryMMPFCPLAINRlcp::load(fp);

		case MMPFCbSD:			return StringDictionaryMMPFCbSD::load(fp);
		case MMPFCbSDLLCP:			return StringDictionaryMMPFCbSDLlcp::load(fp);
		case MMPFCbSDRLCP:			return StringDictionaryMMPFCbSDRlcp::load(fp);

		case MMRPFCbSD:			return StringDictionaryMMRPFCbSD::load(fp);
		case MMRPFCbSDLLCP:			return StringDictionaryMMRPFCbSDLlcp::load(fp);
		case MMRPFCbSDRLCP:			return StringDictionaryMMRPFCbSDRlcp::load(fp);

		case MMRPFCbSDDAC:			return StringDictionaryMMRPFCbSDDAC::load(fp);
		case MMRPFCbSDDACLLCP:			return StringDictionaryMMRPFCbSDDACLlcp::load(fp);
		case MMRPFCbSDDACRLCP:			return StringDictionaryMMRPFCbSDDACRlcp::load(fp);

		case MMRPFCbSDDAC2:			return StringDictionaryMMRPFCbSDDAC2::load(fp);
		case MMRPFCbSDDAC2LLCP:	return StringDictionaryMMRPFCbSDDAC2Llcp::load(fp);
		case MMRPFCbSDDAC2RLCP:	return StringDictionaryMMRPFCbSDDAC2Rlcp::load(fp);

		case MMRPFCDACVLS:			return StringDictionaryMMRPFCDACVLS::load(fp);
		case MMRPFCDACVLSLLCP:			return StringDictionaryMMRPFCDACVLSLlcp::load(fp);
		case MMRPFCDACVLSRLCP:			return StringDictionaryMMRPFCDACVLSRlcp::load(fp);

		case MMRPFCDAC2VLS:			return StringDictionaryMMRPFCDAC2VLS::load(fp);
		case MMRPFCDAC2VLSLLCP:			return StringDictionaryMMRPFCDAC2VLSLlcp::load(fp);
		case MMRPFCDAC2VLSRLCP:			return StringDictionaryMMRPFCDAC2VLSRlcp::load(fp);

	}

	return NULL;
}

uint
StringDictionary::maxLength()
{
	return maxlength;
}

size_t
StringDictionary::numElements()
{
	return elements;
}

