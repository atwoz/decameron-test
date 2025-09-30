#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "execute.h"
#include "decameron.h"

static char response_buffer[1024];

const char *execute(float temperature, float humidity, int presence)
{
    static uint32_t last_presence = 0;
    static bool run_once = true;
    static uint8_t set_temperature = 0;
    if (run_once) {
        DECAMERON_init();
        run_once = false;
    }
    // printf("Setting AC - Temperature: %.1f°C, Presence: %s\n",
    //        temperature, presence ? "ON" : "OFF");

    if (temperature > 0)
    {
        printf("----\n");
        dec_measurement_t measurement;
        measurement.temperature = temperature;
        measurement.humidity = humidity;
        dec_ac_state_t ac_state;
        if (presence)
        {
            last_presence = 0;
        }
        else
        {
            last_presence++;
        }
        if (DECAMERON_feed(&measurement, last_presence, true, &ac_state))
        {
            printf("AC state: %d, %d\n", ac_state.temp, ac_state.fan);
            printf("AC state: %d, %d\n", ac_state.temp, ac_state.fan);
            printf("AC state: %d, %d\n", ac_state.temp, ac_state.fan);
            printf("AC state: %d, %d\n", ac_state.temp, ac_state.fan);
            set_temperature = ac_state.temp;
        }
    }

    // TODO: Add your actual AC control code here
    // Simulate getting AC state after setting temperature and presence

    snprintf(response_buffer, sizeof(response_buffer),
        "{"
        "\"status\":\"success\","
        "\"power\":\"ON\","
        "\"currentTemp\":%u,"
        "\"targetTemp\":%0.1f,"
        "\"mode\":\"Cool\","
        "\"fanSpeed\":\"Auto\","
        "\"humidity\":%0.1f,"
        "\"presence\":%d"
        "}",
        set_temperature,
        temperature,
        humidity,
        presence);



    return response_buffer;
}