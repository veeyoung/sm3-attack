#include <stdio.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include "common.h"

using namespace std;
const unsigned int strLen = 66;

int main(int argc, char* argv[])
{
    unsigned int num = 3;
    unsigned char str[strLen + 1] = {};
    generateString(str, strLen);
    printf("%s\n", str);

    unsigned char output[32];
    SM3Calc(str, strLen, output, ivStd, true);
    printf("sm3:\n");
    printf("%02x\t", output[0]);
    print_Hexvalue(output + 1, SM3_HASH_SIZE - 1);
    printf("\n");

    unsigned char dest[32];
    unsigned char get_str[strLen + 1] = {};
    while(num > 0)
    {
        generateString(get_str, strLen);
        if (strcmp((char *)get_str, (char *)str) ==0 ){
            continue;
        }
        SM3Calc(get_str, strLen, dest, ivStd, true);
        if (dest[0] == output[0])
        {
            //get result
            printf("%s\n", get_str);
            cout << "get result:" << endl;
            printf("%02x\t", dest[0]);
            print_Hexvalue(dest + 1, SM3_HASH_SIZE - 1);
            printf("\n");
            num--;
        }

    }
}
