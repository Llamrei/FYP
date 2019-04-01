#ifndef FIPS202_H
#define FIPS202_H

#include <linux/types.h>


#define SHAKE128_RATE 168
#define SHAKE256_RATE 136

void shake128_absorb(__u64 *s, const unsigned char *input, unsigned int inputByteLen);
void shake128_squeezeblocks(unsigned char *output, unsigned long long nblocks, __u64 *s);
void shake128(unsigned char *output, unsigned long long outlen, const unsigned char *input,  unsigned long long inlen);

void cshake128_simple_absorb(__u64 *s, __u16 cstm, const unsigned char *in, unsigned long long inlen);
void cshake128_simple_squeezeblocks(unsigned char *output, unsigned long long nblocks, __u64 *s);
void cshake128_simple(unsigned char *output, unsigned long long outlen, __u16 cstm, const unsigned char *in, unsigned long long inlen);

void shake256_absorb(__u64 *s, const unsigned char *input, unsigned int inputByteLen);
void shake256_squeezeblocks(unsigned char *output, unsigned long long nblocks, __u64 *s);
void shake256(unsigned char *output, unsigned long long outlen, const unsigned char *input,  unsigned long long inlen);

void cshake256_simple_absorb(__u64 *s, __u16 cstm, const unsigned char *in, unsigned long long inlen);
void cshake256_simple_squeezeblocks(unsigned char *output, unsigned long long nblocks, __u64 *s);
void cshake256_simple(unsigned char *output, unsigned long long outlen, __u16 cstm, const unsigned char *in, unsigned long long inlen);


#endif
