#include "panic.h"

#include <stdio.h>
#include <stdlib.h>

void panic(const char* message)
{
    printf("Panic: %s", message);
    abort();
}