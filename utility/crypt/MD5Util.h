#ifndef __MD5UTIL_H__
#define __MD5UTIL_H__

#include "utility/misc/StdMacro.h"
#include "utility/misc/StdHeader.h"

class MD5Util
{
public:
    // low level logic operations
    static U32 F(U32 x, U32 y, U32 z);
    static U32 G(U32 x, U32 y, U32 z);
    static U32 H(U32 x, U32 y, U32 z);
    static U32 I(U32 x, U32 y, U32 z);
    static U32 rotate_left(U32 x, int n);
    
public:
    static void FF(U32 &a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 ac);
    static void GG(U32 &a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 ac);
    static void HH(U32 &a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 ac);
    static void II(U32 &a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 ac);
    
public:
    static void decode(U32 output[], const U8 input[], U32 len);
    static void encode(U8 output[], const U32 input[], U32 len);
};

#endif // __MD5UTIL_H__
