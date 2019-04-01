/********************************************************************************************
* Hardware-based random number generation function
*
* It uses /dev/urandom in Linux and CNG's BCryptGenRandom function in Windows
*********************************************************************************************/ 

#include "random.h"
#include <linux/string.h>
#include <linux/random.h>


#define passed 0 
#define failed 1

int randombytes(unsigned char* random_array, unsigned long long nbytes)
{ 
    get_random_bytes_wait(random_array,nbytes);
    return passed;
}