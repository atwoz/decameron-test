#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "execute.h"

static char response_buffer[1024];

const char *execute(int temperature, int presence)
{
    printf("Setting AC - Temperature: %d°C, Presence: %s\n",
           temperature, presence ? "ON" : "OFF");

    // TODO: Add your actual AC control code here
    // Simulate getting AC state after setting temperature and presence
    double currentTemp = 20.0 + (rand() % 100) / 10.0; // Random temp between 20-30°C

    snprintf(response_buffer, sizeof(response_buffer),
        "{"
        "\"status\":\"success\","
        "\"power\":\"%s\","
        "\"currentTemp\":%.1f,"
        "\"targetTemp\":%d,"
        "\"mode\":\"Cool\","
        "\"fanSpeed\":\"Auto\","
        "\"presence\":%d"
        "}",
        presence ? "ON" : "OFF",
        currentTemp,
        temperature,
        presence);

    return response_buffer;
}