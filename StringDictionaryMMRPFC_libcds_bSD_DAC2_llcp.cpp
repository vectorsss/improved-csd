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


#include "StringDictionaryMMRPFC_libcds_bSD_DAC2_llcp.h"

StringDictionaryMMRPFCbSDDAC2Llcp::StringDictionaryMMRPFCbSDDAC2Llcp()
{
	this->type = MMRPFCbSDDAC2LLCP;
	this->elements = 0;
	this->maxlength = 0;
}

StringDictionaryMMRPFCbSDDAC2Llcp::StringDictionaryMMRPFCbSDDAC2Llcp(IteratorDictString *it)
{

	this->type = MMRPFCbSDDAC2LLCP;
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
	vector<size_t> minlcp(this->realElements);

	llcp[0] = rlcp[0] = llcp[this->realElements-1] = rlcp[this->realElements-1] = 0;

	buildLcps(dict, pointers, &llcp, &rlcp, 0, (int) this->realElements-1);

	// Para MINlcp:
	//for (uint i = 0; i < this->realElements; i++) {
	//	minlcp[i] = (llcp[i] < rlcp[i])? llcp[i] : rlcp[i];
	//}


//	Block to actually remove characters unneded because of Front-Coding
	uchar *cdict = new uchar[processed];
	uint io = 0, ic = 0, nword = 0;
	uint maxseq = 0, currentseq = 0;
	int minRule = MAXINT;

	while (io<processed) {
		// PARA MINlcp: uint skip = llcp[nword] < rlcp[nword] ? llcp[nword] : rlcp[nword];
		uint skip = llcp[nword];
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



	// this->llcp = new LogSequence(&llcp, bits(maxlength));
	// this->rlcp = new LogSequence(&rlcp, bits(maxlength));

	// ******************* DAC para la codificación de R/Llcp
	// NOTA: se usa la versión que usa un número variable de bits por nivel, ya que nos interesa conseguir
	// la mayor compresión posible, dado que sabemos que vamos a perder más tiempo
	
	uint * foollcp = new uint[llcp.size()];
	for (int i = 0; i < llcp.size();  i++) {
		foollcp[i] = llcp[i];
	}

	DAC *dacllcp = new DAC(foollcp, llcp.size(), maxlength, false);


//	uint * foorlcp = new uint[rlcp.size()];
//	for (int i = 0; i < rlcp.size();  i++) {
//		foorlcp[i] = rlcp[i];
//	}
//
//	DAC *dacrlcp = new DAC(foorlcp, rlcp.size(), maxlength, false);

    // Para MINlcp:
	//uint * foominlcp = new uint[minlcp.size()];
	//for (int i = 0; i < minlcp.size();  i++) {
	//	foominlcp[i] = minlcp[i];
	//}

	//DAC *dacminlcp = new DAC(foominlcp, minlcp.size(), maxlength, false);


	this->llcp = dacllcp;

	//this->minlcp = dacminlcp;

	//std::cout << "Got " << this->minlcp->getSize() << " VS " << this->llcp->getSize() << " + " << this->rlcp->getSize() << std::endl;

	// ******************* DAC para la codificación de R/Llcp


	// Con Front Coding
	//this->textStrings = cdict;
	//this->bytesStrings = ic;

	/// ******************* REPAIR + DAC
	int* rpdict = new int[ic];

	// 1. Transform characters into integers
	for (size_t i = 0; i < ic; i++) {
		rpdict[i] = cdict[i];
	}

	//for (int i = 0; i < ic; i++) {
	//	fprintf(stderr, "rpdict[%d] = %d\n", i, rpdict[i]);
	//}


	// 2. Obtaining the Re-Pair encoding
	rp = new RePair(rpdict, ic, 255);
	bitsrp = rp->getBits();

	// 3. Clean negatives from encoded strings with RePair 
	// ic = cdict size 
	// io = current entry

	io = 0;
	vector<size_t> intStrings;		

	///nword = 0;
	///pointers[0] = 0;
	///nword++;

	while (io<ic)
	{
		if (rpdict[io] >= 0)
		{
			
			intStrings.push_back(rpdict[io]);
			///if (rpdict[io] == 0) {
			///	pointers[nword] = intStrings.size();
			///	nword++;
			///}
			io++; 
			
		}
		else
		{
			if (io < ic) io = -(rpdict[io]+1);
		}
	}

	//for (int i = 0; i < intStrings.size(); i++) {
	//	fprintf(stderr, "intStrings[%d] = %d\n", i, intStrings[i]);
	//}

	fprintf(stderr, "DICT in PLAIN:  %d\nDICT with REPAIR: intStrings.size (in bytes): %d\n", ic, intStrings.size()*4);
	
	// 4. 'pointers' update + codificacion DAC
	size_t bytesVByte = 0;
	size_t curSize = 0;
	
	nword = 0;
	pointers[0] = 0;
	nword++;
	
	maxseq = 0;
	for (int i = 0; i < intStrings.size();  i++) {
		if (intStrings[i] == 0) {
			pointers[nword] = i+1;
			nword++;
		}
	}

	
	/* DESCARTADA la alternativa DAC en lugar de VBYTE + bSD (porque utiliza un número fijo de bits por lo que ocupa más que VBYTE)
		   NOTA: La opción de usar DAC con el número óptimo de bits por nivel tampoco es aplicable porque el cálculo de ese valor se tiene
		   en cuenta por entero, y no por secuencia de enteros que constituyen un string, con lo que seguiríamos necesitando los punteros */
	

	
	uint * intStringsArray = new uint[intStrings.size()];
	for (int i = 0; i < intStrings.size();  i++) {
		intStringsArray[i] = intStrings[i];
	}

	this->textStrings = new DAC(intStringsArray, intStrings.size(), false);
	fprintf(stderr, "\tDAC_optimum SIZE!!!! %d\n", this->textStrings->getSize());

	// TEST
	//int rule;
	//for (int i = 1; i <= 20; i++) {
	//	int diff = pointers[i]-pointers[i-1];
	//	for (int j = 0; j < diff; j++) {
	//		fprintf(stderr, "%d %d: %d ", i, pointers[i-1], this->textStrings->access(pointers[i-1]+j));
	//	}
	//	fprintf(stderr, "\n");
	//}
    // TEST
	

	fprintf(stderr, "Num. words TOTAL: %d\n", nword-1);


	// ******************* REPAIR + DAC 

	int numIntegers = (intStrings.size() + 1)/W+1;
	uint *bitsPointers =  new uint[numIntegers];
	for (int i = 0; i < numIntegers; i++) {
		bitsPointers[i] = 0;
	}
	
	for (int i = 0; i <= this->realElements; i++) {
		cds_utils::bit_set(bitsPointers, pointers[i]);
	}
	
	this->selPointers = new BitSequenceSDArray(bitsPointers, intStrings.size() + 1);

	printInfo();
	
}


   

inline uint
StringDictionaryMMRPFCbSDDAC2Llcp::decodeString(uchar *str, uint pos, uint limit)
{
	
	uint rule;
 	uint strLen = 0;
 	
 	rule = textStrings->access(pos); 

	while (rule && (strLen < limit))
	{

		//printf("rule = %d\n", rule);
		//printf("strLen = %d\n", *strLen);
		if (rule >= rp->terminals) strLen += rp->expandRuleLimit(rule-rp->terminals, (str+strLen), limit-strLen);
		else
		{
			str[strLen] = (uchar)rule;
			strLen++;
		}
		rule = textStrings->next();
		//printf("strLen = %d\n", *strLen);
	}

	str[strLen] = 0;


	return 0;
}


uint 
StringDictionaryMMRPFCbSDDAC2Llcp::locate(uchar *str, uint strLen)
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
StringDictionaryMMRPFCbSDDAC2Llcp::extract(size_t id, uint *strLen)
{

	//printf("\n\nExtract(%d)\n", id);

	uint i, m;

	uchar * res = new uchar[maxlength];
	uchar * decoded = new uchar[maxlength]; 
	uchar * ptr;

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
		//printf("index: %d\n", index);
		//printf("ls[%d] = %d\n", depth, ls[depth]);
		//printf("rs[%d] = %d\n", depth, rs[depth]);
		int nextIndex = 0;
		int maxlcp = 0;
		int llcpValue = llcp->access(index);
		nextIndex = ls[depth];
		maxlcp = llcpValue;
		int i;
		// offset te dice desde donde has rellenado (tienes desde esa posición hasta el final)
		if (maxlcp < offset ) {
			int basepos = this->selPointers->select1(index+1);

			decodeString(decoded, basepos, offset - maxlcp);
			//printf("decoded: %s\n", decoded);

			// maxlcp te dice la posición desde donde tienes que rellenar
			// offset te dice la posición desde donde ya has cubierto

			// Ej.         2      3
			//           maxlcp offset
			//             |      |
			//             |      |
            //
			//        __   __    __    a    e
			
			// -- con lo cual tienes que rellenar la diferencia: offset - maxlcp (i.e. las
			// posiciones que haya entre ambos), y teniendo en cuenta que en decoded ya 
			// esta decodificado el string desde maxlcp, es por ello que la posición 0 de
			// decoded es en realidad la correspondiente a maxlcp del string original
			for (i = maxlcp; i < offset && decoded[i-maxlcp]; i++) {
				res[i] = decoded[i-maxlcp];
				//printf("res[%d] = %c\n", i, res[i]);
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
	
	decodeString(decoded, pos, offset);
	//printf("decoded: %s\n", decoded);
	for (i = 0; i < offset; i++) {
		res[i] = decoded[i];
	}
	res[*strLen]='\0';

	return res;
}

IteratorDictID*
StringDictionaryMMRPFCbSDDAC2Llcp::locatePrefix(uchar *str, uint strLen)
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
StringDictionaryMMRPFCbSDDAC2Llcp::locateSubstr(uchar *str, uint strLen)
{
	cerr << "This dictionary does not provide substring location" << endl;
	return NULL;
}

uint 
StringDictionaryMMRPFCbSDDAC2Llcp::locateRank(uint rank)
{
	return rank;
}

IteratorDictString*
StringDictionaryMMRPFCbSDDAC2Llcp::extractPrefix(uchar *str, uint strLen)
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
StringDictionaryMMRPFCbSDDAC2Llcp::extractTable()
{
	cerr << "Not supported (yet)" << endl;
	return 0;
}

IteratorDictString*
StringDictionaryMMRPFCbSDDAC2Llcp::extractSubstr(uchar *str, uint strLen)
{
	cerr << "This dictionary does not provide substring extraction" << endl;
	return 0; 
}

uchar *
StringDictionaryMMRPFCbSDDAC2Llcp::extractRank(uint rank, uint *strLen)
{
	return extract(rank, strLen);
}

size_t 
StringDictionaryMMRPFCbSDDAC2Llcp::getSize()
{
	return this->textStrings->getSize()+this->selPointers->getSize()+llcp->getSize()+sizeof(StringDictionaryMMRPFCbSDDAC2Llcp)+rp->getSize();
}

void 
StringDictionaryMMRPFCbSDDAC2Llcp::save(ofstream &out)
{
	saveValue<uint32_t>(out, type);
	saveValue<uint64_t>(out, elements);
	saveValue<uint32_t>(out, maxlength);
	//saveValue<uint64_t>(out, bytesStrings);
	//saveValue<uchar>(out, textStrings, bytesStrings);
	textStrings->save(out);
//	minlcp->save(out);
	llcp->save(out);
	selPointers->save(out);
	saveValue<uint32_t>(out, bitsrp);
	rp->save(out);
}



StringDictionary*
StringDictionaryMMRPFCbSDDAC2Llcp::load(ifstream &in)
{
	size_t type = loadValue<uint32_t>(in);
	if(type != MMRPFCbSDDAC2LLCP) return NULL;

	cout << "Reading MMRPFCbSDDAC2LLCP" <<endl;

	StringDictionaryMMRPFCbSDDAC2Llcp *dict = new StringDictionaryMMRPFCbSDDAC2Llcp();

	dict->type = MMRPFCbSDDAC2LLCP;
	dict->elements = loadValue<uint64_t>(in);
	dict->realElements = dict->elements + 2;
	dict->maxlength = loadValue<uint32_t>(in);
	//dict->bytesStrings = loadValue<uint64_t>(in);
	//dict->textStrings = loadValue<uchar>(in, dict->bytesStrings);
	//dict->llcp = new LogSequence(in);
	//dict->rlcp = new LogSequence(in);
	dict->textStrings = DAC::load(in);
//	dict->minlcp = DAC::load(in);
	dict->llcp = DAC::load(in);
	dict->selPointers = BitSequence::load(in);
	dict->bitsrp = loadValue<uint32_t>(in);
	dict->rp = RePair::loadNoSeq(in);

	dict->decoded = new uchar[dict->maxlength+2];

	return dict;
}


//void
//StringDictionaryMMRPFCbSDDAC2Llcp::decodeNextString(uchar **ptr, uint lenPrefix, uchar *str, uint *strLen)
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
//StringDictionaryMMRPFCbSDDAC2Llcp::searchPrefix(uchar **ptr, uint scanneable, uchar *decoded, uint *decLen, uchar *str, uint strLen)
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
//StringDictionaryMMRPFCbSDDAC2Llcp::searchDistinctPrefix(uchar *ptr, uint scanneable, uchar *decoded, uint *decLen, uchar *str, uint strLen)
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

void StringDictionaryMMRPFCbSDDAC2Llcp::printInfo() {
	//printf("%d %d %d\n", this->elements, this->bytesStrings, this->realElements);
    printf("******** MMPFC Size ********\n");
    printf("LLCP -> %lu\n", this->llcp->getSize());
//    printf("RLCP -> %lu\n", this->rlcp->getSize());

//    printf("MINLCP -> %lu\n", this->minlcp->getSize());
    printf("strings (size of not common characters): %lu\n", this->textStrings->getSize());
    printf("selPointers: %lu\n", this->selPointers->getSize());
    printf("repair: %lu\n", this->rp->getSize());
//    printf("XBWInt -> %lu\n", this->xbw->getSize());
//    printf("----------------------------\n");
    printf("MMPFC structure: %lu\n", sizeof(StringDictionaryMMRPFCbSDDAC2Llcp));
    printf("TOTAL  -> %lu\n", this->getSize());
//    printf("****************************\n");
//    printf("\n\n");
//
//    this->xbw->printInfo();
//    this->rp->printInfo();
}

StringDictionaryMMRPFCbSDDAC2Llcp::~StringDictionaryMMRPFCbSDDAC2Llcp()
{
	delete textStrings; delete llcp;  // delete minlcp;
}



void StringDictionaryMMRPFCbSDDAC2Llcp::buildLcps(uchar *dict, uint *pointers, vector<size_t>*llcp, vector<size_t>*rlcp, uint l, uint r) {
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


/*
inline uint
StringDictionaryMMRPFCbSDDAC2Llcp::decodeCompareString(uchar *ptr, uchar *pattern, uint *pos)
{
	
	uint rule;
	uint strLen = 0;
	uint cmp = 0;
	uint posCmp = *pos;
	uchar *str = this->decoded;

	// OJO! 128 es el código en VByte correspondiente al \0
	while (((int) *ptr) != 128)
	{
		ptr += VByte::decode(&rule, ptr); 

		//printf("rule = %d\n", rule);
		//printf("strLen = %d\n", *strLen);
		
		printf("comparando desde posición %d de %d\n", posCmp, strlen((char *)pattern));
		
		if (rule >= rp->terminals) { 
			strLen += rp->expandRule(rule-rp->terminals, (str+strLen));
			while (posCmp < strLen) {
				//printf("str[%d] = %c vs. pattern[%d] = %c\n", posCmp, str[posCmp], posCmp, pattern[posCmp]);
				if (str[posCmp] != pattern[posCmp]) {
					*pos = posCmp;
					return pattern[posCmp] - str[posCmp];
				}
				posCmp++;
			}
		}
		else
		{
			strLen++;
			if ((uchar) rule != pattern[posCmp]) {
				*pos = posCmp;
				return pattern[posCmp] - (uchar) rule;
			}
			posCmp++;

		}
		
		printf("igual hasta posición %d de %d\n", posCmp, strlen((char *)pattern));
	}


	*pos = strLen;
	return pattern[strLen];
}


*/

inline uint
StringDictionaryMMRPFCbSDDAC2Llcp::decodeCompareString(uint dictmPos, uchar *pattern, uint *pos)
{
	
	uint rule;
	uint strLen = 0;
	uint cmp = 0;
	uint posCmp = *pos;
	uchar *str = this->decoded;

	rule = textStrings->access(dictmPos);
	//printf("rule = %d\n", rule);

	while (rule)
	{
		
		//printf("rule = %d\n", rule);
		//printf("strLen = %d\n", strLen);
		

		//printf("comparando desde posición %d de %d\n", posCmp, strlen((char *)pattern));
		if (rule >= rp->terminals) { 
			cmp = -(rp->expandRuleAndCompareString(rule-rp->terminals, pattern, &posCmp));
			if (cmp != 0) {
				*pos = posCmp;
				return cmp;
			}
		} else {
			cmp = pattern[posCmp]-(uchar) rule;
			if (cmp != 0) {	
				*pos = posCmp;			
				return cmp;
			}
			posCmp++;
		}

		rule = textStrings->next();
		
		//printf("igual hasta posición %d de %d\n", posCmp, strlen((char *)pattern));
	}

	*pos = posCmp;
	return pattern[posCmp];
}

inline int 
StringDictionaryMMRPFCbSDDAC2Llcp::doSearchString(uchar * string, uint len, uint lpos, uint rpos, uint l, uint r) {

    uint m = (lpos + rpos) / 2;
    uint foo;
    uint decLen = 0;

    //printf("%s, %d %d %d %d %d\n", string, len, lpos, rpos, l, r);
    
    // Si tanto l como r son iguales (i.e. la parte que tiene en común el patrón con el extremo
    // izquierdo y derecho), podríamos escoger, a priori, tanto el escenario l>r o r>l, pero 
    // puede darse el caso de que escogiendo aleatoriamente uno de ellos, después no puedas
    // continuar porque en realidad almacenes menos caracteres de los que esperarías (ej.
    // si r=l=3 y te vas por el caso r>l, y resulta que Rlcp=3 y Llcp=4, tendrías que comparar
    // desde el caracter 3, pero en realidad tú sólo almacenas desde el 4, i.e. siempre es lo
    // que resta de cadena respecto de el Rlcp o Llcp con quien más caracteres comparta, con lo
    // que no podrías hacer la comparación esperada porque te faltan caracteres.) Solución: determinara
    // de antemano quien es mayor de entre Rlcp y Llcp y en caso de que r=l, escoger el escenario
    // correspondiente a ese lado


	uint llcpvalue = llcp->access(m);
	//printf("llcp = %d\n", llcpvalue);
	if (llcpvalue > l) {
		return doSearchString(string, len, m, rpos, l, r);
    } else if (llcpvalue == l) {

		uint dictmPos = this->selPointers->select1(m+1);
		//printf("%d\n", dictmPos);

		// p: primer caracter en el que difieren (a partir de la parte común)
		uint p = 0;
		int cmp = decodeCompareString(dictmPos, string+l, &p);
		p += l;

		//uint p = l;
		//decodeString(this->decoded, dictm, 5000);
		//int cmp = compareNewPattern(string, decoded-p, &p);
		//printf("cmp = %d     p = %d\n", cmp, p);
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

inline int StringDictionaryMMRPFCbSDDAC2Llcp::compareNewPattern(uchar *pattern, uchar *compared, uint *pos) {
	uint p=*pos;
	while(compared[p] && compared[p] == pattern[p]) {
		//printf("str[%d] = %c vs. pattern[%d] = %c\n", p, compared[p], p, pattern[p]);
		p++;
	}
	*pos = p;
	return pattern[p] - compared[p];
}


