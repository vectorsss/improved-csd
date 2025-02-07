/*  
 * Copyright (C) 2013-current-year, Alberto Ordóñez, all rights reserved.
 *
 * Alberto Ordóñez <alberto.ordonez@udc.es>
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

#include <direct_access/DirectAccess.h>

namespace cds_static{
	

DirectAccess::DirectAccess(){}

DirectAccess * DirectAccess::load(ifstream &fp){
	uint r = loadValue<uint>(fp);
	size_t pos = fp.tellg();
		fp.seekg(pos-sizeof(uint));
		switch(r) {
			case DAC_HDR: return DAC::load(fp);
			case DAC_INC_HDR: return DACInc::load(fp);
			case ARRAY_DA_HDR: return new ArrayDA(fp);
			// case DAC_E_HDR: return DAC::load(fp);
		}
		return NULL;
}

size_t DirectAccess::getLength() const{
	return len;
}
	
size_t DirectAccess::getRealLength() const{
	if (allEqual){
		return this->getLength() - 1;
	} else {
		return this->getLength();
	}
}


uint * DirectAccess::optimizationk(uint * list,size_t listLength, int * nkvalues){
	if (listLength==975)
        cerr << endl;
	int t,m,k;
		
	uint maxInt=0;
	
	for(size_t i=0;i<listLength;i++)
		if(maxInt<list[i])
			maxInt = list[i];

    if (maxInt==(uint)-1){
        cerr << "Max value in the input for optimizationk is -1!" << endl;
    }
	uint nBits = bits(maxInt)-1;
	
	uint tamAux = nBits+2;
	uint * weight=NULL;
	try{
		weight = new uint[maxInt+1];//(uint *) malloc(sizeof(uint)*(maxInt+1));
	}catch (std::bad_alloc& ba){
    	std::cerr << "bad_alloc caught: " << ba.what() << "trying to allocate " << maxInt+1 << " ints" << endl;
   	}


	for(size_t i=0;i<=maxInt;i++)
		weight[i]=0;

	for(size_t i=0;i<listLength;i++)
		weight[list[i]]++;
	
	
	size_t * acumFreq = new size_t[tamAux];//(uint *) malloc(sizeof(uint)*tamAux);
	
	uint acumValue = 0;
	
	acumFreq[0]=0;	
	acumValue = 0;
	uint cntb = 1;
	for(size_t i=0;i<=maxInt;i++){
		if(i==(1<<cntb)){
			acumFreq[cntb]=acumValue;
			cntb++;
		}
			
		acumValue += weight[i];
		
	}
	delete [] weight;

	acumFreq[cntb]=listLength;
	
	long * s = new long[max(1u,nBits+1)];//(long *) malloc(sizeof(long)*(nBits+1));

	uint * l = new uint[max(1u,nBits+1)];//(uint *) malloc(sizeof(uint)*(nBits+1));
	uint * b = new uint[max(1u,nBits+1)];//(uint *) malloc(sizeof(uint*)*(nBits+1));


	ulong currentSize;
	
	m=nBits;

	s[m]=0;
	l[m]=0;
	b[m]=0;
		
	size_t * fc = acumFreq;
	
	ulong minSize;
	uint minPos;
	for(t=m;t>=0;t--){
		minSize=-1;
		minPos=m;
		for(size_t i=m;i>=t+1;i--){
			currentSize = s[i]+(fc[m+1]-fc[t])*(i-t+1)+(fc[m+1]-fc[t])/FACT_RANK;
			if(minSize>currentSize){
				minSize = currentSize;
				minPos=i;
			}
		}

		if(minSize < ((fc[m+1]-fc[t])*(m-t+1))){
			s[t]=minSize;
			l[t]=l[minPos]+1;
			b[t]=minPos-t;
			
		}
	
		else{
			s[t]=(fc[m+1]-fc[t])*(m-t+1);
			l[t]=1;
			b[t]=m-t+1;				
			
		}
		
	}
	
	
	int L = l[0];
	if (L<0) L=1;
	
	uint *kvalues = new uint[L];//(ushort*)malloc(sizeof(ushort)*L);
	
	t=0;
	for(k=0;k< L;k++){
		kvalues[k]=b[t];
		t = t+b[t];
	}
	
	//TODO uncoment this lines
	delete [] acumFreq;
	delete [] l;
	delete [] b;
	delete [] s;
	
	(*nkvalues)=L;
	return kvalues;

	
}

// Implemented by Fernando Silva
uint * DirectAccess::optimizationk(uint *list, size_t listLength, int *nkvalues,
																   uint Lmax) {
		if (listLength==975)
			cerr << endl;
		long t,m,k;

		uint maxInt=0;

		for(size_t i=0;i<listLength;i++)
			if(maxInt<list[i])
				maxInt = list[i];

		if (maxInt==(uint)-1){
			cerr << "Max value in the input for optimizationk is -1!" << endl;
		}


		uint nBits = bits(maxInt)-1;

		uint tamAux = nBits+2;

//		uint * weight = (uint *) malloc(sizeof(uint)*(maxInt+1));
		uint * weight=NULL;
		try{
			weight = new uint[maxInt+1];//(uint *) malloc(sizeof(uint)*(maxInt+1));
		}catch (std::bad_alloc& ba){
			std::cerr << "bad_alloc caught: " << ba.what() << "trying to allocate " << maxInt+1 << " ints" << endl;
		}


		for(size_t i=0;i<maxInt+1;i++)
			weight[i]=0;

		for(size_t i=0;i<listLength;i++)
			weight[list[i]]++;


		size_t * acumFreq = (size_t *) malloc(sizeof(size_t)*tamAux);

		size_t acumValue = 0;
//		for(size_t i=0;i<10;i++){
//			acumValue += weight[i];
//
//		}

		acumFreq[0]=0;
		acumValue = 0;
		uint cntb = 1;
		for(size_t i=0;i<maxInt+1;i++){

			if(i==(1<<cntb)){
				acumFreq[cntb]=acumValue;

				cntb++;
			}

			acumValue += weight[i];

		}
		delete[]weight;

		acumFreq[cntb]=listLength;


		long ** s = (long **) malloc(sizeof(long*)*(nBits+2));


		uint ** l = (uint **) malloc(sizeof(uint*)*(nBits+2));
		uint ** b = (uint **) malloc(sizeof(uint*)*(nBits+2));

//		long * s = new long[max(1u,nBits+2)];//(long *) malloc(sizeof(long)*(nBits+1));
//
//		uint * l = new uint[max(1u,nBits+2)];//(uint *) malloc(sizeof(uint)*(nBits+1));
//		uint * b = new uint[max(1u,nBits+2)];//(uint *) malloc(sizeof(uint*)*(nBits+1));


		m=nBits;

		int v;

		for(t=0;t<=m+1;t++){
			s[t]=(long *) malloc(sizeof(long)*(Lmax+1));
			b[t] = (uint *) malloc(sizeof(uint)*(Lmax+1));
			l[t] = (uint *) malloc(sizeof(uint)*(Lmax+1));
			for(v=0;v<=Lmax;v++){
				s[t][v]=0;
				l[t][v]=0;
				b[t][v]=0;
			}
		}


		size_t * fc = acumFreq;
		rec_optim(l,b,s,0,Lmax,m,fc);


		int L = l[0][Lmax];

		uint *kvalues = new uint[L];

		t=0;
		for(k=0;k< L;k++){
			kvalues[k]=b[t][Lmax-k];
			t = t+b[t][Lmax-k];
		}
		free(acumFreq);

		for(t=0;t<=m+1;t++){
			free(s[t]);
			free(b[t]);
			free(l[t]);
		}
		free(s);
		free(b);
		free(l);

		(*nkvalues)=L;

		return kvalues;
	}

	void DirectAccess::rec_optim(uint **l, uint **b, long **s, uint t, uint v, uint m, size_t *fc){
		size_t minSize=-1,currentSize;
		size_t minPos=m;
		size_t i;

		if(v>1){
			for(i=m;i>=t+1;i--){
				if(l[i][v-1]==0)
					rec_optim(l,b,s,i,v-1,m,fc);
				currentSize = s[i][v-1]+(fc[m+1]-fc[t])*(i-t+1)+(fc[m+1]-fc[t])/FACT_RANK;

				if(minSize>currentSize){
					minSize = currentSize;
					minPos=i;
				}
			}

			if(minSize < ((fc[m+1]-fc[t])*(m-t+1))){
				s[t][v]=minSize;
				l[t][v]=l[minPos][v-1]+1;
				b[t][v]=minPos-t;

			}

			else{
				s[t][v]=(fc[m+1]-fc[t])*(m-t+1);
				l[t][v]=1;
				b[t][v]=m-t+1;

			}
		}

		else{

			s[t][v]=(fc[m+1]-fc[t])*(m-t+1);
			l[t][v]=1;
			b[t][v]=m-t+1;

		}

	}
	
}
