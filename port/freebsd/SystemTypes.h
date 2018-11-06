#ifndef __SYSTEMTYPES_H__
#define __SYSTEMTYPES_H__


#define __u8    unsigned char
#define __u16   unsigned short
#ifndef __LP64__  // ILP32 (32-bit), LLP64 (64-bit MSVC, MinGW)
#define __u32   unsigned long
#else             // LP64 (64-bit Cygwin)
#define __u32   unsigned int
#endif
#define __u64   unsigned __int64

#define __i8    char

#endif // __SYSTEMTYPES_H__