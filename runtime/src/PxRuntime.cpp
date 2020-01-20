extern "C" {
    #include "PxRuntime.h"
}

#include <stdio.h>

extern "C" void printFloat(float f)
{
    printf("%f", f);
}

extern "C" void printInt(int i)
{
    printf("%d", i);
}

extern "C" void printString(PxString str)
{
    printf("%s", str);
}
