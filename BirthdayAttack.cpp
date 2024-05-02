#include <stdio.h>
#include <time.h>
#include <iostream>
#include "common.h"

using namespace std;

int main(int argc, char* argv[])
{
    const unsigned int strlen = 38;
    unsigned int num = 3;
    unsigned char str[strlen + 1] = {};
    generateString(str, strlen);
    for (int i = 0; i < strlen; i++)
    {
        printf("%02x", str[i]);
    }
    printf("\n");
    unsigned char output[32];

    SM3Calc(str, strlen, output, ivStd, true);
    printf("sm3:\n");
    printf("%02x\t", output[0]);
    print_Hexvalue(output + 1, SM3_HASH_SIZE - 1);
    printf("\n");
    unsigned char dest[32];
    unsigned char get_str[strlen + 1] = {};
    generateString(get_str, strlen);
    while(num > 0)
    {
        int j = 0;
        int k = 0;
        unsigned char get_str2[strlen + 1] = {};
        generateString(get_str2, strlen);
        while(j<=strlen)
        {
            if (get_str2[j] != str[j])
            {
                while (k<=strlen)
                {
                    if (get_str2[j] != get_str[j])
                    {
                        int i = 0;
                        while (i < strlen)
                        {
                            get_str[i] = get_str2[i];
                            i++;
                        }
                        break;

                    }
                    else if (k == strlen)
                    {
                        generateString(get_str2, strlen);
                        k = 0;
                    }
                    else
                        k++;
                }
                break;
            }
            else if (j == strlen)
            {
                generateString(get_str2, strlen);
                j = 0;
            }
            else
                j++;
        }

        SM3Calc(get_str, strlen, dest, ivStd, true);
        if (dest[0] == output[0])
        {
            //get result
            for (int i = 0; i < strlen; i++)
            {
                printf("%02x", get_str[i]);
            }
            cout << "\nget result:" << endl;
            printf("%02x\t", dest[0]);
            print_Hexvalue(dest + 1, SM3_HASH_SIZE - 1);
            printf("\n");
            num--;
        }

    }
}
