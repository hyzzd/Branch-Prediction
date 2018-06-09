#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include "utils.h"
#include "tracer.h"
#include <list>
#include <map>
#include <bitset>

#define PHT_SIZE 0x20000

#define TS_SIZE (140 + 32)


/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

template<std::size_t N>
struct bitset_small {
    bool operator() (const std::bitset<N>& a, const std::bitset<N>& b) const
    {
        if (N <= 64) 
            return a.to_ulong() < b.to_ulong();

        for (int i = N-1; i >= 0; i--) {
            if (a[i] && !b[i]) 
                return false;
            if (!a[i] && b[i]) 
                return true;
        }
        return false;
    }
};


class PREDICTOR{

  // The state is defined for Gshare, change for your design

 private:
  UINT32  ghr;           // global history register
  UINT32  pht[PHT_SIZE]; // pattern history table

  bool primpred;
  std::list<char> tempStream;
  std::bitset<TS_SIZE> ts_ghr;  
  std::list<char>::iterator tstart;
  std::map<bitset<TS_SIZE>, std::list<char>::iterator, bitset_small<TS_SIZE> > ts_htr;

 public:

  // The interface to the four functions below CAN NOT be changed

  PREDICTOR(void);
  bool    GetPrediction(UINT32 PC);  
  void    UpdatePredictor(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget);
  void    TrackOtherInst(UINT32 PC, OpType opType, UINT32 branchTarget);

  // Contestants can define their own functions below
 
 private:
  bitset<TS_SIZE> ts_idx(UINT32 PC);

};



/***********************************************************/
#endif

