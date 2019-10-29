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


#include "StringDictionaryMMPFCDAC.h"

StringDictionaryMMPFCDAC::StringDictionaryMMPFCDAC()
{
	this->type = MMPFCDAC;
	this->elements = 0;
	this->maxlength = 0;
}

StringDictionaryMMPFCDAC::StringDictionaryMMPFCDAC(IteratorDictString *it)
{

	this->type = MMPFCDAC;
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

	printf("it->size() = %ld\n", it->size());

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

	while (io<processed) {
		uint skip = llcp[nword] > rlcp[nword] ? llcp[nword] : rlcp[nword];
		pointers[nword] = ic;
		nword++;
		io+=skip;
		while (dict[io] != 0) {
			cdict[ic]=dict[io];
			ic++; io++;
		}
		cdict[ic++] = 0;
		io++;
	}
	pointers[nword] = ic;



	this->llcp = new LogSequence(&llcp, bits(maxlength));
	this->rlcp = new LogSequence(&rlcp, bits(maxlength));

	this->textStrings = dict;
	this->bytesStrings = processed;

	//Non qqu
	this->textStrings = cdict;
	this->bytesStrings = ic;
	this->pointers = pointers;

}

uint 
StringDictionaryMMPFCDAC::locate(uchar *str, uint strLen)
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
StringDictionaryMMPFCDAC::extract(size_t id, uint *strLen)
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
		int rlcpValue = rlcp->getField(index);
		if (llcpValue >= rlcpValue) {
			nextIndex = ls[depth];
			maxlcp = llcpValue;
		} else {
			nextIndex = rs[depth];
			maxlcp = rlcpValue;
		}
		int i;
		if (maxlcp < offset ) {

			for (i = maxlcp; i < offset && this->textStrings[pointers[index]+i-maxlcp]; i++) {
				res[i] = this->textStrings[pointers[index]+i-maxlcp];
			}
			if (first) {
				*strLen = i;
				first = false;
			}
			offset = maxlcp;
		}
		index=nextIndex;
		depth--;
		if (!offset || depth == 1) break;
	}
	int pos = pointers[ms[0]];
	for (i = 0; i < offset; i++) {
		res[i] = this->textStrings[pos+i];
	}
	res[*strLen]='\0';

	return res;
}

IteratorDictID*
StringDictionaryMMPFCDAC::locatePrefix(uchar *str, uint strLen)
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
StringDictionaryMMPFCDAC::locateSubstr(uchar *str, uint strLen)
{
	cerr << "This dictionary does not provide substring location" << endl;
	return NULL;
}

uint 
StringDictionaryMMPFCDAC::locateRank(uint rank)
{
	return rank;
}

IteratorDictString*
StringDictionaryMMPFCDAC::extractPrefix(uchar *str, uint strLen)
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
StringDictionaryMMPFCDAC::extractTable()
{
	cerr << "Not supported (yet)" << endl;
	return 0;
}

IteratorDictString*
StringDictionaryMMPFCDAC::extractSubstr(uchar *str, uint strLen)
{
	cerr << "This dictionary does not provide substring extraction" << endl;
	return 0; 
}

uchar *
StringDictionaryMMPFCDAC::extractRank(uint rank, uint *strLen)
{
	return extract(rank, strLen);
}

size_t 
StringDictionaryMMPFCDAC::getSize()
{
	return (bytesStrings*sizeof(uchar))+(this->realElements+1)*sizeof(uint)+llcp->getSize()+rlcp->getSize()+sizeof(StringDictionaryMMPFCDAC);
}

void 
StringDictionaryMMPFCDAC::save(ofstream &out)
{
	saveValue<uint32_t>(out, type);
	saveValue<uint64_t>(out, elements);
	saveValue<uint32_t>(out, maxlength);
	saveValue<uint64_t>(out, bytesStrings);
	saveValue<uchar>(out, textStrings, bytesStrings);
	llcp->save(out);
	rlcp->save(out);
	saveValue<uint>(out, pointers, realElements+1);
}


StringDictionary*
StringDictionaryMMPFCDAC::load(ifstream &in)
{
	size_t type = loadValue<uint32_t>(in);
	if(type != MMPFCDAC) return NULL;

	cout << "Reading MMPFCDAC" <<endl;

	StringDictionaryMMPFCDAC *dict = new StringDictionaryMMPFCDAC();

	dict->type = MMPFCDAC;
	dict->elements = loadValue<uint64_t>(in);
	dict->realElements = dict->elements + 2;
	dict->maxlength = loadValue<uint32_t>(in);
	dict->bytesStrings = loadValue<uint64_t>(in);
	dict->textStrings = loadValue<uchar>(in, dict->bytesStrings);
	dict->llcp = new LogSequence(in);
	dict->rlcp = new LogSequence(in);
	dict->pointers = loadValue<uint>(in, dict->realElements+1);

	return dict;
}


//void
//StringDictionaryMMPFCDAC::decodeNextString(uchar **ptr, uint lenPrefix, uchar *str, uint *strLen)
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
//StringDictionaryMMPFCDAC::searchPrefix(uchar **ptr, uint scanneable, uchar *decoded, uint *decLen, uchar *str, uint strLen)
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
//StringDictionaryMMPFCDAC::searchDistinctPrefix(uchar *ptr, uint scanneable, uchar *decoded, uint *decLen, uchar *str, uint strLen)
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

void StringDictionaryMMPFCDAC::printInfo() {
	printf("%d %d %d\n", this->elements, this->bytesStrings, this->realElements);
    printf("******** MMPFC Size ********\n");
    printf("LLCP -> %lu\n", this->llcp->getSize());
    printf("RLCP -> %lu\n", this->rlcp->getSize());
    printf("strings (size of not common characters): %lu\n", this->bytesStrings);
    printf("pointers (plain): %lu\n", (this->realElements+1)*sizeof(uint));
//    printf("XBWInt -> %lu\n", this->xbw->getSize());
//    printf("----------------------------\n");
    printf("MMPFC structure: %lu\n", sizeof(StringDictionaryMMPFCDAC));
    printf("TOTAL  -> %lu\n", this->getSize());
//    printf("****************************\n");
//    printf("\n\n");
//
//    this->xbw->printInfo();
//    this->rp->printInfo();
}

StringDictionaryMMPFCDAC::~StringDictionaryMMPFCDAC()
{
	delete [] textStrings; delete llcp; delete rlcp;
}



void StringDictionaryMMPFCDAC::buildLcps(uchar *dict, uint *pointers, vector<size_t>*llcp, vector<size_t>*rlcp, uint l, uint r) {
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

inline int StringDictionaryMMPFCDAC::doSearchString(uchar * string, uint len, uint lpos, uint rpos, uint l, uint r) {
    uint m = (lpos + rpos) / 2;
    uint foo;

//    printf("%s, %d %d %d %d %d\n", string, len, lpos, rpos, l, r);
    int chooseLeft = false;
    if (l == r) {
    	uint ll = llcp->getField(m);
    	uint rl = rlcp->getField(m);
    	if (ll > rl) chooseLeft = true;
    }
    if (l > r || (l==r && chooseLeft) ) {
    	uint llcpvalue = llcp->getField(m);
        if (llcpvalue > l) {
            return doSearchString(string, len, m, rpos, l, r);
        } else if (llcpvalue == l) {
        	uchar * dictm = this->textStrings + this->pointers[m];
        	uint p = l;
            int cmp = compareNewPattern(string, dictm -p, &p);
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
    } else {
    	uint rlcpvalue = rlcp->getField(m);
        if (rlcpvalue > r) {
            return doSearchString(string, len, lpos, m, l, r);
        } else if (rlcpvalue == r) {
        	uchar * dictm = this->textStrings + this->pointers[m];
        	uint p = r;
            int cmp = compareNewPattern(string, dictm -p, &p);
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
            return doSearchString(string, len, m, rpos, rlcpvalue, r);
        }
    }
}

inline int StringDictionaryMMPFCDAC::compareNewPattern(uchar *pattern, uchar *compared, uint *pos) {
	uint p=*pos;
	while(compared[p] && compared[p] == pattern[p]) {
		p++;
	}
	*pos = p;
	return pattern[p] - compared[p];
}


