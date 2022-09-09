#include "messages.h"

#include <stdio.h>
#include <stdlib.h>

void tool_info(char *message)
{
    printf("* %s...\n", message);
}

void tool_error(char *message)
{
    printf("%s...\n", message);
    exit(EXIT_FAILURE);
}
