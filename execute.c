#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "execute.h"

static char response_buffer[1024];

const char *execute(int temperature, int humidity, int presence)
{
    printf("Setting AC - Temperature: %d°C, Humidity: %d%%, Presence: %s\n",
           temperature, humidity, presence ? "ON" : "OFF");

    // TODO: Add your actual AC control code here
    // Simulate getting AC state after setting temperature, humidity, and presence
    double currentTemp = 20.0 + (rand() % 100) / 10.0; // Random temp between 20-30°C

    snprintf(response_buffer, sizeof(response_buffer),
        "{"
        "\"status\":\"success\","
        "\"power\":\"%s\","
        "\"currentTemp\":%.1f,"
        "\"targetTemp\":%d,"
        "\"humidity\":%d,"
        "\"mode\":\"Cool\","
        "\"fanSpeed\":\"Auto\","
        "\"presence\":%d"
        "}",
        presence ? "ON" : "OFF",
        currentTemp,
        temperature,
        humidity,
        presence);

    return response_buffer;
}