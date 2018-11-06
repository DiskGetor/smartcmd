#include "ArrayCounter.h"

ArrayCounter::ArrayCounter(U32 counterCount, U32 counterSizeInBit)
{
    if (counterSizeInBit > 32) counterSizeInBit = 32;

    CounterCount = counterCount;

    CounterSize = counterSizeInBit;
    CounterMask = (1 << CounterSize) - 1;

    U32 counterPerItem = 32 / CounterSize;
    U32 itemCount =( CounterCount + counterPerItem - 1) / counterPerItem;

    CounterArray.clear();
    CounterArray.resize(itemCount);

    for (U32 i = 0; i < itemCount; i++) CounterArray[i] = 0;
}

void ArrayCounter::initCounter(U32 counterValue)
{
    counterValue &= CounterMask;

    U32 itemValue = 0;
    U32 counterPerItem = 32 / CounterSize;
    for (U32 i = 0; i < counterPerItem; i++)
    {
        itemValue = (itemValue << CounterSize) | counterValue;
    }

    U32 itemCount = CounterArray.size();
    for (U32 i = 0; i < itemCount; i++) CounterArray[i] = itemValue;
}

void ArrayCounter::getCounterPosition(U32 counterIndex, U32 &itemIndex, U32 &itemOffset) const
{
    ASSERT (counterIndex < CounterCount);

    U32 counterPerItem = 32 / CounterSize;

    itemIndex  = counterIndex / counterPerItem;
    itemOffset = (counterIndex % counterPerItem) * CounterSize;
}

void ArrayCounter::setCounter(U32 counterIndex, U32 counterValue)
{
    U32 itemIndex, itemOffset;
    getCounterPosition(counterIndex, itemIndex, itemOffset);

    CounterArray[itemIndex] &= ~(CounterMask << itemOffset);
    CounterArray[itemIndex] |= (counterValue & CounterMask) << itemOffset;
}

U32 ArrayCounter::getCounter(U32 counterIndex) const
{
    U32 itemIndex, itemOffset;
    getCounterPosition(counterIndex, itemIndex, itemOffset);

    return (CounterArray[itemIndex] >> itemOffset) & CounterMask;
}

void ArrayCounter::updateCounter(U32 startIndex, U32 counterCount)
{
    if (0 == counterCount) return;

    U32 endIndex = startIndex + counterCount;

    bool limitEndIndex =
            (endIndex < counterCount) || // Overflow case
            (endIndex < startIndex)   || // Overflow case
            (endIndex > CounterCount);   // Exceed the capacity

    if (limitEndIndex) endIndex = CounterCount;

    // Continue here
    for (U32 i = startIndex; i < endIndex; i++)
    {
        setCounter(i, getCounter (i) + 1);
    }
}

string ArrayCounter::toCounterString(U32 startIndex, U32 count) const
{
    U32 endIndex = startIndex + count;

    bool limitEndIndex =
            (endIndex < count) ||        // Overflow case
            (endIndex < startIndex) ||   // Overflow case
            (endIndex == startIndex)||   // count == 0 -> set to CounterCount
            (endIndex > CounterCount);   // Exceed the capacity

    if (limitEndIndex) endIndex = CounterCount;

    stringstream sstr;
    U32 stepSize = 32;

    for (U32 i = startIndex; i < endIndex; i += stepSize)
    {
        U32 minIndex = i;
        U32 maxIndex = i + stepSize;

        maxIndex = MIN2(maxIndex, endIndex);

        sstr << "Counter " << FRMT_U32(10, minIndex) << " -> " << FRMT_U32(10, maxIndex) << ": ";

        for (U32 j = minIndex; j < maxIndex; j++)
        {
            U32 value = getCounter(j);
            sstr << value << " ";
        }

        sstr << endl;
    }

    return sstr.str();
}

string ArrayCounter::toItemString(U32 startIndex, U32 count) const
{
    U32 maxItemCount = CounterArray.size();

    U32 endIndex = startIndex + count;

    bool limitEndIndex =
            (endIndex < count) ||        // Overflow case
            (endIndex < startIndex) ||   // Overflow case
            (endIndex == startIndex)||   // count == 0 -> set to CounterCount
            (endIndex > maxItemCount);   // Exceed the capacity

    if (limitEndIndex) endIndex = maxItemCount;

    stringstream sstr;
    U32 stepSize = 32;

    for (U32 i = startIndex; i < endIndex; i += stepSize)
    {
        U32 minIndex = i;
        U32 maxIndex = i + stepSize;

        maxIndex = MIN2(maxIndex, endIndex);

        sstr << "Item " << FRMT_U32(10, minIndex) << " -> " << FRMT_U32(10, maxIndex) << ": ";

        for (U32 j = minIndex; j < maxIndex; j++)
        {
            sstr << CounterArray[j] << " ";
        }

        sstr << endl;
    }

    return sstr.str();
}
