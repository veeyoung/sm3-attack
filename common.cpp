#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <mmintrin.h>  //MMX

#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <nmmintrin.h>
#include <wmmintrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <x86intrin.h>
#include <chrono>
#include "common.h"

using namespace std;

unsigned int ivStd[8] = {0x7380166F, 0x4914B2B9, 0x172442D7, 0xDA8A0600, 0xA96F30BC, 0x163138AA, 0xE38DEE4D, 0xB0FB0E4E};

const unsigned char allChar[63] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
void generateString(unsigned char* dest, const unsigned int len)
{
    unsigned int cnt, randNo;
    auto now = std::chrono::high_resolution_clock::now();
    srand(std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count());

    for (cnt = 0; cnt < len; cnt++)
    {
        randNo = rand() % 62;
        *dest = allChar[randNo];
        dest++;
    }

    *dest = '\0';
}

void print_Hexvalue(const unsigned char result[], const unsigned int len){
    for (int i = 0; i < len; i++)
    {
        printf("%02x", result[i]);
        if (((i + 1) % 4) == 0) printf(" ");
    }
    cout << endl;
}


unsigned int* ReverseWord(unsigned int* word)
{
    unsigned char* byte, temp;

    byte = (unsigned char*)word;
    temp = byte[0];
    byte[0] = byte[3];
    byte[3] = temp;

    temp = byte[1];
    byte[1] = byte[2];
    byte[2] = temp;
    return word;
}

/*
* T
*/
unsigned int T(int i)
{
    if (i >= 0 && i <= 15)
        return 0x79CC4519;
    else if (i >= 16 && i <= 63)
        return 0x7A879D8A;
    else
        return 0;
}

/*
* FF
*/
unsigned int FF(unsigned int X, unsigned int Y, unsigned int Z, int i)
{
    if (i >= 0 && i <= 15)
        return X ^ Y ^ Z;
    else if (i >= 16 && i <= 63)
        return (X & Y) | (X & Z) | (Y & Z);
    else
        return 0;
}

/*
* GG
*/
unsigned int GG(unsigned int X, unsigned int Y, unsigned int Z, int i)
{
    if (i >= 0 && i <= 15)
        return X ^ Y ^ Z;
    else if (i >= 16 && i <= 63)
        return (X & Y) | (~X & Z);
    else
        return 0;
}

/*
* P0
*/
unsigned int P0(unsigned int X)
{
    return X ^ LeftRotate(X, 9) ^ LeftRotate(X, 17);
}

/*
* P1
*/
unsigned int P1(unsigned int X)
{
    return X ^ LeftRotate(X, 15) ^ LeftRotate(X, 23);
}

/*
* 初始化函数
*/
void SM3Init(SM3Context* context, unsigned int iv[])
{
    _mm256_storeu_epi32(&context->intermediateHash[0], _mm256_loadu_epi32(&iv[0]));
}

/*
* 处理消息块
*/
void SM3ProcessMessageBlock(SM3Context* context)
{
    int i;
    unsigned int W[68];
    unsigned int W_[64];
    unsigned int A, B, C, D, E, F, G, H, SS1, SS2, TT1, TT2;

    /* 消息扩展 */
    for (i = 0; i < 16; i+=2)
    {
        W[i] = *(unsigned int*)(context->messageBlock + i * 4);
        W[i+1] = *(unsigned int*)(context->messageBlock + (i+1) * 4);//add
        if (IsLittleEndian())
        {
            ReverseWord(W + i);
            ReverseWord(W + i+1);//add
        }
    }

    for (i = 16; i < 68; i+=2)
    {
        W[i] = P1(W[i - 16] ^ W[i - 9] ^ LeftRotate(W[i - 3], 15))
            ^ LeftRotate(W[i - 13], 7)
            ^ W[i - 6];
        W[i+1] = P1(W[i - 15] ^ W[i - 8] ^ LeftRotate(W[i - 2], 15))
            ^ LeftRotate(W[i - 12], 7)
            ^ W[i - 5];

    }
    for (i = 0; i < 64; i+=8)
    {

        __m256i temp3 = _mm256_xor_epi32(_mm256_loadu_epi32(&W[i]), _mm256_loadu_epi32(&W[i + 4]));
        _mm256_storeu_epi32(&W_[i],temp3);

    }

    /* 消息压缩 */
    A = context->intermediateHash[0];
    B = context->intermediateHash[1];
    C = context->intermediateHash[2];
    D = context->intermediateHash[3];
    E = context->intermediateHash[4];
    F = context->intermediateHash[5];
    G = context->intermediateHash[6];
    H = context->intermediateHash[7];

    for (i = 0; i < 64; i++)
    {
        SS1 = LeftRotate((LeftRotate(A, 12) + E + LeftRotate(T(i), i)), 7);
        SS2 = SS1 ^ LeftRotate(A, 12);
        TT1 = FF(A, B, C, i) + D + SS2 + W_[i];
        TT2 = GG(E, F, G, i) + H + SS1 + W[i];
        D = C;
        C = LeftRotate(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = LeftRotate(F, 19);
        F = E;
        E = P0(TT2);
    }
    context->intermediateHash[0] ^= A;
    context->intermediateHash[1] ^= B;
    context->intermediateHash[2] ^= C;
    context->intermediateHash[3] ^= D;
    context->intermediateHash[4] ^= E;
    context->intermediateHash[5] ^= F;
    context->intermediateHash[6] ^= G;
    context->intermediateHash[7] ^= H;
}

/*
* 填充消息
*/
unsigned char* padMessage(const unsigned char* message, unsigned int messageLen, unsigned int* paddedLen) {
    *paddedLen = messageLen % 64 <= 55 ? (messageLen / 64 + 1) * 64 : (messageLen / 64 + 2) * 64;
    unsigned char* messageBlock = (unsigned char*)malloc(*paddedLen);
    if (messageBlock == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }

    memset(messageBlock, 0, *paddedLen);
    // 将 message 拷贝到 messageBlock 中
    memcpy(messageBlock, message, messageLen);
    // 填充消息分组
    messageBlock[messageLen] = 0x80;
    // 填充长度

    unsigned int bitLen = messageLen * 8;
    if (IsLittleEndian())
        ReverseWord(&bitLen);
    memcpy(messageBlock + *paddedLen - 4, &bitLen, 4);
    return messageBlock;
}

/*
* SM3算法主函数
*/
unsigned char* SM3Calc(const unsigned char* message,
    unsigned int messageLen, unsigned char digest[SM3_HASH_SIZE], unsigned int iv[], bool pad)
{
    if (iv == NULL){
        iv = ivStd;
    }
    SM3Context context;
    unsigned int i, remainder, bitLen;

    /* 初始化上下文 */
    SM3Init(&context, iv);

    unsigned int messagePaddedLen;
    unsigned char* messagePadded = NULL;
    // 填充消息
    if (pad){
        messagePadded = padMessage(message, messageLen, &messagePaddedLen);
        message = messagePadded;
        messageLen = messagePaddedLen;
    }
    else if (messageLen % 64 != 0){
        printf("error, message not padded\n");
        return NULL;
    }

    /* 对前面的消息分组进行处理 */
    for (i = 0; i < messageLen / 64; i++)
    {
        memcpy(context.messageBlock, message + i * 64, 64);
        SM3ProcessMessageBlock(&context);
    }
    if (messagePadded){
        free(messagePadded);
        message = messagePadded = NULL;
    }

    /* 返回结果 */
    if (IsLittleEndian())
        for (i = 0; i < 8; i++)
            ReverseWord(context.intermediateHash + i);
    memcpy(digest, context.intermediateHash, SM3_HASH_SIZE);

    return digest;
}
