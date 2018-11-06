#ifndef __MD5_H__
#define __MD5_H__

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"

enum eMD5SConst
{
    MD5_S11 = 7 ,
    MD5_S12 = 12,
    MD5_S13 = 17,
    MD5_S14 = 22,
    MD5_S21 = 5 ,
    MD5_S22 = 9 ,
    MD5_S23 = 14,
    MD5_S24 = 20,
    MD5_S31 = 4 ,
    MD5_S32 = 11,
    MD5_S33 = 16,
    MD5_S34 = 23,
    MD5_S41 = 6 ,
    MD5_S42 = 10,
    MD5_S43 = 15,
    MD5_S44 = 21,
};

class MD5
{
public:
    const static U32 BLOCK_SIZE = 64;
    
public:
    MD5();
    MD5(const std::string& text);
    
public:
    void update(const U8 *buf, U32 length);
    void finalize();

    string toHexString() const;
    bool getDigestBuffer(U8* buffer) const;
    
private:
    void init();
    void transform(const U8 block[BLOCK_SIZE]); // Apply MD5 algorithm on a block
    
private:
    bool m_IsFinalized;
    U8 m_Buffer[BLOCK_SIZE]; // bytes that didn't fit in last 64 byte chunk
    U8 m_Digest[16];         // the result
    U32 m_State[4];          // digest so far
    U32 m_BitCount[2];       // 64bit counter for number of bits (lo, hi)
};

#endif // __MD5_H__
