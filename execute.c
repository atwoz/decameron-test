#include <stdio.h>
#include "execute.h"

const char *execute(int temperature)
{
    printf("Setting AC temperature to: %d°C\n", temperature);

    // TODO: Add your actual AC control code here
    // For now, just return success
    return "{\"status\":\"success\",\"message\":\"Temperature set successfully\"}";
}