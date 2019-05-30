/********************************************************************************************
* SHA3-derived functions: SHAKE and cSHAKE
*
* Based on the public domain implementation in crypto_hash/keccakc512/simple/ 
* from http://bench.cr.yp.to/supercop.html by Ronny Van Keer 
* and the public domain "TweetFips202" implementation from https://twitter.com/tweetfips202 
* by Gilles Van Assche, Daniel J. Bernstein, and Peter Schwabe
*
* See NIST Special Publication 800-185 for more information:
* http://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-185.pdf
*
*********************************************************************************************/  

#include "fips202.h"

#define NROUNDS 24
#define ROL(a, offset) ((a << offset) ^ (a >> (64-offset)))


static __u64 load64(const unsigned char *x)
{
  unsigned long long r = 0, i;

  for (i = 0; i < 8; ++i) {
    r |= (unsigned long long)x[i] << 8 * i;
  }
  return r;
}


static void store64(__u8 *x, __u64 u)
{
  unsigned int i;

  for (i = 0; i < 8; ++i) {
    x[i] = (__u8)u;
    u >>= 8;
  }
}


static const __u64 KeccakF_RoundConstants[NROUNDS] = 
{
    (__u64)0x0000000000000001ULL,
    (__u64)0x0000000000008082ULL,
    (__u64)0x800000000000808aULL,
    (__u64)0x8000000080008000ULL,
    (__u64)0x000000000000808bULL,
    (__u64)0x0000000080000001ULL,
    (__u64)0x8000000080008081ULL,
    (__u64)0x8000000000008009ULL,
    (__u64)0x000000000000008aULL,
    (__u64)0x0000000000000088ULL,
    (__u64)0x0000000080008009ULL,
    (__u64)0x000000008000000aULL,
    (__u64)0x000000008000808bULL,
    (__u64)0x800000000000008bULL,
    (__u64)0x8000000000008089ULL,
    (__u64)0x8000000000008003ULL,
    (__u64)0x8000000000008002ULL,
    (__u64)0x8000000000000080ULL,
    (__u64)0x000000000000800aULL,
    (__u64)0x800000008000000aULL,
    (__u64)0x8000000080008081ULL,
    (__u64)0x8000000000008080ULL,
    (__u64)0x0000000080000001ULL,
    (__u64)0x8000000080008008ULL
};


void KeccakF1600_StatePermute(__u64 * state)
{
  int round;

        __u64 Aba, Abe, Abi, Abo, Abu;
        __u64 Aga, Age, Agi, Ago, Agu;
        __u64 Aka, Ake, Aki, Ako, Aku;
        __u64 Ama, Ame, Ami, Amo, Amu;
        __u64 Asa, Ase, Asi, Aso, Asu;
        __u64 BCa, BCe, BCi, BCo, BCu;
        __u64 Da, De, Di, Do, Du;
        __u64 Eba, Ebe, Ebi, Ebo, Ebu;
        __u64 Ega, Ege, Egi, Ego, Egu;
        __u64 Eka, Eke, Eki, Eko, Eku;
        __u64 Ema, Eme, Emi, Emo, Emu;
        __u64 Esa, Ese, Esi, Eso, Esu;

        //copyFromState(A, state)
        Aba = state[ 0];
        Abe = state[ 1];
        Abi = state[ 2];
        Abo = state[ 3];
        Abu = state[ 4];
        Aga = state[ 5];
        Age = state[ 6];
        Agi = state[ 7];
        Ago = state[ 8];
        Agu = state[ 9];
        Aka = state[10];
        Ake = state[11];
        Aki = state[12];
        Ako = state[13];
        Aku = state[14];
        Ama = state[15];
        Ame = state[16];
        Ami = state[17];
        Amo = state[18];
        Amu = state[19];
        Asa = state[20];
        Ase = state[21];
        Asi = state[22];
        Aso = state[23];
        Asu = state[24];

        for( round = 0; round < NROUNDS; round += 2 )
        {
            //    prepareTheta
            BCa = Aba^Aga^Aka^Ama^Asa;
            BCe = Abe^Age^Ake^Ame^Ase;
            BCi = Abi^Agi^Aki^Ami^Asi;
            BCo = Abo^Ago^Ako^Amo^Aso;
            BCu = Abu^Agu^Aku^Amu^Asu;

            //thetaRhoPiChiIotaPrepareTheta(round  , A, E)
            Da = BCu^ROL(BCe, 1);
            De = BCa^ROL(BCi, 1);
            Di = BCe^ROL(BCo, 1);
            Do = BCi^ROL(BCu, 1);
            Du = BCo^ROL(BCa, 1);

            Aba ^= Da;
            BCa = Aba;
            Age ^= De;
            BCe = ROL(Age, 44);
            Aki ^= Di;
            BCi = ROL(Aki, 43);
            Amo ^= Do;
            BCo = ROL(Amo, 21);
            Asu ^= Du;
            BCu = ROL(Asu, 14);
            Eba =   BCa ^((~BCe)&  BCi );
            Eba ^= (__u64)KeccakF_RoundConstants[round];
            Ebe =   BCe ^((~BCi)&  BCo );
            Ebi =   BCi ^((~BCo)&  BCu );
            Ebo =   BCo ^((~BCu)&  BCa );
            Ebu =   BCu ^((~BCa)&  BCe );

            Abo ^= Do;
            BCa = ROL(Abo, 28);
            Agu ^= Du;
            BCe = ROL(Agu, 20);
            Aka ^= Da;
            BCi = ROL(Aka,  3);
            Ame ^= De;
            BCo = ROL(Ame, 45);
            Asi ^= Di;
            BCu = ROL(Asi, 61);
            Ega =   BCa ^((~BCe)&  BCi );
            Ege =   BCe ^((~BCi)&  BCo );
            Egi =   BCi ^((~BCo)&  BCu );
            Ego =   BCo ^((~BCu)&  BCa );
            Egu =   BCu ^((~BCa)&  BCe );

            Abe ^= De;
            BCa = ROL(Abe,  1);
            Agi ^= Di;
            BCe = ROL(Agi,  6);
            Ako ^= Do;
            BCi = ROL(Ako, 25);
            Amu ^= Du;
            BCo = ROL(Amu,  8);
            Asa ^= Da;
            BCu = ROL(Asa, 18);
            Eka =   BCa ^((~BCe)&  BCi );
            Eke =   BCe ^((~BCi)&  BCo );
            Eki =   BCi ^((~BCo)&  BCu );
            Eko =   BCo ^((~BCu)&  BCa );
            Eku =   BCu ^((~BCa)&  BCe );

            Abu ^= Du;
            BCa = ROL(Abu, 27);
            Aga ^= Da;
            BCe = ROL(Aga, 36);
            Ake ^= De;
            BCi = ROL(Ake, 10);
            Ami ^= Di;
            BCo = ROL(Ami, 15);
            Aso ^= Do;
            BCu = ROL(Aso, 56);
            Ema =   BCa ^((~BCe)&  BCi );
            Eme =   BCe ^((~BCi)&  BCo );
            Emi =   BCi ^((~BCo)&  BCu );
            Emo =   BCo ^((~BCu)&  BCa );
            Emu =   BCu ^((~BCa)&  BCe );

            Abi ^= Di;
            BCa = ROL(Abi, 62);
            Ago ^= Do;
            BCe = ROL(Ago, 55);
            Aku ^= Du;
            BCi = ROL(Aku, 39);
            Ama ^= Da;
            BCo = ROL(Ama, 41);
            Ase ^= De;
            BCu = ROL(Ase,  2);
            Esa =   BCa ^((~BCe)&  BCi );
            Ese =   BCe ^((~BCi)&  BCo );
            Esi =   BCi ^((~BCo)&  BCu );
            Eso =   BCo ^((~BCu)&  BCa );
            Esu =   BCu ^((~BCa)&  BCe );

            //    prepareTheta
            BCa = Eba^Ega^Eka^Ema^Esa;
            BCe = Ebe^Ege^Eke^Eme^Ese;
            BCi = Ebi^Egi^Eki^Emi^Esi;
            BCo = Ebo^Ego^Eko^Emo^Eso;
            BCu = Ebu^Egu^Eku^Emu^Esu;

            //thetaRhoPiChiIotaPrepareTheta(round+1, E, A)
            Da = BCu^ROL(BCe, 1);
            De = BCa^ROL(BCi, 1);
            Di = BCe^ROL(BCo, 1);
            Do = BCi^ROL(BCu, 1);
            Du = BCo^ROL(BCa, 1);

            Eba ^= Da;
            BCa = Eba;
            Ege ^= De;
            BCe = ROL(Ege, 44);
            Eki ^= Di;
            BCi = ROL(Eki, 43);
            Emo ^= Do;
            BCo = ROL(Emo, 21);
            Esu ^= Du;
            BCu = ROL(Esu, 14);
            Aba =   BCa ^((~BCe)&  BCi );
            Aba ^= (__u64)KeccakF_RoundConstants[round+1];
            Abe =   BCe ^((~BCi)&  BCo );
            Abi =   BCi ^((~BCo)&  BCu );
            Abo =   BCo ^((~BCu)&  BCa );
            Abu =   BCu ^((~BCa)&  BCe );

            Ebo ^= Do;
            BCa = ROL(Ebo, 28);
            Egu ^= Du;
            BCe = ROL(Egu, 20);
            Eka ^= Da;
            BCi = ROL(Eka, 3);
            Eme ^= De;
            BCo = ROL(Eme, 45);
            Esi ^= Di;
            BCu = ROL(Esi, 61);
            Aga =   BCa ^((~BCe)&  BCi );
            Age =   BCe ^((~BCi)&  BCo );
            Agi =   BCi ^((~BCo)&  BCu );
            Ago =   BCo ^((~BCu)&  BCa );
            Agu =   BCu ^((~BCa)&  BCe );

            Ebe ^= De;
            BCa = ROL(Ebe, 1);
            Egi ^= Di;
            BCe = ROL(Egi, 6);
            Eko ^= Do;
            BCi = ROL(Eko, 25);
            Emu ^= Du;
            BCo = ROL(Emu, 8);
            Esa ^= Da;
            BCu = ROL(Esa, 18);
            Aka =   BCa ^((~BCe)&  BCi );
            Ake =   BCe ^((~BCi)&  BCo );
            Aki =   BCi ^((~BCo)&  BCu );
            Ako =   BCo ^((~BCu)&  BCa );
            Aku =   BCu ^((~BCa)&  BCe );

            Ebu ^= Du;
            BCa = ROL(Ebu, 27);
            Ega ^= Da;
            BCe = ROL(Ega, 36);
            Eke ^= De;
            BCi = ROL(Eke, 10);
            Emi ^= Di;
            BCo = ROL(Emi, 15);
            Eso ^= Do;
            BCu = ROL(Eso, 56);
            Ama =   BCa ^((~BCe)&  BCi );
            Ame =   BCe ^((~BCi)&  BCo );
            Ami =   BCi ^((~BCo)&  BCu );
            Amo =   BCo ^((~BCu)&  BCa );
            Amu =   BCu ^((~BCa)&  BCe );

            Ebi ^= Di;
            BCa = ROL(Ebi, 62);
            Ego ^= Do;
            BCe = ROL(Ego, 55);
            Eku ^= Du;
            BCi = ROL(Eku, 39);
            Ema ^= Da;
            BCo = ROL(Ema, 41);
            Ese ^= De;
            BCu = ROL(Ese, 2);
            Asa =   BCa ^((~BCe)&  BCi );
            Ase =   BCe ^((~BCi)&  BCo );
            Asi =   BCi ^((~BCo)&  BCu );
            Aso =   BCo ^((~BCu)&  BCa );
            Asu =   BCu ^((~BCa)&  BCe );
        }

        //copyToState(state, A)
        state[ 0] = Aba;
        state[ 1] = Abe;
        state[ 2] = Abi;
        state[ 3] = Abo;
        state[ 4] = Abu;
        state[ 5] = Aga;
        state[ 6] = Age;
        state[ 7] = Agi;
        state[ 8] = Ago;
        state[ 9] = Agu;
        state[10] = Aka;
        state[11] = Ake;
        state[12] = Aki;
        state[13] = Ako;
        state[14] = Aku;
        state[15] = Ama;
        state[16] = Ame;
        state[17] = Ami;
        state[18] = Amo;
        state[19] = Amu;
        state[20] = Asa;
        state[21] = Ase;
        state[22] = Asi;
        state[23] = Aso;
        state[24] = Asu;

        #undef    round
}

#include <linux/string.h>
#include <linux/types.h>
#define MIN(a, b) ((a) < (b) ? (a) : (b))


static void keccak_absorb(__u64 *s, unsigned int r, const unsigned char *m, unsigned long long int mlen, unsigned char p)
{
  unsigned long long i;
  unsigned char t[200];
 
  while (mlen >= r) 
  {
    for (i = 0; i < r / 8; ++i)
      s[i] ^= load64(m + 8 * i);
    
    KeccakF1600_StatePermute(s);
    mlen -= r;
    m += r;
  }

  for (i = 0; i < r; ++i)
    t[i] = 0;
  for (i = 0; i < mlen; ++i)
    t[i] = m[i];
  t[i] = p;
  t[r - 1] |= 128;
  for (i = 0; i < r / 8; ++i)
    s[i] ^= load64(t + 8 * i);
}


static void keccak_squeezeblocks(unsigned char *h, unsigned long long int nblocks, __u64 *s, unsigned int r)
{
  unsigned int i;

  while(nblocks > 0) 
  {
    KeccakF1600_StatePermute(s);
    for (i = 0; i < (r>>3); i++)
    {
      store64(h+8*i, s[i]);
    }
    h += r;
    nblocks--;
  }
}


/********** SHAKE128 ***********/

void shake128_absorb(__u64 *s, const unsigned char *input, unsigned int inputByteLen)
{
	keccak_absorb(s, SHAKE128_RATE, input, inputByteLen, 0x1F);
}


void shake128_squeezeblocks(unsigned char *output, unsigned long long nblocks, __u64 *s)
{
	keccak_squeezeblocks(output, nblocks, s, SHAKE128_RATE);
}


void shake128(unsigned char *output, unsigned long long outlen, const unsigned char *input,  unsigned long long inlen)
{
  __u64 s[25] = {0};
  unsigned char t[SHAKE128_RATE];
  unsigned long long nblocks = outlen/SHAKE128_RATE;
  size_t i;
  
  /* Absorb input */
  keccak_absorb(s, SHAKE128_RATE, input, inlen, 0x1F);

  /* Squeeze output */
  keccak_squeezeblocks(output, nblocks, s, SHAKE128_RATE);

  output += nblocks*SHAKE128_RATE;
  outlen -= nblocks*SHAKE128_RATE;

  if (outlen) 
  {
    keccak_squeezeblocks(t, 1, s, SHAKE128_RATE);
    for (i = 0; i < outlen; i++)
      output[i] = t[i];
  }
}


/********** cSHAKE128 ***********/

void cshake128_simple_absorb(__u64 s[25], __u16 cstm, const unsigned char *in, unsigned long long inlen)
{
  unsigned char *sep = (unsigned char*)s;
  unsigned int i;

  for (i = 0; i < 25; i++)
    s[i] = 0;

  /* Absorb customization (domain-separation) string */
  sep[0] = 0x01;
  sep[1] = 0xa8;
  sep[2] = 0x01;
  sep[3] = 0x00;
  sep[4] = 0x01;
  sep[5] = 16; // fixed bitlen of cstm
  sep[6] = cstm & 0xff;
  sep[7] = cstm >> 8;

  KeccakF1600_StatePermute(s);

  /* Absorb input */
  keccak_absorb(s, SHAKE128_RATE, in, inlen, 0x04);
}


void cshake128_simple_squeezeblocks(unsigned char *output, unsigned long long nblocks, __u64 *s)
{
  keccak_squeezeblocks(output, nblocks, s, SHAKE128_RATE);
}


void cshake128_simple(unsigned char *output, unsigned long long outlen, __u16 cstm, const unsigned char *in, unsigned long long inlen)
{
  __u64 s[25];
  unsigned char t[SHAKE128_RATE];
  unsigned int i;

  cshake128_simple_absorb(s, cstm, in, inlen);

  /* Squeeze output */
  keccak_squeezeblocks(output, outlen/SHAKE128_RATE, s, SHAKE128_RATE);
  output += (outlen/SHAKE128_RATE)*SHAKE128_RATE;

  if (outlen%SHAKE128_RATE)
  {
    keccak_squeezeblocks(t, 1, s, SHAKE128_RATE);
    for (i = 0; i < outlen%SHAKE128_RATE; i++)
      output[i] = t[i];
  }
}


/********** SHAKE256 ***********/

void shake256_absorb(__u64 *s, const unsigned char *input, unsigned int inputByteLen)
{
	keccak_absorb(s, SHAKE256_RATE, input, inputByteLen, 0x1F);
}


void shake256_squeezeblocks(unsigned char *output, unsigned long long nblocks, __u64 *s)
{
	keccak_squeezeblocks(output, nblocks, s, SHAKE256_RATE);
}


void shake256(unsigned char *output, unsigned long long outlen, const unsigned char *input,  unsigned long long inlen)
{
  __u64 s[25];
  unsigned char t[SHAKE256_RATE];
  unsigned long long nblocks = outlen/SHAKE256_RATE;
  size_t i;

  for (i = 0; i < 25; ++i)
    s[i] = 0;
  
  /* Absorb input */
  keccak_absorb(s, SHAKE256_RATE, input, inlen, 0x1F);

  /* Squeeze output */
  keccak_squeezeblocks(output, nblocks, s, SHAKE256_RATE);

  output += nblocks*SHAKE256_RATE;
  outlen -= nblocks*SHAKE256_RATE;

  if (outlen) 
  {
    keccak_squeezeblocks(t, 1, s, SHAKE256_RATE);
    for (i = 0; i < outlen; i++)
      output[i] = t[i];
  }
}


/********** cSHAKE256 ***********/

void cshake256_simple_absorb(__u64 s[25], __u16 cstm, const unsigned char *in, unsigned long long inlen)
{
  unsigned char *sep = (unsigned char*)s;
  unsigned int i;

  for (i = 0; i < 25; i++)
    s[i] = 0;

  /* Absorb customization (domain-separation) string */
  sep[0] = 0x01;
  sep[1] = 0x88;
  sep[2] = 0x01;
  sep[3] = 0x00;
  sep[4] = 0x01;
  sep[5] = 16; // fixed bitlen of cstm
  sep[6] = cstm & 0xff;
  sep[7] = cstm >> 8;

  KeccakF1600_StatePermute(s);

  /* Absorb input */
  keccak_absorb(s, SHAKE256_RATE, in, inlen, 0x04);
}


void cshake256_simple_squeezeblocks(unsigned char *output, unsigned long long nblocks, __u64 *s)
{
  keccak_squeezeblocks(output, nblocks, s, SHAKE256_RATE);
}


void cshake256_simple(unsigned char *output, unsigned long long outlen, __u16 cstm, const unsigned char *in, unsigned long long inlen)
{
  __u64 s[25];
  unsigned char t[SHAKE256_RATE];
  unsigned int i;

  cshake256_simple_absorb(s, cstm, in, inlen);

  /* Squeeze output */
  keccak_squeezeblocks(output, outlen/SHAKE256_RATE, s, SHAKE256_RATE);
  output += (outlen/SHAKE256_RATE)*SHAKE256_RATE;

  if(outlen%SHAKE256_RATE)
  {
    keccak_squeezeblocks(t, 1, s, SHAKE256_RATE);
    for (i = 0; i < outlen%SHAKE256_RATE; i++)
      output[i] = t[i];
  }
}