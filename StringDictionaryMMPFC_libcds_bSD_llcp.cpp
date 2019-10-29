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


#include "StringDictionaryMMPFC_libcds_bSD_llcp.h"

StringDictionaryMMPFCbSDLlcp::StringDictionaryMMPFCbSDLlcp()
{
	this->type = MMPFCbSDLLCP;
	this->elements = 0;
	this->maxlength = 0;
}

StringDictionaryMMPFCbSDLlcp::StringDictionaryMMPFCbSDLlcp(IteratorDictString *it)
{

	this->type = MMPFCbSDLLCP;
	this->elements = 0;
	this->maxlength = 0;


	// Count number of elements
	{
		uint lenCurrent = 0;
		while (it->hasNext()) {
			it->next(&lenCurrent);
			this->elements++;
		}
		((IteratorDictStringPlain*)it)->restart();
	} // END BLOCK count number of elemens

	this->realElements = this->elements+2; //Add new markers at beginning and end

	//Initialize dictionary with all strings
	uchar *dict = new uchar[it->size()+this->realElements + 3];
	uint *pointers = new uint[this->realElements+1];

	uchar *strCurrent = NULL;
	uchar maxChar = 0;
	uint lenCurrent = 0;
	size_t processed = 0;
	size_t curEntry = 0;

	//First marker: empty string
	pointers[curEntry++] = 0;
	dict[processed++] = '\0';

	while (it->hasNext()) {
		strCurrent = it->next(&lenCurrent);
//		printf("%s %d\n", strCurrent, lenCurrent);

		// Max entry length of the dictionary
		if (lenCurrent >= this->maxlength) {
			maxlength = lenCurrent+1;
		}
		pointers[curEntry++] = processed;
		// Copy string into "dict"
		for (uint i = 0; i < lenCurrent; i++) {
			if (strCurrent[i] > maxChar) {
				maxChar = strCurrent[i];
			}
			dict[processed] = strCurrent[i];
			processed++;
		}
		dict[processed] = '\0';
		processed++;
	}
	maxChar++;

	printf("Original text length (with separators) : %ld\n", processed);

	printf("Found %ld elements in dict. Added 2 markers\n", elements);

	//Second marker:
	pointers[curEntry++] = processed;
	dict[processed++] = maxChar++;
	dict[processed++] = '\0';
	pointers[curEntry] = processed;

	printf("%ld  VS %ld; %ld %ld\n", it->size(), processed,  elements, realElements, curEntry);

//	assert(processed == it->size() + this->realElements+2);
	assert(curEntry == this->realElements);

	cout << "Initialized dictionary with " << this->elements << " words (adds 2 markers) " << endl;

	vector<size_t> llcp(this->realElements);
	vector<size_t> rlcp(this->realElements);

	llcp[0] = rlcp[0] = llcp[this->realElements-1] = rlcp[this->realElements-1] = 0;

	buildLcps(dict, pointers, &llcp, &rlcp, 0, (int) this->realElements-1);

//	Block to actually remove characters unneded because of Front-Coding
	uchar *cdict = new uchar[processed];
	uint io = 0, ic = 0, nword = 0;
	uint maxseq = 0, currentseq = 0;
	int minRule = MAXINT;

	int lastZero = -1;
	while (io<processed) {
		uint skip = llcp[nword];
		pointers[nword] = ic;
		nword++;
		io+=skip;
		while (dict[io] != 0) {
			cdict[ic]=dict[io];
			ic++; io++;
		}
		if (ic == lastZero+1) {
			cdict[ic++] = 0;
		}
		lastZero = ic-1;
		io++;
	}
	pointers[nword] = ic;



	this->llcp = new LogSequence(&llcp, bits(maxlength));

	this->textStrings = dict;
	this->bytesStrings = processed;

	//Non qqu
	this->textStrings = cdict;
	this->bytesStrings = ic;
	
	int nwords = (this->bytesStrings + 1)/W+1;
	uint *bitsPointers =  new uint[nwords];
	for (int i = 0; i < nwords; i++) {
		bitsPointers[i] = 0;
	}
	
	for (int i = 0; i <= this->realElements; i++) {
		cds_utils::bit_set(bitsPointers, pointers[i]);
	}
	
	this->selPointers = new BitSequenceSDArray(bitsPointers, this->bytesStrings + 1);

}

uint 
StringDictionaryMMPFCbSDLlcp::locate(uchar *str, uint strLen)
{

	//Add one, since we add an empty marker
//	printf("\n\n\n\n\n");
//	cerr << "locate " << str << "----" << strLen << endl;
//	printf("\n\n\n\n\n");
//	str[strLen-1] = '\0';

	uint res = doSearchString(str, strLen, 0, this->realElements-1, 0, 0);
//	uint n1 = this->pointers[res];
//	uint n2 = this->pointers[res+1];
//	uint lcp = 0;
//	const uchar *basestring = str;
//	const uchar *resstring = this->textStrings + n1;
//	longestCommonPrefix(basestring, resstring, strLen-1, &lcp);
//
//	if (n2 - n1 - 1 != strLen || lcp != strLen - 1) {
//		cout << "got " << n2-n1-1 << " expected " << strLen << endl;
//		printf("Got %s expected %s\n", resstring, basestring);
//		exit(-1);
//	}


	return res;
//	cerr << "got " << res << endl;
}



uchar *
StringDictionaryMMPFCbSDLlcp::extract(size_t id, uint *strLen)
{

//	printf("Extract(%d). Should be %s\n", id, textStrings+pointers[id]);

	uint i, m;

	uchar * res = new uchar[maxlength];
//	for (i = 0; i < maxlength; i++) {
//		res[i] = 0;
//	}

	ls[0] = 0;
	rs[0] = this->realElements - 1;

	i = 0;
	do {
		m = (ls[i] + rs[i]) / 2;
		ms[i] = m;
		i++;
		if (id < m) {
			rs[i] = m;
			ls[i] = ls[i-1];
		} else {
			ls[i] = m;
			rs[i] = rs[i-1];
		}
	} while (m != id);
	i--;
	int depth = i;

	uint index = id;
	int offset = maxlength;
	int first = 1;
	while (1) {
		int nextIndex = 0;
		int maxlcp = 0;
		int llcpValue = llcp->getField(index);
		nextIndex = ls[depth];
		maxlcp = llcpValue;
		int i;
		if (maxlcp < offset ) {
			int basepos = this->selPointers->select1(index+1);
			int maxlen = this->selPointers->select1(index+2)-basepos;
			for (i = maxlcp; i < offset && (i - maxlcp < maxlen) && this->textStrings[basepos+i-maxlcp]; i++) {
				res[i] = this->textStrings[basepos+i-maxlcp];
			}
			if (first) {
				*strLen = i;
				first = false;
			}
			offset = maxlcp;
		}
		index=nextIndex;
		depth--;
		if (!offset || depth == 0) break;
	}
	int pos = this->selPointers->select1(ms[0]+1);
	for (i = 0; i < offset; i++) {
		res[i] = this->textStrings[pos+i];
	}
	res[*strLen]='\0';

	return res;
}

IteratorDictID*
StringDictionaryMMPFCbSDLlcp::locatePrefix(uchar *str, uint strLen)
{


//	size_t leftBucket = 1, rightBucket = buckets;
//	size_t leftID = 0, rightID = 0;
//
//	// Locating the candidate buckets for the prefix
//	locateBoundaryBuckets(str, strLen, &leftBucket, &rightBucket);
//
//	if (leftBucket > NORESULT)
//	{
//		uchar *decoded; uint decLen;
//		uchar *ptr = getHeader(leftBucket, &decoded, &decLen);
//
//		uint scanneable = bucketsize;
//		if ((leftBucket == buckets) && ((elements%bucketsize) != 0)) scanneable = (elements%bucketsize);
//
//		if (leftBucket == rightBucket)
//		{
//			// All candidate results are in the same bucket
//			leftID = searchPrefix(&ptr, scanneable, decoded, &decLen, str, strLen);
//
//			// No strings use the required prefix
//			if (leftID == NORESULT)
//				return new IteratorDictIDContiguous(NORESULT, NORESULT);
//			else
//				rightID = leftID+searchDistinctPrefix(ptr, scanneable-leftID, decoded, &decLen, str, strLen)-1;
//
//			leftID += (leftBucket-1)*bucketsize;
//			rightID += (rightBucket-1)*bucketsize;
//		}
//		else
//		{
//			// All prefixes exceed (possibly) a single bucket
//			{
//				// Searching the left limit
//				leftID = searchPrefix(&ptr, scanneable, decoded, &decLen, str, strLen);
//
//				// The first prefix is the next bucket header
//				if (leftID == NORESULT) leftID = leftBucket*bucketsize+1;
//				// The first prefix is an internal string of the leftBucket
//				else leftID += (leftBucket-1)*bucketsize;
//			}
//
//			{
//				// Searching the right limit
//				delete [] decoded;
//
//				ptr = getHeader(rightBucket, &decoded, &decLen);
//
//				scanneable = bucketsize;
//				if ((rightBucket == buckets) && ((elements%bucketsize) != 0)) scanneable = (elements%bucketsize);
//
//				rightID = searchDistinctPrefix(ptr, scanneable-1, decoded, &decLen, str, strLen);
//				rightID += (rightBucket-1)*bucketsize;
//			}
//		}
//
//		delete [] decoded;
//		return new IteratorDictIDContiguous(leftID, rightID);
//	}
//	else
//	{
//		// No strings use the required prefix
//		return new IteratorDictIDContiguous(NORESULT, NORESULT);
//	}
}

IteratorDictID*
StringDictionaryMMPFCbSDLlcp::locateSubstr(uchar *str, uint strLen)
{
	cerr << "This dictionary does not provide substring location" << endl;
	return NULL;
}

uint 
StringDictionaryMMPFCbSDLlcp::locateRank(uint rank)
{
	return rank;
}

IteratorDictString*
StringDictionaryMMPFCbSDLlcp::extractPrefix(uchar *str, uint strLen)
{
//	IteratorDictIDContiguous *it = (IteratorDictIDContiguous*)locatePrefix(str, strLen);
//
//	if (it->getLeftLimit() != NORESULT)
//	{
//		// Positioning the LEFT Limit
//		size_t left = it->getLeftLimit();
//		uint leftbucket = 1+((left-1)/bucketsize);
//		uint leftpos = ((left-1)%bucketsize);
//
//		// Positioning the RIGHT Limit
//		size_t right = it->getRightLimit();
//
//		delete it;
//
//		size_t ptrS = blStrings->getField(leftbucket);
//
//		return new IteratorDictStringPFC(textStrings+ptrS, leftpos, bucketsize, right-left+1, maxlength);
//	}
//	else return NULL;
}

IteratorDictString*
StringDictionaryMMPFCbSDLlcp::extractTable()
{
	cerr << "Not supported (yet)" << endl;
	return 0;
}

IteratorDictString*
StringDictionaryMMPFCbSDLlcp::extractSubstr(uchar *str, uint strLen)
{
	cerr << "This dictionary does not provide substring extraction" << endl;
	return 0; 
}

uchar *
StringDictionaryMMPFCbSDLlcp::extractRank(uint rank, uint *strLen)
{
	return extract(rank, strLen);
}

size_t 
StringDictionaryMMPFCbSDLlcp::getSize()
{
	return (bytesStrings*sizeof(uchar))+this->selPointers->getSize()+llcp->getSize()+sizeof(StringDictionaryMMPFCbSDLlcp);
}

void 
StringDictionaryMMPFCbSDLlcp::save(ofstream &out)
{
	saveValue<uint32_t>(out, type);
	saveValue<uint64_t>(out, elements);
	saveValue<uint32_t>(out, maxlength);
	saveValue<uint64_t>(out, bytesStrings);
	saveValue<uchar>(out, textStrings, bytesStrings);
	llcp->save(out);
	selPointers->save(out);
}


StringDictionary*
StringDictionaryMMPFCbSDLlcp::load(ifstream &in)
{
	size_t type = loadValue<uint32_t>(in);
	if(type != MMPFCbSDLLCP) return NULL;

	cout << "Reading MMPFCbSDLLCP" <<endl;

	StringDictionaryMMPFCbSDLlcp *dict = new StringDictionaryMMPFCbSDLlcp();

	dict->type = MMPFCbSDLLCP;
	dict->elements = loadValue<uint64_t>(in);
	dict->realElements = dict->elements + 2;
	dict->maxlength = loadValue<uint32_t>(in);
	dict->bytesStrings = loadValue<uint64_t>(in);
	dict->textStrings = loadValue<uchar>(in, dict->bytesStrings);
	dict->llcp = new LogSequence(in);
	dict->selPointers = BitSequence::load(in);

	return dict;
}


//void
//StringDictionaryMMPFCbSDLlcp::decodeNextString(uchar **ptr, uint lenPrefix, uchar *str, uint *strLen)
//{
//	uint lenSuffix;
//
//	lenSuffix = strlen((char*)*ptr);
//	strncpy((char*)(str+lenPrefix), (char*)*ptr, lenSuffix+1);
//
//	*ptr += lenSuffix+1;
//	*strLen = lenPrefix+lenSuffix;
//}


//uint
//StringDictionaryMMPFCbSDLlcp::searchPrefix(uchar **ptr, uint scanneable, uchar *decoded, uint *decLen, uchar *str, uint strLen)
//{
//	uint sharedCurr=0, sharedPrev=0;
//	int cmp=0; uint id=1;
//
//	while (true)
//	{
//		cmp=longestCommonPrefix(decoded+sharedCurr, str+sharedCurr, *decLen-sharedCurr, &sharedCurr);
//
//		if (sharedCurr==strLen) break;
//		else
//		{
//			id++;
//			if ((cmp > 0) || (id > scanneable)) break;
//
//			*ptr += VByte::decode(&sharedPrev, *ptr);
//			if  (sharedPrev < sharedCurr) break;
//			decodeNextString(ptr, sharedPrev, decoded, decLen);
//		}
//	}
//
//	return id;
//}
//
//uint
//StringDictionaryMMPFCbSDLlcp::searchDistinctPrefix(uchar *ptr, uint scanneable, uchar *decoded, uint *decLen, uchar *str, uint strLen)
//{
//	uint id = 1;
//	uint lenPrefix;
//
//	for (id=1; id<=scanneable; id++)
//	{
//		ptr += VByte::decode(&lenPrefix, ptr);
//		if (lenPrefix < strLen) break;
//		decodeNextString(&ptr, lenPrefix, decoded, decLen);
//	}
//
//	return id;
//}

void StringDictionaryMMPFCbSDLlcp::printInfo() {
	printf("%d %d %d\n", this->elements, this->bytesStrings, this->realElements);
    printf("******** MMPFC Size ********\n");
    printf("LLCP -> %lu\n", this->llcp->getSize());
    printf("strings (size of not common characters): %lu\n", this->bytesStrings);
    printf("selPointers: %lu\n", this->selPointers->getSize());
//    printf("XBWInt -> %lu\n", this->xbw->getSize());
//    printf("----------------------------\n");
    printf("MMPFC structure: %lu\n", sizeof(StringDictionaryMMPFCbSDLlcp));
    printf("TOTAL  -> %lu\n", this->getSize());
//    printf("****************************\n");
//    printf("\n\n");
//
//    this->xbw->printInfo();
//    this->rp->printInfo();
}

StringDictionaryMMPFCbSDLlcp::~StringDictionaryMMPFCbSDLlcp()
{
	delete [] textStrings; delete llcp;
}



void StringDictionaryMMPFCbSDLlcp::buildLcps(uchar *dict, uint *pointers, vector<size_t>*llcp, vector<size_t>*rlcp, uint l, uint r) {
    uint m = (l + r) / 2;
//    uint level = bits(r-l);
//    if (level > 10)
//    	printf("[%d] building %d (%d-%d)\n", bits(r-l), m, l, r);
    if (m == l) return;

    uchar *wordm = dict + pointers[m];
    uchar *wordl = dict + pointers[l];
    uchar *wordr = dict + pointers[r];
    uint lenm = pointers[m+1] - pointers[m];
    uint lcp = 0;
    longestCommonPrefix(wordm, wordl, lenm, &lcp);
    (*llcp)[m] = lcp;
    lcp = 0;
    longestCommonPrefix(wordm, wordr, lenm, &lcp);
    (*rlcp)[m] = lcp;
    buildLcps(dict, pointers, llcp, rlcp, l, m);
    buildLcps(dict, pointers, llcp, rlcp, m, r);
}

inline int StringDictionaryMMPFCbSDLlcp::doSearchString(uchar * string, uint len, uint lpos, uint rpos, uint l, uint r) {
    uint m = (lpos + rpos) / 2;
    uint foo;

//    printf("%s, %d %d %d %d %d\n", string, len, lpos, rpos, l, r);
    int chooseLeft = false;
	uint llcpvalue = llcp->getField(m);
	if (llcpvalue > l) {
		return doSearchString(string, len, m, rpos, l, r);
	} else if (llcpvalue == l) {
		int base = this->selPointers->select1(m+1);
        uchar * dictm = this->textStrings + base;
        int maxlen = this->selPointers->select1(m+2)-base;
//       	printf("%d %d\n", base, maxlen);
        uint p = l;
        int cmp = compareNewPattern(string, dictm -p, &p, maxlen+p);
//        printf("cmp = %d\n", cmp);
        if (cmp > 0) {
			return doSearchString(string, len, m, rpos, p, r);
		}
		if (cmp < 0) {
			return doSearchString(string, len, lpos, m, l, p);
		}
		return m;
	} else {
		if (m == lpos) {
			return m;
		}
		return doSearchString(string, len, lpos, m, l, llcpvalue);
	}
}

inline int StringDictionaryMMPFCbSDLlcp::compareNewPattern(uchar *pattern, uchar *compared, uint *pos, int len) {
	uint p=*pos;
//	printf("compare $%.10s$ $%.10s$ %d %d\n", pattern+p, compared+p, p, len);
	while(p < len && pattern[p] && compared[p] && compared[p] == pattern[p]) {
//		printf("[%c]", compared[p]);
		p++;
	}

	*pos = p;
	if (p == len) {
		return pattern[p];
	}

	return pattern[p] - compared[p];
}


