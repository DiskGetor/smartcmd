#include "MD5.h"
#include "MD5Util.h"

MD5::MD5()
{
    init();
}

MD5::MD5(const std::string &text)
{
    init();
    update((U8*) text.c_str(), text.length());
    finalize();
}

void MD5::init()
{
    m_IsFinalized=false;

    m_BitCount[0] = 0;
    m_BitCount[1] = 0;

    // Magic initialization constants.
    m_State[0] = 0x67452301;
    m_State[1] = 0xefcdab89;
    m_State[2] = 0x98badcfe;
    m_State[3] = 0x10325476;
}

void MD5::transform(const U8 block[BLOCK_SIZE])
{
    U32 x[16];
    U32 a = m_State[0], b = m_State[1], c = m_State[2], d = m_State[3];
    
    MD5Util::decode (x, block, BLOCK_SIZE);

    /* Round 1 */
    MD5Util::FF (a, b, c, d, x[ 0], MD5_S11, 0xd76aa478); /* 1 */
    MD5Util::FF (d, a, b, c, x[ 1], MD5_S12, 0xe8c7b756); /* 2 */
    MD5Util::FF (c, d, a, b, x[ 2], MD5_S13, 0x242070db); /* 3 */
    MD5Util::FF (b, c, d, a, x[ 3], MD5_S14, 0xc1bdceee); /* 4 */
    MD5Util::FF (a, b, c, d, x[ 4], MD5_S11, 0xf57c0faf); /* 5 */
    MD5Util::FF (d, a, b, c, x[ 5], MD5_S12, 0x4787c62a); /* 6 */
    MD5Util::FF (c, d, a, b, x[ 6], MD5_S13, 0xa8304613); /* 7 */
    MD5Util::FF (b, c, d, a, x[ 7], MD5_S14, 0xfd469501); /* 8 */
    MD5Util::FF (a, b, c, d, x[ 8], MD5_S11, 0x698098d8); /* 9 */
    MD5Util::FF (d, a, b, c, x[ 9], MD5_S12, 0x8b44f7af); /* 10 */
    MD5Util::FF (c, d, a, b, x[10], MD5_S13, 0xffff5bb1); /* 11 */
    MD5Util::FF (b, c, d, a, x[11], MD5_S14, 0x895cd7be); /* 12 */
    MD5Util::FF (a, b, c, d, x[12], MD5_S11, 0x6b901122); /* 13 */
    MD5Util::FF (d, a, b, c, x[13], MD5_S12, 0xfd987193); /* 14 */
    MD5Util::FF (c, d, a, b, x[14], MD5_S13, 0xa679438e); /* 15 */
    MD5Util::FF (b, c, d, a, x[15], MD5_S14, 0x49b40821); /* 16 */

    /* Round 2 */
    MD5Util::GG (a, b, c, d, x[ 1], MD5_S21, 0xf61e2562); /* 17 */
    MD5Util::GG (d, a, b, c, x[ 6], MD5_S22, 0xc040b340); /* 18 */
    MD5Util::GG (c, d, a, b, x[11], MD5_S23, 0x265e5a51); /* 19 */
    MD5Util::GG (b, c, d, a, x[ 0], MD5_S24, 0xe9b6c7aa); /* 20 */
    MD5Util::GG (a, b, c, d, x[ 5], MD5_S21, 0xd62f105d); /* 21 */
    MD5Util::GG (d, a, b, c, x[10], MD5_S22,  0x2441453); /* 22 */
    MD5Util::GG (c, d, a, b, x[15], MD5_S23, 0xd8a1e681); /* 23 */
    MD5Util::GG (b, c, d, a, x[ 4], MD5_S24, 0xe7d3fbc8); /* 24 */
    MD5Util::GG (a, b, c, d, x[ 9], MD5_S21, 0x21e1cde6); /* 25 */
    MD5Util::GG (d, a, b, c, x[14], MD5_S22, 0xc33707d6); /* 26 */
    MD5Util::GG (c, d, a, b, x[ 3], MD5_S23, 0xf4d50d87); /* 27 */
    MD5Util::GG (b, c, d, a, x[ 8], MD5_S24, 0x455a14ed); /* 28 */
    MD5Util::GG (a, b, c, d, x[13], MD5_S21, 0xa9e3e905); /* 29 */
    MD5Util::GG (d, a, b, c, x[ 2], MD5_S22, 0xfcefa3f8); /* 30 */
    MD5Util::GG (c, d, a, b, x[ 7], MD5_S23, 0x676f02d9); /* 31 */
    MD5Util::GG (b, c, d, a, x[12], MD5_S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    MD5Util::HH (a, b, c, d, x[ 5], MD5_S31, 0xfffa3942); /* 33 */
    MD5Util::HH (d, a, b, c, x[ 8], MD5_S32, 0x8771f681); /* 34 */
    MD5Util::HH (c, d, a, b, x[11], MD5_S33, 0x6d9d6122); /* 35 */
    MD5Util::HH (b, c, d, a, x[14], MD5_S34, 0xfde5380c); /* 36 */
    MD5Util::HH (a, b, c, d, x[ 1], MD5_S31, 0xa4beea44); /* 37 */
    MD5Util::HH (d, a, b, c, x[ 4], MD5_S32, 0x4bdecfa9); /* 38 */
    MD5Util::HH (c, d, a, b, x[ 7], MD5_S33, 0xf6bb4b60); /* 39 */
    MD5Util::HH (b, c, d, a, x[10], MD5_S34, 0xbebfbc70); /* 40 */
    MD5Util::HH (a, b, c, d, x[13], MD5_S31, 0x289b7ec6); /* 41 */
    MD5Util::HH (d, a, b, c, x[ 0], MD5_S32, 0xeaa127fa); /* 42 */
    MD5Util::HH (c, d, a, b, x[ 3], MD5_S33, 0xd4ef3085); /* 43 */
    MD5Util::HH (b, c, d, a, x[ 6], MD5_S34,  0x4881d05); /* 44 */
    MD5Util::HH (a, b, c, d, x[ 9], MD5_S31, 0xd9d4d039); /* 45 */
    MD5Util::HH (d, a, b, c, x[12], MD5_S32, 0xe6db99e5); /* 46 */
    MD5Util::HH (c, d, a, b, x[15], MD5_S33, 0x1fa27cf8); /* 47 */
    MD5Util::HH (b, c, d, a, x[ 2], MD5_S34, 0xc4ac5665); /* 48 */

    /* Round 4 */
    MD5Util::II (a, b, c, d, x[ 0], MD5_S41, 0xf4292244); /* 49 */
    MD5Util::II (d, a, b, c, x[ 7], MD5_S42, 0x432aff97); /* 50 */
    MD5Util::II (c, d, a, b, x[14], MD5_S43, 0xab9423a7); /* 51 */
    MD5Util::II (b, c, d, a, x[ 5], MD5_S44, 0xfc93a039); /* 52 */
    MD5Util::II (a, b, c, d, x[12], MD5_S41, 0x655b59c3); /* 53 */
    MD5Util::II (d, a, b, c, x[ 3], MD5_S42, 0x8f0ccc92); /* 54 */
    MD5Util::II (c, d, a, b, x[10], MD5_S43, 0xffeff47d); /* 55 */
    MD5Util::II (b, c, d, a, x[ 1], MD5_S44, 0x85845dd1); /* 56 */
    MD5Util::II (a, b, c, d, x[ 8], MD5_S41, 0x6fa87e4f); /* 57 */
    MD5Util::II (d, a, b, c, x[15], MD5_S42, 0xfe2ce6e0); /* 58 */
    MD5Util::II (c, d, a, b, x[ 6], MD5_S43, 0xa3014314); /* 59 */
    MD5Util::II (b, c, d, a, x[13], MD5_S44, 0x4e0811a1); /* 60 */
    MD5Util::II (a, b, c, d, x[ 4], MD5_S41, 0xf7537e82); /* 61 */
    MD5Util::II (d, a, b, c, x[11], MD5_S42, 0xbd3af235); /* 62 */
    MD5Util::II (c, d, a, b, x[ 2], MD5_S43, 0x2ad7d2bb); /* 63 */
    MD5Util::II (b, c, d, a, x[ 9], MD5_S44, 0xeb86d391); /* 64 */

    m_State[0] += a;
    m_State[1] += b;
    m_State[2] += c;
    m_State[3] += d;

    // Zeroize sensitive information.
    memset(x, 0, sizeof x);
}

void MD5::update(const U8 input[], U32 length)
{
    U32 index = m_BitCount[0] / 8 % BLOCK_SIZE;

    if ((m_BitCount[0] += (length << 3)) < (length << 3)) m_BitCount[1]++;
    
    m_BitCount[1] += (length >> 29);

    U32 firstpart = 64 - index;

    U32 i;
    
    if (length >= firstpart)
    {
        memcpy(&m_Buffer[index], input, firstpart);
        
        transform(m_Buffer);

        // Transform chunks of BLOCK_SIZE (64 bytes)
        for (i = firstpart; i + BLOCK_SIZE <= length; i += BLOCK_SIZE) transform(&input[i]);
        
        index = 0;
    }
    else i = 0;

    memcpy(&m_Buffer[index], &input[i], length-i);
}

void MD5::finalize()
{
    static unsigned char padding[64] = {
        0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    if (!m_IsFinalized) {
        // Save number of bits
        unsigned char bits[8];
        MD5Util::encode(bits, m_BitCount, 8);

        // pad out to 56 mod 64.
        U32 index = m_BitCount[0] / 8 % 64;
        U32 padLen = (index < 56) ? (56 - index) : (120 - index);
        update(padding, padLen);

        // Append length (before padding)
        update(bits, 8);

        // Store state in digest
        MD5Util::encode(m_Digest, m_State, 16);

        // Zeroize sensitive information.
        memset(m_Buffer, 0, sizeof m_Buffer);
        memset(m_BitCount, 0, sizeof m_BitCount);

        m_IsFinalized = true;
    }
}

bool MD5::getDigestBuffer(U8 *buffer) const
{
    if (!m_IsFinalized || (NULL == buffer)) return false;

    memcpy(buffer, m_Digest, sizeof(m_Digest));

    return true;
}
// return hex representation of digest as string
string MD5::toHexString() const
{
    if (!m_IsFinalized)
        return "";

    char buf[33];
    for (int i=0; i<16; i++)
        sprintf(buf+i*2, "%02x", m_Digest[i]);
    buf[32]=0;

    return std::string(buf);
}
