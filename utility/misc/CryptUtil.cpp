#include "utility/crypt/MD5.h"

#include "CryptUtil.h"

bool CryptUtil::EncryptBuffer(U8* buffer, U32 size, eCryptMode mode)
{
    bool status = false;
    
    switch(mode)
    {
        case CRYPT_MD5:    status = Encrypt_MD5(buffer, size); break;
        case CRYPT_SHA256: status = Encrypt_SHA256(buffer, size); break;
        default: break;
    }
    
    return status;
}

bool CryptUtil::VerifyBuffer(const U8* buffer, U32 size, eCryptMode mode)
{
    bool status = false;
    
    switch(mode)
    {
        case CRYPT_MD5:    status = Verify_MD5(buffer, size); break;
        case CRYPT_SHA256: status = Verify_SHA256(buffer, size); break;
        default: break;
    }
    
    return status;
}

bool CryptUtil::GetBufferChecksum(const U8 *buffer, U32 size, U8 *checksum, eCryptMode mode)
{
    bool status = false;

    switch(mode)
    {
        case CRYPT_MD5:    status = Checksum_MD5(buffer, size, checksum); break;
        case CRYPT_SHA256: status = Checksum_SHA256(buffer, size, checksum); break;
        default: break;
    }

    return status;
}

bool CryptUtil::GetSignatureChecksum(U32 sectionID, U8 *checksum, eCryptMode mode)
{
    stringstream sstr;
    sstr << "CREATED BY VIRTIUM'S SOFTWARE (ID " << FRMT_U32(8, sectionID) << ")";

    const string& buffer = sstr.str();

    return GetBufferChecksum((U8*)buffer.c_str(), buffer.length(), checksum, mode);
}

bool CryptUtil::Encrypt_MD5(U8* buffer, U32 size)
{
    if ( (NULL == buffer) || (size <= 16) ) return false;

    U32 codesize = 16;
    U32 remsize = size - codesize;

    return Checksum_MD5(buffer, remsize, buffer + remsize);
}

bool CryptUtil::Verify_MD5(const U8 *buffer, U32 size)
{
    if ( (NULL == buffer) || (size <= 16) ) return false;

    U32 codesize = 16;
    U32 remsize = size - codesize;

    // Get digest data
    U8 digest[16];
    if (false == Checksum_MD5(buffer, remsize, digest)) return false;

    // Compare digest data with buffer checksum
    return (0 == memcmp(digest, buffer + remsize, sizeof(digest)));
}

bool CryptUtil::Checksum_MD5(const U8 *buffer, U32 size, U8* checksum)
{
    if ( (NULL == buffer) || (NULL == checksum) || (0 == size) ) return false;

    MD5 md5;

    md5.update(buffer, size);
    md5.finalize();

    // Get digest data
    if (false == md5.getDigestBuffer(checksum)) return false;

    return true;
}

bool CryptUtil::Encrypt_SHA256(U8* buffer, U32 size)
{
    UNUSE_NAME(size);
    UNUSE_NAME(buffer);
    return false;
}

bool CryptUtil::Verify_SHA256(const U8* buffer, U32 size)
{
    UNUSE_NAME(size);
    UNUSE_NAME(buffer);
    return false;
}

bool CryptUtil::Checksum_SHA256(const U8* buffer, U32 size, U8* checksum)
{
    UNUSE_NAME(size);
    UNUSE_NAME(buffer);
    UNUSE_NAME(checksum);
    return false;
}

bool CryptUtil::EncryptSignature(U8 *sector, eCryptMode mode)
{
    if (NULL == sector) return false;

    // Modify some data in this sector

    U32 sectorSize = 512;
    U32 sectionSize = 128;
    U32 sectionCount = sectorSize / sectionSize;

    U8 checksum[16];

    // Create checksum for sections 0, 1, 2
    for (U32 i = 0; i < sectionCount; i++)
    {
        if (false == GetSignatureChecksum(i, checksum, mode)) return false;

        memcpy(sector + (i + 1) * sectionSize - 16, checksum, 16);
    }

    return true;
}

bool CryptUtil::VerifySignature(const U8 *sector, eCryptMode mode)
{
    if (NULL == sector) return false;

    // Modify some data in this sector

    U32 sectorSize = 512;
    U32 sectionSize = 128;
    U32 sectionCount = sectorSize / sectionSize;

    U8 checksum[16];

    // Create checksum for sections 0, 1, 2
    for (U32 i = 0; i < sectionCount; i++)
    {
        if (false == GetSignatureChecksum(i, checksum, mode)) return false;

        if (0 != memcmp(sector + (i + 1) * sectionSize - 16, checksum, 16)) return false;
    }

    return true;
}

bool CryptUtil::EncryptSector(U8 *buffer, U32 sectorCount, eCryptMode mode)
{
    if ( (NULL == buffer) || (0 == sectorCount) ) return false;

    for (U32 i = 0; i < sectorCount; i++)
    {
        U32 sectorSize = 512;
        U8* sectorPtr = buffer + i * sectorSize;

        if (false == EncryptSignature(sectorPtr)) continue;

        if (false == EncryptBuffer(sectorPtr, sectorSize, mode)) return false;
    }

    return true;
}

bool CryptUtil::VerifySector(const U8 *buffer, U32 sectorCount, eCryptMode mode)
{
    if ( (NULL == buffer) || (0 == sectorCount) ) return false;

    for (U32 i = 0; i < sectorCount; i++)
    {
        U32 sectorSize = 512;
        const U8* sectorPtr = buffer + i * sectorSize;

        if (false == VerifySignature(sectorPtr, mode)) continue;

        if (false == VerifyBuffer(sectorPtr, sectorSize, mode)) return false;
    }

    return true;
}
