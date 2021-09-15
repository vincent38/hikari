#include "mc48.h"

#define HALF_BLOCK_SIZE 24
#define MASK 0xFFFFFFULL
#define RN2 48

/* Randomly generated */
uint64_t RRC0[64] = {31, 52, 34, 55, 25, 5, 20, 55, 42, 41, 8, 42, 11, 56, 0, 34, 4, 27, 14, 50, 58, 49, 11, 63, 42, 44, 17, 53, 45, 3, 13, 57, 60, 62, 50, 31,
	4, 12, 42, 61, 57, 48, 7, 52, 36, 30, 9, 57, 17, 25, 33, 63, 31, 28, 55, 37, 23, 39, 31, 60, 24, 45, 16, 50};
uint64_t RRC1[64] = {10, 31, 49, 14, 38, 14, 1, 18, 42, 14, 54, 14, 10, 21, 45, 49, 50, 50, 5, 44, 43, 34, 29, 63, 57, 19, 53, 4, 0, 60, 20, 27, 13, 63, 62,
	13, 50, 0, 46, 52, 63, 41, 19, 5, 8, 51, 37, 45, 51, 24, 0, 61, 58, 1, 2, 22, 7, 31, 25, 47, 32, 5, 16, 42};
uint64_t ARC0[64] = {0x1d661d6d78ad01aeull, 0x5c0109a38ac7ec12ull, 0x3e23c90f734e79f4ull, 0x41cb54cad99ec432ull, 0x27645c6ea0839de8ull, 0x603a3e6ea97bf5baull,
	0xa88d05b349ce2923ull, 0x52761daa0a66e8b8ull, 0xa1323d8a8ec5fc0bull, 0xc09550353a8cdf70ull, 0x6caa38e3bb4d6335ull, 0x36b7aa6b2e048cb5ull, 0x1c61f90a5012f209ull,
	0xf838ece1020789d8ull, 0xa5ad60808935d271ull, 0xe7574630ca833521ull, 0x99ab71b32480a0ecull, 0x5a4d7bd4ead1899full, 0x1637f6b33ca01defull, 0xf711a958afe661f7ull,
	0xafdaff2bf59393b2ull, 0xcc6472ab9799f775ull, 0x3d1d563ce1980461ull, 0xab57c8783aec9585ull, 0x53cb715df0d06325ull, 0x67d08204dcfe28baull, 0x2294233ccadfc225ull,
	0x1bd501bae2b1f067ull, 0x3a39c4f7b760c2c8ull, 0xc09fa9755171bab4ull, 0xec2edad1c19f52a9ull, 0xd33268fca9566ef2ull, 0xe706ad670325de6cull, 0xb04c080ab96ce80full,
	0x158306387b5a3581ull, 0x3b555f7b4bcaf500ull, 0x21b38d7a2fbaedbcull, 0x7710b2f96f675891ull, 0x721d063ac78b89e6ull, 0x3c6a07ba25f04f86ull, 0xe32faa1c44c17dc7ull,
	0x67aa1ea02800379eull, 0xfa99c19453ebf555ull, 0xb3186b6b9a00f212ull, 0xb1b854810864397full, 0x06353a7c7cd8ac99ull, 0xa7ac586ab467d9acull, 0x64729d4117e9dae3ull,
	0xd7240918ca625e1bull, 0x9d55c5edc5b7cd1cull, 0xdaf17aec8e1533e3ull, 0x8758bcf8999ef977ull, 0x84c2dba8497e3a94ull, 0xe406900fb7127f6full, 0xa3271e964bc2975dull,
	0x6fa104172d9ac5c7ull, 0x6f315ca24eef0630ull, 0xd6b5ce865df2e527ull, 0xd9b2c82b666c93dbull, 0xdf60fb56b3a1b4ffull, 0x3dca01a691a7f3ebull, 0xe0f1cba4c5d8f1f2ull,
	0xffb725dfe46643bcull, 0xfb8cc123387bd4f1ull};
uint64_t ARC1[64] = {0x7f54d9bb2d816e0dull, 0x5daf58e218683940ull, 0x3ba08dded8ae3a37ull, 0xc9fca9e2e07b1fa3ull, 0x83ecc0e1a3e7a6b3ull, 0x2d6f84b9e954ae43ull,
	0x5f9fe0cbbbcb1799ull, 0xd2e4d081bedc2121ull, 0x446ba5cc165e1d57ull, 0x29540536c9f2ed20ull, 0x776eeb4997b23e63ull, 0x4ecb72221f60a0c5ull, 0x5c8095558340bbe6ull,
	0x9ad02db1d8d3c2d3ull, 0x92bfe4948d7bad36ull, 0xc3053a353af2b528ull, 0x2c06803ee264040full, 0x4a4f6d16793bdd57ull, 0x5142c141fba3beaaull, 0x5475022d6668d731ull,
	0x3d5135bc6a13c296ull, 0x680dbe51abd4d422ull, 0xcc637a1c2ce6b912ull, 0xadee54efbe229cafull, 0x6d910803d56b0331ull, 0xc020a01743cebf1bull, 0x5c7f6310ab266d94ull,
	0x178eaa2b7b260cb8ull, 0x5ba52cf0f4eb5e39ull, 0x05f6599b28c13019ull, 0x700af1afe78b074cull, 0x34b0db8294072b41ull, 0x87bf9eec740e0e2aull, 0xd836d97b7a67bcfcull,
	0xef3787c27f3b4d7full, 0xf9b8913b01b80283ull, 0x29c1616e93cd2cbfull, 0x705ea6a1fdb079ceull, 0x9892bf4a4e674a40ull, 0x7600d8ef1921d4f8ull, 0x7a96b99e882dab82ull,
	0x7b01a8a07f79e0e5ull, 0xc4ecb440217e6b90ull, 0x629f7bd5031cc0fdull, 0x474b4aa3ec2241a8ull, 0xd636f4618d1386e9ull, 0x13e3c5fdf11a3fc6ull, 0xd3832e051aa247efull,
	0xcbe4e8d1e42e2104ull, 0xba83574bef62304cull, 0xca9fe260a183871dull, 0xe7bcacf9fc090435ull, 0x92b59d8af45e1948ull, 0xef66a64d8b93e418ull, 0xd5dbb9261cfab44bull,
	0xea596af31be293bbull, 0x5bed2ec8eedb35c6ull, 0xd9173ba12cd7637full, 0xf459dcd5a0f105edull, 0xb440f6f4fe9ba044ull, 0xff0406c79c8712c7ull, 0x71a5358bdcda43ebull,
	0x65d02bcddda467f9ull, 0xc0078460daeaecceull};

/* cf. bithacks (for instance) */
uint64_t reverse64(uint64_t x)
{
	x = ((x >> 1) &  0x5555555555555555ULL) | ((x & 0x5555555555555555ULL) << 1 );
	x = ((x >> 2) &  0x3333333333333333ULL) | ((x & 0x3333333333333333ULL) << 2 );
	x = ((x >> 4) &  0x0F0F0F0F0F0F0F0FULL) | ((x & 0x0F0F0F0F0F0F0F0FULL) << 4 );
	x = ((x >> 8) &  0x00FF00FF00FF00FFULL) | ((x & 0x00FF00FF00FF00FFULL) << 8 );
	x = ((x >> 16) & 0x0000FFFF0000FFFFULL) | ((x & 0x0000FFFF0000FFFFULL) << 16);
	x = ( x >> 32)                          | ( x                          << 32);

	return x;
}

uint64_t rotr64(uint64_t x, unsigned r)
{
	return ((x >> r) | ((x << (64 - r))));
}

/* 
 * The next three functions come from SHA-512
 * Note that (Sigma0 ^ Sigma1 ^ sigma0) is invertible
 */
uint64_t Sigma0(uint64_t x)
{
	return (rotr64(x, 28) ^ rotr64(x, 34) ^ rotr64(x, 39));
}

uint64_t Sigma1(uint64_t x)
{
	return (rotr64(x, 14) ^ rotr64(x, 18) ^ rotr64(x, 41));
}

uint64_t sigma0(uint64_t x)
{
	return (rotr64(x, 1) ^ rotr64(x, 8) ^ (x << 7));
}

/*
 * Rationale for phi: multiply $x$ by a sparse invertible matrix, but replace
 * some of the XORs by modular additions to make it non-linear.
 * Then reverse the bit order, so that the result remains reasonably non-linear,
 * even after a (potentially massive) truncation.
 */
uint64_t phi(uint64_t x)
{
	return (reverse64(Sigma0(x) + Sigma1(x) + sigma0(x)) & MASK);
}

/*
 * Two Feistel rounds, parametered by a two-round index
 */
void tworounds(uint64_t x[2], uint64_t k[2], unsigned i)
{
	uint64_t tmp;

	tmp = x[0];
	x[0] = x[1] ^ phi(x[0] ^ rotr64(k[0] ^ ARC0[i], RRC0[i]));
	x[1] = tmp;
	tmp = x[0];
	x[0] = x[1] ^ phi(x[0] ^ rotr64(k[1] ^ ARC1[i], RRC1[i]));
	x[1] = tmp;

	return;
}

/*
 * Fixed instance with 48-bit blocks, 128-bit keys
 */
void tcz48(uint8_t x[6], const uint8_t k[16])
{
    // dirty temps, manual memcpy?
    uint64_t xp[2];
    uint64_t kp[2];

    xp[0] = x[0] ^ (((uint64_t)x[1]) << 8) ^ (((uint64_t)x[2]) << 16);
    xp[1] = x[3] ^ (((uint64_t)x[4]) << 8) ^ (((uint64_t)x[5]) << 16);

    kp[0] = k[0] ^ (((uint64_t)k[1]) << 8) ^ (((uint64_t)k[2]) << 16) ^ (((uint64_t)k[3]) << 24) ^ (((uint64_t)k[4]) << 32) ^ (((uint64_t)k[5]) << 40) ^ (((uint64_t)k[6]) << 48) ^ (((uint64_t)k[7]) << 56);
    kp[1] = k[8] ^ (((uint64_t)k[9]) << 8) ^ (((uint64_t)k[10]) << 16) ^ (((uint64_t)k[11]) << 24) ^ (((uint64_t)k[12]) << 32) ^ (((uint64_t)k[13]) << 40) ^ (((uint64_t)k[14]) << 48) ^ (((uint64_t)k[15]) << 56);

    for (int i = 0; i < RN2; i++)
	{
		tworounds(xp, kp, i);
	}

    x[0] = xp[0] & 0xFF;
    x[1] = (xp[0] >> 8) & 0xFF;
    x[2] = (xp[0] >> 16) & 0xFF;
    x[3] = xp[1] & 0xFF;
    x[4] = (xp[1] >> 8) & 0xFF;
    x[5] = (xp[1] >> 16) & 0xFF;

	return;
}

/* 
 * The Davies-Meyer compression function based on tcz48 using an XOR feedforward
 * Input m: a 128-bit message block
 * Input h: a 48-bit chaining value
 * Output: void, h is overwritten with the result
 */
void tcz48_dm(const uint8_t m[16], uint8_t h[6])
{
	uint8_t th[6];

    for (int i = 0; i < 6; i++)
       th[i] = h[i];

	tcz48(h, m);

    for (int i = 0; i < 6; i++)
        h[i] ^= th[i];

	return;
}

/* 
 * Toy hash function with 48-bit digests
 * Input m: the message to be hashed, whose length is required to be an integral number of bytes
 * Input blen: the byte length of m
 * Input h: placeholder for the result. Must have been allocated by the caller.
 * Output: void, h is overwritten with the result
 */
void ht48(const uint8_t *m, uint64_t blen, uint8_t h[6])
{
    // simple IV
    h[0] = IVB0;
    h[1] = IVB1;
    h[2] = IVB2;
    h[3] = IVB3;
    h[4] = IVB4;
    h[5] = IVB5;

    uint8_t  mp[16];
    uint64_t bm = blen % 16;
    uint64_t bd = blen - bm;

    for (uint64_t s = 0; s < bd; s += 16)
    {
        tcz48_dm(m + s, h);
    }

    /// padding ///

    // pad with zeros then blen on 64 bits
    if (bm == 0)
    {
        for (int i = 0; i < 8; i++)
        {
            mp[i]     = 0;
            mp[i + 8] = (blen >> (i * 8)) & 0xFF;
        }
        tcz48_dm(mp, h);
        return;
    }

    // remaining message, zeros, blen
    if (bm <= 8)
    {
        for (int i = 0; i < bm; i++)
        {
            mp[i] = m[bd + i];
        }
        for (int i = bm; i < 8; i++)
        {
            mp[i] = 0;
        }
        for (int i = 0; i < 8; i++)
        {
            mp[i + 8] = (blen >> (i * 8)) & 0xFF;
        }
        tcz48_dm(mp, h);
        return;
    }

    // remaining message, zeros
    for (int i = 0; i < bm; i++)
    {
        mp[i] = m[bd + i];
    }
    for (int i = bm; i < 16; i++)
    {
        mp[i] = 0;
    }
    tcz48_dm(mp, h);

    // zeros, blen
    for (int i = 0; i < 8; i++)
    {
        mp[i]     = 0;
        mp[i + 8] = (blen >> (i * 8)) & 0xFF;
    }
    tcz48_dm(mp, h);

    return;
}
