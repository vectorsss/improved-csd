/* StringDictionaryMMRPFCbSDDiff.h
 * 
 *
 * TODO
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
 *
 * Contacting the authors:
 *   Francisco Claude:  	fclaude@recoded.cl
 *   Rodrigo Canovas:  		rcanovas@student.unimelb.edu.au
 *   Miguel A. Martinez-Prieto:	migumar2@infor.uva.es
 */
#ifndef _STRINGDICTIONARY_MMRPFC_LIBCDS_bSD_DIFF_H
#define _STRINGDICTIONARY_MMRPFC_LIBCDS_bSD_DIFF_H

#include <iostream>
using namespace std;


#include "StringDictionary.h"
#include "utils/LogSequence.h"

#define MEMALLOC 32768

class StringDictionaryMMRPFCbSDDiff : public StringDictionary
{
	public:
		/** Generic Constructor. */
		StringDictionaryMMRPFCbSDDiff();

		/** Class Constructor.
		    @param it: iterator scanning the original set of strings.
		*/
		StringDictionaryMMRPFCbSDDiff(IteratorDictString *it);
		
		/** Retrieves the ID corresponding to the given string.
		    @param str: the string to be searched.
		    @param strLen: the string length.
		    @returns the ID (or NORESULT if it is not in the bucket).
		*/
		uint locate(uchar *str, uint strLen);
		
		/** Obtains the string associated with the given ID.
		    @param id: the ID to be extracted.
		    @param strLen: pointer to the extracted string length.
		    @returns the requested string (or NULL if it is not in the
		      dictionary).
		 */
		uchar* extract(size_t id, uint *strLen);
		
		/** Locates all IDs of those elements prefixed by the given 
		    string.
		    @param str: the prefix to be searched.
		    @param strLen: the prefix length.
		    @returns an iterator for direct scanning of all the IDs.
		*/
		IteratorDictID* locatePrefix(uchar *str, uint strLen);
		
		/** Locates all IDs of those elements containing the given 
		    substring.
		    @param str: the substring to be searched.
		    @param strLen: the substring length.
		    @returns an iterator for direct scanning of all the IDs.
		*/
		IteratorDictID* locateSubstr(uchar *str, uint strLen);
		
		/** Retrieves the ID with rank k according to its alphabetical order. 
		    @param rank: the alphabetical ranking.
		    @returns the ID.
		*/
		uint locateRank(uint rank);
		
		/** Extracts all elements prefixed by the given string.
		    @param str: the prefix to be searched.
		    @param strLen: the prefix length.
		    @returns an iterator for direct scanning of all the strings.
		*/
		IteratorDictString* extractPrefix(uchar *str, uint strLen);
		
		/** Extracts all elements containing by the given substring.
		    @param str: the substring to be searched.
		    @param strLen: the substring length.
		    @returns an iterator for direct scanning of all the strings.
		*/
		IteratorDictString* extractSubstr(uchar *str, uint strLen);
		
		/** Obtains the string  with rank k according to its 
		    alphabetical order.
		    @param id: the ID to be extracted.
		    @param strLen: pointer to the extracted string length.
		    @returns the requested string (or NULL if it is not in the
		      dictionary).
		*/
		uchar* extractRank(uint rank, uint *strLen);
		
		/** Extracts all strings in the dictionary sorted in 
		    alphabetical order. 
		    @returns an iterator for direct scanning of all the strings.
		*/
		IteratorDictString* extractTable();
		
		/** Computes the size of the structure in bytes. 
		    @returns the dictionary size in bytes.
		*/
		size_t getSize();
		
		/** Stores the dictionary into an ofstream.
		    @param out: the oftstream.
		*/
		void save(ofstream &out);
		
		/** Loads a dictionary from an ifstream.
		    @param in: the ifstream.
		    @returns the loaded dictionary.
		*/
		static StringDictionary *load(ifstream &in);

		/** Generic destructor. */
		~StringDictionaryMMRPFCbSDDiff();

	    /*** INFO ***/
	    void printInfo();

	protected:


		uint64_t bytesStrings;	//! Length of the strings representation
		uchar *textStrings;	//! Front-Coding of all remaining strings
		
		LogSequence *llcp;	//! Positional index to the strings representation
		LogSequence *rlcp;	//! Positional index to the strings representation

		//Temporary, fix to provide direct access ro bytesStrings
		BitSequence * selPointers;
		//Use in computations, since we have markers
		uint64_t realElements;

		RePair *rp;		//! The RePair representation
		uint bitsrp;		//! Number of bits used for encoding Re-Pair symbols

		uint ls[40];
		uint ms[40];
		uint rs[40];
		uchar *decoded;



		/** Decodes the next internal string according to the 
		    scanning data
		    @param ptr: pointer to the next unprocessed char
		    @param lenPrefix: number of chars shared with the previous string.
		    @param str: the string to be decoded.
		    @param strLen: pointer to the string length.
		    returns the number of chars shared with the previous string
		*/
		inline void decodeNextString(uchar **ptr, uint lenPrefix, uchar *str, uint *strLen);



		/** Searches the first string prefixed for the given one.
		    @param ptr: pointer to the next unprocessed string.
		    @param scanneable: the number of string to be scanned
		    @param decoded: string previously obtained.
		    @param decLen: the decoded string length.
		    @param str: the prefix to be searched.
			@param strLen: the prefix length.
			@returns the internal ID (of 0 if it is not in the bucket).
		*/
		inline uint searchPrefix(uchar **ptr, uint scanneable, uchar *decoded, uint *decLen, uchar *str, uint strLen);

		/** Searches the first string non-prefixed for the given one.
		    @param ptr: pointer to the next unprocessed string.
		    @param scanneable: the number of string to be scanned
		    @param decoded: string previously obtained.
		    @param decLen: the decoded string length.
		    @param str: the prefix to be searched.
		    @param strLen: the prefix length.
		    @returns the internal ID (of 0 if it is not in the bucket).
		*/
		inline uint searchDistinctPrefix(uchar *ptr, uint scanneable, uchar *decoded, uint *decLen, uchar *str, uint strLen);

		/**
		 * TODO
		*/
		inline void buildLcps(uchar *dict, uint *pointers, vector<size_t>*llcp, vector<size_t>*rlcp, uint l, uint r);
		inline void diffLcps(uchar *dict, uint *pointers, vector<size_t>*llcp, vector<size_t>*rlcp,
				vector<size_t>*lorig, vector<size_t>*rorig, uint l, uint r);
		/**
		 * TODO
		*/
		int doSearchString(uchar * string, uint strLen, uint lpos, uint rpos, uint l, uint r, uint rightLlcp, uint leftRlcp);

		/**
		 * TODO
		*/
		inline uchar * getString(uint pos);

		int compareNewPattern(uchar *pattern, uchar *compared, uint *p);

		void doIdToString(uint index, uint depth, uint* ls, uint * ms, uint * rs, uchar * pattern, uint offset);
		inline uint decodeCompareString(uchar *ptr, uchar *compared, uint *pos);
		inline uint decodeString(uchar *str, uchar *ptr, uint limit);

}; 

#endif  /* _STRINGDICTIONARY_MMPFC_H */

