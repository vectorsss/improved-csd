#include <direct_access/DAC.h>

namespace cds_static{

//TODO correct this for 64 bits
DAC::DAC(uint *input,size_t list_length, uint Lmax, bool del):DirectAccess(list_length){
	if (list_length == 0){
		len = list_length;
		return;
	}

	bool swap=(list_length==1);
	uint *newin;
	if (list_length==1){
		cerr << "Warning: In constructor DAC(input,len,del), len<=1. This will cause problems" << endl;
		//abort();
		 newin = new uint[W];
		auto i = 0;
		for (;i<list_length;i++)
			newin[i]=input[i];
		for (;i<W-1;i++)
			newin[i]=newin[list_length-1];
		newin[W-1]=newin[W-2];
		uint *tmp = input;
		input = newin;
		newin = tmp;
	}

	ulong *levelSizeAux;
	ulong *cont;
	ulong *contB;
	
	uint* kvalues;
	int nkvalues;
	
	register size_t i;
	long j, k;
	uint value, newvalue;
	size_t bits_BS_len = 0;

	allEqual = true;
	for (size_t i=1;i<len;i++){
		if (input[i]!=input[i-1]){
			allEqual = false;
			break;
		}
	}
	uint *list;
	if (del) list=input;
	if (!del || allEqual){
		list = new uint[len+1];
		for (size_t i=0;i<len;i++){
			list[i]=input[i];
			assert(list[i]>=0);
		}
		if (allEqual){
			list[len] = list[len-1]+1;
            len++;
		}
	}

    if (Lmax > 0) {
        kvalues = optimizationk(list, len, &nkvalues, Lmax);
    } else {
        kvalues = optimizationk(list, len, &nkvalues);
    }


//    for (uint t = 0; t < nkvalues; t++){
//        printf("Level %u -> %u bits\n", t, kvalues[t]);
//    }


	uint kval;
	uint oldval =0;
	ulong newval =0;

	i=0;
	uint multval=1;
	do{
		oldval=newval;
		if(i>=nkvalues){
			kval = 1<<(kvalues[nkvalues-1]);
		}
		else
			kval=1<<(kvalues[i]);	
		multval*=kval;
		newval = oldval+multval;

		i++;
	}
	while(oldval<newval);

	tamtablebase = i;
	tablebase = new uint[tamtablebase];//(uint *) malloc(sizeof(uint)*tamtablebase);
	levelSizeAux = new ulong[tamtablebase];//(uint *) malloc(sizeof(uint)*tamtablebase);
	cont = new ulong[tamtablebase];//(uint *) malloc(sizeof(uint)*tamtablebase);
	contB = new ulong[tamtablebase];//(uint *) malloc(sizeof(uint)*tamtablebase);

	oldval =0;
	newval =0;
	multval=1;	
	for(i=0;i<tamtablebase;i++){
		oldval=newval;
		if(i>=nkvalues){
			kval = 1<<(kvalues[nkvalues-1]);
		}
		else
			kval=1<<(kvalues[i]);	
		multval*=kval;
		newval = oldval+multval;
		tablebase[i]=oldval;
	}	
	
	
	for(i=0;i<tamtablebase;i++){
		levelSizeAux[i]=0;

	}


	for (i=0;i<len;i++){
		value = list[i];
		for(j=0;j<tamtablebase;j++)
			if(value>=tablebase[j])
				levelSizeAux[j]++;
	}

	j=0;

	while((j<tamtablebase)&&(levelSizeAux[j]!=0)){
		 j++;
		}
	nLevels = j;

	levelsIndex = new ulong[nLevels+1];//(uint *) malloc(sizeof(uint)*(nLevels+1));
	bits_BS_len =0;
	
	base = new uint[nLevels];//(uint *)malloc(sizeof(uint)*nLevels);
	base_bits = new ushort[nLevels];//(ushort *)malloc(sizeof(ushort)*nLevels);
	
	for(i=0;i<nLevels;i++){
			if(i>=nkvalues){
				base[i]=1<<(kvalues[nkvalues-1]);
				base_bits[i]=kvalues[nkvalues-1];
			}
		else{
			base[i]=1<<(kvalues[i]);
			base_bits[i]=kvalues[i];
		}
	}

	ulong tamLevels =0;
		

	tamLevels=0;
	for(i=0;i<nLevels;i++)
		tamLevels+=base_bits[i]*levelSizeAux[i];

	iniLevel = new ulong[nLevels];//(uint *)malloc(sizeof(uint)*nLevels);
	tamCode=tamLevels;
	
	ulong indexLevel=0;
	levelsIndex[0]=0;
	for(j=0;j<nLevels;j++){
		levelsIndex[j+1]=levelsIndex[j] + levelSizeAux[j];
			iniLevel[j] = indexLevel;
			cont[j]=iniLevel[j];
			indexLevel+=levelSizeAux[j]*base_bits[j];
		contB[j]=levelsIndex[j];

	}


	levels = new uint[(tamLevels/W+1)];//(uint *) malloc(sizeof(uint)*(tamLevels/W+1));

	bits_BS_len = levelsIndex[nLevels-1]+1; 

	uint * bits_BS = new uint[(bits_BS_len/W+1)];//(uint *) malloc(sizeof(uint)*(bits_BS_len/W+1));
	for(i=0; i<((bits_BS_len)/W+1);i++)
		bits_BS[i]=0;
	for(i=0;i<len;i++){
		value = list[i];
		j=nLevels-1;

		while(j>=0){
			if(value >= tablebase[j]){

				newvalue = value- tablebase[j];

				for(k=0;k<j;k++){


					bitwriteL(levels,cont[k],base_bits[k],newvalue%base[k]);
					cont[k]+=base_bits[k];
					contB[k]++;

					newvalue = newvalue/base[k];
				}
				k=j;

					bitwriteL(levels,cont[j],base_bits[j],newvalue%base[j]);
					cont[j]+=base_bits[j];
					contB[j]++;
				if(j<nLevels-1){
					bit_set(bits_BS, contB[j] - 1);

				}
									
				break;
							}
			j--;
		}

		
	}
	if (!del || allEqual){
		delete [] list;
	}

		bit_set(bits_BS, bits_BS_len - 1);

	bS = new BitSequenceRG(bits_BS, bits_BS_len , 20);

	delete [] bits_BS;


	
	rankLevels = new ulong[nLevels];//(uint *) malloc(sizeof(uint)*nLevels);
	this->iteratorIndex = new uint[nLevels];//(uint *) malloc(sizeof(uint)*nLevels);
	for(j=0;j<nLevels;j++) {
		rankLevels[j] = bS->rank1(levelsIndex[j] - 1);
		this->iteratorIndex[j] = -1;
	}

	if (swap){
		delete [] input;
		input = newin;
	}
	delete [] cont;
	delete [] contB;
	delete [] levelSizeAux;
	delete [] kvalues;

}
DAC::~DAC(){
	if (len==0) return;
	delete [] levelsIndex;
	delete [] iniLevel;
	delete [] rankLevels;
	delete [] tablebase;
	delete [] levels;
	delete     bS;
	delete [] base;
	delete [] base_bits;
    free(iteratorIndex);
}

uint DAC::access(size_t param) const{

 uint mult=0;
 register long j;
 uint partialSum=0;
 ulong ini = param;
 uint * level;
 uint readByte;
 ulong cont,pos, rankini;

        for(uint i=0; i<this->nLevels; i++) {
            this->iteratorIndex[i] = -1;
        }
        this->iteratorIndex[0]=ini;

		partialSum=0;
		j=0;
		level=levels ;

		pos=levelsIndex[j]+ini;
			
			mult=0;

		cont = iniLevel[j]+ini*base_bits[j];


		readByte = bitreadL(level,cont,base_bits[j]);
		 if(nLevels == 1){
           return readByte;
		}
       
		while((!bS->access(pos))){
			rankini = bS->rank1(levelsIndex[j]+ini-1) - rankLevels[j];
			ini = ini-rankini;
            this->iteratorIndex[j+1]=ini;
			partialSum = partialSum+ (readByte<<mult);
			mult+=base_bits[j];
			j++;
			cont = iniLevel[j]+ini*base_bits[j];
			pos=levelsIndex[j]+ini;
			readByte = bitreadL(level,cont,base_bits[j]);
			if(j==nLevels-1){
				break;
			}
		}

		partialSum = partialSum + (readByte<<mult) + tablebase[j];

	return partialSum;

}

    // Implemented by Susana Ladra and translated to C++ by Fernando Silva
	uint DAC::next() {

        uint mult=0;
        register uint j;
        uint partialSum=0;
        uint nLevels=this->nLevels;
        uint * level;
        uint readByte;
        ulong cont,pos, rankini;

        uint ini;

        partialSum=0;
        j=0;
        level=this->levels ;

        this->iteratorIndex[j]++;
        ini = this->iteratorIndex[j];

        pos=this->levelsIndex[j]+ini;

        this->iteratorIndex[j]=pos;

        mult=0;
        cont = this->iniLevel[j]+ini*this->base_bits[j];
        readByte = bitreadL(level,cont,this->base_bits[j]);

        if(nLevels == 1){
            return readByte;
        }

        while((!this->bS->access(pos))){
            if(this->iteratorIndex[j+1]==-1)
            {
                rankini = this->bS->rank1(this->levelsIndex[j]+ini-1) - this->rankLevels[j];
                ini = ini-rankini;
                this->iteratorIndex[j+1] = ini;
            }
            else{
                this->iteratorIndex[j+1]++;
                ini = this->iteratorIndex[j+1];
            }


            partialSum = partialSum+ (readByte<<mult);

            mult+=this->base_bits[j];
            j++;

            cont = this->iniLevel[j]+ini*this->base_bits[j];
            pos=this->levelsIndex[j]+ini;

            readByte = bitreadL(level,cont,this->base_bits[j]);

            if(j==nLevels-1){
                break;
            }
        }

        partialSum = partialSum + (readByte<<mult) + this->tablebase[j];
        return partialSum;
	}

size_t DAC::getSize() const{
	if (len==0){
		return sizeof(this);
	}
	return sizeof(uint)*tamtablebase 
	+ sizeof(ushort)*nLevels
	+ sizeof(ushort)*nLevels
	+ sizeof(uint)*(nLevels+1)
	+ sizeof(ulong)*(nLevels)
	+ sizeof(uint)*(nLevels)
	+ sizeof(uint)*(tamCode/W+1)
	+ bS->getSize()
    + sizeof(bool); // allEqual
}

DAC::DAC():DirectAccess(){}

void DAC::save(ofstream &fp) const{
	//saveValue(fp,listLength);
	uint wr = DAC_HDR;
	saveValue(fp, wr);
	saveValue(fp, len);//uint
	if (len==0){
		return;
	}
    saveValue(fp, allEqual); // bool
	saveValue(fp, nLevels);//uchar
	saveValue(fp, tamCode);//ulong
	saveValue(fp, tamtablebase);//uint
	saveValue(fp, tablebase, tamtablebase);
	saveValue(fp, base_bits, nLevels);
	saveValue(fp, base, nLevels);
	saveValue(fp, levelsIndex, nLevels+1);
	saveValue(fp, iniLevel, nLevels);
	saveValue(fp, rankLevels, nLevels);
	saveValue(fp, levels,tamCode/W+1);
	bS->save(fp);
}

DAC *DAC::load(ifstream &fp){
	DAC *d = NULL;

	try {
        d = new DAC();
        uint rd = loadValue<uint>(fp);
        if (rd != DAC_HDR) {
            abort();
        }
        d->len = loadValue<size_t>(fp);
        if (d->len == 0)
            return d;
        d->allEqual = loadValue<bool>(fp);
        d->nLevels = loadValue<uchar>(fp);
        d->tamCode = loadValue<ulong>(fp);
        d->tamtablebase = loadValue<uint>(fp);
        d->tablebase = loadValue<uint>(fp, d->tamtablebase);
        d->base_bits = loadValue<ushort>(fp, d->nLevels);
        d->base = loadValue<uint>(fp, d->nLevels);
        d->levelsIndex = loadValue<ulong>(fp, d->nLevels + 1);
        d->iniLevel = loadValue<ulong>(fp, d->nLevels);
        d->rankLevels = loadValue<ulong>(fp, d->nLevels);
        d->levels = loadValue<uint>(fp, d->tamCode / W + 1);
        d->bS = BitSequence::load(fp);

        d->iteratorIndex = (uint *) malloc(sizeof(uint) * d->nLevels);
        for (uint j = 0; j < d->nLevels; j++) {
            d->iteratorIndex[j] = -1;
        }

	}catch(...){
		return NULL;
	}	
	return d; 
}

}
