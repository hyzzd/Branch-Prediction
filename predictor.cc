#include "predictor.h"

#define PHT_CTR_MAX  3
#define PHT_CTR_INIT 2

/////////////// STORAGE BUDGET JUSTIFICATION ////////////////
// Total storage budget: 32KB + 140 bits
// Total PHT counters: 2^17 
// Total PHT size = 2^17 * 2 bits/counter = 2^18 bits = 32KB
// GHR size: 140 bits
// Total Size = PHT size + GHR size
/////////////////////////////////////////////////////////////

//This code is derived from Temporal Stream Branch Predictor by Yongming Shen.
//In this code, my contribution to the performance is quite small.
//About Temporal Stream Predictor, please refer to the online paper:
//URL: http://www.jilp.org/cbp2014/paper/YongmingShen.pdf


//To finish this project, I read a lot of paper about branch prediction.
//Many algorithms and methods are very complicated although some have
//very good performances. As I saw the temporal stream branch predictor,
//the idea is very intelligent and simple. I like it very much.

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

PREDICTOR::PREDICTOR(void){

    ghr = 0;

    for(UINT32 ii = 0; ii < PHT_SIZE; ii++){
        pht[ii]=PHT_CTR_INIT; 
    }

    tstart = tempStream.end();
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

bool   PREDICTOR::GetPrediction(UINT32 PC){

    UINT32 phtIndex = (PC^ghr) % PHT_SIZE;
    UINT32 phtCounter = pht[phtIndex];
  
    if(phtCounter > PHT_CTR_MAX/2){
        primpred = TAKEN; 
    }else{
        primpred = NOT_TAKEN; 
    }

    if (tstart != tempStream.end()) {
        ++tstart;
        if (!*tstart) {
            return !primpred;
        }
    }
    return primpred;
}


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

bitset<TS_SIZE> PREDICTOR::ts_idx(UINT32 PC) {
    
    bitset<TS_SIZE> pc = bitset<TS_SIZE>(PC);
    return pc | (ts_ghr << 32);
}

void  PREDICTOR::UpdatePredictor(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget){


    UINT32 phtIndex   = (PC^ghr) % PHT_SIZE;
    UINT32 phtCounter = pht[phtIndex];

    // update the PHT

    if(resolveDir == TAKEN){
        pht[phtIndex] = SatIncrement(phtCounter, PHT_CTR_MAX);
    }else{
        pht[phtIndex] = SatDecrement(phtCounter);
    }

    // update the GHR
    ghr = (ghr << 1);

    if(resolveDir == TAKEN){
        ghr++; 
    }

    ts_ghr <<= 1;

    if(resolveDir == TAKEN){
        ts_ghr[0] = 1;
    }

    tempStream.push_back(primpred == resolveDir);

    if(resolveDir != predDir && tstart != tempStream.end()){ 
        tstart = tempStream.end();
    }

    if(resolveDir != primpred){

        bitset<TS_SIZE> idx = ts_idx(PC);
        if(tstart == tempStream.end()){ 
            std::map<bitset<TS_SIZE>, std::list<char>::iterator, bitset<TS_SIZE> >::iterator pair = ts_htr.find(idx);
            if (pair != ts_htr.end()) 
                tstart = pair->second;
        }
        ts_htr[idx] = tempStream.end();
        --ts_htr[idx];
    }

}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

void    PREDICTOR::TrackOtherInst(UINT32 PC, OpType opType, UINT32 branchTarget){

  // This function is called for instructions which are not
  // conditional branches, just in case someone decides to design
  // a predictor that uses information from such instructions.
  // We expect most contestants to leave this function untouched.

  return;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
