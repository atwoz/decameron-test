#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "execute.h"
#include "decameron.h"

static char response_buffer[1024];

const char *execute(int temperature, int presence)
{
    static bool run_once = true;
    static uint8_t set_temperature = 0;
    if (run_once) {
        DECAMERON_init();
        run_once = false;
    }
    printf("Setting AC - Temperature: %d°C, Presence: %s\n",
           temperature, presence ? "ON" : "OFF");

    dec_measurement_t measurement;
    measurement.temperature = temperature;
    measurement.humidity = 50;
    dec_ac_state_t ac_state;
    if (DECAMERON_feed(&measurement, &ac_state)) {
        printf("AC state: %d, %d\n", ac_state.temp, ac_state.fan);
        set_temperature = ac_state.temp;
    }

    // TODO: Add your actual AC control code here
    // Simulate getting AC state after setting temperature and presence
    double currentTemp = temperature; // Random temp between 20-30°C

    snprintf(response_buffer, sizeof(response_buffer),
        "{"
        "\"status\":\"success\","
        "\"power\":\"%s\","
        "\"currentTemp\":%.1f,"
        "\"targetTemp\":%u,"
        "\"mode\":\"Cool\","
        "\"fanSpeed\":\"Auto\","
        "\"presence\":%d"
        "}",
        presence ? "ON" : "OFF",
        currentTemp,
        set_temperature,
        presence);


    return response_buffer;
}