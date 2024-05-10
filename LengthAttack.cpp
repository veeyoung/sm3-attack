#include <stdio.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include "common.h"

using namespace std;

const unsigned int appendLen = 42;
const unsigned int secretLen = 41;
const unsigned int dataLen = 40;

int main(int argc, char* argv[])
{
    //生成secret并输出
    unsigned char secret[secretLen + 1] = {};
    generateString(secret, secretLen);
    printf("secret:\n");
    printf("%s\n", secret);

    //生成data并输出
    unsigned char data[dataLen + 1] = {};
    generateString(data, dataLen);
    printf("data:\n");
    printf("%s\n", data);

    //生成m'（append）并输出
    unsigned char append[appendLen + 1] = {};
    generateString(append, appendLen);
    cout << "append message:" << endl;
    for (int i = 0; i < appendLen; i++)
    {
        printf("%02x", append[i]);
    }
    cout << endl;

    unsigned char secretDataAppendPadded[secretLen + dataLen + appendLen + 4 * 64];

    // 生成secret+data
    memcpy(secretDataAppendPadded, secret, secretLen);
    memcpy(secretDataAppendPadded + secretLen, data, dataLen + 1);

    //计算(secret+data)的hash值
    unsigned char result[32];
    SM3Calc(secretDataAppendPadded, secretLen + dataLen, result, ivStd, true);
    cout << "\nhash(secret+data):" << endl;
    print_Hexvalue(result, SM3_HASH_SIZE);

    // 填充secretData
    unsigned int padLen1;
    unsigned int secretDataPaddedLen;
    unsigned char* pad;
    pad = padMessage(secretDataAppendPadded, secretLen + dataLen, &padLen1);
    secretDataPaddedLen = secretLen + dataLen + padLen1;
    memcpy(secretDataAppendPadded + secretLen + dataLen, pad, padLen1);
    free(pad);

    // 追加攻击消息append
    // 攻击者不知道secret，所以把secret部分填充0
    memset(secretDataAppendPadded, 0, secretLen);
    memcpy(secretDataAppendPadded + secretDataPaddedLen, append, appendLen);

    // 填充secretDataAppend
    unsigned int secretDataAppendPaddedLen;
    unsigned int padLen2;
    pad = padMessage(secretDataAppendPadded, secretDataPaddedLen + appendLen, &padLen2);
    secretDataAppendPaddedLen = secretDataPaddedLen + appendLen + padLen2;
    memcpy(secretDataAppendPadded + secretDataPaddedLen + appendLen, pad, padLen2);
    free(pad);

    // 更改初始向量IV为hash(secret+data)来计算hash(m')
    if (IsLittleEndian())
        for (int i = 0; i < 8; i++)
            ReverseWord((unsigned int*)result + i);
    SM3Calc(secretDataAppendPadded + secretDataPaddedLen, secretDataAppendPaddedLen - secretDataPaddedLen, result, (unsigned int*)result, false);
    cout << "hash(append):" << endl;
    print_Hexvalue(result, SM3_HASH_SIZE);

    // 输出攻击消息
    cout << "\ndata+padding+append:" << endl;
    print_Hexvalue(secretDataAppendPadded + secretLen, secretDataPaddedLen + appendLen - secretLen);
    //服务端在开头加上secret,计算最终消息hash(secret+padding+append)
    memcpy(secretDataAppendPadded, secret, secretLen);
    unsigned char result_2[32];
    SM3Calc(secretDataAppendPadded, secretDataAppendPaddedLen, result_2, ivStd, false);
    cout << "hash(secret+data+padding+append):" << endl;
    print_Hexvalue(result_2, SM3_HASH_SIZE);
}
