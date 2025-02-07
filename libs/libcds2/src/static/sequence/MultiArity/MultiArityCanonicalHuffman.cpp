//
// Created by alberto on 4/21/15.
//

#include <sequence/MultiArity/MultiArityCanonicalHuffman.h>
#include <coders/MultiAryHuffman.h>

namespace cds_static {

    MultiArityCanonincalHuffman::MultiArityCanonincalHuffman(const uint *input, const size_t len, uint _arity):
            MultiArityCoder(),
            arity(_arity) {
        vector<uint> _input(len);
        for (size_t i=0;i<len;i++)
            _input[i] = input[i];
        InternalConstructor(_input);
    }

    MultiArityCanonincalHuffman::MultiArityCanonincalHuffman(const vector<uint> &input,
                                                             uint _arity) : MultiArityCoder(),
                                                              arity(_arity) {
        InternalConstructor(input);
    }

    void MultiArityCanonincalHuffman::InternalConstructor(const vector<uint> &input) {
        if (arity<2 || ((1UL<<(nbits(arity)-1)) != arity)) {
            cerr << "Arity must be a power of 2 >=2! (The parameter given is " << arity << ")" << endl;
            throw runtime_error("Arity must be a power of 2 >=2! (The parameter given is " +
                                [](uint arity){stringstream ss;ss << arity;return ss.str();}(arity)+ ")");
        }
        if (input.size()==0){
            return;
        }
        mask_arity = arity-1;
        bits_arity = bits(mask_arity);
        auto max_v = input[0];
        for (const auto &it: input)
            max_v = max(max_v, it);
        vector<pair<ulong, ulong>> symbol_freqs(max_v + 1);
        ulong id = 0;
        for (auto &it: symbol_freqs) {
            it.first = id++;
            it.second = 0UL;
        }
        for (const auto &it: input)
            symbol_freqs[it].second++;
        sort(symbol_freqs.begin(),symbol_freqs.end(),[](const pair<ulong,ulong> &a, const pair<ulong,ulong> &b){return a.second<b.second;});
        MultiAryHuffman<ulong,uint> mah(symbol_freqs, arity);

        codes.reserve(max_v + 1);
        for (ulong i = 0; i <= max_v; i++) {
            mah.GetCodeAndLength(i, codes[i].first, codes[i].second);
            codes[i].first = invertWord(codes[i].first)>>(W-(codes[i].second*bits_arity));
        }
    }
    MultiArityCanonincalHuffman::~MultiArityCanonincalHuffman() {

    }

    uint MultiArityCanonincalHuffman::get_chunk(const uint symbol, const uint level) const {
        return (codes[symbol].first >> (level * bits_arity)) & mask_arity;
    }

    uint MultiArityCanonincalHuffman::get_max_level(const uint symbol) const {
        return codes[symbol].second;
        //return codes[symbol].second/bits_arity+((codes[symbol].second%bits_arity)?1:0);
    }

    void MultiArityCanonincalHuffman::save(ofstream &fp) const {

    }

    MultiArityCanonincalHuffman *MultiArityCanonincalHuffman::load(ifstream &fp) {
        return nullptr;
    }

    MultiArityCanonincalHuffman::MultiArityCanonincalHuffman():MultiArityCoder(),arity(0),mask_arity(0) {

    }
};