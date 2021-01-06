/*********************************************************************
Copyright (c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file  md5.h
@brief MD5 hash
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#ifndef _MD5_H_
#define _MD5_H_

namespace MD5Utils {
/// Copied from code by arcela Ozorio Suarez, Roberto I.
/// Convert code to C++ style.
/// Original = https://github.com/keplerproject/md5/blob/master/src/md5.c
/*vetor de numeros utilizados pelo algoritmo md5 para embaralhar bits */
static const uint32_t T[64]={
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};
/*funcao que implemeta os quatro passos principais do algoritmo MD5 */
static void digest(const uint32_t* m, uint32_t* d) {
    static auto rotate = [](uint32_t d, uint32_t s) {
        return (d << s) | (d >> (sizeof(uint32_t) - s));
    };
    static auto F = [](uint32_t x, uint32_t y, uint32_t z) {
        return (((x) & (y)) | ((~(x)) & (z)));
    };
    static auto G = [](uint32_t x, uint32_t y, uint32_t z) {
        return (((x) & (z)) | ((y) & (~(z))));
    };
    static auto H = [](uint32_t x, uint32_t y, uint32_t z) {
        return ((x) ^ (y) ^ (z));
    };
    static auto I = [](uint32_t x, uint32_t y, uint32_t z) {
        return ((y) ^ ((x) | (~(z))));
    };
    
    int j;
    /*MD5 PASSO1 */
    for (j = 0; j < 4 * 4; j += 4) {
        d[0] = d[0] + F(d[1], d[2], d[3]) + m[j] + T[j];       d[0] = rotate(d[0], 7);
        d[0] += d[1];
        d[3] = d[3] + F(d[0], d[1], d[2]) + m[(j)+1] + T[j + 1]; d[3] = rotate(d[3], 12);
        d[3] += d[0];
        d[2] = d[2] + F(d[3], d[0], d[1]) + m[(j)+2] + T[j + 2]; d[2] = rotate(d[2], 17);
        d[2] += d[3];
        d[1] = d[1] + F(d[2], d[3], d[0]) + m[(j)+3] + T[j + 3]; d[1] = rotate(d[1], 22);
        d[1] += d[2];
    }
    /*MD5 PASSO2 */
    for (j = 0; j < 4 * 4; j += 4) {
        d[0] = d[0] + G(d[1], d[2], d[3]) + m[(5 * j + 1) & 0x0f] + T[(j - 1) + 17];
        d[0] = rotate(d[0], 5);
        d[0] += d[1];
        d[3] = d[3] + G(d[0], d[1], d[2]) + m[((5 * (j + 1) + 1) & 0x0f)] + T[(j + 0) + 17];
        d[3] = rotate(d[3], 9);
        d[3] += d[0];
        d[2] = d[2] + G(d[3], d[0], d[1]) + m[((5 * (j + 2) + 1) & 0x0f)] + T[(j + 1) + 17];
        d[2] = rotate(d[2], 14);
        d[2] += d[3];
        d[1] = d[1] + G(d[2], d[3], d[0]) + m[((5 * (j + 3) + 1) & 0x0f)] + T[(j + 2) + 17];
        d[1] = rotate(d[1], 20);
        d[1] += d[2];
    }
    /*MD5 PASSO3 */
    for (j = 0; j < 4 * 4; j += 4) {
        d[0] = d[0] + H(d[1], d[2], d[3]) + m[(3 * j + 5) & 0x0f] + T[(j - 1) + 33];
        d[0] = rotate(d[0], 4);
        d[0] += d[1];
        d[3] = d[3] + H(d[0], d[1], d[2]) + m[(3 * (j + 1) + 5) & 0x0f] + T[(j + 0) + 33];
        d[3] = rotate(d[3], 11);
        d[3] += d[0];
        d[2] = d[2] + H(d[3], d[0], d[1]) + m[(3 * (j + 2) + 5) & 0x0f] + T[(j + 1) + 33];
        d[2] = rotate(d[2], 16);
        d[2] += d[3];
        d[1] = d[1] + H(d[2], d[3], d[0]) + m[(3 * (j + 3) + 5) & 0x0f] + T[(j + 2) + 33];
        d[1] = rotate(d[1], 23);
        d[1] += d[2];
    }
    /*MD5 PASSO4 */
    for (j = 0; j < 4 * 4; j += 4) {
        d[0] = d[0] + I(d[1], d[2], d[3]) + m[(7 * j) & 0x0f] + T[(j - 1) + 49];
        d[0] = rotate(d[0], 6);
        d[0] += d[1];
        d[3] = d[3] + I(d[0], d[1], d[2]) + m[(7 * (j + 1)) & 0x0f] + T[(j + 0) + 49];
        d[3] = rotate(d[3], 10);
        d[3] += d[0];
        d[2] = d[2] + I(d[3], d[0], d[1]) + m[(7 * (j + 2)) & 0x0f] + T[(j + 1) + 49];
        d[2] = rotate(d[2], 15);
        d[2] += d[3];
        d[1] = d[1] + I(d[2], d[3], d[0]) + m[(7 * (j + 3)) & 0x0f] + T[(j + 2) + 49];
        d[1] = rotate(d[1], 21);
        d[1] += d[2];
    }
}

static void bytestoword32(uint32_t* x, const uint8_t* pt) {
    int i;
    for (i = 0; i < 16; i++) {
        int j = i * 4;
        x[i] = (((uint32_t)(uint8_t)pt[j + 3] << 8 | (uint32_t)(uint8_t)pt[j + 2]) << 8 | (uint32_t)(uint8_t)pt[j + 1]) << 8 | (uint32_t)(uint8_t)pt[j];
    }
}

static void put_length(uint32_t* x, long len) {
    static constexpr uint32_t Mask32 = 0xffffffffu;
    /* in bits! */
    x[14] = (uint32_t)((len << 3) & Mask32);
    x[15] = (uint32_t)(len >> (32 - 3) & 0x7);
}

/*
** returned status:
*  0 - normal message (full 64 bytes)
*  1 - enough room for 0x80, but not for message length (two 4-byte words)
*  2 - enough room for 0x80 plus message length (at least 9 bytes free)
*/
static int convert(uint32_t* x, const uint8_t* pt, size_t num, int old_status) {
    int new_status = 0;
    uint8_t buff[64];
    if (num < 64) {
        memcpy(buff, pt, num);  /* to avoid changing original string */
        memset(buff + num, 0, 64 - num);
        if (old_status == 0)
            buff[num] = '\200';
        new_status = 1;
        pt = buff;
    }
    bytestoword32(x, pt);
    if (num <= (64 - 9))
        new_status = 2;
    return new_status;
}
} // namespace MD5Utils

class MD5
{
public:
    union {
        uint8_t  data8[16];
        uint16_t data16[8];
        uint32_t data32[4];
        uint64_t data64[2];
    };
    MD5(const MD5& src)
    {
        data64[0] = src.data64[0];
        data64[1] = src.data64[1];
    }
    MD5& operator = (const MD5& a) { data64[0] = a.data64[0]; data64[1] = a.data64[1]; return *this; }
    bool operator == (const MD5& a) { return data64[0] == a.data64[0] && data64[1] == a.data64[1]; }

    static MD5 Generate(void* data, size_t size) {
        static constexpr uint64_t ConvertUnitBytes = 64;
        
        MD5 output;

        size_t currpos = 0u;
        uint32_t status = 0u;
        while (status != 2u) {
            uint32_t old[4];
            uint32_t wbuff[16];
            size_t numbytes = (size - currpos > ConvertUnitBytes) ? ConvertUnitBytes : size - currpos;
            old[0] = output.data32[0]; old[1] = output.data32[1]; old[2] = output.data32[2]; old[3] = output.data32[3];
            status = MD5Utils::convert(wbuff, (uint8_t*)data + static_cast<uint64_t>(currpos), numbytes, status);
            if (status == 2u) MD5Utils::put_length(wbuff, size);
            MD5Utils::digest(wbuff, &output.data32[0]);
            output.data32[0] += old[0]; output.data32[1] += old[1]; output.data32[2] += old[2]; output.data32[3] += old[3];
            currpos += numbytes;
        }

        return output;
    }
private:
    MD5() {
        data32[0] = 0x67452301;
        data32[1] = 0xefcdab89;
        data32[2] = 0x98badcfe;
        data32[3] = 0x10325476;
    }
};

#endif