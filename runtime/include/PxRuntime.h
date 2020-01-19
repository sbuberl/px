#ifndef PX_PXRUNTIME_H
#define PX_PXRUNTIME_H

#include <stdint.h>
#include <stdbool.h>

typedef struct _PxString
{
    int8_t *bytes;
    intptr_t length;
    intptr_t byteLength;
} PxString;

void printInt(int32_t i);
void printFloat(float f);

#endif //PX_PXRUNTIME_H
