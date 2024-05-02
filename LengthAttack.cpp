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

    // 生成secret+data
    unsigned char secretData[secretLen + dataLen + 1] = {};
    memcpy(secretData, secret, secretLen);
    memcpy(secretData + secretLen, data, dataLen + 1);

    //计算(secret+data)的hash值
    unsigned char result[32];
    SM3Calc(secretData, secretLen + dataLen, result, ivStd, true);
    cout << "\nhash(secret+data):" << endl;
    print_Hexvalue(result, SM3_HASH_SIZE);

    // 攻击者不知道secret，所以把secret部分填充0
    memset(secretData, 0, secretLen);
    // 填充secretData
    unsigned int secretDataPaddedLen;
    unsigned char* secretDataPadded;
    secretDataPadded = padMessage(secretData, secretLen + dataLen, &secretDataPaddedLen);
    // 追加攻击消息append
    unsigned char secretDataAppend[secretDataPaddedLen + appendLen + 1] = {};
    memcpy(secretDataAppend, secretDataPadded, secretDataPaddedLen);
    memcpy(secretDataAppend + secretDataPaddedLen, append, appendLen);
    // 填充secretDataAppend
    unsigned int secretDataAppendPaddedLen;
    unsigned char* secretDataAppendPadded;
    secretDataAppendPadded = padMessage(secretDataAppend, secretDataPaddedLen + appendLen, &secretDataAppendPaddedLen);

    // 更改初始向量IV为hash(secret+data)来计算hash(m')
    if (IsLittleEndian())
        for (int i = 0; i < 8; i++)
            ReverseWord((unsigned int*)result + i);
    SM3Calc(secretDataAppendPadded + secretDataPaddedLen, secretDataAppendPaddedLen - secretDataPaddedLen, result, (unsigned int*)result, false);
    cout << "hash(append):" << endl;
    print_Hexvalue(result, SM3_HASH_SIZE);

    // 输出攻击消息
    cout << "\ndata+padding+append:" << endl;
    print_Hexvalue(secretDataAppend + secretLen, secretDataPaddedLen + appendLen - secretLen);
    //服务端在开头加上secret,计算最终消息hash(secret+padding+append)
    memcpy(secretDataAppendPadded, secret, secretLen);
    unsigned char result_2[32];
    SM3Calc(secretDataAppendPadded, secretDataAppendPaddedLen, result_2, ivStd, false);
    cout << "hash(secret+data+padding+append):" << endl;
    print_Hexvalue(result_2, SM3_HASH_SIZE);
    free(secretDataPadded);
    free(secretDataAppendPadded);
}
