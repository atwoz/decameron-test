#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
Tomando una muestra cada 1 minuto
*/
#define MAX_MEASUREMENTS 60

typedef struct {
    uint8_t temp;
    uint8_t fan;
} dec_ac_state_t;

typedef struct {
    float temperature;
    float humidity;
} dec_measurement_t;


typedef struct {
    uint8_t cmd_temp_high;
    uint8_t cmd_fan_high;
    uint8_t cmd_temp_low;
    uint8_t cmd_fan_low;
} dec_cmd_t;


typedef struct {
    float min_temperature;
    float max_temperature;
    float max_humidity;
    uint32_t presence_timeout_day;
    uint32_t presence_timeout_night;
    uint32_t cmd_timeout_day;
    uint32_t cmd_timeout_night;
} dec_conditions_t;

typedef struct {
    dec_ac_state_t ac_state;
    dec_cmd_t cmd;
    // circular buffer
    dec_measurement_t measurement[MAX_MEASUREMENTS];
    uint32_t last_presence;
    bool was_present;
    uint32_t is_night;
    uint8_t measurement_index;
    uint32_t minute_count;
    uint32_t last_cmd;
    dec_conditions_t conditions;
} decameron_context_t;


bool DECAMERON_feed(dec_measurement_t *measurement, uint32_t last_presence, bool is_night, dec_ac_state_t *ac_state);
void DECAMERON_init(void);