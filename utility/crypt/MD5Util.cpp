#include "MD5Util.h"

U32 MD5Util::F(U32 x, U32 y, U32 z) 
{
    return x&y | ~x&z;
}

U32 MD5Util::G(U32 x, U32 y, U32 z) 
{
    return x&z | y&~z;
}

U32 MD5Util::H(U32 x, U32 y, U32 z) 
{
    return x^y^z;
}

U32 MD5Util::I(U32 x, U32 y, U32 z) 
{
    return y ^ (x | ~z);
}

U32 MD5Util::rotate_left(U32 x, int n) 
{
    return (x << n) | (x >> (32-n));
}

void MD5Util::FF(U32 &a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 ac) 
{
    a = rotate_left(a+ F(b,c,d) + x + ac, s) + b;
}

void MD5Util::GG(U32 &a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 ac) 
{
    a = rotate_left(a + G(b,c,d) + x + ac, s) + b;
}

void MD5Util::HH(U32 &a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 ac) 
{
    a = rotate_left(a + H(b,c,d) + x + ac, s) + b;
}

void MD5Util::II(U32 &a, U32 b, U32 c, U32 d, U32 x, U32 s, U32 ac) 
{
    a = rotate_left(a + I(b,c,d) + x + ac, s) + b;
}

void MD5Util::decode(U32 output[], const U8 input[], U32 len)
{
    for (unsigned int i = 0, j = 0; j < len; i++, j += 4)
    {
        output[i] = 
            ((U32)input[j]) | 
            (((U32)input[j + 1]) << 8) |
            (((U32)input[j + 2]) << 16) | 
            (((U32)input[j + 3]) << 24);   
    }
}

void MD5Util::encode(U8 output[], const U32 input[], U32 len)
{
    for (U32 i = 0, j = 0; j < len; i++, j += 4) {
        output[j] = input[i] & 0xff;
        output[j + 1] = (input[i] >> 8) & 0xff;
        output[j + 2] = (input[i] >> 16) & 0xff;
        output[j + 3] = (input[i] >> 24) & 0xff;
    }
}
