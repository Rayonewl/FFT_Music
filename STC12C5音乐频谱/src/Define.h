
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint  unsigned int
#endif

#define uint8    unsigned char
#define uint16   unsigned short int
#define uint32   unsigned long int
#define uint64   unsigned long long int
#define int8     signed char
#define int16    signed short int
#define int32    signed long int
#define int64    signed long long int


#define BinToHex(n) (((n>>21)&0x80)|((n>>18)&0x40)|((n>>15)&0x20)|((n>>12)&0x10)|((n>>9)& 0x08)|((n>>6)&0x04)|((n>>3)&0x02)|((n)&0x01))
#define B(n) BinToHex(0x##n##l)

