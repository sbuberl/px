/*
 * PxRuntime.cpp
 *
 *  Created on: Jun 12, 2010
 *      Author: steve
 */

#include <stdio.h>

extern "C" void printFloat(float f)
{
	printf("%f", f);
}

extern "C" void printInt(int i)
{
	printf("%d", i);
}
