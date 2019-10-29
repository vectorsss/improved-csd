/* RePair.cpp
 * Copyright (C) 2014, Francisco Claude & Rodrigo Canovas & Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * This class comprises some utilities for RePair compression and decompression.
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

#include "RePair.h"

RePair::RePair()
{
	this->G = NULL;
	this->Cls = NULL;
	this->Cdac = NULL;
	this->maxchar = 0;
}

RePair::RePair(int *sequence, uint length, uchar maxchar)
{
	this->G = NULL;
	this->Cls = NULL;
	this->Cdac = NULL;
	this->maxchar = maxchar;

	Tdiccarray *dicc;
	IRePair compressor;

	compressor.compress(sequence, length, (size_t*)&terminals, (size_t*)&rules, &dicc);

	// Building the array for the dictionary
	G = new LogSequence(bits(rules+terminals), 2*rules);

	for (uint i=0; i<rules; i++)
	{
		G->setField(2*i, dicc->rules[i].rule.left);
		G->setField((2*i)+1, dicc->rules[i].rule.right);
	}
	Dictionary::destroyDicc(dicc);
}

uint
RePair::expandRule(uint rule, uchar* str)
{
	uint pos = 0;
	uint left = G->getField(2*rule);
	uint right = G->getField((2*rule)+1);

	if (left >= terminals) pos += expandRule(left-terminals, str+pos);
	else { str[pos] = (uchar)left; pos++; }

	if (right >= terminals) pos += expandRule(right-terminals, str+pos);
	else { str[pos] = (uchar)right; pos++; }

	return pos;
}

uint
RePair::expandRuleLimit(uint rule, uchar* str, uint limit)
{
	uint pos = 0;
	uint left = G->getField(2*rule);
	uint right = G->getField((2*rule)+1);

	if (left >= terminals) pos += expandRuleLimit(left-terminals, str+pos, limit);
	else { str[pos] = (uchar)left; pos++; }
	if (pos < limit) {
		if (right >= terminals) pos += expandRuleLimit(right-terminals, str+pos, limit - pos);
		else { str[pos] = (uchar)right; pos++; }
	}

	return pos;
}
int
RePair::expandRuleAndCompareString(uint rule, uchar *str, uint *pos)
{
	int cmp = 0;

//	printf("MMM%d %s %d\n", rule, str, *pos);

	uint left = G->getField(2*rule);
	if (left >= terminals)
	{
		cmp = expandRuleAndCompareString(left-terminals, str, pos);
//		printf("(l)%d", cmp);
		if (cmp != 0) return cmp;
	}
	else
	{
//		printf("l[%c]", (uchar)left);
		if ((uchar)left != str[*pos]) return (int)((uchar)left-str[*pos]);
		(*pos)++;
	}

	uint right = G->getField((2*rule)+1);
	if (right >= terminals)
	{
		cmp = expandRuleAndCompareString(right-terminals, str, pos);
//		printf("(r)%d", cmp);
		if (cmp != 0) return cmp;
	}
	else
	{
//		printf("r[%c]", (uchar)right);
		if ((uchar)right != str[*pos]) return (int)((uchar)right-str[*pos]);
		(*pos)++;
	}

	return cmp;
}

int
RePair::expandRuleAndCompareStringLimit(uint rule, uchar *str, uint *pos, int limit)
{
	int cmp = 0;

	uint left = G->getField(2*rule);
	if (left >= terminals)
	{
//			printf("lexpanding %d\n", left);
		cmp = expandRuleAndCompareStringLimit(left-terminals, str, pos, limit);
		if (cmp != 0) return cmp;
	}
	else
	{
//		printf("[%c]", (char) left);
		if ((uchar)left != str[*pos]) return (int)((uchar)left-str[*pos]);
		(*pos)++;
	}

	if (*pos >= limit) return 1;

	uint right = G->getField((2*rule)+1);
	if (right >= terminals)
	{
//		printf("rexpanding %d\n", right);
		cmp = expandRuleAndCompareStringLimit(right-terminals, str, pos, limit);
		if (cmp != 0) return cmp;
	}
	else
	{
//		printf("[%c]", (char) left);
		if ((uchar)right != str[*pos]) return (int)((uchar)right-str[*pos]);
		(*pos)++;
	}

	if (*pos >= limit) return 1;

	return cmp;
}


int
RePair::extractStringAndCompareRP(uint id, uchar* str, uint strLen)
{
	str[strLen] = maxchar;

	uint l = 0, pos = 0, next;
	int cmp = 0;

	while (pos <= strLen)
	{
		next = Cls->getField(id+l);

		if  (next >= terminals)
		{
			cmp = expandRuleAndCompareString(next-terminals, str, &pos);
			if (cmp != 0) break;
		}
		else
		{
			if ((uchar)next != str[pos]) return (int)((uchar)next-str[pos]);
			pos++;
		}

		l++;
	}

	str[strLen] = 0;

	return cmp;
}

int
RePair::extractStringAndCompareDAC(uint id, uchar* str, uint strLen)
{
	uint l = 0, pos = 0, next;
	int cmp = 0;

	while(id != (uint)-1)
	{
		next = Cdac->access_next(l, &id);

		if (next >= terminals)
		{
			cmp = expandRuleAndCompareString(next-terminals, str, &pos);
			if (cmp != 0) return cmp;
		}
		else
		{
			if ((uchar)next != str[pos]) return (int)((uchar)next-str[pos]);
			pos++;
		}

		l++;
	}

	if (pos == strLen) return cmp;
	else return -str[pos];
}

int
RePair::extractStringAndCompareDAC2(uint id, uchar* str, uint *pos)
{
	uint l = 0, next;
	int cmp = 0;

	while(id != (uint) -1)
	{
		next = Cdac->access_next(l, &id);

//		printf("rule = %d\n", next);


		if (next >= terminals)
		{
			cmp = -expandRuleAndCompareString(next-terminals, str, pos);
			if (cmp != 0) return cmp;
		}
		else
		{
			cmp = (int)(str[*pos]-(uchar)next);
			if (cmp != 0) {
				return cmp;
			}
			(*pos)++;
		}

		l++;

	}

	//If we arrive at the end the pattern must be greater than the entry.
//	printf("end at %d (%c)\n", *pos, str[*pos]);

	return str[*pos];
}

int
RePair::expandRuleAndComparePrefixDAC(uint rule, uchar *str, uint *pos)
{
	int cmp = 0;

	uint left = G->getField(2*rule);
	if (left >= terminals)
	{
		cmp = expandRuleAndComparePrefixDAC(left-terminals, str, pos);
		if (cmp != 0) return cmp;
	}
	else
	{
		if ((uchar)left != str[*pos]) return (int)((uchar)left-str[*pos]);
		(*pos)++;
	}

	if (str[*pos] == '\0') return cmp;

	uint right = G->getField((2*rule)+1);
	if (right >= terminals)
	{
		cmp = expandRuleAndComparePrefixDAC(right-terminals, str, pos);
		if (cmp != 0) return cmp;
	}
	else
	{
		if ((uchar)right != str[*pos]) return (int)((uchar)right-str[*pos]);
		(*pos)++;
	}

	return cmp;
}

int
RePair::extractPrefixAndCompareDAC(uint id, uchar* prefix, uint prefixLen)
{
	uint l = 0, pos = 0, next;
	int cmp = 0;

	while(id != (uint)-1)
	{
		next = Cdac->access_next(l, &id);

		if (next >= terminals)
		{
			cmp = expandRuleAndComparePrefixDAC(next-terminals, prefix, &pos);
			if (cmp != 0) return cmp;
		}
		else
		{
			if ((uchar)next != prefix[pos]) return (int)((uchar)next-prefix[pos]);
			pos++;
		}

		if (prefix[pos] == '\0') return 0;

		l++;
	}

	if (pos == prefixLen) return cmp;
	else return -prefix[pos];
}

void
RePair::save(ofstream &out,  uint encoding)
{
	saveValue<uchar>(out, maxchar);
	saveValue<uint64_t>(out, terminals);
	saveValue<uint64_t>(out, rules);
	G->save(out);

	saveValue<uint32_t>(out, encoding);

	// if (encoding == MMRPFCbSDDAC3) Cdac->save(out);
	// else Cls->save(out);
	Cls->save(out);
}

void
RePair::save(ofstream &out)
{
	saveValue<uchar>(out, maxchar);
	saveValue<uint64_t>(out, terminals);
	saveValue<uint64_t>(out, rules);
	G->save(out);
}

RePair*
RePair::load(ifstream &in)
{
	RePair *dict = new RePair();

	dict->maxchar = loadValue<uchar>(in);
	dict->terminals = loadValue<uint64_t>(in);
	dict->rules = loadValue<uint64_t>(in);
	dict->G = new LogSequence(in);

	uint encoding = loadValue<uint32_t>(in);

	// if (encoding == MMRPFCbSDDAC3) dict->Cdac = DAC_VLS::load(in);
	// else dict->Cls = new LogSequence(in);
	dict->Cls = new LogSequence(in);

	return dict;
}

RePair*
RePair::loadNoSeq(ifstream &in)
{
	RePair *dict = new RePair();

	dict->maxchar = loadValue<uchar>(in);
	dict->terminals = loadValue<uint64_t>(in);
	dict->rules = loadValue<uint64_t>(in);
	dict->G = new LogSequence(in);

	return dict;
}


size_t
RePair::getSize()
{
	if (Cdac != NULL) return G->getSize()+Cdac->getSize()+sizeof(RePair);
	if (Cls != NULL) return G->getSize()+Cls->getSize()+sizeof(RePair);
	return G->getSize()+sizeof(RePair);
}

RePair::~RePair()
{
	delete G;
	if (Cls != NULL) delete Cls;
	if (Cdac != NULL) delete Cdac;
}


void RePair::printRules() {
	for (uint i = 0; i < this->getNumOfRules(); i++) {
		uint left = G->getField(2*i);
		uint right = G->getField((2*i)+1);

		printf("%u -> (", i + this->getNumOfTernimals());
		if (left < this->getNumOfTernimals()) {
			printf("%c", (uchar)left);
		} else {
			printf("%u", left);
		}
		printf(", ");
		if (right < this->getNumOfTernimals()) {
			printf("%c", (uchar)right);
		} else {
			printf("%u", right);
		}
		printf(")\n");
	}
	return;
}

void RePair::printInfo() {
	printf("*** RePair info ***\n");
	printf("Number of terminal -> %lu\n", this->terminals);
	printf("Number of rules    -> %lu\n", this->rules);
	printf("MaxChar            -> %u\n", (uint)this->maxchar);

	printf("--- Size ---\n");
	printf("G      -> %lu \n", this->G->getSize());
	printf("Params -> %lu \n", sizeof(RePair));
    printf("------------\n");
	printf("Total  -> %lu \n", this->getSize());
	printf("------------\n");

	printf("*******************\n");
	return;
}
