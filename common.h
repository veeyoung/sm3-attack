#ifndef _SM3_H_
#define _SM3_H_

/*
* SM3算法产生的哈希值大小（单位：字节）
*/
#define SM3_HASH_SIZE 32

/*
* SM3上下文
*/
typedef struct SM3Context
{
	unsigned int intermediateHash[SM3_HASH_SIZE / 4];
	unsigned char messageBlock[64];
} SM3Context;

/*
* 判断运行环境是否为小端
*/
static const int endianTest = 1;
#define IsLittleEndian() (*(char *)&endianTest == 1)

/*
* 向左循环移位
*/
#define LeftRotate(word, bits) ( (word) << (bits) | (word) >> (32 - (bits)) )

/*
* SM3标准IV初始值
*/
extern unsigned int ivStd[8];

/*
* 生成随机字符串
*/
void generateString(unsigned char* dest, const unsigned int len);

/*
* 填充消息
*/
unsigned char* padMessage(const unsigned char* message, unsigned int messageLen, unsigned int* paddedLen);

/*
* SM3计算函数
*/
unsigned char* SM3Calc(const unsigned char* message,
    unsigned int messageLen, unsigned char digest[SM3_HASH_SIZE], unsigned int iv[], bool pad);

/*
* 反转四字节整型字节序
*/
unsigned int* ReverseWord(unsigned int* word);

/*
* 打印hex
*/
void print_Hexvalue(const unsigned char result[], const unsigned int len);

#endif
