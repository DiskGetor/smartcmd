#include "utility/misc/StdHeader.h"
#include "VscVirtiumDefine.h"

const U8 VscVirtiumConstant::BUFFER_SIGNATURE_1[VscVirtiumConstant::SIGNATURE_SIZE_IN_BYTE] =
{
    'V', 'S', 'C', ' ', 'S', 'E', 'Q', 'U', 'E', 'N', 'C', 'E', '1', ' ', ' ', ' '
};

const U8 VscVirtiumConstant::BUFFER_SIGNATURE_2[VscVirtiumConstant::SIGNATURE_SIZE_IN_BYTE] =
{
    'V', 'S', 'C', ' ', 'S', 'E', 'Q', 'U', 'E', 'N', 'C', 'E', '2', ' ', ' ', ' '
};

const U8 VscVirtiumConstant::BUFFER_SIGNATURE_3[VscVirtiumConstant::SIGNATURE_SIZE_IN_BYTE] =
{
    'V', 'S', 'C', ' ', 'T', 'R', 'A', 'N', 'S', 'F', 'E', 'R', ' ', ' ', ' ', ' '
};

const U8 VscVirtiumConstant::BUFFER_SIGNATURE_4[VscVirtiumConstant::SIGNATURE_SIZE_IN_BYTE] =
{
    'V', 'S', 'C', ' ', 'D', 'O', 'N', 'E', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
};

const U8 VscVirtiumConstant::VSC_COMMAND_1_0_SIGNATURE[VscVirtiumConstant::SIGNATURE_SIZE_IN_BYTE] =
{
    'V', 'S', 'C', ' ', 'C', 'O', 'M', 'M', 'A', 'N', 'D', ' ', '1', '.', '0', ' '
};

const U8 VscVirtiumConstant::VSC_COMMAND_2_0_SIGNATURE[VscVirtiumConstant::SIGNATURE_SIZE_IN_BYTE] =
{
    'V', 'S', 'C', ' ', 'C', 'O', 'M', 'M', 'A', 'N', 'D', ' ', '2', '.', '0', ' '
};

const U32 VscVirtiumConstant::m_NumberSequences_0 = 0;
const U32 VscVirtiumConstant::m_NumberSequences_1 = VscVirtiumConstant::m_NumberSequences_0 + (U32)'V';
const U32 VscVirtiumConstant::m_NumberSequences_2 = VscVirtiumConstant::m_NumberSequences_0 + (U32)'i';
const U32 VscVirtiumConstant::m_NumberSequences_3 = VscVirtiumConstant::m_NumberSequences_0 + (U32)'r';

//---- define structure sVscBuffer
sBuffer::sBuffer()
{
    reset();
}

sBuffer::~sBuffer()
{
    free();
}

void sBuffer::reset()
{
    BufferPtr         = NULL;
    BuferSizeInSector = 0;
}

bool sBuffer::allocateSector(U32 sizeInSector)
{
    free();

    BuferSizeInSector = sizeInSector * 512;
    BufferPtr = new U8[BuferSizeInSector];

    return (NULL != BufferPtr);
}

void sBuffer::copyTo(U8* pDestMem, const U32 &bytesCount, const U32 startIndex)
{
    memcpy(pDestMem, &BufferPtr[startIndex], bytesCount);
}

void sBuffer::free()
{
    if(NULL != BufferPtr) delete BufferPtr;
    reset();
}

//---- end of define structure sVscBuffer

U16 ByteToU16(const U8* buff, U32 index)
{
    U16 retVal = 0;

    retVal = (buff[index + 1] << 8) | buff[index];

    return retVal;
}

U32 ByteToU32(const U8* buff, U32 index)
{
    U32 retVal = 0;
    const U8* pValue = &buff[index + sizeof(U32) - 1];

    for(U8 nByte = 0; nByte < sizeof(U32); ++ nByte)
    {
        retVal <<= 8;
        retVal += (*pValue);
        -- pValue;
    }

    return retVal;
}

U64 ByteToU64(const U8* buff, U32 index)
{
    U64 retVal = 0;
    const U8* pValue = &buff[index + sizeof(U64) - 1];

    for(U8 nByte = 0; nByte < sizeof(U64); ++ nByte)
    {
        retVal <<= 8;
        retVal += (*pValue);
        -- pValue;
    }

    return retVal;
}

void consoleProgressPercent(U32 current, U32 total)
{
    U32 tick = total / 10; // about 10%;

    ++ current;

    if( (0 == (current % tick)) || (total == current) )
    {
        double dTotal   = total;
        double dCurrent = current;
        double percent  = (dCurrent * 100.0) / dTotal;

        cout << endl << "Done " << std::fixed << std::setprecision(1) << percent << " %";
    }
}
