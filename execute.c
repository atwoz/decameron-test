#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "execute.h"

static char response_buffer[1024];

const char *execute(int temperature)
{
    printf("Setting AC temperature to: %d°C\n", temperature);

    // TODO: Add your actual AC control code here
    // Simulate getting AC state after setting temperature
    double currentTemp = 20.0 + (rand() % 100) / 10.0; // Random temp between 20-30°C

    snprintf(response_buffer, sizeof(response_buffer),
        "{"
        "\"status\":\"success\","
        "\"power\":\"ON\","
        "\"currentTemp\":%.1f,"
        "\"targetTemp\":%d,"
        "\"mode\":\"Cool\","
        "\"fanSpeed\":\"Auto\""
        "}",
        currentTemp, temperature);

    return response_buffer;
}