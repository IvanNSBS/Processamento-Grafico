#ifndef RNGH
#define RNGH

#include <stdlib.h>

float drand48()
{
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

#endif