#ifndef CRYPTUTIL_H
#define CRYPTUTIL_H

#include "StdMacro.h"
#include "StdHeader.h"

enum eCryptMode
{
    CRYPT_MD5,
    CRYPT_SHA256,
};

class CryptUtil
{
public:
    // Encrypt sequence 512-byte-buffers
    static bool EncryptSector(U8* buffer, U32 sectorCount, eCryptMode mode = CRYPT_MD5);
    static bool VerifySector(const U8* buffer, U32 sectorCount, eCryptMode mode = CRYPT_MD5);

    static bool EncryptSignature(U8* sector, eCryptMode mode = CRYPT_MD5);
    static bool VerifySignature(const U8* sector, eCryptMode mode = CRYPT_MD5);
    
    // Encrypt common buffer
    static bool EncryptBuffer(U8* buffer, U32 size, eCryptMode mode = CRYPT_MD5);
    static bool VerifyBuffer(const U8* buffer, U32 size, eCryptMode mode = CRYPT_MD5);
    static bool GetBufferChecksum(const U8* buffer, U32 size, U8* checksum, eCryptMode mode = CRYPT_MD5);
    static bool GetSignatureChecksum(U32 sectionID, U8* checksum, eCryptMode mode = CRYPT_MD5);

private:
    static bool Encrypt_MD5(U8* buffer, U32 size);
    static bool Verify_MD5(const U8* buffer, U32 size);
    static bool Checksum_MD5(const U8* buffer, U32 size, U8* checksum);

    static bool Encrypt_SHA256(U8* buffer, U32 size);
    static bool Verify_SHA256(const U8* buffer, U32 size);
    static bool Checksum_SHA256(const U8* buffer, U32 size, U8* checksum);
};

#endif //CRYPTUTIL_H
