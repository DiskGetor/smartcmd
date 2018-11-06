#ifndef __ARRAYCOUNTER_H__
#define __ARRAYCOUNTER_H__

#include "StdMacro.h"
#include "StdHeader.h"

class ArrayCounter
{
public:
    ArrayCounter(U32 counterCount, U32 counterSizeInBit);
    
public:
    void initCounter(U32 counterValue);

    U32 getCounter(U32 counterIndex) const;
    void updateCounter(U32 counterIndex, U32 counterCount = 1);

    void setCounter(U32 counterIndex, U32 counterValue);

    string toItemString(U32 startIndex = 0, U32 count = 0) const;
    string toCounterString(U32 startIndex = 0, U32 count = 0) const;

public:
    void getCounterPosition(U32 counterIndex, U32& itemIndex, U32& itemOffset) const;

public:
    U32 CounterMask;
    U32 CounterSize;  // Size of counter in bits: 2, 4, 8, 16, 32, ..
    U32 CounterCount; // Number of counters

    vector<U32> CounterArray;
};

#endif // __ARRAYCOUNTER_H__
